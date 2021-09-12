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

void* ServiceHandler::closeConnections(void *arg) {
	ServiceHandler* ths = (ServiceHandler*)arg;
	std::map<std::string, long long> addrs;
	std::map<std::string, BaseSocket*> sifMap;
	std::map<std::string, long long>::iterator it;
	while(ths->run) {
		Thread::sSleep(5);
		BaseSocket* si;
		while(ths->toBeClosedConns.try_dequeue(si)) {
			std::string as = si->address + CastUtil::fromNumber(si->fd);
			if(addrs.find(as)==addrs.end()) {
				addrs[as] = Timer::getTimestamp();
				sifMap[as] = si;
			}
		}
		for(it=addrs.begin();it!=addrs.end();) {
			long long t = Timer::getTimestamp();
			if(t-it->second>=15 && sifMap[it->first]->useCounter==0) {
				delete sifMap[it->first];
				sifMap.erase(it->first);
				addrs.erase(it++);
			} else {
				++it;
			}
		}
	}
	return NULL;
}

static const char* get_date() {
	time_t t;
	struct tm tm;
	static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	static __thread char date[59] = "Date: Thu, 01 Jan 1970 00:00:00 GMT\r\nServer: FFEAD 2.0\r\n";

	time(&t);
	gmtime_r(&t, &tm);
	strftime(date, 58, "Date: ---, %d --- %Y %H:%M:%S GMT\r\nServer: FFEAD 2.0\r\n", &tm);
	memcpy(date + 6, days[tm.tm_wday], 3);
	memcpy(date + 14, months[tm.tm_mon], 3);

	return date;
}

void* ServiceHandler::timer(void *arg) {
	ServiceHandler* ths = (ServiceHandler*)arg;
	while(ths->run) {
		CommonUtils::dateStr = get_date();
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


void ServiceHandler::start() {
	if(!run) {
		run = true;
		Thread* mthread = new Thread(&closeConnections, this);
		mthread->execute(-1);
		Thread* tthread = new Thread(&timer, this);
		tthread->execute();
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
