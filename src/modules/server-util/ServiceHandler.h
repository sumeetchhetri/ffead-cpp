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
#include "Http11Handler.h"
#include "Http2Handler.h"
#include "Http11WebSocketHandler.h"
#include "ThreadPool.h"
#include "Thread.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "concurrentqueue.h"
#include "queue"
#include "map"
#include <stdint.h>
#include "ReusableInstanceHolder.h"

class ServiceHandler;

class HandlerRequest {
	void* request;
	SocketInterface* sif;
	void* context;
	void* response;
	int reqPos;
	int protType;
	ServiceHandler* sh;
	friend class ServiceHandler;
	friend class Http11Handler;
	friend class Http2Handler;
	friend class HttpServiceTask;
	friend class HttpReadTask;
	friend class HttpWriteTask;
	friend class HttpServiceHandler;
	HandlerRequest();
public:
	void* getContext();
	int getProtType() const;
	void* getRequest();
	void* getResponse();
	SocketInterface* getSif();
	bool isValidWriteRequest();
	bool doneWithWrite(int reqPos);
	void clearObjects();
	virtual ~HandlerRequest();
};

class ServiceHandler {
	static void* closeConnections(void *arg);
	moodycamel::ConcurrentQueue<SocketInterface*> toBeClosedConns;
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
	friend class HttpReadTask;
	virtual void sockInit(SocketInterface* si)=0;
protected:
	void submitTask(Task* task);
	virtual void handleService(void* request, SocketInterface* sif, void* context, int reqPos)=0;
	virtual void handleRead(SocketInterface* req)=0;
	virtual void handleWrite(SocketInterface* sif)=0;
public:
	void closeConnection(SocketInterface* si);
	void registerReadRequest(SocketInterface* si);
	void registerWriteRequest(SocketInterface* sif);
	void start();
	void stop();
	ServiceHandler(const int& spoolSize);
	virtual ~ServiceHandler();
};

#endif /* SERVICEHANDLER_H_ */
