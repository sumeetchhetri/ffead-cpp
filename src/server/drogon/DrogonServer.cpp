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
 * DrogonServer.cpp
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#include "DrogonServer.h"

static Logger logger;
libcuckoo::cuckoohash_map<std::string, DrogonSocketHandler*> DrogonServer::websocks;

void DrogonWebsockHandler::handleNewMessage(const WebSocketConnectionPtr& wcon, std::string &&msg, const WebSocketMessageType &type) {
	std::string address = StringUtil::toHEX((long long)wcon.get());
	if(DrogonServer::websocks.contains(address)) {
		DrogonSocketHandler* dsk = DrogonServer::websocks.find(address);

		WebSocketData wreq;
		if(type==WebSocketMessageType::Binary) {
			wreq.collectBinary(msg);
		} else if(type==WebSocketMessageType::Text) {
			wreq.collectText(msg);
		}
		WebSocketRespponseData wres;

		if(dsk->h->onMessage(&wreq, &wres, address)) {
			if(!dsk->h->isWriteControl()) {
				dsk->writeWsData(&wres);
			}
		}
	}
}

void DrogonWebsockHandler::handleNewConnection(const HttpRequestPtr& hreq, const WebSocketConnectionPtr& wcon) {
	drogon::HttpRequest* request = (drogon::HttpRequest*)&hreq;
	drogon::Version v = request->getVersion();
	int hv = v==drogon::Version::kHttp10?0:1;

	DrogonSocketHandler* dsk = new DrogonSocketHandler(wcon.get());
	DrogonServer::websocks.insert(dsk->getAddress(), dsk);

	::HttpRequest req(request->bodyData(), request->bodyLength(), request->getHeaders(), request->getPath(), request->getQuery(), request->getMethodString(), hv);
	ServiceTask task;
	WebSocketData wreq;
	WebSocketRespponseData wres;
	dsk->h = task.handleWebsockOpen(&wreq, &wres, dsk, &req);
	dsk->writeWsData(&wres);
}

void DrogonWebsockHandler::handleConnectionClosed(const WebSocketConnectionPtr& wcon) {
	std::string address = StringUtil::toHEX((long long)wcon.get());
	if(DrogonServer::websocks.contains(address)) {
		DrogonSocketHandler* dsk = DrogonServer::websocks.find(address);
		dsk->onClose();
		DrogonServer::websocks.erase(address);
	}
}

void DrogonHttpHandler::handle(const HttpRequestPtr &requestptr, std::function<void (const HttpResponsePtr &)> &&callback) {
	drogon::HttpRequest* request = requestptr.get();
	drogon::Version v = request->getVersion();
	int hv = v==drogon::Version::kHttp10?0:1;

	::HttpRequest req(request->bodyData(), request->bodyLength(), request->getHeaders(), request->getPath(), request->getQuery(), request->getMethodString(), hv);
	::HttpResponse respo;
	ServiceTask task;
	task.handle(&req, &respo);

	drogon::HttpResponsePtr resp = NULL;
	if(respo.isDone()) {
		resp = drogon::HttpResponse::newHttpResponse();
		for (int var = 0; var < (int)respo.getCookies().size(); var++)
		{
			resp.get()->addHeader("Set-Cookie", respo.getCookies().at(var));
		}
		std::map<std::string,std::string>::const_iterator it;
		for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
			resp.get()->addHeader(it->first, it->second);
		}

		drogon::HttpStatusCode st = static_cast<drogon::HttpStatusCode>(respo.getCode());
		resp.get()->setStatusCode(st);
		std::string& data = respo.generateNginxApacheResponse();
		resp.get()->setBody(data);
	} else {
		if(access(req.getUrl().c_str(), F_OK) != -1) {
			resp = drogon::HttpResponse::newFileResponse(req.getUrl());
		} else {
			resp = drogon::HttpResponse::newNotFoundResponse();
		}
	}
	callback(resp);
}

void DrogonServer::runServer(std::string ip_addr, std::string port, std::vector<std::string> servedAppNames) {
	int max_thread_num = std::thread::hardware_concurrency();
	drogon::app().setLogPath("./")
		 .addListener(ip_addr, CastUtil::toInt(port))
		 .setThreadNum(max_thread_num)
		 //.enableRunAsDaemon()
	     .run();
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

	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::DROGON);
	ServerInitUtil::init(logger);
	DrogonServer::runServer(ipaddr, port, servedAppNames);

	/*std::string serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(10);
	}*/
	// Build an api.
}

DrogonSocketHandler::DrogonSocketHandler(WebSocketConnection* conn) {
	this->conn = conn;
	address = StringUtil::toHEX((long long)conn);
	h = NULL;
}

DrogonSocketHandler::~DrogonSocketHandler() {
}

std::string DrogonSocketHandler::getProtocol(void *context) {
	return "";
}

int DrogonSocketHandler::getType(void *context) {
	return -1;
}

int DrogonSocketHandler::getTimeout() {
	return -1;
}

bool DrogonSocketHandler::readRequest(void *request, void *&context, int &pending, int &reqPos) {
	return false;
}

bool DrogonSocketHandler::writeResponse(void *req, void *res, void *context, std::string &data, int reqPos) {
	return false;
}

void DrogonSocketHandler::onOpen() {
}

void DrogonSocketHandler::onClose() {
	h->onClose(getAddress());
}

void DrogonSocketHandler::addHandler(SocketInterface *handler) {
}

void DrogonSocketHandler::sendResponse(std::string str, bool isText) {
	conn->send(str, isText?WebSocketMessageType::Text:WebSocketMessageType::Binary);
}

std::string DrogonSocketHandler::getAddress() {
	return address;
}

bool DrogonSocketHandler::isClosed() {
	return conn->disconnected();
}

int DrogonSocketHandler::writeWsData(void *d) {
	WebSocketRespponseData* wres = (WebSocketRespponseData*)d;
	if(!wres->isEmpty()) {
		for (int var = 0; var < (int)wres->getMore().size(); ++var) {
			if(wres->getMore()[var].hasData()) {
				if(wres->getMore()[var].getTextData().length()>0) {
					sendResponse(wres->getMore()[var].getTextData(), true);
				} else if(wres->getMore()[var].getBinaryData().length()>0) {
					sendResponse(wres->getMore()[var].getBinaryData(), false);
				}
			}
		}
	}
	return 1;
}
