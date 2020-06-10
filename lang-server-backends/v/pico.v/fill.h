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
