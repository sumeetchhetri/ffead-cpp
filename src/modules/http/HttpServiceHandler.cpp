/*
 * HttpServiceHandler.cpp
 *
 *  Created on: 07-Jan-2015
 *      Author: sumeetc
 */

#include "HttpServiceHandler.h"

HttpServiceHandler::HttpServiceHandler(const std::string& cntEncoding, const HttpServiceTaskFactory& f, const int& spoolSize, const HttpReadTaskFactory& fr)
	: ServiceHandler(spoolSize) {
	this->cntEncoding = cntEncoding;
	this->f = f;
	this->fr = fr;
}

HttpServiceHandler::~HttpServiceHandler() {
}

void HttpServiceHandler::handleService(HandlerRequest* handlerRequest)
{
	HttpServiceTask* task = f();
	task->handlerRequest = handlerRequest;
	task->service = this;
	task->setCleanUp(true);
	submitTask(task);
}

void HttpServiceHandler::handleRead(SocketInterface* sif)
{
	HttpReadTask* task = new HttpReadTask();
	task->sif = sif;
	task->service = this;
	task->setCleanUp(true);
	submitTask(task);
}

void HttpServiceHandler::handleWrite(HandlerRequest *handlerRequest) {
	HttpWriteTask* task = new HttpWriteTask();
	task->handlerRequest = handlerRequest;
	task->service = this;
	task->setCleanUp(true);
	submitTask(task);
}

HttpReadTask::HttpReadTask() {
	this->sif = NULL;
	service = NULL;
}

HttpReadTask::~HttpReadTask() {
}

void HttpReadTask::run() {
	Timer t;
	t.start();

	int pending = 1;
	while(pending>0)
	{
		void* context = NULL;
		int reqPos = 0;
		void* request = sif->readRequest(context, pending, reqPos);
		if(sif->isClosed()) {
			if(request!=NULL) {
				delete request;
			}
			service->closeConnection(sif);
			break;
		} else if(request!=NULL) {
			service->registerServiceRequest(request, sif, context, reqPos);
		}
	}

	t.end();
	CommonUtils::tsRead += t.timerNanoSeconds();
}

HttpServiceTask::HttpServiceTask() {
	this->handlerRequest = NULL;
	service = NULL;
}

HttpServiceTask::HttpServiceTask(ReusableInstanceHolder* h) {
	this->handlerRequest = NULL;
	service = NULL;
	this->hdlr = h;
}

HttpServiceTask::~HttpServiceTask() {
	if(handlerRequest!=NULL) {
		delete handlerRequest;
	}
}

void HttpServiceTask::run() {
	Timer t;
	t.start();

	if(handlerRequest->getSif()->isClosed()) {
		handlerRequest->doneWithWrite(handlerRequest->reqPos);
		handlerRequest->sif->onClose();
		service->closeConnection(handlerRequest->sif);
		t.end();
		CommonUtils::tsService += t.timerNanoSeconds();
		return;
	}

	CommonUtils::cReqs += 1;

	void* resp = NULL;
	if(handlerRequest->getProtocol()=="HTTP2.0" || handlerRequest->getProtocol()=="HTTP1.1")
	{
		HttpRequest* req = (HttpRequest*)handlerRequest->getRequest();
		HttpResponse* res = new HttpResponse();
		resp = res;

		std::string_view mimeType = CommonUtils::getMimeType(req->ext);
		std::string cntEncoding = service->cntEncoding;
		if(req->isAgentAcceptsCE() && (cntEncoding=="gzip" || cntEncoding=="deflate") && req->isNonBinary(mimeType)) {
			res->addHeader(HttpResponse::ContentEncoding, cntEncoding);
		}

		time_t rt;
		struct tm ti;
		time (&rt);
		gmtime_r(&rt, &ti);
		char buffer[31];
		strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &ti);
		res->addHeader(HttpResponse::DateHeader, std::string(buffer));

		if(handlerRequest->getProtocol()=="HTTP1.1" && req->hasHeaderValuePart(HttpRequest::Connection, "upgrade", true))
		{
			if(req->isHeaderValue(HttpRequest::Upgrade, "websocket", true)
					&& req->getHeader(HttpRequest::SecWebSocketKey)!=""
					&& (req->hasHeaderValuePart(HttpRequest::SecWebSocketVersion, "7", false)
					|| req->hasHeaderValuePart(HttpRequest::SecWebSocketVersion, "8", false)
					|| req->hasHeaderValuePart(HttpRequest::SecWebSocketVersion, "13", false)))
			{
				std::string_view seckey = req->getHeader(HttpRequest::SecWebSocketKey);
				std::string servseckey = std::string(seckey) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
				servseckey = CryptoHandler::sha1(servseckey);
				servseckey = CryptoHandler::base64encodeStr(servseckey);

				res->addHeader(HttpResponse::Upgrade, "websocket");
				res->addHeader(HttpResponse::Connection, "upgrade");
				res->addHeader(HttpResponse::SecWebSocketAccept, servseckey);
				res->setHTTPResponseStatus(HTTPResponseStatus::Switching);
				res->setDone(true);
				handlerRequest->getSif()->addHandler(new Http11WebSocketHandler(handlerRequest->getSif()->fd, handlerRequest->getSif()->ssl,
						handlerRequest->getSif()->io, req->getUrl(), true));

				WebSocketData wreq;
				wreq.url = req->getCurl();
				wreq.cnxtName = req->getCntxt_name();
				handleWebsockOpen(&wreq);
			}
			else if(req->hasHeaderValuePart(HttpRequest::Connection, "HTTP2-Settings", false)
					&& req->isHeaderValue(HttpRequest::Upgrade, "h2c", false))
			{
				std::string http2settings = std::string(req->getHeader(HttpRequest::Http2Settings));
				http2settings = CryptoHandler::base64decodeStr(http2settings);

				res->addHeader(HttpResponse::Upgrade, "h2c");
				res->addHeader(HttpResponse::Connection, "Upgrade");
				res->setHTTPResponseStatus(HTTPResponseStatus::Switching);
				res->setDone(true);
				Http2Handler* prev = (Http2Handler*)handlerRequest->getSif();
				handlerRequest->getSif()->addHandler(new Http2Handler(handlerRequest->getSif()->fd, handlerRequest->getSif()->ssl,
						handlerRequest->getSif()->io, true, prev->getWebpath(), http2settings));
			}
			else
			{
				res->addHeader(HttpResponse::Connection, "close");
				res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
				res->setDone(true);
			}
		}

		if(!res->isDone())
		{
			handle(req, res);
		}
	}
	else
	{
		Http11WebSocketHandler* h = (Http11WebSocketHandler*)handlerRequest->getSif();
		WebSocketData* request = (WebSocketData*)handlerRequest->getRequest();
		WebSocketData* response = new WebSocketData();
		handleWebsockMessage(h->getUrl(), request, response);
		resp = response;
	}

	t.end();
	CommonUtils::tsService += t.timerNanoSeconds();

	CommonUtils::cResps += 1;
	handlerRequest->response = resp;
	int ret = handlerRequest->getSif()->pushResponse(handlerRequest->getRequest(), handlerRequest->response, handlerRequest->getContext(), handlerRequest->reqPos);
	if(ret==0) {
		handlerRequest->doneWithWrite(handlerRequest->reqPos);
		handlerRequest->sif->onClose();
		service->closeConnection(handlerRequest->sif);
	}
	//service->registerWriteRequest(handlerRequest);
	//handlerRequest->getSif()->pushResponse(handlerRequest->getRequest(), resp, handlerRequest->getContext(), handlerRequest->reqPos);
}

void HttpWriteTask::run() {
	int ret = handlerRequest->getSif()->pushResponse(handlerRequest->getRequest(), handlerRequest->response, handlerRequest->getContext(), handlerRequest->reqPos);
	if(ret==0) {
		handlerRequest->doneWithWrite(handlerRequest->reqPos);
		handlerRequest->sif->onClose();
		service->closeConnection(handlerRequest->sif);
	}
}

HttpWriteTask::~HttpWriteTask() {
	if(handlerRequest!=NULL) {
		delete handlerRequest;
	}
}

HttpWriteTask::HttpWriteTask() {
	this->handlerRequest = NULL;
	service = NULL;
	this->hdlr = NULL;
}
