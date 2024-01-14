/// Minimalistic low-overhead wrapper for nodejs/http-parser
/// Used for benchmarks with simple server
module ffeadcpp;

import hunt.logging.ConsoleLogger;
import std.conv;
import std.range.primitives;
import core.stdc.string;
import http.Parser;

import std.experimental.allocator;

struct phr_header_fcp {
	const char *name;
	size_t name_len;
	const char *value;
	size_t value_len;
}

struct ffead_request {
	const char* server_str;
	size_t server_str_len;
	const char* method;
	size_t method_len;
	const char* path;
	size_t path_len;
	phr_header_fcp* headers;
	size_t headers_len;
	char* body;
	size_t body_len;
	int version_;
}

extern (C) pure @nogc nothrow int phr_parse_request_fcp(const char *buf, size_t len, const char **method,
	size_t *method_len, const char **path, size_t *path_len,
    int *minor_version, phr_header_fcp *headers, size_t *num_headers, size_t last_len, int* content_length);

extern (C) pure @nogc nothrow void ffead_cpp_resp_cleanup(void* ptr);
extern (C) pure @nogc nothrow void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);
extern (C) pure @nogc nothrow void ffead_cpp_init();
extern (C) pure @nogc nothrow int ffead_cpp_is_inited();
extern (C) pure @nogc nothrow void ffead_cpp_cleanup();
extern (C) pure @nogc nothrow void* ffead_cpp_handle_d_1(const ffead_request *request, int* done,
	    const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
		const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len);
