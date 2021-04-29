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
/*
 * UvCppServer.cpp
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#include "UvCppServer.h"

Logger logger;

void UvCppServer::handler(uv::http::Request& req2, uv::http::Response* res)
{
	std::string_view cont = std::string_view(HttpRequest::BLANK);
	if(req2.getContent().length()>0) {
		cont = std::string_view(req2.getContent().data(), req2.getContent().length());
	}
	phr_header_fcp headers[100];
	std::map<std::string, std::string>::const_iterator it;
	int hc = 0;
	for(it=req2.getHeaders().begin();it!=req2.getHeaders().end();++it,hc++) {
		headers[hc].name = it->first.c_str();
		headers[hc].name_len = it->first.length();
		headers[hc].value = it->second.c_str();
		headers[hc].value_len = it->second.length();
	}

	std::string mth = uv::http::Request::MethonToStr(req2.getMethon());
	std::string_view meth = std::string_view(mth.data(), mth.length());
	std::string path;
	req2.packPathParam(path);
	std::string_view url = std::string_view(path.data(), path.length());

	HttpRequest req(&headers, hc, url, meth, 11, cont);
	HttpResponse respo;
	ServiceTask task;
	task.handle(&req, &respo);

	if(respo.isDone()) {
		for (int var = 0; var < (int)respo.getCookies().size(); var++)
		{
			res->appendHead(std::string("Set-Cookie"), std::string(respo.getCookies().at(var)));
		}
		std::map<std::string,std::string>::const_iterator it;
		for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
			res->appendHead(std::string(it->first), std::string(it->second));
		}

		res->swapContent(std::string(respo.getContent()));
		res->setStatus(uv::http::Response::StatusCode(respo.getCode()), respo.getStatusMsg());
	} else {
		if(access(req.getUrl().c_str(), F_OK) != -1) {
			char * buffer = 0;
			long length;
			FILE * f = fopen (req.getUrl().c_str(), "rb");
			if(f) {
				fseek (f, 0, SEEK_END);
				length = ftell (f);
				fseek (f, 0, SEEK_SET);
				buffer = (char *)malloc (length);
				if (buffer)
				{
					fread (buffer, 1, length, f);
				}
				fclose (f);
				res->appendHead("content-length", std::to_string(length));
				res->appendHead("content-type", std::string(CommonUtils::getMimeType(req.getExt())));
				std::string content = std::string(buffer, length);
				free(buffer);
				res->swapContent(std::move(content));
				res->setStatus(uv::http::Response::StatusCode(200), "OK");
			} else {
				res->setStatus(uv::http::Response::StatusCode(404), "Not Found");
			}
		} else {
			res->setStatus(uv::http::Response::StatusCode(404), "Not Found");
		}
	}
	res->setVersion(uv::http::HttpVersion::Http1_1);
	res->appendHead("Server", "uv-cpp");
}

void UvCppServer::runServer(std::string ipaddr, int port, std::vector<std::string> servedAppNames, std::string serverRootDirectory, bool isSSLEnabled) {
	uv::EventLoop loop;
	uv::http::HttpServer::SetBufferMode(uv::GlobalConfig::BufferMode::CycleBuffer);
	uv::http::HttpServer server(&loop);
	server.Get("/*", std::bind(&handler, std::placeholders::_1, std::placeholders::_2));
	server.Put("/*", std::bind(&handler, std::placeholders::_1, std::placeholders::_2));
	server.Post("/*", std::bind(&handler, std::placeholders::_1, std::placeholders::_2));
	server.Delete("/*", std::bind(&handler, std::placeholders::_1, std::placeholders::_2));
	server.Options("/*", std::bind(&handler, std::placeholders::_1, std::placeholders::_2));
	uv::SocketAddr addr(ipaddr, port);
	server.bindAndListen(addr);
	loop.run();
}

int main(int argc, char *argv[]) {
	if(argc == 1)
	{
		//cout << "No Server root directory specified, quitting..." << std::endl;
		return 0;
	}

	std::string serverRootDirectory = argv[1];
	serverRootDirectory += "/";
	if(serverRootDirectory.find("//")==0)
	{
		RegexUtil::replace(serverRootDirectory,"[/]+","/");
	}

	PropFileReader pread;
	std::string respath = serverRootDirectory + "resources/";
	propMap srprps = pread.getProperties(respath+"server.prop");

	std::ofstream local("/dev/null");
	if(StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])!="true") {
		//std::streambuf* cout_buff = std::cout.rdbuf();
		std::cout.rdbuf(local.rdbuf());
	}

	std::string port = srprps["PORT_NO"];
	std::string ipaddr = srprps["IP_ADDR"];
	std::string servingAppNames = srprps["SRV_APPS"];
	std::vector<std::string> servedAppNames = StringUtil::splitAndReturn<std::vector<std::string> >(servingAppNames, ",");

	propMultiMap mpmap = pread.getPropertiesMultiMap(respath+"server.prop");
	if(mpmap.find("VHOST_ENTRY")!=mpmap.end() && mpmap["VHOST_ENTRY"].size()>0) {
		//TODO
	}

	bool isSSLEnabled = false;
	std::string sslEnabled = srprps["SSL_ENAB"];
	if(sslEnabled=="true" || sslEnabled=="TRUE")
		isSSLEnabled = true;

	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::UV_CPP);
	ServerInitUtil::init(logger);

	UvCppServer::runServer(ipaddr, CastUtil::toInt(port), servedAppNames, serverRootDirectory, isSSLEnabled);

	/*std::string serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(10);
	}*/
	// Build an api.
}
