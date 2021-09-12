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
 * RequestReaderHandler.cpp
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#include "RequestReaderHandler.h"

RequestReaderHandler* RequestReaderHandler::_i = NULL;

RequestReaderHandler::RequestReaderHandler(ServiceHandler* shi, const bool& isMain, bool isSinglEVH, const SOCKET& listenerSock) {
	this->shi = shi;
	this->listenerSock = listenerSock;
	this->isNotRegisteredListener = (listenerSock == INVALID_SOCKET);
	this->run = false;
	this->siIdentifierSeries = 1;
	this->sf = NULL;
	this->complete = 0;
	this->isMain = isMain;
	this->isSinglEVH = isSinglEVH;
	this->selector.setCtx(this);
}

void RequestReaderHandler::setInstance(RequestReaderHandler* ins) {
	if(_i==NULL) {
		_i = ins;
	}
}

RequestReaderHandler* RequestReaderHandler::getInstance() {
	return _i;
}

void RequestReaderHandler::startNL(unsigned int cid) {
	if(run) {
		return;
	}
	if(!run) {
		selector.initialize(-1);
		run = true;
		Thread* pthread = new Thread(&handle, this);
		pthread->execute(cid);
	}
}

void RequestReaderHandler::addListenerSocket(doRegisterListener drl, const SOCKET& listenerSock) {
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
				Logger logger = LoggerFactory::getLogger("RequestReaderHandler");
				logger << "Cannot wait more than 60 seconds for cache/database to initialize, will forcefully start server now...." << std::endl;
				break;
			}
		}
	}
	selector.addListeningSocket(this->listenerSock);
}

void RequestReaderHandler::start(unsigned int cid) {
	if(run) {
		return;
	}
	if(!run) {
		run = true;
		selector.initialize(listenerSock, -1);
		Thread* pthread = new Thread(&handle, this);
		pthread->execute(cid);
	}
}

void RequestReaderHandler::stop(std::string ip, int port, bool isSSLEnabled) {
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

bool RequestReaderHandler::isActive() {
	return run;
}

void RequestReaderHandler::registerSocketInterfaceFactory(const SocketInterfaceFactory& f) {
	this->sf = f;
}

void RequestReaderHandler::addSf(SocketInterface* psi) {
	if(psi->getTimeout()>0)
	{
		//addToTimeoutSocks.push(psi);
	}
	psi->onOpen();
}

RequestReaderHandler::~RequestReaderHandler() {
}

bool RequestReaderHandler::loopContinue(SelEpolKqEvPrt* ths) {
	RequestReaderHandler* ins = static_cast<RequestReaderHandler*>(ths->getCtx());
	return ins->isActive();
}

BaseSocket* RequestReaderHandler::loopEventCb(SelEpolKqEvPrt* ths, BaseSocket* bi, int type, int fd, char* buf, size_t len, bool isClosed) {
	RequestReaderHandler* ins = static_cast<RequestReaderHandler*>(ths->getCtx());
	switch(type) {
		case ACCEPTED: {
			SocketInterface* sockIntf = ins->sf(fd);
			sockIntf->eh = &(ins->selector);
			ins->addSf(sockIntf);
			ins->shi->sockInit(sockIntf);
			CommonUtils::cSocks += 1;
			if(!ins->run) {
				ins->clsdConns.push_back(sockIntf);
			}
			return sockIntf;
		}
		case READ_READY: {
			SocketInterface* si = (SocketInterface*)bi;
			if(ins->isSinglEVH) {
				if(!si->isClosed()) {
					si->rdTsk->run();
				} else {
					si->onClose();
					ins->shi->closeConnection(si);
				}
			} else {
				if(!si->isClosed()) {
					ins->shi->registerReadRequest(si);
				} else {
					si->onClose();
					ins->shi->closeConnection(si);
				}
			}
			break;
		}
		case WRITE_READY: {
			SocketInterface* si = (SocketInterface*)bi;
			if(ins->isSinglEVH) {
				if(!si->isClosed()) {
					ins->selector.unRegisterWrite(si);
					ins->shi->registerWriteRequest(si);
				} else {
					si->onClose();
					ins->shi->closeConnection(si);
				}
			} else {
				if(!si->isClosed()) {
					ins->selector.unRegisterWrite(si);
					ins->shi->registerWriteRequest(si);
				} else {
					si->onClose();
					ins->shi->closeConnection(si);
				}
			}
			break;
		}
		case CLOSED: {
			SocketInterface* si = (SocketInterface*)bi;
			si->onClose();
			ins->shi->closeConnection(si);
			break;
		}
		case ON_DATA_READ: {
			SocketInterface* si = (SocketInterface*)bi;
			si->buffer.append(buf, len);
			if(ins->isSinglEVH) {
				si->rdTsk->run();
			} else {
				ins->shi->registerReadRequest(si);
			}
			break;
		}
		case ON_DATA_WRITE: {
			SocketInterface* si = (SocketInterface*)bi;
			si->endRequest(-1);
			break;
		}
	}
	return NULL;
}

void* RequestReaderHandler::handle(void* inp) {
	RequestReaderHandler* ins  = static_cast<RequestReaderHandler*>(inp);
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

//Deprecated old handler -- can be used if existing event loop needs to be overriden
void* RequestReaderHandler::handle_Old(void* inp) {
	//Logger logger = LoggerFactory::getLogger("RequestReaderHandler");
	RequestReaderHandler* ins  = static_cast<RequestReaderHandler*>(inp);
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	while(ins->isActive())
	{
		//Timer t;
		//t.start();
		int num = ins->selector.getEvents();
		//t.end();
		//CommonUtils::tsPoll += t.timerNanoSeconds();

		if (num<=0)
		{
			if(num==-1) {
				//print errors
			}
			continue;
		}

		//t.start();
		for(int n=0;n<num;n++)
		{
			void* vsi = NULL;
			bool isRead = true;
			SOCKET descriptor = ins->selector.getDescriptor(n, vsi, isRead);
			if(descriptor!=-1)
			{
				if(ins->selector.isListeningDescriptor(descriptor))
				{
#if defined USE_EPOLL && defined(USE_EPOLL_ET)
					while (true) {
						sin_size = sizeof their_addr;
#ifdef HAVE_ACCEPT4
#ifdef HAVE_SSLINC
						SOCKET newSocket = accept4(ins->listenerSock, (struct sockaddr *)&(their_addr), &sin_size,
								SSLHandler::getInstance()->getIsSSL()?0:SOCK_NONBLOCK);
#else
						SOCKET newSocket = accept4(ins->listenerSock, (struct sockaddr *)&(their_addr), &sin_size, SOCK_NONBLOCK);
#endif
#else
						SOCKET newSocket = accept(ins->listenerSock, (struct sockaddr *)&(their_addr), &sin_size);
#endif
						if (newSocket == -1)
						{
							if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
							{
								break;
							}
						}
						SocketInterface* sockIntf = ins->sf(newSocket);
						sockIntf->eh = &(ins->selector);
						ins->addSf(sockIntf);
						ins->shi->sockInit(sockIntf);
						CommonUtils::cSocks += 1;
						if(!ins->run) {
							ins->clsdConns.push_back(sockIntf);
						}
					}
#else
					sin_size = sizeof their_addr;
					SOCKET newSocket = accept(ins->listenerSock, (struct sockaddr *)&(their_addr), &sin_size);
					SocketInterface* sockIntf = ins->sf(newSocket);
					sockIntf->eh = &(ins->selector);
					ins->addSf(sockIntf);
					ins->shi->sockInit(sockIntf);
					CommonUtils::cSocks += 1;
					if(!ins->run) {
						ins->clsdConns.push_back(sockIntf);
					}
#endif
				}
				else
				{
					SocketInterface* si = (SocketInterface*)vsi;
					if(ins->isSinglEVH) {
						if(isRead) {
							if(!si->isClosed()) {
								si->rdTsk->run();
							} else {
								si->onClose();
								ins->shi->closeConnection(si);
							}
						} else {
							if(!si->isClosed()) {
								ins->selector.unRegisterWrite(si);
								ins->shi->registerWriteRequest(si);
							} else {
								si->onClose();
								ins->shi->closeConnection(si);
							}
						}
					} else {
						if(isRead) {
							if(!si->isClosed()) {
								ins->shi->registerReadRequest(si);
							} else {
								si->onClose();
								ins->shi->closeConnection(si);
							}
						} else {
							if(!si->isClosed()) {
								ins->selector.unRegisterWrite(si);
								ins->shi->registerWriteRequest(si);
							} else {
								si->onClose();
								ins->shi->closeConnection(si);
							}
						}
					}
				}
			}
		}

		//t.end();
		//CommonUtils::tsProcess += t.timerNanoSeconds();
	}

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
