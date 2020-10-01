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
	if(!run) {
		run = true;
		selector.initialize(-1);
		Thread* pthread = new Thread(&handle, this);
		pthread->execute(cid);
	}
}

void RequestReaderHandler::addListenerSocket(const SOCKET& listenerSock) {
	if(listenerSock != INVALID_SOCKET) {
		this->listenerSock = listenerSock;
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
			SSLClient sc;
			sc.connectionNB(ip, port);
			sc.closeConnection();
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
	selector.registerRead(psi);
	psi->onOpen();
}

RequestReaderHandler::~RequestReaderHandler() {
}

void* RequestReaderHandler::handle(void* inp) {
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
						SOCKET newSocket = accept4(ins->listenerSock, (struct sockaddr *)&(their_addr), &sin_size, SOCK_NONBLOCK);
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
