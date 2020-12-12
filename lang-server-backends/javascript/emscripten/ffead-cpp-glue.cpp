/*
 * ffead-cpp-glue.cpp
 *
 *  Created on: 11-Dec-2020
 *      Author: sumeetc
 */

#include <functional>
#include <emscripten/bind.h>
using namespace emscripten;

extern "C" {
#include "ffead-cpp.h"
}

thread_local void* fcpres = NULL;

struct http_req get_http_req() {
	struct http_req t;
	return t;
}

struct http_res handle(struct http_req t) {
	if(fcpres!=NULL) {
		ffead_cpp_resp_cleanup(fcpres);
	}

	struct phr_header_fcp headers[20];
	for (int i = 0; i < t.num_headers; ++i) {
		headers[i].name = t.headers[i].key.c_str();
		headers[i].name_len = t.headers[i].key.length();
		headers[i].value = t.headers[i].value.c_str();
		headers[i].value_len = t.headers[i].value.length();
	}
	struct ffead_request freq;
	freq.method = t.meth.c_str();
	freq.method_len = t.meth.length();
	freq.path = t.path.c_str();
	freq.path_len = t.path.length();
	freq.body = t.body.c_str();
	freq.body_len = t.body.length();
	freq.version = t.version;
	freq.headers = headers;

	int scode = 0;
	const char *out_mime;
	size_t out_mime_len;
	const char* out_url;
	size_t out_url_len;
	const char* out_body;
	size_t out_body_len;
	struct phr_header_fcp out_headers[20];
	size_t out_headers_len;
	const char* smsg;
	size_t smsg_len;

	fcpres = ffead_cpp_handle_crystal_picov_1(&freq, &scode, &smsg, &smsg_len, &out_mime, &out_mime_len, &out_url, &out_url_len,
			out_headers, &out_headers_len, &out_body, &out_body_len);

	//printf("%s %s %d %s", t.meth.c_str(), t.path.c_str(), t.version, t.body.c_str());
	struct http_res rt;
	if(scode>0) {
		for (int i = 0; i < out_headers_len; ++i) {
			rt.headers[i].key = std::string(out_headers[i].name, out_headers[i].name_len);
			rt.headers[i].value = std::string(out_headers[i].value, out_headers[i].value_len);
		}
		rt.num_headers = (int)out_headers_len;
		rt.body = std::string(out_body, out_body_len);
		rt.scode = scode;
	} else {
		rt.out_url = std::string(out_url, out_url_len);
		rt.out_mine_type = std::string(out_mime, out_mime_len);
	}
	return rt;
}

void bootstrap(std::string srvDir) {
	ffead_cpp_bootstrap(srvDir.c_str(), srvDir.length(), -1);
}

EMSCRIPTEN_BINDINGS(ffeadcpp) {
    value_object<http_header>("http_header")
        .field("key", &http_header::key)
        .field("value", &http_header::value);

    value_object<http_req>("http_req")
        .field("meth", &http_req::meth)
        .field("path", &http_req::path)
        .field("qstr", &http_req::qstr)
        .field("version", &http_req::version)
		.field("body", &http_req::body)
		.field("headers", &http_req::headers)
		.field("num_headers", &http_req::num_headers);

    value_object<http_res>("http_res")
        .field("out_url", &http_res::out_url)
        .field("out_mine_type", &http_res::out_mine_type)
        .field("scode", &http_res::scode)
		.field("body", &http_res::body)
		.field("headers", &http_res::headers)
		.field("num_headers", &http_res::num_headers);

	value_array<std::array<http_header, 20>>("array_http_header_20")
		.element(emscripten::index<0>())
		.element(emscripten::index<1>())
		.element(emscripten::index<2>())
		.element(emscripten::index<3>())
		.element(emscripten::index<4>())
		.element(emscripten::index<5>())
		.element(emscripten::index<6>())
		.element(emscripten::index<7>())
		.element(emscripten::index<8>())
		.element(emscripten::index<9>())
		.element(emscripten::index<10>())
		.element(emscripten::index<11>())
		.element(emscripten::index<12>())
		.element(emscripten::index<13>())
		.element(emscripten::index<14>())
		.element(emscripten::index<15>())
		.element(emscripten::index<16>())
		.element(emscripten::index<17>())
		.element(emscripten::index<18>())
		.element(emscripten::index<19>());

    function("get_http_req", &get_http_req);
    function("handle", &handle);
    function("bootstrap", &bootstrap);
    function("init", &ffead_cpp_init);
    function("clean_up", &ffead_cpp_cleanup);
}
