#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

typedef struct phr_header_fcp {
	const char *name;
	size_t name_len;
	const char *value;
	size_t value_len;
}phr_header_fcp;

typedef struct ffead_request {
	const char* server_str;
	size_t server_str_len;
	const char* method;
	size_t method_len;
	const char* path;
	size_t path_len;
	phr_header_fcp* headers;
	size_t headers_len;
	const char* body;
	size_t body_len;
	int version;
}ffead_request;

extern void ffead_cpp_resp_cleanup(void* ptr);
extern void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);
extern void ffead_cpp_init();
extern void ffead_cpp_cleanup();
extern void* ffead_cpp_handle_c_1(const ffead_request *request, int* scode, const char** smsg, size_t *smsg_len,
		const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
		const char **out_body, size_t *out_body_len);
