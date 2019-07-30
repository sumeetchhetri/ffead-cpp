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

void ServiceHandler::registerWriteRequest(HandlerRequest* request, void* response) {
	if(request!=NULL && response!=NULL) {
		request->response = response;
		handleWrite(request);
	}
}

void ServiceHandler::registerReadRequest(SocketInterface* sif) {
	HandlerRequest* req = new HandlerRequest;
	req->sif = sif;
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
	}
}

void ServiceHandler::stop() {
	if(spoolSize > 0) {
		spool.joinAll();
	}
	run = false;
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
	stop();
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
}

SocketUtil* HandlerRequest::getSocketUtil() {
	return sif->sockUtil;
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

bool HandlerRequest::doneWithWrite() {
	sif->endRequest();
	return sif->allRequestsDone();
}

SocketInterface* HandlerRequest::getSif() {
	return sif;
}
