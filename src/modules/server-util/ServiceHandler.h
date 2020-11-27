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
#include "CommonUtils.h"

class ServiceHandler;

class HandlerRequest {
	void* request;
	SocketInterface* sif;
	void* context;
	void* response;
	int reqPos;
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
	static void* timer(void *arg);
	moodycamel::ConcurrentQueue<SocketInterface*> toBeClosedConns;
	std::atomic<bool> run;
	bool isThreadPerRequests;
	int spoolSize;
	ThreadPool spool;
	static time_t rt;
	static struct tm ti;
	bool addOpenRequest(SocketInterface* si);
	void addCloseRequest(SocketInterface* si);
	void registerServiceRequest(void* request, SocketInterface* sif, void* context, int reqPos);
	bool isActive();
	static void* taskService(void* inp);
	friend class RequestReaderHandler;
	friend class HandlerRequest;
	friend class HttpReadTask;
	friend class HttpServiceTask;
	virtual void sockInit(SocketInterface* si)=0;
protected:
	static std::string getDateStr();
	static void getDateStr(std::string&);
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
	ServiceHandler(const int& spoolSize, bool isSinglEVH);
	virtual ~ServiceHandler();
};

#endif /* SERVICEHANDLER_H_ */
