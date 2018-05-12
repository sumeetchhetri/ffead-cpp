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

void ServiceHandler::registerWriteRequest(HandlerRequest* request, void* response) {
	if(request!=NULL && response!=NULL) {
		request->response = response;
		handleWrite(request);
	}
}

void ServiceHandler::registerServiceRequest(void* request, SocketInterface* sif, void* context, int reqPos, ReaderSwitchInterface* switchReaderIntf) {
	HandlerRequest* req = NULL;
	//int val;
	//if(requestNumMap.find(sif->identifier, val))
	//{
		req = new HandlerRequest;
		req->request = request;
		req->response = NULL;
		req->sif = sif;
		req->context = context;
		req->sh = this;
		req->reqPos = reqPos;
		req->switchReaderIntf = switchReaderIntf;
		req->protocol = sif->getProtocol(context);
		//requestNumMap.put(sif->identifier, val+1);
	/*}
	else
	{
		if(request!=NULL)
			delete request;
		if(context!=NULL)
			delete context;
	}
	if(req!=NULL) {*/
		handleService(req);
	//}
}

bool ServiceHandler::addOpenRequest(SocketInterface* si) {
	//requestNumMap.put(si->identifier, 0);
	return true;
}

void ServiceHandler::flagDone(SocketInterface* sif) {
	/*int val;
	if(requestNumMap.find(sif->identifier, val)) {
		requestNumMap.put(sif->identifier, val-1);
	}*/
}

bool ServiceHandler::isAvailable(SocketInterface* sif) {
	//return requestNumMap.find(sif->identifier);
	return true;
}

void ServiceHandler::addCloseRequest(SocketInterface* si) {
	//tbcSifQ.push(si);
	//std::cout << "Closing connection " << si->getDescriptor() << " " << si->identifier << std::endl;
}

void ServiceHandler::submitServiceTask(Task* task) {
	if(isThreadPerRequests) {
		Thread* pthread = new Thread(&taskService, task);
		pthread->execute();
	} else {
		spool.submit(task);
	}
}

void ServiceHandler::submitWriteTask(Task* task) {
	if(isThreadPerRequestw) {
		Thread* pthread = new Thread(&taskService, task);
		pthread->execute();
	} else {
		wpool.submit(task);
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
				//std::cout << "Connection resources released " << si->getDescriptor() << " " << si->identifier << std::endl;
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
			//std::cout << "Connection resources released " << si->getDescriptor() << " " << si->identifier << std::endl;
			connectionsWithTimeouts.erase(si->getDescriptor());
			requestNumMap.erase(si->identifier);
			delete si;
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
		//Thread* pthread = new Thread(&cleanSifs, this);
		//pthread->execute();
		mutex.unlock();
	}
}

void ServiceHandler::stop() {
	if(spoolSize > 0) {
		spool.joinAll();
	}
	if(wpoolSize > 0) {
		wpool.joinAll();
	}
	mutex.lock();
	run = false;
	mutex.unlock();
}

ServiceHandler::ServiceHandler(const int& spoolSize, const int& wpoolSize) {
	this->spoolSize = spoolSize;
	this->wpoolSize = wpoolSize;
	run = false;
	isThreadPerRequests = false;
	isThreadPerRequestw = false;
	if(spoolSize <= 0) {
		isThreadPerRequests = true;
	} else {
		spool.init(spoolSize);
	}
	if(wpoolSize <= 0) {
		isThreadPerRequestw = true;
	} else {
		wpool.init(wpoolSize);
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
	reqPos = 0;
}

HandlerRequest::~HandlerRequest() {
	sh->flagDone(sif);
}

SocketUtil* HandlerRequest::getSocketUtil() {
	return &(sif->sockUtil);
}

void HandlerRequest::clearObjects() {
	if(request!=NULL)delete request;
	request = NULL;
	if(context!=NULL)delete context;
	context = NULL;
	if(response!=NULL)delete response;
	response = NULL;
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

void* HandlerRequest::getResponse() {
	return response;
}

bool HandlerRequest::isSentResponse() const {
	return sentResponse;
}

bool HandlerRequest::isValidWriteRequest() {
	return sif->isCurrentRequest(reqPos);
}

bool HandlerRequest::doneWithWrite() {
	sif->endRequest();
	return sif->allRequestsDone();
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

void ServiceHandler::registerRead(HandlerRequest* hr) {
	hr->switchReaderIntf->registerRead(hr->getSif());
}
