/*
 * Copyright (c) 2014 DeNA Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "h2o.h"
#include "h2o/http1.h"
#include "h2o/http2.h"
#include "h2o/memcached.h"
#include "ffead-cpp.h"

#define USE_HTTPS 1
#define USE_MEMCACHED 0

static h2o_pathconf_t *register_handler(h2o_hostconf_t *hostconf, const char *path, int (*on_req)(h2o_handler_t *, h2o_req_t *))
{
    h2o_pathconf_t *pathconf = h2o_config_register_path(hostconf, path, 0);
    h2o_handler_t *handler = h2o_create_handler(pathconf, sizeof(*handler));
    handler->on_req = on_req;
    return pathconf;
}

static int ffeadcpp_hanlder(h2o_handler_t *self, h2o_req_t *request)
{
	static h2o_generator_t generator = {NULL, NULL};

	ffead_request freq;
	freq.server_str = "libh2o";
	freq.server_str_len = 6;
	freq.method = (const char*)request->method.base;
	freq.method_len = request->method.len;
	freq.path = (const char*)request->path.base;
	freq.path_len = request->path.len;
	freq.body_len = 0;
	if(request->entity.len>0) {
		freq.body = (const char*)request->entity.base;
		freq.body_len = request->entity.len;
	}
	freq.version = request->version;
	freq.headers_len = request->headers.size;

	phr_header_fcp f_headers[request->headers.size];
	for(int i=0;i<(int)request->headers.size;i++) {
		f_headers[i].name = (const char*)request->headers.entries[i].name->base;
		f_headers[i].name_len = request->headers.entries[i].name->len;
		f_headers[i].value = (const char*)request->headers.entries[i].value.base;
		f_headers[i].value_len = request->headers.entries[i].value.len;
	}
	freq.headers = f_headers;

    int scode = 0;
	const char* out_url;
	size_t out_url_len;
	const char* out_body;
	size_t out_body_len;
	phr_header_fcp out_headers[100];
	size_t out_headers_len;
	const char* smsg;
	size_t smsg_len;

	void* fres = ffead_cpp_handle_c_1(&freq, &scode, &smsg, &smsg_len, &out_url, &out_url_len, out_headers, &out_headers_len, &out_body, &out_body_len);
	if(scode>0) {
		for(int i=0;i<(int)out_headers_len;i++) {
			h2o_set_header_by_str(&request->pool, &request->res.headers, out_headers[i].name, out_headers[i].name_len, 1, out_headers[i].value, out_headers[i].value_len, 1);
		}
		request->res.status = scode;
		request->res.reason = smsg;
		if(out_body_len>0) {
			request->entity.base = (void*)out_body;
			request->entity.len = out_body_len;
		}
		h2o_start_response(request, &generator);
		h2o_send(request, &request->entity, 1, 1);
	} else {
		if(out_url_len>0 && access(out_url, F_OK) != -1 ) {
			h2o_file_send(request, 200, "OK", out_url, h2o_iovec_init(out_url, out_url_len), 0);
		} else {
			h2o_send_error_404(request, "Not Found", "not found", 0);
		}
	}

	ffead_cpp_resp_cleanup(fres);
    return -1;
}

static h2o_globalconf_t config;
static h2o_context_t ctx;
static h2o_multithread_receiver_t libmemcached_receiver;
static h2o_accept_ctx_t accept_ctx;

#if H2O_USE_LIBUV

static void on_accept(uv_stream_t *listener, int status)
{
    uv_tcp_t *conn;
    h2o_socket_t *sock;

    if (status != 0)
        return;

    conn = h2o_mem_alloc(sizeof(*conn));
    uv_tcp_init(listener->loop, conn);

    if (uv_accept(listener, (uv_stream_t *)conn) != 0) {
        uv_close((uv_handle_t *)conn, (uv_close_cb)free);
        return;
    }

    sock = h2o_uv_socket_create((uv_handle_t *)conn, (uv_close_cb)free);
    h2o_accept(&accept_ctx, sock);
}

static int create_listener(char* ip_addr, int port)
{
    static uv_tcp_t listener;
    struct sockaddr_in addr;
    int r;

    uv_tcp_init(ctx.loop, &listener);
    uv_ip4_addr(ip_addr, port, &addr);
    if ((r = uv_tcp_bind(&listener, (struct sockaddr *)&addr, 0)) != 0) {
        fprintf(stderr, "uv_tcp_bind:%s\n", uv_strerror(r));
        goto Error;
    }
    if ((r = uv_listen((uv_stream_t *)&listener, 128, on_accept)) != 0) {
        fprintf(stderr, "uv_listen:%s\n", uv_strerror(r));
        goto Error;
    }

    return 0;
Error:
    uv_close((uv_handle_t *)&listener, NULL);
    return r;
}

#else

static void on_accept(h2o_socket_t *listener, const char *err)
{
    h2o_socket_t *sock;

    if (err != NULL) {
        return;
    }

    if ((sock = h2o_evloop_socket_accept(listener)) == NULL)
        return;
    h2o_accept(&accept_ctx, sock);
}

static int create_listener(char* ip_addr, int port)
{
    struct sockaddr_in addr;
    int fd, reuseaddr_flag = 1;
    h2o_socket_t *sock;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    addr.sin_port = htons(port);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ||
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_flag, sizeof(reuseaddr_flag)) != 0 ||
        bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(fd, SOMAXCONN) != 0) {
        return -1;
    }

    sock = h2o_evloop_socket_create(ctx.loop, fd, H2O_SOCKET_FLAG_DONT_READ);
    h2o_socket_read_start(sock, on_accept);

    return 0;
}

#endif

static int setup_ssl(const char *cert_file, const char *key_file, const char *ciphers, const char* mcip, int mcport)
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    accept_ctx.ssl_ctx = SSL_CTX_new(SSLv23_server_method());
    SSL_CTX_set_options(accept_ctx.ssl_ctx, SSL_OP_NO_SSLv2);

    if (USE_MEMCACHED) {
        accept_ctx.libmemcached_receiver = &libmemcached_receiver;
        h2o_accept_setup_memcached_ssl_resumption(h2o_memcached_create_context(mcip, mcport, 0, 1, "h2o:ssl-resumption:"), 86400);
        h2o_socket_ssl_async_resumption_setup_ctx(accept_ctx.ssl_ctx);
    }

#ifdef SSL_CTX_set_ecdh_auto
    SSL_CTX_set_ecdh_auto(accept_ctx.ssl_ctx, 1);
#endif

    /* load certificate and private key */
    if (SSL_CTX_use_certificate_chain_file(accept_ctx.ssl_ctx, cert_file) != 1) {
        fprintf(stderr, "an error occurred while trying to load server certificate file:%s\n", cert_file);
        return -1;
    }
    if (SSL_CTX_use_PrivateKey_file(accept_ctx.ssl_ctx, key_file, SSL_FILETYPE_PEM) != 1) {
        fprintf(stderr, "an error occurred while trying to load private key file:%s\n", key_file);
        return -1;
    }

    if (SSL_CTX_set_cipher_list(accept_ctx.ssl_ctx, ciphers) != 1) {
        fprintf(stderr, "ciphers could not be set: %s\n", ciphers);
        return -1;
    }

/* setup protocol negotiation methods */
#if H2O_USE_NPN
    h2o_ssl_register_npn_protocols(accept_ctx.ssl_ctx, h2o_http2_npn_protocols);
#endif
#if H2O_USE_ALPN
    h2o_ssl_register_alpn_protocols(accept_ctx.ssl_ctx, h2o_http2_alpn_protocols);
#endif

    return 0;
}

int main(int argc, char **argv)
{
	const char* sdir = argv[1];
	const char* ip_addr = argv[2];
	int port = atoi(argv[3]);

	const char* scert = NULL;
	const char* skey = NULL;
	const char* ciphers = NULL;

	if(argc>4) {
		scert = argv[4];
		skey = argv[5];
		ciphers = argv[6];
	}

	const char* mcip_addr = NULL;
	int mcport = -1;
	if(argc>7) {
		mcip_addr = argv[7];
		mcport = atoi(argv[8]);
	}

    h2o_hostconf_t *hostconf;
    h2o_access_log_filehandle_t *logfh = h2o_access_log_open_handle("/dev/stdout", NULL, H2O_LOGCONF_ESCAPE_APACHE);
    h2o_pathconf_t *pathconf;

    signal(SIGPIPE, SIG_IGN);

    h2o_config_init(&config);
    hostconf = h2o_config_register_host(&config, h2o_iovec_init(H2O_STRLIT("default")), 65535);

    pathconf = register_handler(hostconf, "/", ffeadcpp_hanlder);
    if (logfh != NULL)
        h2o_access_log_register(pathconf, logfh);

#if H2O_USE_LIBUV
    uv_loop_t loop;
    uv_loop_init(&loop);
    h2o_context_init(&ctx, &loop, &config);
#else
    h2o_context_init(&ctx, h2o_evloop_create(), &config);
#endif
    if (USE_MEMCACHED)
        h2o_multithread_register_receiver(ctx.queue, &libmemcached_receiver, h2o_memcached_receiver);

    if (USE_HTTPS && setup_ssl(scert, skey, ciphers, mcip_addr, mcport) != 0)
        goto Error;


    printf("Bootstrapping ffead-cpp start...\n");
	ffead_cpp_bootstrap(sdir, strlen(sdir), 8);
	printf("Bootstrapping ffead-cpp end...\n");

	printf("Initializing ffead-cpp start...\n");
	ffead_cpp_init();
	printf("Initializing ffead-cpp end...\n");

    accept_ctx.ctx = &ctx;
    accept_ctx.hosts = config.hosts;

    if (create_listener(ip_addr, port) != 0) {
        fprintf(stderr, "failed to listen to %s:%d:%s\n", ip_addr, port, strerror(errno));
        goto Error;
    }

#if H2O_USE_LIBUV
    uv_run(ctx.loop, UV_RUN_DEFAULT);
#else
    while (h2o_evloop_run(ctx.loop, INT32_MAX) == 0)
        ;
#endif

    printf("Cleaning up ffead-cpp start...\n");
	ffead_cpp_cleanup();
	printf("Cleaning up ffead-cpp end...\n");

Error:
	printf("Cleaning up ffead-cpp start...\n");
	ffead_cpp_cleanup();
	printf("Cleaning up ffead-cpp end...\n");
    return 1;
}
