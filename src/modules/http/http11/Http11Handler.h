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
 * Http11Handler.h
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#ifndef HTTP11HANDLER_H_
#define HTTP11HANDLER_H_
#include "SocketInterface.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class Http11Handler : public SocketInterface {
	bool isHeadersDone;
	int bytesToRead;
	bool isTeRequest;

	int chunkSize;
	int connKeepAlive;
	int maxReqHdrCnt;
	int maxEntitySize;
	SocketInterface* handler;
	friend class CHServer;
	friend class HttpServiceHandler;
	friend class HttpServiceTask;
public:
	int readFrom();
	void addHandler(SocketInterface* handler);
	std::string getProtocol(void* context);
	int getType(void* context);
	int getTimeout();
	bool readRequest(void* request, void*& context, int& pending, int& reqPos);
	bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos);
	Http11Handler(const SOCKET& fd, void* ssl, void* io, const int& chunkSize,
			const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize);
	virtual ~Http11Handler();
	bool isEmbedded();
	bool hasPendingRead();
};

#endif /* HTTP11HANDLER_H_ */
