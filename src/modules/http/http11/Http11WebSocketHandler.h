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
/*
 * Http11WebSocketHandler.h
 *
 *  Created on: 30-Nov-2014
 *      Author: sumeetc
 */

#ifndef HTTP11WEBSOCKETHANDLER_H_
#define HTTP11WEBSOCKETHANDLER_H_
#include "Compatibility.h"
#include "map"
#include "vector"
#include "string"
#include "string"
#include <unistd.h>
#include <sys/types.h>
#include "Http11WebSocketDataFrame.h"
#include "cstring"
#include <iostream>
#include <bitset>
#include "string"
#include "stdint.h"
#include "vector"
#include "CommonUtils.h"
#include "SocketInterface.h"
#include "SocketInterface.h"
#include "LoggerFactory.h"
#include "WebSockHandler.h"
#define MAXBUFLENMWS 32768

class Http11WebSocketHandler : public SocketInterface {
	Logger logger;
	std::string url;
	WebSockHandler* h;
	std::map<int, std::string> dataframes;
	std::map<int, bool> dataframesComplete;
	short lastOpCode;
	void replyPong();
	bool processFrame(Http11WebSocketDataFrame* frame, WebSocketData* request, bool& isReq);
	bool nextFrame(Http11WebSocketDataFrame* frame);
	friend class HttpServiceTask;
public:
	void addHandler(SocketInterface* handler);
	std::string getUrl();
	std::string getProtocol(void* context);
	int getType(void* context);
	int getTimeout();
	bool readRequest(void* request, void*& context, int& pending, int& reqPos);
	bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos);
	Http11WebSocketHandler(const SOCKET& fd, void* ssl, void* io, const std::string& url, const bool& isServer);
	virtual ~Http11WebSocketHandler();
	bool isEmbedded();
};

#endif /* HTTP11WEBSOCKETHANDLER_H_ */
