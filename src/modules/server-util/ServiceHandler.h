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
#include "SynchronizedQueue.h"
#include "SynchronizedMap.h"

class ServiceHandler;

class HandlerRequest {
	void* request;
	SocketInterface* sif;
	void* context;
	bool sentResponse;
	void* response;
	int reqPos;
	std::string protocol;
	ServiceHandler* sh;
	ReaderSwitchInterface* switchReaderIntf;
	friend class ServiceHandler;
	friend class HttpWriteTask;
	friend class HttpServiceTask;
	HandlerRequest();
public:
	SocketUtil* getSocketUtil();
	void setSentResponse();
	virtual ~HandlerRequest();
	void* getContext();
	const std::string& getProtocol() const;
	void* getRequest();
	void* getResponse();
	bool isSentResponse() const;
	SocketInterface* getSif();
	ReaderSwitchInterface* getSwitchReaderIntf();
	bool isValidWriteRequest();
	bool doneWithWrite();
	void clearObjects();
};

class ServiceHandler {
	Mutex mutex;
	SynchronizedQueue<SocketInterface*> tbcSifQ;
	SynchronizedMap<long, int> requestNumMap;
	SynchronizedMap<long, bool> donelist;
	bool run;
	bool isThreadPerRequests;
	bool isThreadPerRequestw;
	int spoolSize;
	int wpoolSize;
	ThreadPool spool;
	ThreadPool wpool;
	bool addOpenRequest(SocketInterface* si);
	void addCloseRequest(SocketInterface* si);
	bool isAvailable(SocketInterface* si);
	void registerServiceRequest(void* request, SocketInterface* sif, void* context, int reqPos, ReaderSwitchInterface* switchReaderIntf);
	bool isActive();
	static void* taskService(void* inp);
	static void* cleanSifs(void* inp);
	void flagDone(SocketInterface* si);
	void cleanSif(std::map<int, SocketInterface*> connectionsWithTimeouts);
	friend class RequestReaderHandler;
	friend class HandlerRequest;
	friend class HttpWriteTask;
protected:
	void submitServiceTask(Task* task);
	void submitWriteTask(Task* task);
	virtual void handleService(HandlerRequest* req)=0;
	virtual void handleWrite(HandlerRequest* req)=0;
public:
	void switchReaders(HandlerRequest* hr, SocketInterface* next);
	void registerWriteRequest(HandlerRequest* request, void* response);
	void registerRead(HandlerRequest* hr);
	void start();
	void stop();
	ServiceHandler(const int& spoolSize, const int& wpoolSize);
	virtual ~ServiceHandler();
};

#endif /* SERVICEHANDLER_H_ */
