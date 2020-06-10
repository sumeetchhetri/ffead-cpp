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
 * CinatraServer.h
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#ifndef SRC_SERVER_CINATRA_CINATRASERVER_H_
#define SRC_SERVER_CINATRA_CINATRASERVER_H_
#include "ServerInitUtil.h"
#include "cinatra.hpp"
#include <libcuckoo/cuckoohash_map.hh>

using namespace cinatra;

class CinatraSocketHandler : public SocketInterface {
	WebSockHandler* h;
	std::string address;
#ifdef CINATRA_ENABLE_SSL
	connection<cinatra::SSL>* sconn;
#else
	connection<cinatra::NonSSL>* pconn;
#endif
	friend class CinatraServer;
public:
#ifdef CINATRA_ENABLE_SSL
	CinatraSocketHandler(connection<cinatra::SSL>* sconn);
#else
	CinatraSocketHandler(connection<cinatra::NonSSL>* pconn);
#endif
	virtual ~CinatraSocketHandler();
	std::string getProtocol(void* context);
	int getType(void* context);
	int getTimeout();
	bool readRequest(void* request, void*& context, int& pending, int& reqPos);
	bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos);
	void onOpen();
	void onClose();
	void addHandler(SocketInterface* handler);
	void sendResponse(std::string, bool isText = true);
	std::string getAddress();
	bool isClosed();
	int writeWsData(void* d);
	bool isEmbedded();
};

class CinatraServer {
	static libcuckoo::cuckoohash_map<std::string, CinatraSocketHandler*> websocks;
public:
	static void runServer(std::string ip_addr, std::string port, std::vector<std::string> servedAppNames);
};

#endif /* SRC_SERVER_CINATRA_CINATRASERVER_H_ */
