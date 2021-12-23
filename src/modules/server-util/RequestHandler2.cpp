/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
/*
 * RequestHandler2.cpp
 *
 *  Created on: 26-Aug-2021
 *      Author: sumeetc
 */

#include "RequestHandler2.h"

RequestHandler2* RequestHandler2::_i = NULL;

RequestHandler2::RequestHandler2(ServiceHandler* sh, const bool& isMain, bool isSSLEnabled, httpSockHandle h, const SOCKET& listenerSock) {
	this->shi = sh;
	this->listenerSock = listenerSock;
	this->isNotRegisteredListener = (listenerSock == INVALID_SOCKET);
	this->run = false;
	this->complete = 0;
	this->isMain = isMain;
	this->isSSLEnabled = isSSLEnabled;
	this->selector.setCtx(this);
	this->hsh = h;
	this->sf = NULL;
}

void RequestHandler2::setInstance(RequestHandler2* ins) {
	if(_i==NULL) {
		_i = ins;
	}
}

void RequestHandler2::registerSocketInterfaceFactory(const SocketInterfaceFactory2& f) {
	this->sf = f;
}

RequestHandler2* RequestHandler2::getInstance() {
	return _i;
}

void RequestHandler2::startNL(unsigned int cid, bool withWQ) {
	if(run) {
		return;
	}
	if(!run) {
		selector.initialize(-1);
		run = true;
		Thread* pthread = new Thread(&handle, this);
		pthread->execute(cid);
		if(withWQ) {
			Thread* pthread = new Thread(&handleWrites, this);
			pthread->execute(cid);
		}
	}
}

void RequestHandler2::addListenerSocket(doRegisterListener drl, const SOCKET& listenerSock) {
	if(listenerSock != INVALID_SOCKET) {
		this->listenerSock = listenerSock;
	} else {
		return;
	}
	if(drl!=NULL) {
		int counter = 0;
		while(!drl()) {
			Thread::sSleep(1);
			if(counter++==60) {
				Logger logger = LoggerFactory::getLogger("RequestHandler2");
				logger << "Cannot wait more than 60 seconds for cache/database to initialize, will forcefully start server now...." << std::endl;
				break;
			}
		}
	}
	selector.addListeningSocket(this->listenerSock);
}

void RequestHandler2::start(unsigned int cid, bool withWQ) {
	if(run) {
		return;
	}
	if(!run) {
		run = true;
		selector.initialize(listenerSock, -1);
		Thread* pthread = new Thread(&handle, this);
		pthread->execute(cid);
		if(withWQ) {
			Thread* pthread = new Thread(&handleWrites, this);
			pthread->execute(cid);
		}
	}
}

void RequestHandler2::stop(std::string ip, int port, bool isSSLEnabled) {
	run = false;
	while(complete<1) {
		Thread::mSleep(1000);

		if(isSSLEnabled) {
#ifdef HAVE_SSLINC
			SSLClient sc;
			sc.connectionNB(ip, port);
			sc.closeConnection();
#endif
		} else {
			Client sc;
			sc.connectionNB(ip, port);
			sc.closeConnection();
		}
	}
}

bool RequestHandler2::isActive() {
	return run;
}

RequestHandler2::~RequestHandler2() {
}

bool RequestHandler2::loopContinue(SelEpolKqEvPrt* ths) {
	RequestHandler2* ins = static_cast<RequestHandler2*>(ths->getCtx());
	return ins->isActive();
}

BaseSocket* RequestHandler2::loopEventCb(SelEpolKqEvPrt* ths, BaseSocket* bi, int type, int fd, char* buf, size_t len, bool isClosed) {
	RequestHandler2* ins = static_cast<RequestHandler2*>(ths->getCtx());
	switch(type) {
		case ACCEPTED: {
			Http11Socket* si = ins->sf(fd);
			si->eh = &(ins->selector);
			si->onOpen();
			if(!ins->run) {
				ins->clsdConns.push_back(si);
			}
			return si;
		}
		case READ_READY: {
			bi->handle();
			break;
		}
		case CLOSED: {
			bi->onClose();
			ins->shi->closeConnection(bi);
			break;
		}
		case WRITE_READY: {
			Http11Socket* si = (Http11Socket*)bi;
#ifndef USE_IO_URING
			ins->selector.unRegisterWrite(bi);
			std::vector<ResponseData>::iterator it = si->outbuf.begin();
			while(it != si->outbuf.end()) {
				int done = si->writeTo(&*it);
				if(done == -1) {
					ins->selector.registerWrite(si);
					break;
				} else {
					it = si->outbuf.erase(it);
				}
			}
#endif
			break;
		}
		case ON_DATA_READ: {
			Http11Socket* si = (Http11Socket*)bi;
			si->buffer.append(buf, len);
			si->handle();
			break;
		}
		case ON_DATA_WRITE: {
			break;
		}
	}
	return NULL;
}

void* RequestHandler2::handle(void* inp) {
	RequestHandler2* ins  = static_cast<RequestHandler2*>(inp);
	ins->selector.loop(&loopContinue, &loopEventCb);

	for(int i=0;i<(int)ins->clsdConns.size();i++) {
		delete ins->clsdConns.at(i);
	}

	if(ins->isMain) {
		ins->shi->stop();
		while(ins->shi->run) {
			Thread::mSleep(100);
		}
	}
	Thread::mSleep(500);
	ins->complete += 1;
	return 0;
}

void* RequestHandler2::handleWrites(void* inp) {
	RequestHandler2* ins  = static_cast<RequestHandler2*>(inp);
	SockWriteRequest swr;
	ins->selector.wQ = new moodycamel::BlockingConcurrentQueue<SockWriteRequest>;
	while(ins->shi->run) {
		ins->selector.wQ->wait_dequeue(swr);
		swr.f(swr.bs, swr.arg);
	}
	return 0;
}

Http11Socket::Http11Socket(const SOCKET& fd, const int& chunkSize, const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize): BaseSecureSocket(fd) {
	isHeadersDone = false;
	bytesToRead = 0;
	this->chunkSize = chunkSize<=0?0:chunkSize;
	this->isTeRequest = false;
	this->connKeepAlive = connKeepAlive;
	this->maxReqHdrCnt = maxReqHdrCnt;
	this->maxEntitySize = maxEntitySize;
}

Http11Socket::~Http11Socket() {
}

int Http11Socket::getTimeout() {
	return connKeepAlive;
}

bool Http11Socket::read() {
	size_t ix = buffer.find(HttpResponse::HDR_FIN);
	if(!isHeadersDone && ix!=std::string::npos)
	{
		bytesToRead = 0;
		std::string headers = buffer.substr(0, ix+4);
		req.reset(std::move(headers), &bytesToRead);
		buffer = buffer.substr(ix+4);
		res.reset();
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
			std::string content = buffer.substr(0, bytesToRead);
			req.setContent(std::move(content));
			buffer = buffer.substr(bytesToRead);
			bytesToRead = 0;
		}

		if(!isTeRequest && bytesToRead==0)
		{
			isHeadersDone = false;
			fl = true;
		}
	}
	return fl;
}

void Http11Socket::handle() {
	if(readFrom()==0) {
		onClose();
		RequestHandler2::_i->shi->closeConnection(this);
		return;
	}

	bool pd = false;
	while(buffer.length()>0 && read()) {
		if(RequestHandler2::_i->hsh(&req, &res, this)) {
			pd = true;

			if(req.isClose()) {
				res.addHeader(HttpResponse::Connection, "close");
			} else if(req.getHttpVers()>=1.1) {
				res.addHeader(HttpResponse::Connection, "keep-alive");
			}

			outbuf.emplace_back();
			ResponseData& rd = outbuf.back();

			if(res.isDone()) {
				res.generateResponse(&req, rd._b);
			} else {
				res.updateContent(&req, chunkSize);
				if(!res.isContentRemains()) {
					res.generateResponse(&req, rd._b);
				} else {
					res.generateResponse(&req, rd._b, false);
					bool isFirst = true;
					while(res.hasContent && res.getRemainingContent(req.getUrl(), isFirst, rd._b)) {
						isFirst = false;
					}
				}
			}
		}
	}

	if(pd) {
		std::vector<ResponseData>::iterator it = outbuf.begin();
		while(it != outbuf.end()) {
#if defined(USE_IO_URING)
			eh->post_write(this, it->_b);
			it = outbuf.erase(it);
#else
			int done = writeTo(&*it);
			if(done == -1) {
				eh->registerWrite(this);
				break;
			} else {
				it = outbuf.erase(it);
			}
#endif
		}
	}

	if(isClosed()) {
		onClose();
		RequestHandler2::_i->shi->closeConnection(this);
	} else {
		doneRead();
	}
}
