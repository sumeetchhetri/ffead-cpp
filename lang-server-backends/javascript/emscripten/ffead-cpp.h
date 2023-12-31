/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "string"

struct phr_header_fcp {
	const char *name;
	size_t name_len;
	const char *value;
	size_t value_len;
};

struct ffead_request {
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
};

struct http_header {
	std::string key;
	std::string value;
};

struct http_req {
	std::string meth;
	std::string path;
	std::string qstr;
	int version;
	std::string body;
	http_header headers[20];
	int num_headers;
};

struct http_res {
	std::string out_url;
	std::string out_mine_type;
	int scode;
	std::string body;
	http_header headers[20];
	int num_headers;
};

extern void ffead_cpp_resp_cleanup(void* ptr);
extern void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);
extern void ffead_cpp_init();
extern bool ffead_cpp_is_inited();
extern void ffead_cpp_cleanup();
extern void* ffead_cpp_handle_crystal_js_1(const ffead_request *request, int* scode, const char** smsg, size_t *smsg_len,
		const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len,
	    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len);
