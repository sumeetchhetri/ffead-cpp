/*
 * ServiceHandler.cpp
 *
 *  Created on: 03-Jan-2015
 *      Author: sumeetc
 */

#include "ServiceHandler.h"

bool ServiceHandler::isActive() {
	return run;
}

void* ServiceHandler::closeConnections(void *arg) {
	ServiceHandler* ths = (ServiceHandler*)arg;
	std::map<std::string, long long> addrs;
	std::map<std::string, SocketInterface*> sifMap;
	std::map<std::string, long long>::iterator it;
	while(ths->run) {
		Thread::sSleep(5);
		SocketInterface* si;
		while(ths->toBeClosedConns.try_dequeue(si)) {
			std::string as = si->address + CastUtil::lexical_cast<std::string>(si->fd);
			if(addrs.find(as)==addrs.end()) {
				addrs[as] = Timer::getTimestamp();
				sifMap[as] = si;
			}
		}
		for(it=addrs.begin();it!=addrs.end();) {
			long long t = Timer::getTimestamp();
			if(t-it->second>=15) {
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

void ServiceHandler::closeConnection(SocketInterface* si) {
	this->toBeClosedConns.enqueue(si);
}

void ServiceHandler::registerReadRequest(SocketInterface* sif) {
	handleRead(sif);
}

void ServiceHandler::registerWriteRequest(SocketInterface* sif) {
	handleWrite(sif);
}

void ServiceHandler::registerServiceRequest(void* request, SocketInterface* sif, void* context, int reqPos) {
	HandlerRequest* req = new HandlerRequest;
	req->request = request;
	req->response = NULL;
	req->sif = sif;
	req->context = context;
	req->sh = this;
	req->reqPos = reqPos;
	req->protocol = sif->getProtocol(context);
	handleService(req);
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
	}
}

void ServiceHandler::stop() {
	if(spoolSize > 0) {
		spool.joinAll();
	}
	run = false;
	Thread::sSleep(15);
}

ServiceHandler::ServiceHandler(const int& spoolSize) {
	this->spoolSize = spoolSize;
	run = false;
	isThreadPerRequests = false;
	if(spoolSize <= 0) {
		isThreadPerRequests = true;
	} else {
		spool.init(spoolSize, true);
	}
}

ServiceHandler::~ServiceHandler() {
}

HandlerRequest::HandlerRequest() {
	sh = NULL;
	request = NULL;
	context = NULL;
	sif = NULL;
	protocol = "";
	reqPos = 0;
	response = NULL;
}

HandlerRequest::~HandlerRequest() {
	clearObjects();
}

void HandlerRequest::clearObjects() {
	if(request!=NULL)delete request;
	request = NULL;
	if(context!=NULL)delete context;
	context = NULL;
	if(response!=NULL)delete response;
	response = NULL;
}

void* HandlerRequest::getContext() {
	return context;
}

const std::string& HandlerRequest::getProtocol() const {
	return protocol;
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
