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
	HttpServiceTask(HandlerRequest* handlerRequest, HttpServiceHandler* service);
	void run();
	friend class HttpServiceHandler;
public:
	virtual ~HttpServiceTask();
	HttpServiceTask();
	virtual void handle(HttpRequest* request, HttpResponse* response)=0;
	virtual void handleWebsockOpen(WebSocketData* request)=0;
	virtual void handleWebsockClose(WebSocketData* request)=0;
	virtual void handleWebsockMessage(const std::string& url, WebSocketData* request, WebSocketData* response)=0;
};

class HttpWriteTask : public Task {
	HandlerRequest* handlerRequest;
	HttpServiceHandler* service;
	void run();
	friend class HttpServiceHandler;
public:
	virtual ~HttpWriteTask();
	HttpWriteTask();
};

class HttpReadTask : public Task {
	HandlerRequest* handlerRequest;
	HttpServiceHandler* service;
	void run();
	friend class HttpServiceHandler;
public:
	virtual ~HttpReadTask();
	HttpReadTask();
};

typedef HttpServiceTask* (*HttpServiceTaskFactory) ();

class HttpServiceHandler : public ServiceHandler {
	std::string cntEncoding;
	HttpServiceTaskFactory f;
	friend class HttpServiceTask;
	friend class HttpWriteTask;
	void handleService(HandlerRequest* handlerRequest);
	void handleWrite(HandlerRequest* handlerRequest);
	void handleRead(HandlerRequest* handlerRequest);
public:
	HttpServiceHandler(const std::string& cntEncoding, const HttpServiceTaskFactory& f, const int& spoolSize);
	virtual ~HttpServiceHandler();
};

#endif /* HTTPSERVICEHANDLER_H_ */
