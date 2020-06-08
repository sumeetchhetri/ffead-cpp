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
 * DrogonServer.h
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#ifndef SRC_SERVER_Drogon_HTTP_DROGONSERVER_H_
#define SRC_SERVER_Drogon_HTTP_DROGONSERVER_H_
#include "ServerInitUtil.h"
#include <drogon/drogon.h>
#include <drogon/WebSocketController.h>
#include <drogon/HttpController.h>
#include <drogon/HttpSimpleController.h>
#include <libcuckoo/cuckoohash_map.hh>

using namespace drogon;

class DrogonWebsockHandler;

class DrogonSocketHandler : public SocketInterface {
	WebSocketConnection* conn;
	WebSockHandler* h;
	std::string address;
	friend class DrogonWebsockHandler;
public:
	DrogonSocketHandler(WebSocketConnection* conn);
	virtual ~DrogonSocketHandler();
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

class DrogonWebsockHandler : public drogon::WebSocketController<DrogonWebsockHandler>
{
public:
	void handleNewMessage(const WebSocketConnectionPtr&, std::string &&, const WebSocketMessageType &);
	void handleNewConnection(const HttpRequestPtr &, const WebSocketConnectionPtr&);
	void handleConnectionClosed(const WebSocketConnectionPtr&);
	WS_PATH_LIST_BEGIN
	WS_PATH_ADD("/");
	WS_PATH_LIST_END
};

class DrogonHttpHandler : public drogon::HttpController<DrogonHttpHandler>
{
public:
	void handle(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback);
	METHOD_LIST_BEGIN
	ADD_METHOD_VIA_REGEX(DrogonHttpHandler::handle, ".*", Get, Post, Put, Delete, Head, Options);
	METHOD_LIST_END
};

class DrogonServer {
	static libcuckoo::cuckoohash_map<std::string, DrogonSocketHandler*> websocks;
	friend class DrogonWebsockHandler;
public:
	static void runServer(std::string ip_addr, std::string port, std::vector<std::string> servedAppNames);
};

#endif /* SRC_SERVER_Drogon_HTTP_DROGONSERVER_H_ */
