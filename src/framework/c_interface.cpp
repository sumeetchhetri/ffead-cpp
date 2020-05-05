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

void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type) {
    ServerInitUtil::bootstrapIB(std::string(srv, srv_len), static_cast<SERVER_BACKEND>(type));
}

void ffead_cpp_init() {
    ServerInitUtil::initIB();
}

void ffead_cpp_cleanup() {
    ServerInitUtil::cleanUp();
}

void* ffead_cpp_handle_return_hdrs_body(const char *server_str, size_t server_str_len, 
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, bool with_status_line, int* scode, const char** smsg, size_t *smsg_len, 
    const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
    HttpRequest req(in_headers, in_headers_len, in_body, in_body_len);
    HttpResponse* respo = new HttpResponse();
    ServiceTask task;
    task.handle(&req, respo);
    respo->setUrl(req.getUrl());
    const std::string& resUrl = respo->getUrl();
    *out_url = resUrl.c_str();
    *out_url_len = resUrl.length();
    *done = respo->isDone()?1:0;
    const std::string& statmsg = respo->getStatusMsg();
    *smsg = statmsg.c_str();
    *smsg_len = statmsg.length();
    *scode = respo->getCode();
    std::string server;
    if(server_str_len>0 && server_str!=NULL) {
        server = std::string(server_str, server_str_len);
    }
    const std::string& hdrs = respo->getHeadersStr(server, false);
    *out_headers = hdrs.c_str();
    *out_headers_len = hdrs.length();
    const std::string& cnt = respo->getContent();
    *out_body = cnt.c_str();
    *out_body_len = cnt.length();
    return respo;
}

void* ffead_cpp_handle_return_all(const char *server_str, size_t server_str_len, 
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, const char **out_headers, size_t *out_headers_len, 
    const char **out_body, size_t *out_body_len
)
{
    HttpRequest req(in_headers, in_headers_len, in_body, in_body_len);
    HttpResponse* respo = new HttpResponse();
    ServiceTask task;
    task.handle(&req, respo);
    respo->setUrl(req.getUrl());
    const std::string& resUrl = respo->getUrl();
    *out_url = resUrl.c_str();
    *out_url_len = resUrl.length();
    *done = respo->isDone()?1:0;
    std::string server;
    if(server_str_len>0 && server_str!=NULL) {
        server = std::string(server_str, server_str_len);
    }
    const std::string& hdrs = respo->getHeadersStr(server, true);
    *out_headers = hdrs.c_str();
    *out_headers_len = hdrs.length();
    const std::string& cnt = respo->getContent();
    *out_body = cnt.c_str();
    *out_body_len = cnt.length();
    return respo;
}

void* ffead_cpp_handle_request(const char *in_headers, size_t in_headers_len, 
    const char *in_body, size_t in_body_len, int* done, const char **out_url, size_t *out_url_len)
{
    HttpRequest req(in_headers, in_headers_len, in_body, in_body_len);
    HttpResponse* respo = new HttpResponse();
    ServiceTask task;
    task.handle(&req, respo);
    respo->setUrl(req.getUrl());
    const std::string& resUrl = respo->getUrl();
    *out_url = resUrl.c_str();
    *out_url_len = resUrl.length();
    *done = respo->isDone()?1:0;
    return respo;
}

int ffead_cpp_get_resp_done(void* ptr) {
    HttpResponse* respo = (HttpResponse*)ptr;
    return respo->isDone()?1:0;
}

int ffead_cpp_get_resp_code(void* ptr) {
    HttpResponse* respo = (HttpResponse*)ptr;
    return respo->getCode();
}

int ffead_cpp_get_resp_status(void* ptr, const char** smsg, size_t *smsg_len) {
    HttpResponse* respo = (HttpResponse*)ptr;
    const std::string& statmsg = respo->getStatusMsg();
    *smsg = statmsg.c_str();
    *smsg_len = statmsg.length();
    return respo->getCode();
}

void ffead_cpp_get_resp_header_str(void* ptr, const char **headers, size_t *headers_len, const char *server_str, size_t server_str_len) {
    HttpResponse* respo = (HttpResponse*)ptr;
    std::string server;
    if(server_str_len>0 && server_str!=NULL) {
        server = std::string(server_str, server_str_len);
    }
    const std::string& hdrs = respo->getHeadersStr(server, false);
    *headers = hdrs.c_str();
    *headers_len = hdrs.length();
}

void ffead_cpp_get_resp_header_str_with_statusline(void* ptr, const char **headers, size_t *headers_len, const char *server_str, size_t server_str_len) {
    HttpResponse* respo = (HttpResponse*)ptr;
    std::string server;
    if(server_str_len>0 && server_str!=NULL) {
        server = std::string(server_str, server_str_len);
    }
    const std::string& hdrs = respo->getHeadersStr(server, false);
    *headers = hdrs.c_str();
    *headers_len = hdrs.length();
}

void ffead_cpp_get_resp_get_body(void* ptr, const char **body, size_t *body_len) {
    HttpResponse* respo = (HttpResponse*)ptr;
    const std::string& cnt = respo->getContent();
    *body = cnt.c_str();
    *body_len = cnt.length();
}

void ffead_cpp_get_resp_cleanup(void* ptr) {
    HttpResponse* respo = (HttpResponse*)ptr;
    delete respo;
}