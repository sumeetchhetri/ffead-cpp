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

int HttpServiceTask::getTid() {
	return handlerRequest.sif->tid;
}

void HttpServiceTask::setTid(int tid) {
	handlerRequest.sif->tid = tid;
}

void HttpServiceHandler::sockInit(SocketInterface* sif) {
	sif->rdTsk = fr();
	HttpReadTask* task = (HttpReadTask*)sif->rdTsk;
	task->sif = sif;
	task->service = this;

	sif->srvTsk = f();
	HttpServiceTask* stask = (HttpServiceTask*)sif->srvTsk;
	stask->service = this;
	stask->handlerRequest.sh = this;
	stask->handlerRequest.sif = sif;
	stask->handlerRequest.request = new HttpRequest();
	((HttpRequest*)stask->handlerRequest.request)->resp = new HttpResponse();

	sif->wrTsk = new HttpWriteTask();
	HttpWriteTask* wtask = (HttpWriteTask*)sif->wrTsk;
	wtask->sif = sif;
	wtask->service = this;
}

void HttpServiceHandler::handleService(void* request, SocketInterface* sif, void* context, int reqPos)
{
	HttpServiceTask* task = f();
	task->handlerRequest.sh = this;
	task->service = this;
	task->handlerRequest.request = request;
	task->handlerRequest.response = NULL;
	task->handlerRequest.sif = sif;
	task->handlerRequest.context = context;
	task->handlerRequest.reqPos = reqPos;
	task->handlerRequest.protType = sif->getProtocol(context).find("HTTP")==0?1:2;
	submitTask(task);
}

void HttpServiceHandler::handleRead(SocketInterface* sif) {
	submitTask(sif->rdTsk);
}

void HttpServiceHandler::handleWrite(SocketInterface* sif) {
	submitTask(sif->wrTsk);
}

HttpReadTask::HttpReadTask() {
	this->sif = NULL;
	service = NULL;
}

HttpReadTask::~HttpReadTask() {
}

int HttpReadTask::getTid() {
	return sif->tid;
}

void HttpReadTask::setTid(int tid) {
	sif->tid = tid;
}

void HttpReadTask::run() {
	Timer t;
	t.start();

	int pending = 1;

	Timer to;
	to.start();
	if(sif->readFrom()==0) {
		service->closeConnection(sif);
		to.end();
		CommonUtils::tsReqSockRead += to.timerNanoSeconds();

		t.end();
		CommonUtils::tsReqTotal += t.timerNanoSeconds();
		return;
	}
	to.end();
	CommonUtils::tsReqSockRead += to.timerNanoSeconds();

	to.start();
	HttpServiceTask* task = (HttpServiceTask*)sif->srvTsk;
	HandlerRequest& hr = task->handlerRequest;
	while(pending>0 && sif->readRequest(hr.request, hr.context, pending, hr.reqPos))
	{
		task->handlerRequest.protType = sif->getProtocol(hr.context).find("HTTP")==0?1:2;
		task->run();
	}
	to.end();
	CommonUtils::tsReqPrsSrvc += to.timerNanoSeconds();

	if(sif->isClosed()) {
		service->closeConnection(sif);
	}

	t.end();
	CommonUtils::tsReqTotal += t.timerNanoSeconds();
}

HttpServiceTask::HttpServiceTask() {
	service = NULL;
	rt = 0;
}

HttpServiceTask::HttpServiceTask(ReusableInstanceHolder* h) {
	service = NULL;
	this->hdlr = h;
	rt = 0;
}

HttpServiceTask::~HttpServiceTask() {
}

std::string HttpServiceTask::getCntEncoding() {
	return service->cntEncoding;
}

void HttpServiceTask::run() {
	Timer t;
	t.start();

	CommonUtils::cReqs += 1;

	if(handlerRequest.getProtType()==1)
	{
		HttpRequest* req = (HttpRequest*)handlerRequest.getRequest();
		HttpResponse* res = (HttpResponse*)req->resp;
		handlerRequest.response = res;

		time (&rt);
		gmtime_r(&rt, &ti);
		strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &ti);
		res->headers[HttpResponse::DateHeader] = std::string(buffer);

		/*if(req->httpVers<2 && req->hasHeaderValuePart(HttpRequest::Connection, "upgrade", true))
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

				res->addHeader(HttpResponse::Upgrade, "websocket");
				res->addHeader(HttpResponse::Connection, "upgrade");
				res->addHeader(HttpResponse::SecWebSocketAccept, servseckey);
				res->setHTTPResponseStatus(HTTPResponseStatus::Switching);
				res->setDone(true);
				handlerRequest.getSif()->addHandler(new Http11WebSocketHandler(handlerRequest.getSif()->fd, handlerRequest.getSif()->ssl,
						handlerRequest.getSif()->io, req->getUrl(), true));

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

				res->addHeader(HttpResponse::Upgrade, "h2c");
				res->addHeader(HttpResponse::Connection, "Upgrade");
				res->setHTTPResponseStatus(HTTPResponseStatus::Switching);
				res->setDone(true);
				Http2Handler* prev = (Http2Handler*)handlerRequest.getSif();
				handlerRequest.getSif()->addHandler(new Http2Handler(handlerRequest.getSif()->fd, handlerRequest.getSif()->ssl,
						handlerRequest.getSif()->io, true, prev->getWebpath(), http2settings));
			}
			else
			{
				res->addHeader(HttpResponse::Connection, "close");
				res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
				res->setDone(true);
			}
		}*/

		//if(!res->isDone())
		//{
			handle(req, res);
		//}
	}
	else
	{
		Http11WebSocketHandler* h = (Http11WebSocketHandler*)handlerRequest.getSif();
		WebSocketData* request = (WebSocketData*)handlerRequest.getRequest();
		WebSocketData* response = new WebSocketData();
		handleWebsockMessage(h->getUrl(), request, response);
		handlerRequest.response = response;
	}

	t.end();
	CommonUtils::tsService += t.timerNanoSeconds();

	CommonUtils::cResps += 1;
	int ret = handlerRequest.getSif()->pushResponse(handlerRequest.getRequest(), handlerRequest.response, handlerRequest.getContext(), handlerRequest.reqPos);
	if(ret==0) {
		handlerRequest.sif->onClose();
		service->closeConnection(handlerRequest.sif);
	}
}

void HttpWriteTask::run() {
	if(sif->completeWrite()==0) {
		sif->onClose();
		service->closeConnection(sif);
	}
}

HttpWriteTask::~HttpWriteTask() {
}

HttpWriteTask::HttpWriteTask() {
	this->sif = NULL;
	service = NULL;
	this->hdlr = NULL;
}

int HttpWriteTask::getTid() {
	return sif->tid;
}

void HttpWriteTask::setTid(int tid) {
	sif->tid = tid;
}
