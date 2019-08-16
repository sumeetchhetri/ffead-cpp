/*
 * Http11Handler.cpp
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#include "Http11Handler.h"

void* Http11Handler::readRequest(void*& context, int& pending, int& reqPos) {
	if(handler!=NULL) {
		return handler->readRequest(context, pending, reqPos);
	}

	m.lock();

	Timer t;
	t.start();

	if(readFrom()) {
		return NULL;
	}

	t.end();
	CommonUtils::tsActRead += t.timerNanoSeconds();

	size_t ix = buffer.find(HttpResponse::HDR_FIN);
	if(!isHeadersDone && ix!=std::string::npos)
	{
		request = new HttpRequest(webpath);
		std::string headers = buffer.substr(0, ix);
		buffer = buffer.substr(ix+4);
		size_t he = 0;
		int hdrc = 0;
		while((he = headers.find(HttpResponse::HDR_END, he))!=std::string::npos) {
			std::string tv = headers.substr(0, he);
			if(request->method.length()==0) {
				request->buildRequest("httpline", tv);
			} else {
				size_t hki = tv.find(HttpResponse::HDR_SEPT);
				if(hki!=std::string::npos) {
					request->buildRequest(tv.substr(0, hki), tv.find(HttpResponse::HDR_SEP)==std::string::npos?tv.substr(hki+1):tv.substr(hki+2));
					if(hdrc++>maxReqHdrCnt) {
						closeSocket();
						delete request;
						request = NULL;
						m.unlock();
						return NULL;
					}
				} else {
					//TODO stop processing request some invalid http line
				}
			}
			he += 2;
		}
		noBody = strcasestr(request->method.c_str(), "GET")!=NULL;
		if(!noBody) {
			std::string bytesstr = request->getHeader(HttpRequest::ContentLength);
			if(bytesstr.length()>0) {
				bytesToRead = CastUtil::lexical_cast<int>(bytesstr);
				if((int)buffer.length()>=bytesToRead) {
					request->content = buffer.substr(0, bytesToRead);
					bytesToRead = 0;
					buffer = buffer.substr(bytesToRead);
					doneReq = true;
				} else if(bytesToRead>maxEntitySize) {
					closeSocket();
					delete request;
					request = NULL;
					m.unlock();
					return NULL;
				} else if(bytesToRead==0) {
					doneReq = true;
				}
			} else if(request->getHeader(HttpRequest::TransferEncoding).length()>0 && buffer.find(HttpResponse::HDR_END)!=std::string::npos) {
				bytesstr = buffer.substr(0, buffer.find(HttpResponse::HDR_END));
				buffer = buffer.substr(buffer.find(HttpResponse::HDR_END)+2);
				if(bytesstr.length()>0) {
					isTeRequest = true;
					bytesToRead = (int)StringUtil::fromHEX(bytesstr);
					if(bytesToRead==0) {
						isTeRequest = false;
						doneReq = true;
					} else if((int)buffer.length()>=bytesToRead) {
						request->content = buffer.substr(0, bytesToRead);
						buffer = buffer.substr(bytesToRead);
						bytesToRead = 0;
						doneReq = true;
					}
				}
			}
		} else {
			doneReq = true;
		}
		isHeadersDone = true;
	}
	if(!doneReq && isHeadersDone)
	{
		if(isTeRequest && bytesToRead==0 && buffer.find(HttpResponse::HDR_END)!=std::string::npos) {
			std::string bytesstr = buffer.substr(0, buffer.find(HttpResponse::HDR_END));
			buffer = buffer.substr(buffer.find(HttpResponse::HDR_END)+2);
			if(bytesstr.length()>0) {
				bytesToRead = (int)StringUtil::fromHEX(bytesstr);
				if(bytesToRead==0) {
					isTeRequest = false;
				}
			}
		}
		if(bytesToRead>0 && (int)buffer.length()>=bytesToRead) {
			request->content = buffer.substr(0, bytesToRead);
			buffer = buffer.substr(bytesToRead);
			bytesToRead = 0;
		}
	}

	if(pending==(int)buffer.length())
	{
		pending = 0;
	}
	else
	{
		pending = buffer.length();
	}

	if(!isTeRequest && bytesToRead==0 && request!=NULL)
	{
		reqPos = startRequest();
		isHeadersDone = false;
		doneReq = false;
		bytesToRead = 0;
		noBody = true;
		void* temp = request;
		request = NULL;
		m.unlock();
		return temp;
	}
	m.unlock();
	return NULL;
}

int Http11Handler::getTimeout() {
	if(handler!=NULL) {
		return handler->getTimeout();
	}
	return connKeepAlive;
}

Http11Handler::Http11Handler(const SOCKET& fd, SSL* ssl, BIO* io, const std::string& webpath, const int& chunkSize,
		const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize) : SocketInterface(fd, ssl, io) {
	isHeadersDone = false;
	noBody = true;
	doneReq = false;
	bytesToRead = 0;
	this->webpath = webpath;
	this->chunkSize = chunkSize<=0?0:chunkSize;
	this->isTeRequest = false;
	this->connKeepAlive = connKeepAlive;
	this->maxReqHdrCnt = maxReqHdrCnt;
	this->maxEntitySize = maxEntitySize;
	this->handler = NULL;
	this->request = NULL;
	logger = LoggerFactory::getLogger("Http11Handler");
}

void Http11Handler::addHandler(SocketInterface* handler) {
	this->handler = handler;
}

Http11Handler::~Http11Handler() {
	if(handler!=NULL) {
		delete handler;
		handler = NULL;
	}
	if(request!=NULL) {
		delete request;
	}
}

std::string Http11Handler::getProtocol(void* context) {
	if(handler!=NULL) {
		return handler->getProtocol(context);
	}
	return "HTTP1.1";
}

bool Http11Handler::writeResponse(void* req, void* res, void* context, std::string& data, int reqPos) {
	if(handler!=NULL) {
		return handler->writeResponse(req, res, context, data, reqPos);
	}

	HttpRequest* request = (HttpRequest*)req;
	HttpResponse* response = (HttpResponse*)res;

	if(isClosed()) {
		return true;
	}

	if(!response->isDone()) {
		response->updateContent(request, chunkSize);
	}

	if(response->getHeader(HttpRequest::Connection).length()==0)
	{
		if(StringUtil::toLowerCopy(request->getHeader(HttpRequest::Connection))!="keep-alive"
				|| CastUtil::lexical_cast<int>(response->getStatusCode())>307 || request->getHttpVers()<1.1)
		{
			response->addHeader(HttpResponse::Connection, "close");
		}
		else
		{
			response->addHeader(HttpResponse::Connection, "keep-alive");
		}
	}

	if(!response->isContentRemains()) {
		response->generateResponse(request, data);
	} else {
		response->generateResponse(request, data, false);
		bool isFirst = true;
		while(response->hasContent && response->getRemainingContent(request->getUrl(), isFirst, data)) {
			isFirst = false;
		}
	}

	return true;
}
void Http11Handler::onOpen(){
	if(handler!=NULL) {
		handler->onOpen();
		return;
	}
}
void Http11Handler::onClose(){
	if(handler!=NULL) {
		return handler->onClose();
		return;
	}
}
