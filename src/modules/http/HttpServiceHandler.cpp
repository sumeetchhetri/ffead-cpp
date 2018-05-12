/*
 * HttpServiceHandler.cpp
 *
 *  Created on: 07-Jan-2015
 *      Author: sumeetc
 */

#include "HttpServiceHandler.h"

HttpServiceHandler::HttpServiceHandler(const std::string& cntEncoding, const HttpServiceTaskFactory& f, const int& spoolSize= 0, const int& wpoolSize= 0)
	: ServiceHandler(spoolSize, wpoolSize) {
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
	submitServiceTask(task);
}

void HttpServiceHandler::handleWrite(HandlerRequest* handlerRequest)
{
	HttpWriteTask* task = new HttpWriteTask();
	task->handlerRequest = handlerRequest;
	task->service = this;
	task->setCleanUp(true);
	submitWriteTask(task);
}

void HttpWriteTask::run() {
	//Logger logger = LoggerFactory::getLogger("HttpWriteTask");
	if(handlerRequest->getSif()->isClosed()) {
		handlerRequest->clearObjects();
		bool flag = handlerRequest->doneWithWrite();
		if(flag && handlerRequest->getSif()->isClosed()) {
			//logger << "Delete Sif from writer " << handlerRequest->getSif()->identifier << std::endl;
			//handlerRequest->sh->donelist.put(handlerRequest->getSif()->identifier, true);
			//delete handlerRequest->getSif()->sockUtil;
			//delete handlerRequest->getSif();
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

	//CommonUtils::cResps += 1;
	handlerRequest->getSif()->writeResponse(handlerRequest->getRequest(), handlerRequest->getResponse(), handlerRequest->getContext());

	bool flag = handlerRequest->doneWithWrite();
	if(flag && handlerRequest->getSif()->isClosed()) {
		//logger << "Delete Sif from writer " << handlerRequest->getSif()->identifier << std::endl;
		//handlerRequest->sh->donelist.put(handlerRequest->getSif()->identifier, true);
		//delete handlerRequest->getSif()->sockUtil;
		//delete handlerRequest->getSif();
	}
}

HttpServiceTask::HttpServiceTask() {
	this->handlerRequest = NULL;
}

HttpServiceTask::HttpServiceTask(HandlerRequest* handlerRequest, HttpServiceHandler* service) {
	this->handlerRequest = handlerRequest;
	this->service = service;
}

HttpServiceTask::~HttpServiceTask() {
}

HttpWriteTask::HttpWriteTask() {
	this->handlerRequest = NULL;
}

HttpWriteTask::~HttpWriteTask() {
	if(handlerRequest!=NULL) {
		delete handlerRequest;
	}
}

void HttpServiceTask::run() {
	if(handlerRequest->getSif()->isClosed()) {
		handlerRequest->doneWithWrite();
		handlerRequest->clearObjects();
		delete handlerRequest;
		return;
	}

	//Timer t;
	//t.start();

	void* resp = NULL;
	SocketInterface* switchedIntf = NULL;

	//std::cout << "servicing request" << handlerRequest->getSif()->getDescriptor() << " " << handlerRequest->getSif()->identifier << std::endl;
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
				switchedIntf = new Http11WebSocketHandler(req->getUrl(), true, handlerRequest->getSif()->getDescriptor());

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
				switchedIntf = new Http2Handler(true, handlerRequest->getSif()->getDescriptor(), prev->getWebpath(),
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

		//std::string url = req->getUrl();
		//std::cout << url << handlerRequest->getSif()->getDescriptor() << " " << handlerRequest->getSif()->identifier << std::endl;
	}
	else
	{
		Http11WebSocketHandler* h = (Http11WebSocketHandler*)handlerRequest->getSif();
		WebSocketData* request = (WebSocketData*)handlerRequest->getRequest();
		WebSocketData* response = new WebSocketData();
		handleWebsockMessage(h->getUrl(), request, response);
		resp = response;
	}

	//std::cout << "\n\nwriting response " << handlerRequest->getSif()->getDescriptor() << " " << handlerRequest->getSif()->identifier << std::endl;
	//handlerRequest->getSif()->writeResponse(handlerRequest->getRequest(), resp, handlerRequest->getContext());
	service->registerWriteRequest(handlerRequest, resp);
	//service->registerRead(handlerRequest);
	//std::cout << "done writing response " << handlerRequest->getSif()->getDescriptor() << " " << handlerRequest->getSif()->identifier << std::endl << std::endl;
	if(switchedIntf!=NULL)
	{
		service->switchReaders(handlerRequest, switchedIntf);
	}

	//t.end();
	//CommonUtils::tsService += t.timerMilliSeconds();
}
