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
 * CinatraServer.cpp
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#include "CinatraServer.h"

static Logger logger;
libcuckoo::cuckoohash_map<std::string, CinatraSocketHandler*> CinatraServer::websocks;

void CinatraServer::runServer(std::string ip_addr, std::string port, std::vector<std::string> servedAppNames) {
	std::function<void(request&, response&)> ffead_cpp_handler = [&](request& r, response& response) {
		if(r.get_content_type() == content_type::websocket) {
			r.on(ws_open, [](request& req) {
				CinatraSocketHandler* csk = NULL;
				#ifdef CINATRA_ENABLE_SSL
					csk = new CinatraSocketHandler(req.get_raw_conn<cinatra::SSL>());
				#else
					csk = new CinatraSocketHandler(req.get_raw_conn<cinatra::NonSSL>());
				#endif
				websocks.insert(csk->getAddress(), csk);

				std::pair<phr_header*, size_t> hl = req.get_headers();
				HttpRequest reqf(hl.first, hl.second, req.raw_url(), req.get_method(), req.minor_version(), req.body());

				WebSocketData wreq;
				WebSocketRespponseData wres;
				csk->h = ServiceTask::handleWebsockOpen(&wreq, &wres, csk, &reqf);
				csk->writeWsData(&wres);
			});

			r.on(ws_message, [](request& req) {
				std::string address;
				#ifdef CINATRA_ENABLE_SSL
					address = StringUtil::toHEX((long long)req.get_raw_conn<cinatra::SSL>());
				#else
					address = StringUtil::toHEX((long long)req.get_raw_conn<cinatra::NonSSL>());
				#endif
				if(websocks.contains(address)) {
					CinatraSocketHandler* csk = websocks.find(address);

					auto part_data = req.get_part_data();
					std::string str = std::string(part_data.data(), part_data.length());

					WebSocketData wreq;
					wreq.collectText(str);
					WebSocketRespponseData wres;

					if(csk->h->onMessage(&wreq, &wres, address)) {
						if(!csk->h->isWriteControl()) {
							csk->writeWsData(&wres);
						}
					}
				}
			});

			r.on(ws_close, [](request& req) {
				std::string address;
				#ifdef CINATRA_ENABLE_SSL
					address = StringUtil::toHEX((long long)req.get_raw_conn<cinatra::SSL>());
				#else
					address = StringUtil::toHEX((long long)req.get_raw_conn<cinatra::NonSSL>());
				#endif
				if(websocks.contains(address)) {
					CinatraSocketHandler* csk = websocks.find(address);
					csk->onClose();
					websocks.erase(address);
				}
			});

			r.on(ws_error, [](request& req) {
				std::string address;
				#ifdef CINATRA_ENABLE_SSL
					address = StringUtil::toHEX((long long)req.get_raw_conn<cinatra::SSL>());
				#else
					address = StringUtil::toHEX((long long)req.get_raw_conn<cinatra::NonSSL>());
				#endif
				if(websocks.contains(address)) {
					CinatraSocketHandler* csk = websocks.find(address);
					csk->onClose();
					websocks.erase(address);
				}
			});
		} else {
			std::pair<phr_header*, size_t> hl = r.get_headers();
			HttpRequest req(hl.first, hl.second, r.raw_url(), r.get_method(), r.minor_version(), r.body());
			HttpResponse respo;
			ServiceTask::handle(&req, &respo);
			if(respo.isDone()) {
				for (int var = 0; var < (int)respo.getCookies().size(); var++)
				{
					std::string val = respo.getCookies().at(var);
					response.add_header("Set-Cookie", std::move(val));
				}
				RMap::const_iterator it;
				for(it=respo.getCHeaders().begin();it!=respo.getCHeaders().end();++it) {
					std::string key = it->first;
					std::string val = it->second;
					response.add_header(std::move(key), std::move(val));
				}

				status_type st = static_cast<status_type>(respo.getCode());
				std::string& data = respo.generateNginxApacheResponse();
				int contentLength = data.length();
				response.enable_response_time(true);
				response.set_status_and_content(st, std::move(data));
			} else {
				response.enable_response_time(true);
				if(access(req.getUrl().c_str(), F_OK) != -1) {
					response.set_static_file_abs_url(req.getUrl());
				} else {
					response.set_status_and_content(status_type::not_found);
				}
			}
		}
	};

	int max_thread_num = std::thread::hardware_concurrency();
	http_server server(max_thread_num);
	server.listen(ip_addr, port);
	server.set_http_handler<GET, PUT, POST, DEL, OPTIONS>("/*", ffead_cpp_handler);
	server.run();
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

	ServerInitUtil::bootstrap(serverRootDirectory, logger, SERVER_BACKEND::CINATRA);
	ServerInitUtil::init(logger);
	CinatraServer::runServer(ipaddr, port, servedAppNames);

	/*std::string serverCntrlFileNm = serverRootDirectory + "ffead.cntrl";
	struct stat buffer;
	while(stat (serverCntrlFileNm.c_str(), &buffer) == 0)
	{
		Thread::sSleep(10);
	}*/
	// Build an api.
}

std::string CinatraSocketHandler::getProtocol(void *context) {
	return "";
}

int CinatraSocketHandler::getType(void *context) {
	return 0;
}

int CinatraSocketHandler::getTimeout() {
	return -1;
}

bool CinatraSocketHandler::readRequest(void *request, void *&context, int &pending, int &reqPos) {
	return false;
}

bool CinatraSocketHandler::writeResponse(void *req, void *res, void *context, std::string &data, int reqPos) {
	return false;
}

void CinatraSocketHandler::onOpen() {
}

void CinatraSocketHandler::onClose() {
	h->onClose(getAddress());
}

void CinatraSocketHandler::addHandler(SocketInterface *handler) {
}

int CinatraSocketHandler::writeWsData(void* d) {
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

#ifdef CINATRA_ENABLE_SSL
CinatraSocketHandler::CinatraSocketHandler(connection<cinatra::SSL>* sconn) {
	this->sconn = sconn;
	address = StringUtil::toHEX((long long)sconn);
	this->h = NULL;
}
#else
CinatraSocketHandler::CinatraSocketHandler(connection<cinatra::NonSSL>* pconn) {
	this->pconn = pconn;
	address = StringUtil::toHEX((long long)pconn);
	this->h = NULL;
}
#endif

CinatraSocketHandler::~CinatraSocketHandler() {
	delete h;
}

void CinatraSocketHandler::sendResponse(std::string str, bool isText) {
#ifdef CINATRA_ENABLE_SSL
	isText?sconn->send_ws_string(str):sconn->send_ws_binary(str);
#else
	isText?pconn->send_ws_string(str):pconn->send_ws_binary(str);
#endif
}

std::string CinatraSocketHandler::getAddress() {
	return address;
}

bool CinatraSocketHandler::isClosed() {
#ifdef CINATRA_ENABLE_SSL
	return sconn->has_close();
#else
	return pconn->has_close();
#endif
}

bool CinatraSocketHandler::isEmbedded() {
	return false;
}
