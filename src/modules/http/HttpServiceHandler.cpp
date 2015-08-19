/*
 * HttpServiceHandler.cpp
 *
 *  Created on: 07-Jan-2015
 *      Author: sumeetc
 */

#include "HttpServiceHandler.h"

HttpServiceHandler::HttpServiceHandler(const string& cntEncoding, const HttpServiceTaskFactory& f, const int& poolSize= 0) : ServiceHandler(poolSize) {
	this->cntEncoding = cntEncoding;
	this->f = f;
}

HttpServiceHandler::~HttpServiceHandler() {
}

void HttpServiceHandler::service(HandlerRequest* handlerRequest)
{
	HttpServiceTask* task = f();
	task->handlerRequest = handlerRequest;
	task->service = this;
	task->setCleanUp(true);
	submitTask(task);
}

HttpServiceTask::HttpServiceTask() {}

HttpServiceTask::HttpServiceTask(HandlerRequest* handlerRequest, HttpServiceHandler* service) {
	this->handlerRequest = handlerRequest;
	this->service = service;
}

HttpServiceTask::~HttpServiceTask() {
	if(handlerRequest!=NULL) {
		delete handlerRequest;
	}
}

void HttpServiceTask::run() {
	if(handlerRequest->getSif()->isClosed())return;

	void* resp = NULL;
	SocketInterface* switchedIntf = NULL;

	cout << "servicing request" << handlerRequest->getSif()->getDescriptor() << " " << handlerRequest->getSif()->identifier << endl;
	if(handlerRequest->getProtocol()=="HTTP2.0" || handlerRequest->getProtocol()=="HTTP1.1")
	{
		HttpRequest* req = (HttpRequest*)handlerRequest->getRequest();
		HttpResponse* res = new HttpResponse();
		resp = res;

		string mimeType = CommonUtils::getMimeType(req->ext);
		string cntEncoding = service->cntEncoding;
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
				string seckey = req->getHeader(HttpRequest::SecWebSocketKey);
				string servseckey = seckey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
				servseckey = CryptoHandler::sha1(servseckey);
				servseckey = CryptoHandler::base64encodeStr(servseckey);

				res->addHeaderValue(HttpResponse::Upgrade, "websocket");
				res->addHeaderValue(HttpResponse::Connection, "upgrade");
				res->addHeaderValue(HttpResponse::SecWebSocketAccept, servseckey);
				res->setHTTPResponseStatus(HTTPResponseStatus::Switching);
				res->setDone(true);
				switchedIntf = new Http11WebSocketHandler(req->getUrl(), true, handlerRequest->getSocketUtil());

				handleWebsockOpen(req->getUrl());
			}
			else if(req->hasHeaderValuePart(HttpRequest::Connection, "HTTP2-Settings", false)
					&& req->isHeaderValue(HttpRequest::Upgrade, "h2c", false))
			{
				string http2settings = req->getHeader(HttpRequest::Http2Settings);
				http2settings = CryptoHandler::base64decodeStr(http2settings);

				res->addHeaderValue(HttpResponse::Upgrade, "h2c");
				res->addHeaderValue(HttpResponse::Connection, "Upgrade");
				res->setHTTPResponseStatus(HTTPResponseStatus::Switching);
				res->setDone(true);
				Http2Handler* prev = (Http2Handler*)handlerRequest->getSif();
				switchedIntf = new Http2Handler(true, handlerRequest->getSocketUtil(), prev->getWebpath(),
						http2settings);
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

		string url = req->getUrl();
		cout << url << handlerRequest->getSif()->getDescriptor() << " " << handlerRequest->getSif()->identifier << endl;
	}
	else
	{
		Http11WebSocketHandler* h = (Http11WebSocketHandler*)handlerRequest->getSif();
		WebSocketData* request = (WebSocketData*)handlerRequest->getRequest();
		WebSocketData* response = new WebSocketData();
		handleWebsockMessage(h->getUrl(), request, response);
		resp = response;
	}

	cout << "\n\nwriting response " << handlerRequest->getSif()->getDescriptor() << " " << handlerRequest->getSif()->identifier << endl;
	handlerRequest->getSif()->writeResponse(handlerRequest->getRequest(), resp, handlerRequest->getContext());
	cout << "done writing response " << handlerRequest->getSif()->getDescriptor() << " " << handlerRequest->getSif()->identifier << endl << endl;
	if(switchedIntf!=NULL)
	{
		service->switchReaders(handlerRequest, switchedIntf);
	}
}
