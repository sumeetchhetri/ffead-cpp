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
#include <time.h>

extern const char* get_date();
extern void ffead_cpp_resp_cleanup(void* ptr);
extern void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);
extern void ffead_cpp_init();
extern void ffead_cpp_cleanup();
extern  void* ffead_cpp_handle_v(const char *server_str, size_t server_str_len,
	const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
	const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
	const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
);
