/*
 * Http11Handler.cpp
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#include "Http11Handler.h"

bool Http11Handler::readRequest(void* request, void*& context, int& pending, int& reqPos) {
	//if(handler!=NULL) {
	//	return handler->readRequest(request, context, pending, reqPos);
	//}

	Timer t;
	t.start();

	HttpRequest* currReq = (HttpRequest*)request;

	size_t ix = buffer.find(HttpResponse::HDR_FIN);
	if(!isHeadersDone && ix!=std::string::npos)
	{
		bytesToRead = 0;
		std::string headers = buffer.substr(0, ix+4);
		buffer = buffer.substr(ix+4);
		currReq->reset(std::move(headers), &bytesToRead);
		((HttpResponse*)currReq->resp)->reset();

		/*
		currReq->reset();
		currReq->webpath = webpath;
		std::string headers = buffer.substr(0, ix);
		buffer = buffer.substr(ix+4);
		std::vector<std::string> lines = StringUtil::splitAndReturn<std::vector<std::string> >(headers, HttpResponse::HDR_END);
		currReq->buildRequest("httpline", lines.at(0));
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
				currReq->buildRequest(key, value);
				if(hdrc++>maxReqHdrCnt) {
					closeSocket();
					currReq->isInit = false;
					currReq = NULL;
					return NULL;
				}
			} else if(var!=0) {
				//TODO stop processing request some invalid http line
			}
		}
		std::string bytesstr = currReq->getHeader(HttpRequest::ContentLength);
		if(bytesstr!="") {
			bytesToRead = CastUtil::toInt(bytesstr);
			if(bytesToRead>maxEntitySize) {
				closeSocket();
				currReq->isInit = false;
				currReq = NULL;
				return NULL;
			}
		} else if(currReq->getHeader(HttpRequest::TransferEncoding)!="" && buffer.find(HttpResponse::HDR_END)!=std::string::npos) {
			bytesstr = buffer.substr(0, buffer.find(HttpResponse::HDR_END));
			buffer = buffer.substr(buffer.find(HttpResponse::HDR_END)+2);
			if(bytesstr!="") {
				isTeRequest = true;
				bytesToRead = (int)StringUtil::fromHEX(bytesstr);
				if(bytesToRead==0) {
					isTeRequest = false;
				} else if((int)buffer.length()>=bytesToRead) {
					currReq->content = buffer.substr(0, bytesToRead);
					buffer = buffer.substr(bytesToRead);
				}
			}
		}*/
		isHeadersDone = true;
	}

	bool fl = false;
	if(isHeadersDone)
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
			currReq->content = buffer.substr(0, bytesToRead);
			buffer = buffer.substr(bytesToRead);
			bytesToRead = 0;
		}

		if(!isTeRequest && bytesToRead==0)
		{
			reqPos = startRequest();
			isHeadersDone = false;
			fl = true;
		}
	}

	pending = buffer.length();

	t.end();
	CommonUtils::tsReqParse += t.timerNanoSeconds();

	return fl;
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
	//logger = LoggerFactory::getLogger("Http11Handler");
}

HttpRequest* Http11Handler::getAvailableRequest() {
	if((int)requests.size()<10) {
		HttpRequest* r = new HttpRequest();
		r->resp = new HttpResponse();
		this->requests.push_back(r);
		return r;
	} else {
		for(int i=0;i<(int)requests.size();i++) {
			if(requests.at(i)->isInit==false) {
				return requests.at(i);
			}
		}
		//TODO need to clean up requests after peak load is handled and done
		//also need a max cap on the number of pending or active requests per connection
		HttpRequest* r = new HttpRequest();
		r->resp = new HttpResponse();
		this->requests.push_back(r);
		return r;
	}
	return NULL;
}

void Http11Handler::addHandler(SocketInterface* handler) {
	this->handler = handler;
}

Http11Handler::~Http11Handler() {
	if(handler!=NULL) {
		delete handler;
		handler = NULL;
	}
	for(int i=0;i<(int)requests.size();i++) {
		HttpRequest* r = requests.at(i);
		delete (HttpResponse*)r->resp;
		delete r;
	}
	if(srvTsk!=NULL) {
		delete srvTsk;
	}
	if(rdTsk!=NULL) {
		delete rdTsk;
	}
	if(wrTsk!=NULL) {
		delete wrTsk;
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

	if(!response->isDone()) {
		response->updateContent(request, chunkSize);
	}

	if(request->isHeaderValue(HttpRequest::Connection, "keep-alive") && request->getHttpVers()>=1.1) {
		response->addHeader(HttpResponse::Connection, "keep-alive");
	} else {
		response->addHeader(HttpResponse::Connection, "close");
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
