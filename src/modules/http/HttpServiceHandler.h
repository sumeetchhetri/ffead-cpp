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

class HttpServiceHandler;

class HttpServiceTask : public Task {
	HandlerRequest* handlerRequest;
	HttpServiceHandler* service;
	void run();
	friend class HttpServiceHandler;
	friend class CHServer;
public:
	virtual ~HttpServiceTask();
	HttpServiceTask(ReusableInstanceHolder* h);
	virtual void handle(HttpRequest* request, HttpResponse* response)=0;
	virtual void handleWebsockOpen(WebSocketData* request)=0;
	virtual void handleWebsockClose(WebSocketData* request)=0;
	virtual void handleWebsockMessage(const std::string& url, WebSocketData* request, WebSocketData* response)=0;
};

class HttpReadTask : public Task {
	SocketInterface* sif;
	HttpServiceHandler* service;
	void run();
	friend class HttpServiceHandler;
	friend class CHServer;
public:
	virtual ~HttpReadTask();
	HttpReadTask(ReusableInstanceHolder* h);
};

typedef HttpServiceTask* (*HttpServiceTaskFactory) ();
typedef HttpReadTask* (*HttpReadTaskFactory) ();

class HttpServiceHandler : public ServiceHandler {
	std::string cntEncoding;
	HttpServiceTaskFactory f;
	HttpReadTaskFactory fr;
	friend class HttpServiceTask;
	void handleService(HandlerRequest* handlerRequest);
	void handleRead(SocketInterface* sif);
public:
	HttpServiceHandler(ReusableInstanceHolder* h1Hldr, ReusableInstanceHolder* h2Hldr, const std::string& cntEncoding, const HttpServiceTaskFactory& f, const int& spoolSize, const HttpReadTaskFactory& fr);
	virtual ~HttpServiceHandler();
};

#endif /* HTTPSERVICEHANDLER_H_ */
