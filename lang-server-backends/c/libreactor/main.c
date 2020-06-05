#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include "ffead-cpp.h"

#include <dynamic.h>
#include <reactor.h>

#include "setup.h"

static reactor_status handle(reactor_event *event)
{
	reactor_server_session *session = (reactor_server_session *) event->data;

	ffead_request freq;
	freq.server_str = "libreactor";
	freq.server_str_len = 10;
	freq.method = (const char*)session->request->method.base;
	freq.method_len = session->request->method.size;
	freq.path = (const char*)session->request->target.base;
	freq.path_len = session->request->target.size;
	freq.body_len = 0;
	if(session->request->body.size>0) {
		freq.body = (const char*)session->request->body.base;
		freq.body_len = session->request->body.size;
	}
	freq.version = 1;
	freq.headers_len = session->request->headers.count;

	phr_header_fcp f_headers[session->request->headers.count];
	for(int i=0;i<(int)session->request->headers.count;i++) {
		f_headers[i].name = (const char*)session->request->headers.header[i].name.base;
		f_headers[i].name_len = session->request->headers.header[i].name.size;
		f_headers[i].value = (const char*)session->request->headers.header[i].value.base;
		f_headers[i].value_len = session->request->headers.header[i].value.size;
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

	reactor_http_response response;
	response.version = session->request->version;
	if(scode>0) {
		for(int i=0;i<(int)out_headers_len;i++) {
			response.headers.header[i].name.base = (void*)out_headers[i].name;
			response.headers.header[i].name.size = out_headers[i].name_len;
			response.headers.header[i].value.base = (void*)out_headers[i].value;
			response.headers.header[i].value.size = out_headers[i].value_len;
		}
		response.headers.header[out_headers_len].name = reactor_vector_string("Date");
		response.headers.header[out_headers_len].value = reactor_http_message_date(NULL);
		response.headers.count = out_headers_len;
		response.code = scode;
		response.reason.base = (void*)smsg;
		response.reason.size = smsg_len;
		if(out_body_len>0) {
			response.body.base = (void*)out_body;
			response.body.size = out_body_len;
		}
		reactor_server_respond(session, &response);
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
				snprintf(slen, 9, "%d", length);
				response.body.base = (void*)buffer;
				response.body.size = (size_t)length;
				response.headers.count = 2;
				response.headers.header[0].name.base = (void*)"Content-Length";
				response.headers.header[0].name.size = 14;
				response.headers.header[0].value.base = slen;
				response.headers.header[0].value.size = strlen(slen);
				response.headers.header[1].name.base = (void*)out_headers[0].name;
				response.headers.header[1].name.size = out_headers[0].name_len;
				response.headers.header[1].value.base = (void*)out_headers[0].value;
				response.headers.header[1].value.size = out_headers[0].value_len;
				response.headers.header[2].name.base = (void*)out_headers[1].name;
				response.headers.header[2].name.size = out_headers[1].name_len;
				response.headers.header[2].value.base = (void*)out_headers[1].value;
				response.headers.header[2].value.size = out_headers[1].value_len;
				response.headers.header[3].name = reactor_vector_string("Date");
				response.headers.header[3].value = reactor_http_message_date(NULL);
				reactor_server_respond(session, &response);
			} else {
				reactor_server_not_found(session);
			}
		} else {
			reactor_server_not_found(session);
		}
	}
	ffead_cpp_resp_cleanup(fres);
	return REACTOR_OK;
}


int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		//cout << "No Server root directory specified, quitting..." << std::endl;
		return 0;
	}

	printf("Bootstrapping ffead-cpp start...\n");
	ffead_cpp_bootstrap(argv[1], strlen(argv[1]), 13);
	printf("Bootstrapping ffead-cpp end...\n");

	printf("Initializing ffead-cpp start...\n");
	ffead_cpp_init();
	printf("Initializing ffead-cpp end...\n");

	reactor_server server;
	setup();
	reactor_construct();
	reactor_server_construct(&server, NULL, NULL);
	reactor_server_route(&server, handle, NULL);
	(void) reactor_server_open(&server, "0.0.0.0", argv[2]);
	printf("libreactor listening on port %s\n", argv[2]);
	reactor_run();
	reactor_destruct();

	printf("Cleaning up ffead-cpp start...\n");
	ffead_cpp_cleanup();
	printf("Cleaning up ffead-cpp end...\n");
}
