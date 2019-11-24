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

	Timer t;
	t.start();

	if(readFrom()==0) {
		return NULL;
	}

	t.end();
	CommonUtils::tsActRead += t.timerNanoSeconds();

	size_t ix = buffer.find(HttpResponse::HDR_FIN);
	if(!isHeadersDone && ix!=std::string::npos)
	{
		request = new HttpRequest(webpath);
		bytesToRead = 0;
		std::string headers = buffer.substr(0, ix);
		buffer = buffer.substr(ix+4);
		std::vector<std::string> lines = StringUtil::splitAndReturn<std::vector<std::string> >(headers, HttpResponse::HDR_END);
		request->buildRequest("httpline", lines.at(0));
		int hdrc = 0;
		for (int var = 0; var < (int)lines.size(); ++var) {
			size_t kind = std::string::npos;
			std::string key, value;
			if(var>0 && (kind = lines.at(var).find(":"))!=std::string::npos) {
				if(lines.at(var).find(":")==lines.at(var).find(": ")) {
					key = lines.at(var).substr(0, lines.at(var).find_first_of(": "));
					value = lines.at(var).substr(lines.at(var).find_first_of(": ")+2);
				} else {
					key = lines.at(var).substr(0, lines.at(var).find_first_of(":"));
					value = lines.at(var).substr(lines.at(var).find_first_of(":")+1);
				}
				request->buildRequest(key, value);
				if(hdrc++>maxReqHdrCnt) {
					closeSocket();
					delete request;
					request = NULL;
					return NULL;
				}
			} else if(var!=0) {
				//TODO stop processing request some invalid http line
			}
		}
		std::string bytesstr = request->getHeader(HttpRequest::ContentLength);
		if(bytesstr!="") {
			bytesToRead = CastUtil::toInt(bytesstr);
			if(bytesToRead>maxEntitySize) {
				closeSocket();
				delete request;
				request = NULL;
				return NULL;
			}
		} else if(request->getHeader(HttpRequest::TransferEncoding)!="" && buffer.find(HttpResponse::HDR_END)!=std::string::npos) {
			bytesstr = buffer.substr(0, buffer.find(HttpResponse::HDR_END));
			buffer = buffer.substr(buffer.find(HttpResponse::HDR_END)+2);
			if(bytesstr!="") {
				isTeRequest = true;
				bytesToRead = (int)StringUtil::fromHEX(bytesstr);
				if(bytesToRead==0) {
					isTeRequest = false;
				} else if((int)buffer.length()>=bytesToRead) {
					request->content = buffer.substr(0, bytesToRead);
					buffer = buffer.substr(bytesToRead);
				}
			}
		}
		isHeadersDone = true;
	}
	if(request!=NULL && isHeadersDone)
	{
		if(isTeRequest && bytesToRead==0 && buffer.find(HttpResponse::HDR_END)!=std::string::npos) {
			std::string bytesstr = buffer.substr(0, buffer.find(HttpResponse::HDR_END));
			buffer = buffer.substr(buffer.find(HttpResponse::HDR_END)+2);
			if(bytesstr!="") {
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
		void* temp = request;
		request = NULL;
		return temp;
	}
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

	if(response->getHeader(HttpRequest::Connection)=="")
	{
		if(StringUtil::toLowerCopy(request->getHeader(HttpRequest::Connection))!="keep-alive"
				|| CastUtil::toInt(response->getStatusCode())>307 || request->getHttpVers()<1.1)
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
