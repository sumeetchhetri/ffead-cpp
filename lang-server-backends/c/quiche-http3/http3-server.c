// Copyright (C) 2018-2019, Cloudflare, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <ev.h>
#include <uthash.h>

#include <quiche.h>
#include <ffead-cpp.h>

#include <time.h>

#define LOCAL_CONN_ID_LEN 16

#define MAX_DATAGRAM_SIZE 1350

#define MAX_TOKEN_LEN \
    sizeof("quiche") - 1 + \
    sizeof(struct sockaddr_storage) + \
    QUICHE_MAX_CONN_ID_LEN

struct connections {
    int sock;

    struct conn_io *h;
};

struct conn_io {
    ev_timer timer;

    int sock;

    uint8_t cid[LOCAL_CONN_ID_LEN];

    quiche_conn *conn;
    quiche_h3_conn *http3;

    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;

    struct ffead_request freq;
    struct phr_header_fcp fhdrs[50];
    void* fres;

    UT_hash_handle hh;
};

static quiche_config *config = NULL;

static quiche_h3_config *http3_config = NULL;

static struct connections *conns = NULL;

static void timeout_cb(EV_P_ ev_timer *w, int revents);

static void debug_log(const char *line, void *argp) {
    fprintf(stderr, "%s\n", line);
}

static const char* get_date() {
	time_t t;
	struct tm tm;
	static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	static __thread char date[30] = "Thu, 01 Jan 1970 00:00:00 GMT";

	time(&t);
	gmtime_r(&t, &tm);
	strftime(date, 30, "---, %d --- %Y %H:%M:%S GMT", &tm);
	memcpy(date, days[tm.tm_wday], 3);
	memcpy(date + 8, months[tm.tm_mon], 3);

	return date;
}

static void flush_egress(struct ev_loop *loop, struct conn_io *conn_io) {
    static uint8_t out[MAX_DATAGRAM_SIZE];

    while (1) {
        ssize_t written = quiche_conn_send(conn_io->conn, out, sizeof(out));

        if (written == QUICHE_ERR_DONE) {
            //fprintf(stderr, "done writing\n");
            break;
        }

        if (written < 0) {
            fprintf(stderr, "failed to create packet: %zd\n", written);
            return;
        }

        ssize_t sent = sendto(conn_io->sock, out, written, 0,
                              (struct sockaddr *) &conn_io->peer_addr,
                              conn_io->peer_addr_len);
        if (sent != written) {
            perror("failed to send");
            return;
        }

        //fprintf(stderr, "sent %zd bytes\n", sent);
    }

    double t = quiche_conn_timeout_as_nanos(conn_io->conn) / 1e9f;
    conn_io->timer.repeat = t;
    ev_timer_again(loop, &conn_io->timer);
}

static void mint_token(const uint8_t *dcid, size_t dcid_len,
                       struct sockaddr_storage *addr, socklen_t addr_len,
                       uint8_t *token, size_t *token_len) {
    memcpy(token, "quiche", sizeof("quiche") - 1);
    memcpy(token + sizeof("quiche") - 1, addr, addr_len);
    memcpy(token + sizeof("quiche") - 1 + addr_len, dcid, dcid_len);

    *token_len = sizeof("quiche") - 1 + addr_len + dcid_len;
}

static bool validate_token(const uint8_t *token, size_t token_len,
                           struct sockaddr_storage *addr, socklen_t addr_len,
                           uint8_t *odcid, size_t *odcid_len) {
    if ((token_len < sizeof("quiche") - 1) ||
         memcmp(token, "quiche", sizeof("quiche") - 1)) {
        return false;
    }

    token += sizeof("quiche") - 1;
    token_len -= sizeof("quiche") - 1;

    if ((token_len < addr_len) || memcmp(token, addr, addr_len)) {
        return false;
    }

    token += addr_len;
    token_len -= addr_len;

    if (*odcid_len < token_len) {
        return false;
    }

    memcpy(odcid, token, token_len);
    *odcid_len = token_len;

    return true;
}

static uint8_t *gen_cid(uint8_t *cid, size_t cid_len) {
    int rng = open("/dev/urandom", O_RDONLY);
    if (rng < 0) {
        perror("failed to open /dev/urandom");
        return NULL;
    }

    ssize_t rand_len = read(rng, cid, cid_len);
    if (rand_len < 0) {
        perror("failed to create connection ID");
        return NULL;
    }

    return cid;
}

static struct conn_io *create_conn(uint8_t *scid, size_t scid_len,
                                   uint8_t *odcid, size_t odcid_len) {
    struct conn_io *conn_io = calloc(1, sizeof(*conn_io));
    if (conn_io == NULL) {
        fprintf(stderr, "failed to allocate connection IO\n");
        return NULL;
    }

    if (scid_len != LOCAL_CONN_ID_LEN) {
        fprintf(stderr, "failed, scid length too short\n");
    }

    memcpy(conn_io->cid, scid, LOCAL_CONN_ID_LEN);

    quiche_conn *conn = quiche_accept(conn_io->cid, LOCAL_CONN_ID_LEN,
                                      odcid, odcid_len, config);
    if (conn == NULL) {
        fprintf(stderr, "failed to create connection\n");
        return NULL;
    }


    conn_io->fres = NULL;
    conn_io->freq.headers_len = 0;
    conn_io->freq.headers = conn_io->fhdrs;

    conn_io->sock = conns->sock;
    conn_io->conn = conn;

    ev_init(&conn_io->timer, timeout_cb);
    conn_io->timer.data = conn_io;

    HASH_ADD(hh, conns->h, cid, LOCAL_CONN_ID_LEN, conn_io);

    //fprintf(stderr, "new connection\n");

    return conn_io;
}

static int for_each_header(uint8_t *name, size_t name_len,
                           uint8_t *value, size_t value_len,
                           void *argp) {
	struct conn_io *conn_io = (struct conn_io *)argp;
	conn_io->fhdrs[conn_io->freq.headers_len].name = (const char *)name;
	conn_io->fhdrs[conn_io->freq.headers_len].name_len = name_len;
	conn_io->fhdrs[conn_io->freq.headers_len].value = (const char *)value;
	conn_io->fhdrs[conn_io->freq.headers_len++].value_len = value_len;
    //fprintf(stderr, "got HTTP header: %.*s=%.*s\n",
    //        (int) name_len, name, (int) value_len, value);
    return 0;
}

static void ffead_handle(struct conn_io* conn_io, int64_t s, uint8_t* buf, ssize_t len, int hdr) {
	for(int i=0;i<conn_io->freq.headers_len;i++) {
		if(strncmp((const char*)conn_io->fhdrs[i].name, ":method", 7)==0) {
			conn_io->freq.method = (const char*)conn_io->fhdrs[i].value;
			conn_io->freq.method_len = conn_io->fhdrs[i].value_len;
		} else if(strncmp((const char*)conn_io->fhdrs[i].name, ":path", 5)==0) {
			conn_io->freq.path = (const char*)conn_io->fhdrs[i].value;
			conn_io->freq.path_len = conn_io->fhdrs[i].value_len;
		}
	}
	conn_io->freq.server_str = "quiche";
	conn_io->freq.server_str_len = 5;
	conn_io->freq.version = 3;

	conn_io->freq.body = (const char *)buf;
	conn_io->freq.body_len = len;
	//printf("%.*s", (int) len, buf);

	int scode = 0;
	const char* out_url;
	size_t out_url_len = 0;
	const char* out_body;
	size_t out_body_len = 0;
	size_t out_headers_len = 0;
	const char* smsg;
	size_t smsg_len = 0;

	if(conn_io->fres!=NULL) {
		ffead_cpp_resp_cleanup(conn_io->fres);
	}
	conn_io->fres = ffead_cpp_handle_c_1(&conn_io->freq, &scode, &smsg, &smsg_len, &out_url, &out_url_len,
			&conn_io->fhdrs[0], &out_headers_len, &out_body, &out_body_len);

	//printf("%d %ld %ld\n", scode, out_headers_len, out_url_len);
	//printf("%p\n", (void *) conn_io->fres);

	char sc[4];
	snprintf(sc, 4, "%d", scode);
	if(scode>0) {
		quiche_h3_header qhdrs[out_headers_len+3];
		qhdrs[0].name = (const uint8_t *)":status";
		qhdrs[0].name_len = strlen((const char *)qhdrs[0].name);
		qhdrs[0].value = (const uint8_t *)sc;
		qhdrs[0].value_len = strlen((const char *)qhdrs[0].value);
		qhdrs[1].name = (const uint8_t *)"server";
		qhdrs[1].name_len = strlen((const char *)qhdrs[1].name);
		qhdrs[1].value = (const uint8_t *)"quiche";
		qhdrs[1].value_len = strlen((const char *)qhdrs[1].value);
		qhdrs[2].name = (const uint8_t *)"date";
		qhdrs[2].name_len = strlen((const char *)qhdrs[2].name);
		qhdrs[2].value = (const uint8_t *)get_date();
		qhdrs[2].value_len = strlen((const char *)qhdrs[2].value);
		for(int i=3;i<(int)out_headers_len+3;i++) {
			qhdrs[i].name = (const uint8_t *)conn_io->fhdrs[i-3].name;
			qhdrs[i].name_len = conn_io->fhdrs[i-3].name_len;
			qhdrs[i].value = (const uint8_t *)conn_io->fhdrs[i-3].value;
			qhdrs[i].value_len = conn_io->fhdrs[i-3].value_len;
		}
		quiche_h3_send_response(conn_io->http3, conn_io->conn, s, qhdrs, out_headers_len+3, false);
		if(out_body_len>0) {
			quiche_h3_send_body(conn_io->http3, conn_io->conn, s, (uint8_t *) out_body, out_body_len, true);
		}
	} else {
		if(out_url_len>0 && access(out_url, F_OK) != -1 ) {
			char * buffer = 0;
			long length;
			FILE * f = fopen (out_url, "rb");

			if(f) {
				fseek (f, 0, SEEK_END);
				length = ftell (f);
				fseek (f, 0, SEEK_SET);
				buffer = malloc (length);
				if (buffer)
				{
					fread (buffer, 1, length, f);
				}
				fclose (f);

				char slen[10];
				snprintf(slen, 9, "%ld", length);

				quiche_h3_header qhdrs[5];
				qhdrs[0].name = (const uint8_t *)":status";
				qhdrs[0].name_len = strlen((const char *)qhdrs[0].name);
				qhdrs[0].value = (const uint8_t *)"200";
				qhdrs[0].value_len = strlen((const char *)qhdrs[0].value);
				qhdrs[1].name = (const uint8_t *)"server";
				qhdrs[1].name_len = strlen((const char *)qhdrs[1].name);
				qhdrs[1].value = (const uint8_t *)"quiche";
				qhdrs[1].value_len = strlen((const char *)qhdrs[1].value);
				qhdrs[2].name = (const uint8_t *)"date";
				qhdrs[2].name_len = strlen((const char *)qhdrs[2].name);
				qhdrs[2].value = (const uint8_t *)get_date();
				qhdrs[2].value_len = strlen((const char *)qhdrs[2].value);
				qhdrs[3].name = (const uint8_t *)"content-length";
				qhdrs[3].name_len = strlen((const char *)qhdrs[3].name);
				qhdrs[3].value = (const uint8_t *)slen;
				qhdrs[3].value_len = strlen((const char *)qhdrs[3].value);
				qhdrs[4].name = (const uint8_t *)conn_io->fhdrs[0].name;
				qhdrs[4].name_len = conn_io->fhdrs[0].name_len;
				qhdrs[4].value = (const uint8_t *)conn_io->fhdrs[0].value;
				qhdrs[4].value_len = conn_io->fhdrs[0].value_len;
				quiche_h3_send_response(conn_io->http3, conn_io->conn, s, qhdrs, 5, false);
				if(length>0) {
					quiche_h3_send_body(conn_io->http3, conn_io->conn, s, (uint8_t *) buffer, length, true);
				}
				free(buffer);
			} else {
				quiche_h3_header qhdrs[3];
				qhdrs[0].name = (const uint8_t *)":status";
				qhdrs[0].name_len = strlen((const char *)qhdrs[0].name);
				qhdrs[0].value = (const uint8_t *)"404";
				qhdrs[0].value_len = strlen((const char *)qhdrs[0].value);
				qhdrs[1].name = (const uint8_t *)"server";
				qhdrs[1].name_len = strlen((const char *)qhdrs[1].name);
				qhdrs[1].value = (const uint8_t *)"quiche";
				qhdrs[1].value_len = strlen((const char *)qhdrs[1].value);
				qhdrs[2].name = (const uint8_t *)"date";
				qhdrs[2].name_len = strlen((const char *)qhdrs[2].name);
				qhdrs[2].value = (const uint8_t *)get_date();
				qhdrs[2].value_len = strlen((const char *)qhdrs[2].value);
				quiche_h3_send_response(conn_io->http3, conn_io->conn, s, qhdrs, 3, true);
			}
		} else {
			quiche_h3_header qhdrs[3];
			qhdrs[0].name = (const uint8_t *)":status";
			qhdrs[0].name_len = strlen((const char *)qhdrs[0].name);
			qhdrs[0].value = (const uint8_t *)"404";
			qhdrs[0].value_len = strlen((const char *)qhdrs[0].value);
			qhdrs[1].name = (const uint8_t *)"server";
			qhdrs[1].name_len = strlen((const char *)qhdrs[1].name);
			qhdrs[1].value = (const uint8_t *)"quiche";
			qhdrs[1].value_len = strlen((const char *)qhdrs[1].value);
			qhdrs[2].name = (const uint8_t *)"date";
			qhdrs[2].name_len = strlen((const char *)qhdrs[2].name);
			qhdrs[2].value = (const uint8_t *)get_date();
			qhdrs[2].value_len = strlen((const char *)qhdrs[2].value);
			printf("dasdas\n");
			quiche_h3_send_response(conn_io->http3, conn_io->conn, s, qhdrs, 3, true);
		}
	}
}

static void recv_cb(EV_P_ ev_io *w, int revents) {
    struct conn_io *tmp, *conn_io = NULL;

    static uint8_t buf[65535];
    static uint8_t out[MAX_DATAGRAM_SIZE];

    while (1) {
        struct sockaddr_storage peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        memset(&peer_addr, 0, peer_addr_len);

        ssize_t read = recvfrom(conns->sock, buf, sizeof(buf), 0,
                                (struct sockaddr *) &peer_addr,
                                &peer_addr_len);

        if (read < 0) {
            if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
                //fprintf(stderr, "recv would block\n");
                break;
            }

            perror("failed to read");
            return;
        }

        uint8_t type;
        uint32_t version;

        uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
        size_t scid_len = sizeof(scid);

        uint8_t dcid[QUICHE_MAX_CONN_ID_LEN];
        size_t dcid_len = sizeof(dcid);

        uint8_t odcid[QUICHE_MAX_CONN_ID_LEN];
        size_t odcid_len = sizeof(odcid);

        uint8_t token[MAX_TOKEN_LEN];
        size_t token_len = sizeof(token);

        int rc = quiche_header_info(buf, read, LOCAL_CONN_ID_LEN, &version,
                                    &type, scid, &scid_len, dcid, &dcid_len,
                                    token, &token_len);
        if (rc < 0) {
            fprintf(stderr, "failed to parse header: %d\n", rc);
            return;
        }

        HASH_FIND(hh, conns->h, dcid, dcid_len, conn_io);

        if (conn_io == NULL) {
            if (!quiche_version_is_supported(version)) {
                fprintf(stderr, "version negotiation\n");

                ssize_t written = quiche_negotiate_version(scid, scid_len,
                                                           dcid, dcid_len,
                                                           out, sizeof(out));

                if (written < 0) {
                    fprintf(stderr, "failed to create vneg packet: %zd\n",
                            written);
                    continue;
                }

                ssize_t sent = sendto(conns->sock, out, written, 0,
                                      (struct sockaddr *) &peer_addr,
                                      peer_addr_len);
                if (sent != written) {
                    perror("failed to send");
                    continue;
                }

                //fprintf(stderr, "sent %zd bytes\n", sent);
                continue;
            }

            if (token_len == 0) {
                //fprintf(stderr, "stateless retry\n");

                mint_token(dcid, dcid_len, &peer_addr, peer_addr_len,
                           token, &token_len);

                uint8_t new_cid[LOCAL_CONN_ID_LEN];

                if (gen_cid(new_cid, LOCAL_CONN_ID_LEN) == NULL) {
                    continue;
                }

                ssize_t written = quiche_retry(scid, scid_len,
                                               dcid, dcid_len,
                                               new_cid, LOCAL_CONN_ID_LEN,
                                               token, token_len,
                                               version, out, sizeof(out));

                if (written < 0) {
                    fprintf(stderr, "failed to create retry packet: %zd\n",
                            written);
                    continue;
                }

                ssize_t sent = sendto(conns->sock, out, written, 0,
                                      (struct sockaddr *) &peer_addr,
                                      peer_addr_len);
                if (sent != written) {
                    perror("failed to send");
                    continue;
                }

                //fprintf(stderr, "sent %zd bytes\n", sent);
                continue;
            }


            if (!validate_token(token, token_len, &peer_addr, peer_addr_len,
                               odcid, &odcid_len)) {
                fprintf(stderr, "invalid address validation token\n");
                continue;
            }

            conn_io = create_conn(dcid, dcid_len, odcid, odcid_len);
            if (conn_io == NULL) {
                continue;
            }

            memcpy(&conn_io->peer_addr, &peer_addr, peer_addr_len);
            conn_io->peer_addr_len = peer_addr_len;
        }

        ssize_t done = quiche_conn_recv(conn_io->conn, buf, read);

        if (done < 0) {
            fprintf(stderr, "failed to process packet: %zd\n", done);
            continue;
        }

        //fprintf(stderr, "recv %zd bytes\n", done);

        if (quiche_conn_is_established(conn_io->conn)) {
            quiche_h3_event *ev;

            if (conn_io->http3 == NULL) {
                conn_io->http3 = quiche_h3_conn_new_with_transport(conn_io->conn,
                                                                   http3_config);
                if (conn_io->http3 == NULL) {
                    fprintf(stderr, "failed to create HTTP/3 connection\n");
                    continue;
                }
            }

            while (1) {
                int64_t s = quiche_h3_conn_poll(conn_io->http3, conn_io->conn, &ev);

                if (s < 0) {
                    break;
                }

                switch (quiche_h3_event_type(ev)) {
                    case QUICHE_H3_EVENT_HEADERS: {
                        int rc = quiche_h3_event_for_each_header(ev, for_each_header, conn_io);
                        if (rc != 0) {
                            fprintf(stderr, "failed to process headers\n");
                        }

                        if(!quiche_h3_event_headers_has_body(ev)) {
                        	ffead_handle(conn_io, s, buf, 0, 1);
                        }
                        break;
                    }

                    case QUICHE_H3_EVENT_DATA: {
                    	ssize_t len = quiche_h3_recv_body(conn_io->http3, conn_io->conn, s, buf, sizeof(buf));
						if (len <= 0) {
							break;
						}

						ffead_handle(conn_io, s, buf, len, 0);
						break;
                    }

                    case QUICHE_H3_EVENT_FINISHED:
                        break;

                    case QUICHE_H3_EVENT_DATAGRAM:
                        break;

                    case QUICHE_H3_EVENT_GOAWAY: {
                        fprintf(stderr, "got GOAWAY\n");
                        break;
                    }
                }

                quiche_h3_event_free(ev);
            }
        }
    }

    HASH_ITER(hh, conns->h, conn_io, tmp) {
        flush_egress(loop, conn_io);

        if (quiche_conn_is_closed(conn_io->conn)) {
            quiche_stats stats;

            quiche_conn_stats(conn_io->conn, &stats);
            fprintf(stderr, "connection closed, recv=%zu sent=%zu lost=%zu rtt=%" PRIu64 "ns cwnd=%zu\n",
                    stats.recv, stats.sent, stats.lost, stats.rtt, stats.cwnd);

            HASH_DELETE(hh, conns->h, conn_io);

            ev_timer_stop(loop, &conn_io->timer);

            quiche_conn_free(conn_io->conn);
            free(conn_io);
        }
    }
}

static void timeout_cb(EV_P_ ev_timer *w, int revents) {
    struct conn_io *conn_io = w->data;
    quiche_conn_on_timeout(conn_io->conn);

    //fprintf(stderr, "timeout\n");

    flush_egress(loop, conn_io);

    if (quiche_conn_is_closed(conn_io->conn)) {
        quiche_stats stats;

        quiche_conn_stats(conn_io->conn, &stats);
        //fprintf(stderr, "connection closed, recv=%zu sent=%zu lost=%zu rtt=%" PRIu64 "ns cwnd=%zu\n",
        //        stats.recv, stats.sent, stats.lost, stats.rtt, stats.cwnd);

        HASH_DELETE(hh, conns->h, conn_io);

        ev_timer_stop(loop, &conn_io->timer);
        quiche_conn_free(conn_io->conn);
        free(conn_io);

        return;
    }
}

int main(int argc, char *argv[]) {
    const char *host = argv[1];
    const char *port = argv[2];

    const struct addrinfo hints = {
        .ai_family = PF_UNSPEC,
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = IPPROTO_UDP
    };

    //quiche_enable_debug_logging(debug_log, NULL);

    struct addrinfo *local;
    if (getaddrinfo(host, port, &hints, &local) != 0) {
        perror("failed to resolve host");
        return -1;
    }

    int sock = socket(local->ai_family, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("failed to create socket");
        return -1;
    }

    if (fcntl(sock, F_SETFL, O_NONBLOCK) != 0) {
        perror("failed to make socket non-blocking");
        return -1;
    }

    if (bind(sock, local->ai_addr, local->ai_addrlen) < 0) {
        perror("failed to connect socket");
        return -1;
    }

    config = quiche_config_new(QUICHE_PROTOCOL_VERSION);
    if (config == NULL) {
        fprintf(stderr, "failed to create config\n");
        return -1;
    }

    quiche_config_load_cert_chain_from_pem_file(config, "./cert.crt");
    quiche_config_load_priv_key_from_pem_file(config, "./cert.key");

    quiche_config_set_application_protos(config,
        (uint8_t *) QUICHE_H3_APPLICATION_PROTOCOL,
        sizeof(QUICHE_H3_APPLICATION_PROTOCOL) - 1);

    quiche_config_set_max_idle_timeout(config, 5000);
    quiche_config_set_max_recv_udp_payload_size(config, MAX_DATAGRAM_SIZE);
    quiche_config_set_max_send_udp_payload_size(config, MAX_DATAGRAM_SIZE);
    quiche_config_set_initial_max_data(config, 10000000);
    quiche_config_set_initial_max_stream_data_bidi_local(config, 1000000);
    quiche_config_set_initial_max_stream_data_bidi_remote(config, 1000000);
    quiche_config_set_initial_max_stream_data_uni(config, 1000000);
    quiche_config_set_initial_max_streams_bidi(config, 100);
    quiche_config_set_initial_max_streams_uni(config, 100);
    quiche_config_set_disable_active_migration(config, true);
    quiche_config_set_cc_algorithm(config, QUICHE_CC_RENO);

    http3_config = quiche_h3_config_new();
    if (http3_config == NULL) {
        fprintf(stderr, "failed to create HTTP/3 config\n");
        return -1;
    }

    printf("Bootstrapping ffead-cpp start...\n");
	ffead_cpp_bootstrap(argv[3], strlen(argv[3]), 7);
	printf("Bootstrapping ffead-cpp end...\n");

	printf("Initializing ffead-cpp start...\n");
	ffead_cpp_init();
	printf("Initializing ffead-cpp end...\n");

    struct connections c;
    c.sock = sock;
    c.h = NULL;

    conns = &c;

    ev_io watcher;

    struct ev_loop *loop = ev_default_loop(0);

    ev_io_init(&watcher, recv_cb, sock, EV_READ);
    ev_io_start(loop, &watcher);
    watcher.data = &c;

    ev_loop(loop, 0);

    freeaddrinfo(local);

    quiche_h3_config_free(http3_config);

    quiche_config_free(config);

    printf("Cleaning up ffead-cpp start...\n");
	ffead_cpp_cleanup();
	printf("Cleaning up ffead-cpp end...\n");

    return 0;
}
