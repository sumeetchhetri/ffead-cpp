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
	HttpReadTask* task = fr();
	task->sif = sif;
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
			sif->startRequest();
			service->registerServiceRequest(NULL, sif, context, reqPos);
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
		handlerRequest->doneWithWrite();
		handlerRequest->clearObjects();
		handlerRequest->sif->onClose();
		if(handlerRequest->sif->allRequestsDone()) {
			service->closeConnection(handlerRequest->sif);
		}
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

		std::string mimeType = CommonUtils::getMimeType(req->ext);
		std::string cntEncoding = service->cntEncoding;
		if(req->isAgentAcceptsCE() && (cntEncoding=="gzip" || cntEncoding=="deflate") && req->isNonBinary(mimeType)) {
			res->addHeaderValue(HttpResponse::ContentEncoding, cntEncoding);
		}

		time_t rt;
		struct tm ti;
		time (&rt);
		gmtime_r(&rt, &ti);
		char buffer[31];
		strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &ti);
		res->addHeaderValue(HttpResponse::DateHeader, std::string(buffer));

		if(handlerRequest->getProtocol()=="HTTP1.1" && req->hasHeaderValuePart(HttpRequest::Connection, "upgrade", true))
		{
			if(req->isHeaderValue(HttpRequest::Upgrade, "websocket", true)
					&& req->getHeader(HttpRequest::SecWebSocketKey)!=""
					&& (req->hasHeaderValuePart(HttpRequest::SecWebSocketVersion, "7", false)
					|| req->hasHeaderValuePart(HttpRequest::SecWebSocketVersion, "8", false)
					|| req->hasHeaderValuePart(HttpRequest::SecWebSocketVersion, "13", false)))
			{
				std::string seckey = req->getHeader(HttpRequest::SecWebSocketKey);
				std::string servseckey = seckey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
				servseckey = CryptoHandler::sha1(servseckey);
				servseckey = CryptoHandler::base64encodeStr(servseckey);

				res->addHeaderValue(HttpResponse::Upgrade, "websocket");
				res->addHeaderValue(HttpResponse::Connection, "upgrade");
				res->addHeaderValue(HttpResponse::SecWebSocketAccept, servseckey);
				res->setHTTPResponseStatus(HTTPResponseStatus::Switching);
				res->setDone(true);
				handlerRequest->getSif()->addHandler(new Http11WebSocketHandler(req->getUrl(), true));

				WebSocketData wreq;
				wreq.url = req->getCurl();
				wreq.cnxtName = req->getCntxt_name();
				handleWebsockOpen(&wreq);
			}
			else if(req->hasHeaderValuePart(HttpRequest::Connection, "HTTP2-Settings", false)
					&& req->isHeaderValue(HttpRequest::Upgrade, "h2c", false))
			{
				std::string http2settings = req->getHeader(HttpRequest::Http2Settings);
				http2settings = CryptoHandler::base64decodeStr(http2settings);

				res->addHeaderValue(HttpResponse::Upgrade, "h2c");
				res->addHeaderValue(HttpResponse::Connection, "Upgrade");
				res->setHTTPResponseStatus(HTTPResponseStatus::Switching);
				res->setDone(true);
				Http2Handler* prev = (Http2Handler*)handlerRequest->getSif();
				handlerRequest->getSif()->addHandler(new Http2Handler(true, prev->getWebpath(), http2settings));
			}
			else
			{
				res->addHeaderValue(HttpResponse::Connection, "close");
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
	handlerRequest->getSif()->pushResponse(handlerRequest->getRequest(), resp, handlerRequest->getContext(), handlerRequest->reqPos);
}
