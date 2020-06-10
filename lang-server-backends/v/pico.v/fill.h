#include <stddef.h>

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
}ffead_request3;

typedef struct ffead_request3 ffead_request3_t;
