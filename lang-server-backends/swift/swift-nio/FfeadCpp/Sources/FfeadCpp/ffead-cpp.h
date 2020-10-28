//
//  ffead-cpp.h
//  app
//
//  Created by Sumeet Chhetri on 14/10/20.
//

#ifndef ffead_cpp_h
#define ffead_cpp_h
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

struct phr_header_fcp {
    char* name;
    size_t name_len;
    char* value;
    size_t value_len;
};

struct ffead_request {
    const char* server_str;
    size_t server_str_len;
    const char* method;
    size_t method_len;
    const char* path;
    size_t path_len;
    struct phr_header_fcp* headers;
    size_t headers_len;
    const char* body;
    size_t body_len;
    int version;
};

extern void ffead_cpp_resp_cleanup(void* ptr);
extern void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type);
extern void ffead_cpp_init();
extern void ffead_cpp_cleanup();
extern void* ffead_cpp_handle_rust_swift_1(const struct ffead_request *request, int* scode,
    const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
    struct phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len);

#endif /* ffead_cpp_h */
