/*
 * ServiceHandler.cpp
 *
 *  Created on: 03-Jan-2015
 *      Author: sumeetc
 */

#include "ServiceHandler.h"

bool ServiceHandler::isActive() {
	bool fl = false;
	mutex.lock();
	fl = run;
	mutex.unlock();
	return fl;
}

void ServiceHandler::registerRequest(void* request, SocketInterface* sif, void* context, ReaderSwitchInterface* switchReaderIntf) {
	HandlerRequest* req = NULL;
	int val;
	if(requestNumMap.find(sif->identifier, val))
	{
		req = new HandlerRequest;
		req->request = request;
		req->sif = sif;
		req->context = context;
		req->sh = this;
		req->switchReaderIntf = switchReaderIntf;
		req->protocol = sif->getProtocol(context);
		requestNumMap.put(sif->identifier, val+1);
	}
	else
	{
		if(request!=NULL)
			delete request;
		if(context!=NULL)
			delete context;
	}
	if(req!=NULL) {
		service(req);
	}
}

bool ServiceHandler::addOpenRequest(SocketInterface* si) {
	requestNumMap.put(si->identifier, 0);
	return true;
}

void ServiceHandler::flagDone(SocketInterface* sif) {
	int val;
	if(requestNumMap.find(sif->identifier, val)) {
		requestNumMap.put(sif->identifier, val-1);
	}
}

bool ServiceHandler::isAvailable(SocketInterface* sif) {
	return requestNumMap.find(sif->identifier);
}

void ServiceHandler::addCloseRequest(SocketInterface* si) {
	tbcSifQ.push(si);
	std::cout << "Closing connection " << si->getDescriptor() << " " << si->identifier << std::endl;
}

void ServiceHandler::submitTask(Task* task) {
	if(isThreadPerRequest) {
		Thread pthread(&taskService, task);
		pthread.execute();
	} else {
		pool.submit(task);
	}
}

void* ServiceHandler::taskService(void* inp) {
	Task* task  = static_cast<Task*>(inp);
	task->run();
	return NULL;
}

void* ServiceHandler::cleanSifs(void* inp) {
	ServiceHandler* ins  = static_cast<ServiceHandler*>(inp);
	while(ins->isActive())
	{
		SocketInterface* si = NULL;
		while(ins->tbcSifQ.pop(si))
		{
			int val;
			if(ins->requestNumMap.find(si->identifier, val) && val<=0)
			{
				std::cout << "Connection resources released " << si->getDescriptor() << " " << si->identifier << std::endl;
				delete si->sockUtil;
				delete si;
				ins->requestNumMap.erase(si->identifier);
			}
			else
			{
				ins->tbcSifQ.push(si);
			}
			Thread::mSleep(10);
		}
		Thread::sSleep(1);
	}
	return NULL;
}

void ServiceHandler::cleanSif(std::map<int, SocketInterface*> connectionsWithTimeouts) {
	SocketInterface* si = NULL;
	while(tbcSifQ.pop(si))
	{
		int val;
		if(requestNumMap.find(si->identifier, val) && val<=0)
		{
			std::cout << "Connection resources released " << si->getDescriptor() << " " << si->identifier << std::endl;
			connectionsWithTimeouts.erase(si->getDescriptor());
			delete si->sockUtil;
			delete si;
			requestNumMap.erase(si->identifier);
		}
		else
		{
			tbcSifQ.push(si);
		}
		Thread::mSleep(1);
	}
}

void ServiceHandler::start() {
	if(!run) {
		mutex.lock();
		if(run) {
			mutex.unlock();
			return;
		}
		run = true;
		//Thread csifthr(&cleanSifs, this);
		//csifthr.execute();
		mutex.unlock();
	}
}

void ServiceHandler::stop() {
	mutex.lock();
	run = false;
	mutex.unlock();
}

ServiceHandler::ServiceHandler(const int& poolSize) {
	this->poolSize = poolSize;
	run = false;
	isThreadPerRequest = false;
	if(poolSize <= 0) {
		isThreadPerRequest = true;
	} else {
		pool.init(poolSize);
	}
}

ServiceHandler::~ServiceHandler() {
	stop();
}

HandlerRequest::HandlerRequest() {
	sh = NULL;
	request = NULL;
	context = NULL;
	sif = NULL;
	switchReaderIntf = NULL;
	sentResponse = false;
	protocol = "";
}

HandlerRequest::~HandlerRequest() {
	sh->flagDone(sif);
}

SocketUtil* HandlerRequest::getSocketUtil() {
	return sif->sockUtil;
}

void HandlerRequest::setSentResponse() {
	sentResponse = true;
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

bool HandlerRequest::isSentResponse() const {
	return sentResponse;
}

SocketInterface* HandlerRequest::getSif() {
	return sif;
}

ReaderSwitchInterface* HandlerRequest::getSwitchReaderIntf() {
	return switchReaderIntf;
}

void ServiceHandler::switchReaders(HandlerRequest* hr, SocketInterface* next) {
	hr->switchReaderIntf->switchReaders(hr->getSif(), next);
}
