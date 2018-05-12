/*
 * Http11Handler.cpp
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#include "Http11Handler.h"

void* Http11Handler::readRequest(void*& context, int& pending, int& reqPos) {
	//Timer t;
	//t.start();
	if(readFrom())return NULL;
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
			if(var>0 && (kind = lines.at(var).find(":"))!=std::string::npos) {
				std::string key = lines.at(var).substr(0, kind);
				std::string value = lines.at(var).substr(kind+1);
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
		//t.end();
		//CommonUtils::tsRead += t.timerMilliSeconds();
		return temp;
	}
	//t.end();
	//CommonUtils::tsRead += t.timerMilliSeconds();
	return NULL;
}

int Http11Handler::getTimeout() {
	return connKeepAlive;
}

Http11Handler::Http11Handler(const int& fd, const std::string& webpath, const int& chunkSize, const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize) {
	init(fd);
	isHeadersDone = false;
	bytesToRead = 0;
	request = NULL;
	this->webpath = webpath;
	this->chunkSize = chunkSize<=0?0:chunkSize;
	this->isTeRequest = false;
	this->connKeepAlive = connKeepAlive;
	this->maxReqHdrCnt = maxReqHdrCnt;
	this->maxEntitySize = maxEntitySize;
}

Http11Handler::~Http11Handler() {
	if(request!=NULL) {
		delete request;
		request = NULL;
	}
}

std::string Http11Handler::getProtocol(void* context) {
	return "HTTP1.1";
}

bool Http11Handler::writeResponse(void* req, void* res, void* context) {
	//Timer t;
	//t.start();

	HttpRequest* request = (HttpRequest*)req;
	HttpResponse* response = (HttpResponse*)res;

	if(isClosed()) {
		if(request!=NULL) {
			delete request;
			request = NULL;
		}
		delete response;
		response = NULL;
		//t.end();
		//CommonUtils::tsWrite += t.timerMilliSeconds();
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
	//t.end();
	//CommonUtils::tsWrite += t.timerMilliSeconds();
	return true;
}
void Http11Handler::onOpen(){}
void Http11Handler::onClose(){}
