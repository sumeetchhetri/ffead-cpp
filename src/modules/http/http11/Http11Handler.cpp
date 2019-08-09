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

	if(readFrom()) {
		return NULL;
	}

	HttpRequest* request = NULL;
	if(!isHeadersDone && buffer.find("\r\n\r\n")!=std::string::npos)
	{
		bytesToRead = 0;
		std::string headers = buffer.substr(0, buffer.find("\r\n\r\n"));
		buffer = buffer.substr(buffer.find("\r\n\r\n")+4);
		std::vector<std::string> lines = StringUtil::splitAndReturn<std::vector<std::string> >(headers, "\r\n");
		request = new HttpRequest(webpath);
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
					close();
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
			bytesToRead = CastUtil::lexical_cast<int>(bytesstr);
			if(bytesToRead>maxEntitySize) {
				close();
				delete request;
				request = NULL;
				return NULL;
			}
		} else if(request->getHeader(HttpRequest::TransferEncoding)!="" && buffer.find("\r\n")!=std::string::npos) {
			bytesstr = buffer.substr(0, buffer.find("\r\n"));
			buffer = buffer.substr(buffer.find("\r\n")+2);
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
		if(isTeRequest && bytesToRead==0 && buffer.find("\r\n")!=std::string::npos) {
			std::string bytesstr = buffer.substr(0, buffer.find("\r\n"));
			buffer = buffer.substr(buffer.find("\r\n")+2);
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

void Http11Handler::init(const std::string& webpath, const int& chunkSize, const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize) {
	reqPos = 0;
	current = 0;
	address = StringUtil::toHEX((long long)this);
	wtl.clear();
	buffer.clear();
	isHeadersDone = false;
	bytesToRead = 0;
	this->webpath = webpath;
	this->chunkSize = chunkSize<=0?0:chunkSize;
	this->isTeRequest = false;
	this->connKeepAlive = connKeepAlive;
	this->maxReqHdrCnt = maxReqHdrCnt;
	this->maxEntitySize = maxEntitySize;
	fd = sockUtil.fd;
	this->handler = NULL;
}

Http11Handler::Http11Handler(const std::string& webpath, const int& chunkSize, const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize) {
	reqPos = 0;
	current = 0;
	address = StringUtil::toHEX((long long)this);
	isHeadersDone = false;
	bytesToRead = 0;
	this->webpath = webpath;
	this->chunkSize = chunkSize<=0?0:chunkSize;
	this->isTeRequest = false;
	this->connKeepAlive = connKeepAlive;
	this->maxReqHdrCnt = maxReqHdrCnt;
	this->maxEntitySize = maxEntitySize;
	fd = sockUtil.fd;
	this->handler = NULL;
}

void Http11Handler::addHandler(SocketInterface* handler) {
	this->handler = handler;
}

Http11Handler::~Http11Handler() {
	if(handler!=NULL) {
		delete handler;
		handler = NULL;
	}
}

std::string Http11Handler::getProtocol(void* context) {
	if(handler!=NULL) {
		return handler->getProtocol(context);
	}
	return "HTTP1.1";
}

bool Http11Handler::writeResponse(void* req, void* res, void* context) {
	if(handler!=NULL) {
		return handler->writeResponse(req, res, context);
	}

	HttpRequest* request = (HttpRequest*)req;
	HttpResponse* response = (HttpResponse*)res;

	if(isClosed()) {
		if(request!=NULL) {
			delete request;
			request = NULL;
		}
		delete response;
		response = NULL;
		return true;
	}

	if(!response->isDone()) {
		response->updateContent(request, chunkSize);
	}

	if(response->getHeader(HttpRequest::Connection)=="")
	{
		if(StringUtil::toLowerCopy(request->getHeader(HttpRequest::Connection))!="keep-alive"
				|| CastUtil::lexical_cast<int>(response->getStatusCode())>307 || request->getHttpVers()<1.1)
		{
			response->addHeaderValue(HttpResponse::Connection, "close");
		}
		else
		{
			response->addHeaderValue(HttpResponse::Connection, "keep-alive");
		}
	}

	if(!response->isContentRemains()) {
		std::string data = response->generateResponse(request);
		writeTo(data);
	} else {
		std::string data = response->generateResponse(request, false);
		if(!writeTo(data)) {
			bool isFirst = true;
			while(response->hasContent && (data = response->getRemainingContent(request->getUrl(), isFirst)) != "") {
				isFirst = false;
				if(writeTo(data)) {
					break;
				}
			}
		}
	}

	if(request!=NULL) {
		delete request;
		request = NULL;
	}
	delete response;
	response = NULL;
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
