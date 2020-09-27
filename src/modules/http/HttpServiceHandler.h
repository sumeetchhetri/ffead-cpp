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
 * HttpServiceHandler.h
 *
 *  Created on: 07-Jan-2015
 *      Author: sumeetc
 */

#ifndef HTTPSERVICEHANDLER_H_
#define HTTPSERVICEHANDLER_H_
#include "ServiceHandler.h"
#include "Task.h"
#include "CommonUtils.h"
#include "WebSockHandler.h"

class HttpServiceHandler;

class HttpServiceTask : public Task {
	HandlerRequest handlerRequest;
	HttpServiceHandler* service;
	void run();
	friend class HttpReadTask;
	friend class HttpServiceHandler;
	friend class CHServer;
public:
	int getTid();
	void setTid(int tid);
	virtual ~HttpServiceTask();
	HttpServiceTask();
	virtual std::string getCntEncoding();
	HttpServiceTask(ReusableInstanceHolder* h);
	virtual bool handle(HttpRequest* request, HttpResponse* response, SocketInterface* sif)=0;
	virtual WebSockHandler* handleWebsockOpen(WebSocketData* request, WebSocketRespponseData* response, SocketInterface* sif, HttpRequest* hreq)=0;
};

class HttpReadTask : public Task {
	SocketInterface* sif;
	HttpServiceHandler* service;
	void run();
	friend class HttpServiceHandler;
	friend class CHServer;
public:
	int getTid();
	void setTid(int tid);
	virtual ~HttpReadTask();
	HttpReadTask();
};

class HttpWriteTask : public Task {
	SocketInterface* sif;
	HttpServiceHandler* service;
	void run();
	friend class HttpServiceHandler;
	friend class CHServer;
public:
	int getTid();
	void setTid(int tid);
	virtual ~HttpWriteTask();
	HttpWriteTask();
};

typedef HttpServiceTask* (*HttpServiceTaskFactory) ();
typedef HttpReadTask* (*HttpReadTaskFactory) ();

class HttpServiceHandler : public ServiceHandler {
	std::string cntEncoding;
	HttpServiceTaskFactory f;
	HttpReadTaskFactory fr;
	friend class HttpServiceTask;
	friend class HttpReadTask;
	void sockInit(SocketInterface* si);
	void handleService(void* request, SocketInterface* sif, void* context, int reqPos);
	void handleRead(SocketInterface* sif);
	void handleWrite(SocketInterface* sif);
public:
	HttpServiceHandler(const std::string& cntEncoding, const HttpServiceTaskFactory& f, const int& spoolSize, bool isSinglEVH, const HttpReadTaskFactory& fr);
	virtual ~HttpServiceHandler();
};

#endif /* HTTPSERVICEHANDLER_H_ */
