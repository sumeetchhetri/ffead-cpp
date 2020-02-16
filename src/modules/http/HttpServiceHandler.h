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
	HandlerRequest handlerRequest;
	HttpServiceHandler* service;
	time_t rt;
	struct tm ti;
	char buffer[31];
	void run();
	friend class HttpReadTask;
	friend class HttpServiceHandler;
	friend class CHServer;
public:
	int getTid();
	void setTid(int tid);
	virtual ~HttpServiceTask();
	HttpServiceTask();
	std::string getCntEncoding();
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
	HttpServiceHandler(const std::string& cntEncoding, const HttpServiceTaskFactory& f, const int& spoolSize, const HttpReadTaskFactory& fr);
	virtual ~HttpServiceHandler();
};

#endif /* HTTPSERVICEHANDLER_H_ */
