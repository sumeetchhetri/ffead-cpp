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

/*
    This function should be called before any other ffead-cpp methods
    It is responsible for boostrapping the ffead-cpp framework 
*/
void ffead_cpp_bootstrap(const char* srv, size_t srv_len, int type) {
    ServerInitUtil::bootstrapIB(std::string(srv, srv_len), static_cast<SERVER_BACKEND>(type));
}

/*
    This function should be called after ffead_cpp_bootstrap
    It is responsible for initializing configuration settings/parameters/singletons instances for ffead-cpp framework 
*/
void ffead_cpp_init() {
    ServerInitUtil::initIB();
}

void ffead_cpp_init_for_pv(cb_reg_ext_fd_pv pvregfd, cb_into_pv cb, cb_into_pv_for_date cdt) {
    ServerInitUtil::initIB(pvregfd, cb, cdt);
}

/*
    This should be called before application exit, responsible for cleaning up the ffead-cpp framework
*/
void ffead_cpp_cleanup() {
    ServerInitUtil::cleanUp();
}

/*
    Used by Actix, Hyper and Rocket (Rust)
*/
void* ffead_cpp_handle_rust_1(const ffead_request *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len
)
{
    HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len},
    		std::string_view{request->method, request->method_len}, request->version, std::string_view{request->body, request->body_len});
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
    }
    *out_headers_len = 0;
    RMap::const_iterator it = respo->getCHeaders().cbegin();
    for(;it!=respo->getCHeaders().cend();++it) {
    	out_headers[*out_headers_len].name = it->first.c_str();
    	out_headers[*out_headers_len].name_len = it->first.length();
    	out_headers[*out_headers_len].value = it->second.c_str();
    	out_headers[*out_headers_len].value_len = it->second.length();
    	*out_headers_len = *out_headers_len+1;
    }
    return respo;
}
/*
    Used by Thruster (Rust), Swift-Nio (Swift)
*/
void* ffead_cpp_handle_rust_swift_1(const ffead_request *request, int* scode,
    const char **out_url, size_t *out_url_len, const char **out_url_mime, size_t *out_url_mime_len,
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
    HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len},
    		std::string_view{request->method, request->method_len}, request->version, std::string_view{request->body, request->body_len});
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
        const std::string& mime_type = CommonUtils::getMimeType(req.getExt());
        *out_url_mime = mime_type.c_str();
        *out_url_mime_len = mime_type.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
    }
    *out_headers_len = 0;
    RMap::const_iterator it = respo->getCHeaders().cbegin();
    for(;it!=respo->getCHeaders().cend();++it) {
    	out_headers[*out_headers_len].name = it->first.c_str();
    	out_headers[*out_headers_len].name_len = it->first.length();
    	out_headers[*out_headers_len].value = it->second.c_str();
    	out_headers[*out_headers_len].value_len = it->second.length();
    	*out_headers_len = *out_headers_len+1;
    }
    return respo;
}

/*
    Used by libreactor, h2o (C)
*/
void* ffead_cpp_handle_c_1(const ffead_request *request, int* scode, const char** smsg, size_t *smsg_len,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len)
{
    HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len},
    		std::string_view{request->method, request->method_len}, request->version, std::string_view{request->body, request->body_len});
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
		*scode = 0;
        *out_body_len = 0;
        respo->addHeader(HttpResponse::ContentType, CommonUtils::getMimeType(req.getExt()));
    } else {
        *out_url_len = 0;
    	*scode = respo->getCode();
        const std::string& cnt = respo->generateNginxApacheResponse();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
        if(cnt.length()>0) {
            respo->addHeader(HttpResponse::ContentLength, std::to_string((int)cnt.length()));
        }
    }
    if(req.isClose() || req.getHttpVers()<=1.0) {
		respo->addHeader(HttpResponse::Connection, "close");
	} else {
		respo->addHeader(HttpResponse::Connection, "keep-alive");
	}
    const std::string& statmsg = respo->getStatusMsg();
    *smsg = statmsg.c_str();
    *smsg_len = statmsg.length();
    *out_headers_len = 0;
    RMap::const_iterator it = respo->getCHeaders().cbegin();
    for(;it!=respo->getCHeaders().cend();++it) {
    	out_headers[*out_headers_len].name = it->first.c_str();
    	out_headers[*out_headers_len].name_len = it->first.length();
    	out_headers[*out_headers_len].value = it->second.c_str();
    	out_headers[*out_headers_len].value_len = it->second.length();
    	*out_headers_len = *out_headers_len+1;
    }
    return respo;
}

/*
    Used by Crystal-Http/H2O (Crystal) and picov (vlang)
*/
void* ffead_cpp_handle_picov_1(const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
	const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len, 
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
	HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len},
    		std::string_view{request->method, request->method_len}, request->version, std::string_view{request->body, request->body_len});
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
        const std::string& mime_type = CommonUtils::getMimeType(req.getExt());
        *out_mime = mime_type.c_str();
        *out_mime_len = mime_type.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
        *smsg = respo->getStatusMsg().c_str();
        *smsg_len = respo->getStatusMsg().length();
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
        respo->addHeader(HttpResponse::ContentLength, std::to_string((int)cnt.length()));
    }
    *out_headers_len = 0;
    RMap::const_iterator it = respo->getCHeaders().cbegin();
    for(;it!=respo->getCHeaders().cend();++it) {
    	out_headers[*out_headers_len].name = it->first.c_str();
    	out_headers[*out_headers_len].name_len = it->first.length();
    	out_headers[*out_headers_len].value = it->second.c_str();
    	out_headers[*out_headers_len].value_len = it->second.length();
    	*out_headers_len = *out_headers_len+1;
    }
    return respo;
}
void ffead_cpp_handle_picov_2(const ffead_request3 *request)
{
	PicoVWriter* writer = (PicoVWriter*)request->writer;
	HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len},
    		std::string_view{request->method, request->method_len}, request->version, std::string_view{request->body, request->body_len});
    ServiceTask::handleAsync(&req, NULL, writer);
}
void* ffead_cpp_handle_picov_2_init_sock(int fd, void* pv)
{
	return new PicoVWriter(fd, pv);
}
void ffead_cpp_handle_picov_2_deinit_sock(int fd, void* data)
{
	ServerInitUtil::closeConnection(data);
}
void ffead_cpp_handle_picov_clean_sockets()
{
	ServerInitUtil::closeConnections();
}
void ffead_cpp_handle_picov_ext_fd_cb(int fd, void* data)
{
	#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* libpq = (LibpqDataSourceImpl*)data;
	libpq->handle();
	#endif
}

void* ffead_cpp_handle_crystal_js_1(const ffead_request3 *request, int* scode, const char** smsg, size_t *smsg_len,
	const char **out_mime, size_t *out_mime_len, const char **out_url, size_t *out_url_len,
    phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
	HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len},
    		std::string_view{request->method, request->method_len}, request->version, std::string_view{request->body, request->body_len});
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
        const std::string& mime_type = CommonUtils::getMimeType(req.getExt());
        *out_mime = mime_type.c_str();
        *out_mime_len = mime_type.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
        *smsg = respo->getStatusMsg().c_str();
        *smsg_len = respo->getStatusMsg().length();
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
    }
    *out_headers_len = 0;
    RMap::const_iterator it = respo->getCHeaders().cbegin();
    for(;it!=respo->getCHeaders().cend();++it) {
    	out_headers[*out_headers_len].name = it->first.c_str();
    	out_headers[*out_headers_len].name_len = it->first.length();
    	out_headers[*out_headers_len].value = it->second.c_str();
    	out_headers[*out_headers_len].value_len = it->second.length();
    	*out_headers_len = *out_headers_len+1;
    }
    return respo;
}

/*
    Used by fasthttp, atruego (golang)
*/
void* ffead_cpp_handle_go_1(const char *server_str, size_t server_str_len,
	const char *method, size_t method_len, const char *path, size_t path_len, const char *query, size_t query_len, int version,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
    const char **out_url, size_t *out_url_len,  const char **out_mime, size_t *out_mime_len,
	phr_header_fcp *out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
	HttpRequest req(method, method_len, path, path_len, query, query_len, in_headers, in_headers_len, in_body, in_body_len, version);
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
        const std::string& mime_type = CommonUtils::getMimeType(req.getExt());
        *out_mime = mime_type.c_str();
        *out_mime_len = mime_type.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
    }
    *out_headers_len = 0;
    RMap::const_iterator it = respo->getCHeaders().cbegin();
    for(;it!=respo->getCHeaders().cend();++it) {
    	out_headers[*out_headers_len].name = it->first.c_str();
    	out_headers[*out_headers_len].name_len = it->first.length();
    	out_headers[*out_headers_len].value = it->second.c_str();
    	out_headers[*out_headers_len].value_len = it->second.length();
    	*out_headers_len = *out_headers_len+1;
    }
    return respo;
}

/*
    Used by gnet (golang)
*/
void* ffead_cpp_handle_go_2(const char *server_str, size_t server_str_len,
	const char *method, size_t method_len, const char *path, size_t path_len, int version,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* scode,
    const char **out_url, size_t *out_url_len,  const char **out_mime, size_t *out_mime_len,
	const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
	HttpRequest req(method, method_len, path, path_len, NULL, 0, in_headers, in_headers_len, in_body, in_body_len, version);
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
        const std::string& mime_type = CommonUtils::getMimeType(req.getExt());
        *out_mime = mime_type.c_str();
        *out_mime_len = mime_type.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
        std::string server;
        if(server_str_len>0 && server_str!=NULL) {
            server = std::string(server_str, server_str_len);
        }
        const std::string& hdrs = respo->getHeadersStr(server, true, false, true);
        *out_headers = hdrs.c_str();
        *out_headers_len = hdrs.length()-2;
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
    }
    return respo;
}

/*
    Used by vweb (vlang)
*/
void* ffead_cpp_handle_v(const char *server_str, size_t server_str_len,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len, 
	const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
    HttpRequest req(in_headers, in_headers_len, in_body, in_body_len);
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
        const std::string& mime_type = CommonUtils::getMimeType(req.getExt());
        *out_mime = mime_type.c_str();
        *out_mime_len = mime_type.length();
		*done = 0;
    } else {
    	*done = respo->getCode();
        std::string server;
        if(server_str_len>0 && server_str!=NULL) {
            server = std::string(server_str, server_str_len);
        }
        const std::string& hdrs = respo->getHeadersStr(server, true, false, true);
        *out_headers = hdrs.c_str();
        *out_headers_len = hdrs.length()-2;
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
    }
    return respo;
}


/*
    Used by hunt (dlang)
*/
void* ffead_cpp_handle_d_1(const ffead_request *request, int* scode,
	    const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
		const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len)
{
	HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len},
	    		std::string_view{request->method, request->method_len}, request->version, std::string_view{request->body, request->body_len});
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
        const std::string& mime_type = CommonUtils::getMimeType(req.getExt());
        *out_mime = mime_type.c_str();
        *out_mime_len = mime_type.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
        std::string server;
        const std::string& hdrs = respo->getHeadersStr(server, true, false, false);
        *out_headers = hdrs.c_str();
        *out_headers_len = hdrs.length()-2;
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
    }
    return respo;
}

/*
    Used by firenio, wizzardo-http and rapidoid (Java)
*/
void* ffead_cpp_handle_java(int *scode, const char **out_url, size_t *out_url_len, const char **out_mime, size_t *out_mime_len,
		const char **out_body, size_t *out_body_len, const char **out_headers, size_t *out_headers_len,
		const char *server_str, size_t server_str_len, const char* method, size_t method_len,
		const char* path, size_t path_len, const char* body, size_t body_len, int version,
		int headers_len, ...
)
{
    HttpRequest req(method, method_len, path, path_len, body, body_len, version);
    va_list valist;
	va_start(valist, headers_len);
	for (int i = 0; i < (int)headers_len; i=i+4) {
        const char* kp = va_arg(valist, const char*);
        size_t kl = va_arg(valist, size_t);
        const char* vp = va_arg(valist, const char*);
        size_t vl = va_arg(valist, size_t);
        req.addNginxApacheHeader(kp, kl, vp, vl);
	}
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
        const std::string& mime_type = CommonUtils::getMimeType(req.getExt());
        *out_mime = mime_type.c_str();
        *out_mime_len = mime_type.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
        std::string server;
        bool isStatusLine = true;
        bool isContent = false;
        bool isServerLine = true;
        if(server_str_len>0 && server_str!=NULL) {
            server = std::string(server_str, server_str_len);
            if(server.find("wizzardo")==0) {
                server = "";
                isServerLine = false;
                isStatusLine = false;
            }
        }
        const std::string& hdrs = respo->getHeadersStr(server, isStatusLine, isContent, isServerLine);
        *out_headers = hdrs.c_str();
        *out_headers_len = hdrs.length()-2;
        const std::string& cnt = respo->getContent();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
    }
    return respo;
}



void* ffead_cpp_handle_1t(const ffead_request2 *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len)
{
    HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len}, 
            std::string_view{request->qstr, request->qstr_len}, std::string_view{request->method, request->method_len}, 
            request->version, std::string_view{request->body, request->body_len});
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
		*scode = 0;
        *out_body_len = 0;
        respo->addHeader(HttpResponse::ContentType, CommonUtils::getMimeType(req.getExt()));
    } else {
        *out_url_len = 0;
    	*scode = respo->getCode();
        const std::string& cnt = respo->generateNginxApacheResponse();
        *out_body = cnt.c_str();
        *out_body_len = cnt.length();
        if(cnt.length()>0) {
            respo->addHeader(HttpResponse::ContentLength, std::to_string((int)cnt.length()));
        }
    }
    *out_headers_len = 0;
    RMap::const_iterator it = respo->getCHeaders().cbegin();
    for(;it!=respo->getCHeaders().cend();++it) {
    	out_headers[*out_headers_len].name = it->first.c_str();
    	out_headers[*out_headers_len].name_len = it->first.length();
    	out_headers[*out_headers_len].value = it->second.c_str();
    	out_headers[*out_headers_len].value_len = it->second.length();
    	*out_headers_len = *out_headers_len+1;
    }
    const std::string& cnt = respo->getContent();
    *out_body = cnt.c_str();
    *out_body_len = cnt.length();
    return respo;
}

void* ffead_cpp_handle_2(const ffead_request1 *request, int* scode,
    const char **out_url, size_t *out_url_len, phr_header_fcp *out_headers, size_t *out_headers_len,
    const char **out_body, size_t *out_body_len
)
{
    HttpRequest req(std::string_view{request->path, request->path_len}, std::string_view{request->method, request->method_len}, request->version,
    		request->headers, request->headers_len, request->body, request->body_len);
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url = resUrl.c_str();
		*out_url_len = resUrl.length();
		*scode = 0;
    } else {
    	*scode = respo->getCode();
    }
    *out_headers_len = 0;
    RMap::const_iterator it = respo->getCHeaders().cbegin();
    for(;it!=respo->getCHeaders().cend();++it) {
    	out_headers[*out_headers_len].name = it->first.c_str();
    	out_headers[*out_headers_len].name_len = it->first.length();
    	out_headers[*out_headers_len].value = it->second.c_str();
    	out_headers[*out_headers_len].value_len = it->second.length();
    	*out_headers_len = *out_headers_len+1;
    }
    const std::string& cnt = respo->getContent();
    *out_body = cnt.c_str();
    *out_body_len = cnt.length();
    return respo;
}

void* ffead_cpp_handle_3(const char *server_str, size_t server_str_len,
    const char *in_headers, size_t in_headers_len, const char *in_body, size_t in_body_len, int* done,
    const char **out_url, size_t *out_url_len, bool with_status_line, int* scode, const char** smsg, size_t *smsg_len, 
    const char **out_headers, size_t *out_headers_len, const char **out_body, size_t *out_body_len
)
{
    HttpRequest req(in_headers, in_headers_len, in_body, in_body_len);
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
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
    const std::string& hdrs = respo->getHeadersStr(server, false, false, true);
    *out_headers = hdrs.c_str();
    *out_headers_len = hdrs.length();
    const std::string& cnt = respo->getContent();
    *out_body = cnt.c_str();
    *out_body_len = cnt.length();
    return respo;
}

void* ffead_cpp_handle_5(const char *in_headers, size_t in_headers_len,
    const char *in_body, size_t in_body_len, int* done, const char **out_url, size_t *out_url_len)
{
    HttpRequest req(in_headers, in_headers_len, in_body, in_body_len);
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
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
    const std::string& hdrs = respo->getHeadersStr(server, false, false, true);
    *headers = hdrs.c_str();
    *headers_len = hdrs.length();
}

void ffead_cpp_get_resp_header_str_with_statusline(void* ptr, const char **headers, size_t *headers_len, const char *server_str, size_t server_str_len) {
    HttpResponse* respo = (HttpResponse*)ptr;
    std::string server;
    if(server_str_len>0 && server_str!=NULL) {
        server = std::string(server_str, server_str_len);
    }
    const std::string& hdrs = respo->getHeadersStr(server, false, false, true);
    *headers = hdrs.c_str();
    *headers_len = hdrs.length();
}

void ffead_cpp_get_resp_get_body(void* ptr, const char **body, size_t *body_len) {
    HttpResponse* respo = (HttpResponse*)ptr;
    const std::string& cnt = respo->getContent();
    *body = cnt.c_str();
    *body_len = cnt.length();
}

void ffead_cpp_resp_cleanup(void* ptr) {
    HttpResponse* respo = (HttpResponse*)ptr;
    delete respo;
}

void* ffead_cpp_handle_js_1(const ffead_request *request, int* scode, size_t *out_url_len, size_t *out_headers_len, size_t *out_body_len) {
    HttpRequest req((void*)request->headers, request->headers_len, std::string_view{request->path, request->path_len},
    		std::string_view{request->method, request->method_len}, request->version, std::string_view{request->body, request->body_len});
    HttpResponse* respo = new HttpResponse();

    ServiceTask::handle(&req, respo);
    if(!respo->isDone()) {
    	respo->setUrl(req.getUrl());
		const std::string& resUrl = respo->getUrl();
		*out_url_len = resUrl.length();
		*scode = 0;
        *out_body_len = 0;
        respo->addHeader(HttpResponse::ContentType, CommonUtils::getMimeType(req.getExt()));
    } else {
        *out_url_len = 0;
    	*scode = respo->getCode();
        const std::string& cnt = respo->generateNginxApacheResponse();
        *out_body_len = cnt.length();
        if(cnt.length()>0) {
            respo->addHeader(HttpResponse::ContentLength, std::to_string((int)cnt.length()));
        }
    }
    *out_headers_len = respo->getHeaders().size();
    return respo;
}
const char* ffead_cpp_handle_js_out_url(void* res) {
	HttpResponse* respo = (HttpResponse*)res;
	return respo->getUrl().c_str();
}
const char* ffead_cpp_handle_js_out_body(void* res) {
	HttpResponse* respo = (HttpResponse*)res;
	const std::string& cnt = respo->generateNginxApacheResponse();
	return cnt.c_str();
}
const char* ffead_cpp_handle_js_out_hdr_name(void* res, int pos, size_t* name_len) {
	HttpResponse* respo = (HttpResponse*)res;
	RMap::const_iterator it = respo->getCHeaders().cbegin();
	*name_len = 0;
	for(int i=0;it!=respo->getCHeaders().cend();++it,i++) {
		if(i==pos) {
			*name_len = it->first.length();
			return it->first.c_str();
		}
	}
	return NULL;
}
const char* ffead_cpp_handle_js_out_hdr_value(void* res, int pos, size_t* value_len) {
	HttpResponse* respo = (HttpResponse*)res;
	RMap::const_iterator it = respo->getCHeaders().cbegin();
	*value_len = 0;
	for(int i=0;it!=respo->getCHeaders().cend();++it,i++) {
		if(i==pos) {
			*value_len = it->second.length();
			return it->second.c_str();
		}
	}
	return NULL;
}
