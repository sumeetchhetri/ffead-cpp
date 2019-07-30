/*
 * ServiceHandler.h
 *
 *  Created on: 03-Jan-2015
 *      Author: sumeetc
 */

#ifndef SERVICEHANDLER_H_
#define SERVICEHANDLER_H_
#include "Thread.h"
#include "Mutex.h"
#include "queue"
#include "ReaderSwitchInterface.h"
#include "Http11Handler.h"
#include "Http2Handler.h"
#include "Http11WebSocketHandler.h"
#include "ThreadPool.h"
#include "Thread.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "concurrentqueue.h"

class ServiceHandler;

class HandlerRequest {
	void* request;
	SocketInterface* sif;
	void* context;
	void* response;
	int reqPos;
	std::string protocol;
	ServiceHandler* sh;
	friend class ServiceHandler;
	friend class HttpWriteTask;
	friend class HttpServiceTask;
	HandlerRequest();
public:
	SocketUtil* getSocketUtil();
	virtual ~HandlerRequest();
	void* getContext();
	const std::string& getProtocol() const;
	void* getRequest();
	void* getResponse();
	SocketInterface* getSif();
	bool isValidWriteRequest();
	bool doneWithWrite();
	void clearObjects();
};

class ServiceHandler {
	std::atomic<bool> run;
	bool isThreadPerRequests;
	int spoolSize;
	ThreadPool spool;
	bool addOpenRequest(SocketInterface* si);
	void addCloseRequest(SocketInterface* si);
	void registerServiceRequest(void* request, SocketInterface* sif, void* context, int reqPos);
	bool isActive();
	static void* taskService(void* inp);
	friend class RequestReaderHandler;
	friend class HandlerRequest;
	friend class HttpWriteTask;
protected:
	void submitTask(Task* task);
	virtual void handleService(HandlerRequest* req)=0;
	virtual void handleWrite(HandlerRequest* req)=0;
	virtual void handleRead(HandlerRequest* req)=0;
public:
	void registerWriteRequest(HandlerRequest* request, void* response);
	void registerReadRequest(SocketInterface* si);
	void start();
	void stop();
	ServiceHandler(const int& spoolSize);
	virtual ~ServiceHandler();
};

#endif /* SERVICEHANDLER_H_ */
