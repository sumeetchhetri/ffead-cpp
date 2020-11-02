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
 * CppServerServer.cpp
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#include "CppServerServer.h"

Logger logger;

thread_local void* hrq = NULL;
thread_local void* hrs = NULL;

class CppIntSession : public CppServer::HTTP::HTTPSession
{
public:
    using CppServer::HTTP::HTTPSession::HTTPSession;
protected:
    void onReceivedRequest(const CppServer::HTTP::HTTPRequest& request) override {
    	if(hrq!=NULL) {
    		HttpRequest* req = (HttpRequest*)hrq;
    		delete req;
    		HttpResponse* res = (HttpResponse*)hrs;
    		delete res;
    		hrq = NULL;
    		hrs = NULL;
    	}

    	std::string_view cont = std::string_view(HttpRequest::BLANK);
    	if(request.body_length()>0) {
    		cont = request.body();
    	}
		phr_header_fcp headers[100];
		std::unordered_map<std::string, std::string>::const_iterator it;
		int hc = 0;
		for(int i=0;i<request.headers();i++) {
			std::tuple<std::string_view, std::string_view> hdr = request.header(i);
			headers[hc].name = std::get<0>(hdr).data();
			headers[hc].name_len = std::get<0>(hdr).length();
			headers[hc].value = std::get<1>(hdr).data();
			headers[hc].value_len = std::get<1>(hdr).length();
		}

		HttpRequest* req = new HttpRequest(&headers, hc, request.url(), request.method(), 11, cont);
		hrq = req;
		HttpResponse* respo = new HttpResponse();
		hrs = respo;
		ServiceTask task;
		task.handle(req, respo);

		CppServer::HTTP::HTTPResponse& cresp = response();

		if(respo->isDone()) {
			cresp.SetBegin(respo->getCode());
			for (int var = 0; var < (int)respo->getCookies().size(); var++)
			{
				cresp.SetHeader(std::string_view("Set-Cookie"), std::string_view(respo->getCookies().at(var)));
			}
			std::map<std::string,std::string>::const_iterator it;
			for(it=respo->getCHeaders().begin();it!=respo->getCHeaders().end();++it) {
				cresp.SetHeader(std::string_view(it->first), std::string_view(it->second));
			}
			cresp.SetBody(respo->getContent());
			cresp.SetBodyLength(respo->getContent().length());
		} else {
			if(access(req->getUrl().c_str(), F_OK) != -1) {
				char * buffer = 0;
				long length;
				FILE * f = fopen (req->getUrl().c_str(), "rb");
				if(f) {
					fseek (f, 0, SEEK_END);
					length = ftell (f);
					fseek (f, 0, SEEK_SET);
					buffer = (char*)malloc (length);
					if (buffer)
					{
						fread (buffer, 1, length, f);
					}
					fclose (f);
					cresp.SetBegin(200);
					cresp.SetHeader(std::string_view("content-length"), std::string_view(std::to_string(length)));
					cresp.SetHeader(std::string_view("content-type"), std::string_view(CommonUtils::getMimeType(req->getExt())));
					cresp.SetBody(std::string_view(std::string(buffer, length)));
					free(buffer);
					cresp.SetBodyLength(length);
				} else {
					cresp.SetBegin(404);
				}
			} else {
				cresp.SetBegin(404);
			}
		}
		SendResponseAsync(cresp);
    }
    void onReceivedRequestError(const CppServer::HTTP::HTTPRequest& request, const std::string& error) override {
    }
    void onError(int error, const std::string& category, const std::string& message) override {
    }
};

class CppIntServer : public CppServer::HTTP::HTTPServer
{
public:
    using CppServer::HTTP::HTTPServer::HTTPServer;
protected:
    std::shared_ptr<CppServer::Asio::TCPSession> CreateSession(const std::shared_ptr<CppServer::Asio::TCPServer>& server) override {
    	return std::make_shared<CppIntSession>(std::dynamic_pointer_cast<CppServer::HTTP::HTTPServer>(server));
    }
protected:
    void onError(int error, const std::string& category, const std::string& message) override {
    }
};

void CppServerServer::runServer(std::string ipaddr, int port, std::vector<std::string> servedAppNames, std::string serverRootDirectory, bool isSSLEnabled) {
	// Create a new Asio service
	auto service = std::make_shared<CppServer::Asio::Service>();

	service->Start();
	auto server = std::make_shared<CppIntServer>(service, port);
	//server->AddStaticContent(www, "/api");
	server->Start();

	std::string serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(10);
	}

	server->Stop();
	service->Stop();
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

	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::EMBEDDED_NGHTTP2);
	ServerInitUtil::init(logger);

	CppServerServer::runServer(ipaddr, CastUtil::toInt(port), servedAppNames, serverRootDirectory, isSSLEnabled);

	/*std::string serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(10);
	}*/
	// Build an api.
}
