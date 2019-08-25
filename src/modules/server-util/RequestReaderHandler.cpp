/*
 * RequestReaderHandler.cpp
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#include "RequestReaderHandler.h"

RequestReaderHandler::RequestReaderHandler(ServiceHandler* shi, const bool& isMain, const SOCKET& listenerSock) {
	this->shi = shi;
	this->listenerSock = listenerSock;
	this->isNotRegisteredListener = (listenerSock == INVALID_SOCKET);
	this->run = false;
	this->siIdentifierSeries = 1;
	this->sf = NULL;
	this->complete = 0;
	this->isMain = isMain;
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
			sc.connection(ip, port);
			sc.closeConnection();
		} else {
			Client sc;
			sc.connection(ip, port);
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
	Logger logger = LoggerFactory::getLogger("RequestReaderHandler");
	RequestReaderHandler* ins  = static_cast<RequestReaderHandler*>(inp);
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	while(ins->isActive())
	{
		Timer t;
		t.start();
		int num = ins->selector.getEvents();
		t.end();
		CommonUtils::tsPoll += t.timerNanoSeconds();

		if (num<=0)
		{
			if(num==-1) {
				//print errors
			}
			continue;
		}

		t.start();
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
						SOCKET newSocket = accept(ins->listenerSock, (struct sockaddr *)&(their_addr), &sin_size);
						if (newSocket == -1)
						{
						  if ((errno == EAGAIN) ||
							  (errno == EWOULDBLOCK))
							{
							  break;
							}
						}
						SocketInterface* sockIntf = ins->sf(newSocket);
						sockIntf->eh = &(ins->selector);
						ins->addSf(sockIntf);
						CommonUtils::cSocks += 1;
					}
#else
					sin_size = sizeof their_addr;
					SOCKET newSocket = accept(ins->listenerSock, (struct sockaddr *)&(their_addr), &sin_size);
					SocketInterface* sockIntf = ins->sf(newSocket);
					sockIntf->eh = &(ins->selector);
					ins->addSf(sockIntf);
					CommonUtils::cSocks += 1;
#endif
				}
				else
				{
					SocketInterface* si = (SocketInterface*)vsi;
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

		t.end();
		CommonUtils::tsProcess += t.timerNanoSeconds();
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
