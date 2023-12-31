/*
	Copyright 2009-2020, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (const the& "License"); 
    you may not use this file except in compliance with the License. 
    You may obtain a copy of the License at 
  
        http://www.apache.org/licenses/LICENSE-2.0 
  
    Unless required by applicable law or agreed to in writing, software 
    distributed under the License is distributed on an "AS IS" BASIS, 
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
    See the License for the specific language governing permissions and 
    limitations under the License.  
*/

#ifndef c_interface_h
#define c_interface_h

#include "ServerInitUtil.h"

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
typedef struct ffead_request3 {
    const char* server_str;
    size_t server_str_len;
    const char* method;
    size_t method_len;
    const char* path;
    size_t path_len;
    const char* qstr;
    size_t qstr_len;
    phr_header_fcp* headers;
    size_t headers_len;
    const char* body;
    size_t body_len;
    int version;
    int fd;
    void* writer;
}ffead_request3;
typedef struct ffead_request1 {
    const char* server_str;
    size_t server_str_len;
    const char* method;
    size_t method_len;
    const char* path;
    size_t path_len;
    const char* headers;
    size_t headers_len;
    const char* body;
    size_t body_len;
    int version;
}ffead_request1;
typedef struct ffead_request2 {
    const char* server_str;
    size_t server_str_len;
    const char* method;
    size_t method_len;
    const char* path;
    size_t path_len;
    const char* qstr;
    size_t qstr_len;
    const char* headers;
    size_t headers_len;
    const char* body;
    size_t body_len;
    int version;
}ffead_request2;
typedef struct ffead_response {
    const char* out_url;
    size_t out_url_len;
    const char* out_mime;
    size_t out_mime_len;
    phr_header_fcp headers[100];
    size_t headers_len;
    const char* body;
    size_t body_len;
    int scode;
    const char* statmsg;
    size_t statmsg_len;
    int done;
    const char* headers_str;
    size_t headers_str_len;
    const char* headers_ss_str;
    size_t headers_ss_str_len;
    const char* resp_str;
    size_t resp_str_len;
}ffead_response;

/*
    Bootstrap the ffead-cpp server framework
*/
extern "C" void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);

/*
    Initialize the ffead-cpp server framework
*/
extern "C" void ffead_cpp_init();
extern "C" void ffead_cpp_init_for_pv(cb_reg_ext_fd_pv pvregfd, cb_into_pv cb, cb_into_pv_for_date cdt);
extern "C" bool ffead_cpp_is_inited();

/*
    Cleanup the ffead-cpp server framework
*/
extern "C" void ffead_cpp_cleanup();

/*
    Rust interface support functions
*/
extern "C" void* ffead_cpp_handle_rust_1(const ffead_request *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len);
extern "C" void* ffead_cpp_handle_rust_swift_1(const ffead_request *request, int* scode,
    const char **out_url, size_t *out_url_len, const char **out_url_mime, size_t *out_url_mime_len,
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len);

/*
    C interface support functions
*/
extern "C" void* ffead_cpp_handle_c_1(const ffead_request *request, int* scode, const char** smsg, size_t *smsg_len,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len);

/*
    Crystal interface support functions
*/
extern "C" void* ffead_cpp_handle_picov_1(const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
	const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len,
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len);
extern "C" void ffead_cpp_handle_picov_2(const ffead_request3 *request);
extern "C" void* ffead_cpp_handle_picov_2_init_sock(int fd, void* pv);
extern "C" void ffead_cpp_handle_picov_2_deinit_sock(int fd, void* data);
extern "C" void ffead_cpp_handle_picov_clean_sockets();
extern "C" void ffead_cpp_handle_picov_ext_fd_cb(int fd, void* data);
extern "C" void* ffead_cpp_handle_crystal_js_1(const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
	const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len, 
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len);

/*
    Go interface support functions
*/
extern "C" void* ffead_cpp_handle_go_1(const char *server_str, size_t server_str_len,
	const char *method, size_t method_len, const char *path, size_t path_len, const char *query, size_t query_len, int version,
	const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
	const char **out_url, size_t *out_url_len,  const char **out_mime, size_t *out_mime_len,
	phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
);
extern "C" void* ffead_cpp_handle_go_2(const char *server_str, size_t server_str_len,
	const char *method, size_t method_len, const char *path, size_t path_len, int version,
	const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
	const char **out_url, size_t *out_url_len,  const char **out_mime, size_t *out_mime_len,
	const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
);

/*
    V interface support interface
*/
extern "C" void* ffead_cpp_handle_v(const char *server_str, size_t server_str_len,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len, 
	const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
);

/*
    Java interface support interface
*/
extern "C" void* ffead_cpp_handle_java(int *scode, const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
		const char **out_body, size_t *out_body_len, const char **out_headers, size_t *out_headers_len,
		const char *server_str, size_t server_str_len, const char* method, size_t method_len,
		const char* path, size_t path_len, const char* body, size_t body_len, int version,
		int headers_len, ...
);

extern "C" void* ffead_cpp_handle_d_1(const ffead_request *request, int* scode,
	    const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
		const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len);


extern "C" void* ffead_cpp_handle_1t(const ffead_request2 *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len);

extern "C" void* ffead_cpp_handle_2(const ffead_request1 *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len);


/*
    Handle a request and return the HttpResponse pointer, along-with, if with_status_line is true, then the header string
    also has the http response status line as well,
    server_str - is the calling server's name which will be passed in the Server header
    Return:
        - done flag, 1 - handled by framework, 0 - static file (needs to be handled by calling server)
        - static file path if requested
        - http status code as int
        - http status message
        - response headers as a string
        - response body as a string
*/
extern "C" void* ffead_cpp_handle_3(const char *server_str, size_t server_str_len,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, bool with_status_line, int* scode, const char** smsg, size_t *smsg_len, 
    const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
);

/*
    Handle a request and return the HttpResponse pointer, along-with any static file path if requested
*/
extern "C" void* ffead_cpp_handle_5(const char *in_headers, size_t in_headers_len,
    const char *in_body, size_t in_body_len, int* done, const char **out_url, size_t *out_url_len
);

/*
    Get the done status: 1 - handled by framework, 0 - static file (needs to be handled by calling server)
*/
extern "C" int ffead_cpp_get_resp_done(void* ptr);

/*
    Get the http response status code
*/
extern "C" int ffead_cpp_get_resp_code(void* ptr);

/*
    Get the http response status message
*/
extern "C" int ffead_cpp_get_resp_status(void* ptr, const char** smsg, size_t *smsg_len);

/*
    Get the http response headers as a string without the http status line 
    server_str - is the calling server's name which will be passed in the Server header
*/
extern "C" void ffead_cpp_get_resp_header_str(void* ptr, const char **out_headers, size_t *out_headers_len, 
    const char *server_str, size_t server_str_len);

/*
    Get the http response headers as a string with the http status line as well
    server_str - is the calling server's name which will be passed in the Server header
*/
extern "C" void ffead_cpp_get_resp_header_str_with_statusline(void* ptr, const char **out_headers, size_t *out_headers_len, 
    const char *server_str, size_t server_str_len);

/*
    Get the response body as a string
*/
extern "C" void ffead_cpp_get_resp_get_body(void* ptr, const char **out_body, size_t *out_body_len);

/*
    Needs to be called necessarily after the ffead_cpp_handle_* phase
    The response object returned from the ffead_cpp_handle_* function needs to be cleaned up eventually
    after all other required functions have been called
*/
extern "C" void ffead_cpp_resp_cleanup(void* ptr);

extern "C" void* ffead_cpp_handle_js_1(const ffead_request *request, int* scode, size_t *out_url_len, size_t *out_headers_len, size_t *out_body_len);
extern "C" const char* ffead_cpp_handle_js_out_url(void* res);
extern "C" const char* ffead_cpp_handle_js_out_body(void* res);
extern "C" const char* ffead_cpp_handle_js_out_hdr_name(void* res, int pos, size_t* name_len);
extern "C" const char* ffead_cpp_handle_js_out_hdr_value(void* res, int pos, size_t* value_len);

#endif
