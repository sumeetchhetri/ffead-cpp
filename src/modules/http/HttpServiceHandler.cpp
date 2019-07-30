/*
 * HttpServiceHandler.cpp
 *
 *  Created on: 07-Jan-2015
 *      Author: sumeetc
 */

#include "HttpServiceHandler.h"

HttpServiceHandler::HttpServiceHandler(const std::string& cntEncoding, const HttpServiceTaskFactory& f, const int& spoolSize= 0)
	: ServiceHandler(spoolSize) {
	this->cntEncoding = cntEncoding;
	this->f = f;
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

void HttpServiceHandler::handleWrite(HandlerRequest* handlerRequest)
{
	HttpWriteTask* task = new HttpWriteTask();
	task->handlerRequest = handlerRequest;
	task->service = this;
	task->setCleanUp(true);
	submitTask(task);
}

void HttpServiceHandler::handleRead(HandlerRequest* handlerRequest)
{
	HttpReadTask* task = new HttpReadTask();
	task->handlerRequest = handlerRequest;
	task->service = this;
	task->setCleanUp(true);
	submitTask(task);
}

HttpWriteTask::HttpWriteTask() {
	this->handlerRequest = NULL;
	service = NULL;
}

HttpWriteTask::~HttpWriteTask() {
	if(handlerRequest!=NULL) {
		delete handlerRequest;
	}
}

void HttpWriteTask::run() {
	//Logger logger = LoggerFactory::getLogger("HttpWriteTask");
	if(handlerRequest->getSif()->isClosed()) {
		handlerRequest->clearObjects();
		bool flag = handlerRequest->doneWithWrite();
		if(flag && handlerRequest->getSif()->isClosed()) {
		}
		return;
	}

	if(!handlerRequest->isValidWriteRequest()) {
		//This handlerRequest will be processed in a pipelined manner and will be later cleared by the handling HttpWriteTask object
		//Set handlerRequest to NULL to avoid deleting requests getting handled in an out of order manner
		HandlerRequest* handlerRequestOrig = handlerRequest;
		this->handlerRequest = NULL;
		service->registerWriteRequest(handlerRequestOrig, handlerRequestOrig->getResponse());
		return;
	}

	CommonUtils::cResps += 1;
	handlerRequest->getSif()->writeResponse(handlerRequest->getRequest(), handlerRequest->getResponse(), handlerRequest->getContext());

	bool flag = handlerRequest->doneWithWrite();
	if(flag && handlerRequest->getSif()->isClosed()) {
		//delete handlerRequest->getSif()->sockUtil;
		//delete handlerRequest->getSif();
	}
}

HttpReadTask::HttpReadTask() {
	this->handlerRequest = NULL;
	service = NULL;
}

HttpReadTask::~HttpReadTask() {
	if(handlerRequest!=NULL) {
		delete handlerRequest;
	}
}

void HttpReadTask::run() {
	int pending = 1;
	while(pending>0)
	{
		void* context = NULL;
		int reqPos = 0;
		void* request = handlerRequest->sif->readRequest(context, pending, reqPos);
		if(handlerRequest->sif->isClosed()) {
			handlerRequest->sif->onClose();
			if(handlerRequest->context!=NULL) {
				delete handlerRequest->context;
			}
			if(handlerRequest->sif->allRequestsDone()) {
				delete handlerRequest->sif;
			}
			pending = 0;
			break;
		} else if(request!=NULL) {
			service->registerServiceRequest(request, handlerRequest->sif, context, reqPos);
			CommonUtils::cReqs += 1;
		}
	}
}

HttpServiceTask::HttpServiceTask() {
	this->handlerRequest = NULL;
	service = NULL;
}

HttpServiceTask::HttpServiceTask(HandlerRequest* handlerRequest, HttpServiceHandler* service) {
	this->handlerRequest = handlerRequest;
	this->service = service;
}

HttpServiceTask::~HttpServiceTask() {
}

void HttpServiceTask::run() {
	Timer t;
	t.start();

	if(handlerRequest->getSif()->isClosed()) {
		handlerRequest->doneWithWrite();
		handlerRequest->clearObjects();
		delete handlerRequest;
		t.end();
		CommonUtils::tsService += t.timerNanoSeconds();
		return;
	}

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

		Date cdate(true);
		DateFormat df("ddd, dd mmm yyyy hh:mi:ss GMT");
		res->addHeaderValue(HttpResponse::DateHeader, df.format(cdate));

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
				handlerRequest->getSif()->addHandler(new Http11WebSocketHandler(req->getUrl(), true, handlerRequest->getSif()->sockUtil));

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
				handlerRequest->getSif()->addHandler(new Http2Handler(true, handlerRequest->getSif()->sockUtil, prev->getWebpath(), http2settings));
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

		//std::string url = req->getUrl();
	}
	else
	{
		Http11WebSocketHandler* h = (Http11WebSocketHandler*)handlerRequest->getSif();
		WebSocketData* request = (WebSocketData*)handlerRequest->getRequest();
		WebSocketData* response = new WebSocketData();
		handleWebsockMessage(h->getUrl(), request, response);
		resp = response;
	}

	//handlerRequest->getSif()->writeResponse(handlerRequest->getRequest(), resp, handlerRequest->getContext());

	//service->registerWriteRequest(handlerRequest, resp);
	CommonUtils::cResps += 1;
	handlerRequest->getSif()->pushResponse(handlerRequest->getRequest(), resp, handlerRequest->getContext(), handlerRequest->reqPos);


	t.end();
	CommonUtils::tsService += t.timerNanoSeconds();
}
