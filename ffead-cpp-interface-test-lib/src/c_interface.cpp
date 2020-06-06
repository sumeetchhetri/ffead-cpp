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

#include "c_interface.h"

void printStr(const char* nm, const char* str, size_t str_len) {
	printf("%s", nm);
	char *p = (char*)str;
	for (int r = 0; r < (int)str_len; ++r) {
		printf("%c", *p++);
	}
	printf("\n");
}

void printStr1(const char* str, size_t str_len, bool nl) {
	char *p = (char*)str;
	for (int r = 0; r < (int)str_len; ++r) {
		printf("%c", *p++);
	}
	if(nl) printf("\n");
}

void printHdr(phr_header_fcp h) {
	printf("Header = {");
	printStr1(h.name, h.name_len, false);
	printf(": ");
	printStr1(h.value, h.value_len, false);
	printf("}\n");
}

void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type) {
	printf("dummy ffead_cpp_bootstrap ");
	printStr1(srv, srv_len, true);
	fflush(stdout);
}

void ffead_cpp_init() {
	printf("dummy ffead_cpp_init\n");
	fflush(stdout);
}

void ffead_cpp_cleanup() {
	printf("dummy ffead_cpp_cleanup\n");
	fflush(stdout);
}

ffead_response* get_resp(int scode, phr_header_fcp *out_headers) {
	ffead_response* resp = new ffead_response();
	if(scode==1) {
		resp->scode = 200;
		resp->statmsg = "OK";
		resp->statmsg_len = strlen(resp->statmsg);
		resp->done = 200;
		resp->out_url_len = 0;
		resp->headers_str = "Content-Type: application/json\r\nContent-Length: 27\r\n";
		resp->headers_str_len = strlen(resp->headers_str);
		resp->headers_ss_str = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 27\r\n";
		resp->headers_ss_str_len = strlen(resp->headers_ss_str);
		resp->resp_str = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 27\r\n\r\n{\"message\": \"Hello World!\"}";
		resp->resp_str_len = strlen(resp->resp_str);
		if(out_headers!=NULL) {
			out_headers[0].name = "Content-Type";
			out_headers[0].name_len = strlen(out_headers[0].name);
			out_headers[0].value = "application/json";
			out_headers[0].value_len = strlen(out_headers[0].value);
			out_headers[1].name = "Content-Length";
			out_headers[1].name_len = strlen(out_headers[1].name);
			out_headers[1].value = "27";
			out_headers[1].value_len = strlen(out_headers[1].value);
		}
		resp->headers_len = 2;
		resp->body = "{\"message\": \"Hello World!\"}";
		resp->body_len = strlen(resp->body);
		resp->out_url_len = 0;
		resp->out_mime_len = 0;
	} else {
		resp->scode = 0;
		resp->done = 0;
		resp->out_url = "/Users/sumeetc/Projects/GitHub/ffead-cpp/web/default/public/index.html";
		resp->out_url_len = strlen(resp->out_url);
		resp->out_mime = "text/html";
		resp->out_mime_len = strlen(resp->out_mime);
		resp->headers_len = 0;
		resp->body_len = 0;
	}
	return resp;
}

void* ffead_cpp_handle_go(const char *server_str, size_t server_str_len,
	const char *method, size_t method_len, const char *path, size_t path_len, const char *query, size_t query_len, int version,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
    const char **out_url, size_t *out_url_len,  const char **out_mime, size_t *out_mime_len,
	phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
	printStr("Server = ", server_str, server_str_len);
	printStr("Method = ", method, method_len);
	printStr("Path = ", path, path_len);
	printStr("QueryStr = ", query, query_len);
	printf("Version = %d\n", version);
	printStr("HeaderStr = ", in_headers, in_headers_len);
	printStr("Body = ", in_body, in_body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, out_headers);
	*scode = resp->scode;
	*out_headers_len = resp->headers_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	*out_mime = resp->out_mime;
	*out_mime_len = resp->out_mime_len;
	return resp;
}

void* ffead_cpp_handle_go1(const char *server_str, size_t server_str_len,
	const char *method, size_t method_len, const char *path, size_t path_len, const char *query, size_t query_len, int version,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
    const char **out_url, size_t *out_url_len,  const char **out_mime, size_t *out_mime_len,
	const char **out_body, size_t *out_body_len
)
{
	printStr("Server = ", server_str, server_str_len);
	printStr("Method = ", method, method_len);
	printStr("Path = ", path, path_len);
	printStr("QueryStr = ", query, query_len);
	printf("Version = %d\n", version);
	printStr("HeaderStr = ", in_headers, in_headers_len);
	printStr("Body = ", in_body, in_body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, NULL);
	*scode = resp->scode;
	*out_body = resp->resp_str;
	*out_body_len = resp->resp_str_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	*out_mime = resp->out_mime;
	*out_mime_len = resp->out_mime_len;
	return resp;
}

void* ffead_cpp_handle_1(const ffead_request *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len
)
{
	printStr("Server = ", request->server_str, request->server_str_len);
	printStr("Method = ", request->method, request->method_len);
	printStr("Path = ", request->path, request->path_len);
	printf("Version = %d\n", request->version);
	for (int i = 0; i < (int)request->headers_len; ++i) {
		printHdr(request->headers[i]);
	}
	printStr("Body = ", request->body, request->body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, out_headers);
	*scode = resp->scode;
	*out_headers_len = resp->headers_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	return resp;
}

void* ffead_cpp_handle_1r(const ffead_request *request, int* scode,
	const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
	phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
	printStr("Server = ", request->server_str, request->server_str_len);
	printStr("Method = ", request->method, request->method_len);
	printStr("Path = ", request->path, request->path_len);
	printf("Version = %d\n", request->version);
	for (int i = 0; i < (int)request->headers_len; ++i) {
		printHdr(request->headers[i]);
	}
	printStr("Body = ", request->body, request->body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, out_headers);
	*scode = resp->scode;
	*out_headers_len = resp->headers_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_mime = resp->out_mime;
	*out_mime_len = resp->out_mime_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	return resp;
}

void* ffead_cpp_handle_1j(int *scode, const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
		const char **out_body, size_t *out_body_len, const char **out_headers, size_t *out_headers_len,
		const char *server_str, size_t server_str_len, const char* method, size_t method_len,
		const char* path, size_t path_len, const char* body, size_t body_len, int version,
		int headers_len, ...
)
{
	printf("%d\n", headers_len);
	printf("%d\n", headers_len/4);
	printf("dummy ffead_cpp_handle_1 %d %d %d %d %d %d \n", *scode, server_str_len, method_len, path_len, version, headers_len);
	printStr("Server = ", server_str, server_str_len);
	printStr("Method = ", method, method_len);
	printStr("Path = ", path, path_len);
	printf("Version = %d\n", version);
	phr_header_fcp in_headers[100];
	va_list valist;
	va_start(valist, headers_len);
	int counter = 0;
	for (int i = 0; i < (int)headers_len; i=i+4) {
		in_headers[counter].name = va_arg(valist, const char*);
		in_headers[counter].name_len = va_arg(valist, size_t);
		in_headers[counter].value = va_arg(valist, const char*);
		in_headers[counter].value_len = va_arg(valist, size_t);
		printHdr(in_headers[counter]);
		counter++;
	}
	fflush(stdout);

	ffead_response* resp = get_resp(*scode, NULL);
	*scode = resp->scode;
	*out_headers = resp->headers_ss_str;
	*out_headers_len = resp->headers_ss_str_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	*out_mime = resp->out_mime;
	*out_mime_len = resp->out_mime_len;
	return resp;
}

void* ffead_cpp_handle_1s(const ffead_request *request, int* scode, const char** smsg, size_t *smsg_len,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len)
{
	printf("dummy ffead_cpp_handle_1\n");
	printStr("Server = ", request->server_str, request->server_str_len);
	printStr("Method = ", request->method, request->method_len);
	printStr("Path = ", request->path, request->path_len);
	printf("Version = %d\n", request->version);
	for (int i = 0; i < (int)request->headers_len; ++i) {
		printHdr(request->headers[i]);
	}
	printStr("Body = ", request->body, request->body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, out_headers);
	*scode = resp->scode;
	*smsg = resp->statmsg;
	*smsg_len = resp->statmsg_len;
	*out_headers_len = resp->headers_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	return resp;
}

void* ffead_cpp_handle_crystal_1(const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
	const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len,
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
	printf("dummy ffead_cpp_handle_1\n");
	printStr("Server = ", request->server_str, request->server_str_len);
	printStr("Method = ", request->method, request->method_len);
	printStr("Path = ", request->path, request->path_len);
	printStr("QueryStr = ", request->qstr, request->qstr_len);
	printf("Version = %d\n", request->version);
	for (int i = 0; i < (int)request->headers_len; ++i) {
		printHdr(request->headers[i]);
	}
	printStr("Body = ", request->body, request->body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, out_headers);
	*scode = resp->scode;
	*smsg = resp->statmsg;
	*smsg_len = resp->statmsg_len;
	*out_headers_len = resp->headers_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_mime = resp->out_mime;
	*out_mime_len = resp->out_mime_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	return resp;
}

void* ffead_cpp_handle_1c(const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
	const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len)
{
	printf("dummy ffead_cpp_handle_1\n");
	printStr("Server = ", request->server_str, request->server_str_len);
	printStr("Method = ", request->method, request->method_len);
	printStr("Path = ", request->path, request->path_len);
	printStr("QueryStr = ", request->qstr, request->qstr_len);
	printf("Version = %d\n", request->version);
	for (int i = 0; i < (int)request->headers_len; ++i) {
		printHdr(request->headers[i]);
	}
	printStr("Body = ", request->body, request->body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, out_headers);
	*scode = resp->scode;
	*smsg = resp->statmsg;
	*smsg_len = resp->statmsg_len;
	*out_headers_len = resp->headers_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_mime = resp->out_mime;
	*out_mime_len = resp->out_mime_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	return resp;
}

void* ffead_cpp_handle_1t(const ffead_request2 *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len)
{
	printf("dummy ffead_cpp_handle_1\n");
	printStr("Server = ", request->server_str, request->server_str_len);
	printStr("Method = ", request->method, request->method_len);
	printStr("Path = ", request->path, request->path_len);
	printStr("QueryStr = ", request->qstr, request->qstr_len);
	printStr("HeaderStr = ", request->headers, request->headers_len);
	printf("Version = %d\n", request->version);
	printStr("Body = ", request->body, request->body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, out_headers);
	*scode = resp->scode;
	*out_headers_len = resp->headers_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	return resp;
}

void* ffead_cpp_handle_2(const ffead_request1 *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len
)
{
	printf("dummy ffead_cpp_handle_2\n");
	printStr("Server = ", request->server_str, request->server_str_len);
	printStr("Method = ", request->method, request->method_len);
	printStr("Path = ", request->path, request->path_len);
	printStr("HeaderStr = ", request->headers, request->headers_len);
	printf("Version = %d\n", request->version);
	printStr("Body = ", request->body, request->body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, out_headers);
	*scode = resp->scode;
	*out_headers_len = resp->headers_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	return resp;
}

void* ffead_cpp_handle_3(const char *server_str, size_t server_str_len,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, bool with_status_line, int* scode, const char** smsg, size_t *smsg_len,
    const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
	printf("dummy ffead_cpp_handle_1\n");
	printStr("Server = ", server_str, server_str_len);
	printStr("HeaderStr = ", in_headers, in_headers_len);
	printStr("Body = ", in_body, in_body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*scode, NULL);
	*scode = resp->scode;
	*done = resp->done;
	*smsg = resp->statmsg;
	*smsg_len = resp->statmsg_len;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	if(with_status_line) {
		*out_headers = resp->headers_ss_str;
		*out_headers_len = resp->headers_ss_str_len;
	} else {
		*out_headers = resp->headers_str;
		*out_headers_len = resp->headers_str_len;
	}
	return resp;
}

void* ffead_cpp_handle_4(const char *server_str, size_t server_str_len,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, const char **out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len
)
{
	printf("dummy ffead_cpp_handle_1\n");
	printStr("Server = ", server_str, server_str_len);
	printStr("HeaderStr = ", in_headers, in_headers_len);
	printStr("Body = ", in_body, in_body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*done, NULL);
	*done = resp->done;
	*out_body = resp->body;
	*out_body_len = resp->body_len;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	*out_headers = resp->headers_ss_str;
	*out_headers_len = resp->headers_ss_str_len;
	return resp;
}

void* ffead_cpp_handle_5(const char *in_headers, size_t in_headers_len,
    const char *in_body, size_t in_body_len, int* done, const char **out_url, size_t *out_url_len)
{
	printf("dummy ffead_cpp_handle_1\n");
	printStr("HeaderStr = ", in_headers, in_headers_len);
	printStr("Body = ", in_body, in_body_len);
	fflush(stdout);
	ffead_response* resp = get_resp(*done, NULL);
	*done = resp->done;
	*out_url = resp->out_url;
	*out_url_len = resp->out_url_len;
	return resp;
}

int ffead_cpp_get_resp_done(void* ptr) {
	ffead_response* respo = (ffead_response*)ptr;
    return respo->done?1:0;
}

int ffead_cpp_get_resp_code(void* ptr) {
	ffead_response* respo = (ffead_response*)ptr;
    return respo->scode;
}

int ffead_cpp_get_resp_status(void* ptr, const char** smsg, size_t *smsg_len) {
	ffead_response* respo = (ffead_response*)ptr;
    *smsg = respo->statmsg;
    *smsg_len = strlen(respo->statmsg);
    return respo->scode;
}

void ffead_cpp_get_resp_header_str(void* ptr, const char **headers, size_t *headers_len, const char *server_str, size_t server_str_len) {
	ffead_response* respo = (ffead_response*)ptr;
    *headers = respo->headers_str;
    *headers_len = strlen(respo->headers_str);
}

void ffead_cpp_get_resp_header_str_with_statusline(void* ptr, const char **headers, size_t *headers_len, const char *server_str, size_t server_str_len) {
	ffead_response* respo = (ffead_response*)ptr;
    *headers = respo->headers_ss_str;
    *headers_len = strlen(respo->headers_ss_str);
}

void ffead_cpp_get_resp_get_body(void* ptr, const char **body, size_t *body_len) {
    ffead_response* respo = (ffead_response*)ptr;
    *body = respo->body;
    *body_len = strlen(respo->body);
}

void ffead_cpp_resp_cleanup(void* ptr) {
    ffead_response* respo = (ffead_response*)ptr;
    delete respo;
}
