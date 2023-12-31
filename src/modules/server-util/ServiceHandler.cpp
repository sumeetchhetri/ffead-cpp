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
 * ServiceHandler.cpp
 *
 *  Created on: 03-Jan-2015
 *      Author: sumeetc
 */

#include "ServiceHandler.h"

time_t ServiceHandler::rt;
struct tm ServiceHandler::ti;

bool ServiceHandler::isActive() {
	return run;
}

void ServiceHandler::closeConnectionsInternal() {
	BaseSocket* si;
	Timer t;
	t.start();
	int c = 0;
	while(toBeClosedConns.try_dequeue(si)) {
		if((Timer::getTimestamp() - si->cqat)>10 && si->useCounter==0) {
			cls(si);
		} else {
			c++;
			toBeClosedConns.enqueue(si);
		}
		if(t.elapsedMilliSeconds()>900) {
			CommonUtils::setDate();
			t.start();
		}
		if(c>=10) {
			break;
		}
	}
}

void* ServiceHandler::closeConnections(void *arg) {
	ServiceHandler* ths = (ServiceHandler*)arg;
	int counter = 0;
	while(ths->run) {
		CommonUtils::setDate();
		Thread::sSleep(1);
		if(counter++>=5) {
			ths->closeConnectionsInternal();
			counter = 0;
		}
	}
	return NULL;
}


void* ServiceHandler::timer(void *arg) {
	ServiceHandler* ths = (ServiceHandler*)arg;
	while(ths->run) {
		CommonUtils::setDate();
		Thread::sSleep(1);
	}
	return NULL;
}

void ServiceHandler::getDateStr(std::string& resp) {
	resp.append(CommonUtils::dateStr);
}

std::string ServiceHandler::getDateStr() {
	return std::string(CommonUtils::dateStr);
}

void ServiceHandler::closeConnection(BaseSocket* si) {
	si->cqat = Timer::getTimestamp();
	this->toBeClosedConns.enqueue(si);
}

void ServiceHandler::registerReadRequest(SocketInterface* sif) {
	handleRead(sif);
}

void ServiceHandler::registerWriteRequest(SocketInterface* sif) {
	handleWrite(sif);
}

void ServiceHandler::registerServiceRequest(void* request, SocketInterface* sif, void* context, int reqPos) {
	handleService(request, sif, context, reqPos);
}

void ServiceHandler::submitTask(Task* task) {
	if(isThreadPerRequests) {
		Thread* pthread = new Thread(&taskService, task);
		pthread->execute();
	} else {
		spool.submit(task);
	}
}

void* ServiceHandler::taskService(void* inp) {
	Task* task  = static_cast<Task*>(inp);
	task->run();
	return NULL;
}

void ServiceHandler::start(const CleanSocket& cls, bool withCCQ) {
	if(!run) {
		this->cls = cls;
		run = true;
		if(withCCQ) {
			Thread* mthread = new Thread(&closeConnections, this);
			mthread->execute(-1);
		}
		//Thread* tthread = new Thread(&timer, this);
		//tthread->execute();
	}
}

void ServiceHandler::stop() {
	if(spoolSize > 0) {
		spool.joinAll();
	}
	run = false;
	Thread::sSleep(15);
}

ServiceHandler::ServiceHandler(const int& spoolSize, bool isSinglEVH) {
	this->spoolSize = spoolSize;
	cls = NULL;
	run = false;
	isThreadPerRequests = false;
	if(!isSinglEVH) {
		if(spoolSize <= 0) {
			isThreadPerRequests = true;
		} else {
			spool.init(spoolSize, true);
		}
	}
}

ServiceHandler::~ServiceHandler() {
}

HandlerRequest::HandlerRequest() {
	sh = NULL;
	request = NULL;
	context = NULL;
	sif = NULL;
	reqPos = 0;
	response = NULL;
}

HandlerRequest::~HandlerRequest() {
	clearObjects();
}

void HandlerRequest::clearObjects() {
	if(request!=NULL) {
		if(getProtType()==1) {
			HttpRequest* req = (HttpRequest*)request;
			delete (HttpResponse*)req->resp;
			delete req;
		} else if(getProtType()==2) {
			delete (WebSocketData*)request;
			delete (WebSocketData*)response;
		}
	}
	request = NULL;
	if(context!=NULL){
		delete context;
	}
	context = NULL;
	response = NULL;
}

void* HandlerRequest::getContext() {
	return context;
}

int HandlerRequest::getProtType() const {
	return sif->getType(context);
}

void* HandlerRequest::getRequest() {
	return request;
}

void* HandlerRequest::getResponse() {
	return response;
}

bool HandlerRequest::isValidWriteRequest() {
	return sif->isCurrentRequest(reqPos);
}

bool HandlerRequest::doneWithWrite(int reqPos) {
	sif->endRequest(reqPos);
	return sif->allRequestsDone();
}

SocketInterface* HandlerRequest::getSif() {
	return sif;
}
