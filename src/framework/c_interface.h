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

/*
    Bootstrap the ffead-cpp server framework
*/
extern "C" void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);

/*
    Initialize the ffead-cpp server framework
*/
extern "C" void ffead_cpp_init();

/*
    Cleanup the ffead-cpp server framework
*/
extern "C" void ffead_cpp_cleanup();

/*
    Handle a request and return the HttpResponse pointer, along-with any static file path if requested
*/
extern "C" void* ffead_cpp_handle_request(const char *in_headers, size_t in_headers_len, 
    const char *in_body, size_t in_body_len, int* done, const char **out_url, size_t *out_url_len
);

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
extern "C" void* ffead_cpp_handle_return_hdrs_body(const char *server_str, size_t server_str_len, 
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, bool with_status_line, int* scode, const char** smsg, size_t *smsg_len, 
    const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
);

/*
    Handle a request and return the HttpResponse pointer, along-with, 
    server_str - is the calling server's name which will be passed in the Server header
    Return:
        - done flag, 1 - handled by framework, 0 - static file (needs to be handled by calling server)
        - static file path if requested
        - response headers as a string with the http status line as well
        - response body as a string
*/
extern "C" void* ffead_cpp_handle_return_all(const char *server_str, size_t server_str_len, 
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, const char **out_headers, size_t *out_headers_len, 
    const char **out_body, size_t *out_body_len
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
extern "C" void ffead_cpp_get_resp_cleanup(void* ptr);

#endif