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
 * PeerServerController.h
 *
 *  Created on: Apr 17, 2020
 *      Author: sumeet
 */

#ifndef PEERSERVERCONTROLLER_H_
#define PEERSERVERCONTROLLER_H_
#include <iostream>
#include "WebSockHandler.h"
#include "JSONUtil.h"
#include "Router.h"
#include "CacheManager.h"
#ifdef HAVE_UUIDINC
#include <uuid/uuid.h>
#elif defined(HAVE_BSDUUIDINC)
#include <uuid.h>
#elif defined(HAVE_OSSPUUIDINC)
#include <ossp/uuid.h>
#elif defined(HAVE_OSSPUUIDINC_2)
#include <uuid.h>
#endif
#include "yuarel.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "Timer.h"
#include "Thread.h"
#include "ConfigurationData.h"

class PeerState {
	std::string id;
	std::string token;
	BaseSocket* sif;
	PeerState(const std::string& uniqueAddress, const std::string& token, BaseSocket* sif);
	friend class PeerServerRouter;
	friend class PeerServerController;
};

class PeerServerRouter : public Router {
	std::string authKey;
	bool auth(std::string_view path, HttpResponse* res, std::string& key, std::string& id, std::string& token);
	bool isAuthorizedKey(const std::string &key, HttpResponse* res);
	bool isValidUser(const std::string& id, const std::string& token, HttpResponse* res);
public:
	bool route(HttpRequest* req, HttpResponse* res, Router* sif);
	PeerServerRouter();
	virtual ~PeerServerRouter();
};

class PeerServerController : public WebSockHandler {
	std::string nodeIdentifier;
	std::string authKey;
	bool isDistributedMode;
	volatile bool runHandler;
	static void* handle(void* inp);
	static std::string generateId();
	libcuckoo::cuckoohash_map<std::string, PeerState*> clients;
	libcuckoo::cuckoohash_map<std::string, std::string> clientIds;
	friend class PeerServerRouter;
public:
	PeerServerController();
	virtual ~PeerServerController();

	bool isWriteControl();
	bool onOpen(WebSocketData* req, WebSocketRespponseData* res, std::string uniqueAddress, void* hreq);
	void onClose(std::string uniqueAddress);
	bool onMessage(WebSocketData* req, WebSocketRespponseData* res, std::string uniqueAddress);
};

#endif /* PEERSERVERCONTROLLER_H_ */
