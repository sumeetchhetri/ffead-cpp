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
#include <stddef.h>
#include <string.h>

typedef int cb_into_pv_for_date();
typedef int cb_into_pv(const char* hline, size_t hline_len, const char* body, size_t body_len, int fd, void* pv);
typedef void cb_reg_ext_fd_pv(int fd, void* pv);

typedef struct ffead_request3 {
    const char* server_str;
    size_t server_str_len;
    const char* method;
    size_t method_len;
    const char* path;
    size_t path_len;
    const char* qstr;
    size_t qstr_len;
    void* headers;
    size_t headers_len;
    const char* body;
    size_t body_len;
    int version;
    int fd;
    void* writer;
}ffead_request3;

typedef struct ffead_request3 ffead_request3_t;

static inline void* fc_memcpy(void* ptr, char* c, size_t len) {
	memcpy(ptr, (const void *)c, len);
	ptr += len;
	return ptr;
}
static inline const char* get_date_2() {
	time_t t;
	struct tm tm;
	static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	static __thread char date[52] = "Date: Thu, 01 Jan 1970 00:00:00 GMT\r\nServer: fcpv\r\n";

	time(&t);
	gmtime_r(&t, &tm);
	strftime(date, 51, "Date: ---, %d --- %Y %H:%M:%S GMT\r\nServer: fcpv\r\n", &tm);
	memcpy(date + 6, days[tm.tm_wday], 3);
	memcpy(date + 14, months[tm.tm_mon], 3);

	return date;
}
extern void ffead_cpp_resp_cleanup(void* ptr);
extern void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);
extern void ffead_cpp_init();
extern void ffead_cpp_init_for_pv(cb_reg_ext_fd_pv pvregfd, cb_into_pv cb, cb_into_pv_for_date cdt);
extern int ffead_cpp_is_inited();
extern void ffead_cpp_cleanup();
extern void* ffead_cpp_handle_picov_1(const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
    const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len,
    phr_header_t *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len);
extern void ffead_cpp_handle_picov_2(const ffead_request3 *request);
extern void* ffead_cpp_handle_picov_2_init_sock(int fd, void* pv);
extern void ffead_cpp_handle_picov_2_deinit_sock(int fd, void* data);
extern void ffead_cpp_handle_picov_ext_fd_cb(int fd, void* data);
extern void ffead_cpp_handle_picov_clean_sockets();
