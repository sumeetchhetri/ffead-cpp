/*
 * RequestReaderHandler.cpp
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#include "RequestReaderHandler.h"

RequestReaderHandler::RequestReaderHandler(ServiceHandler* shi, const SOCKET& listenerSock) {
	this->shi = shi;
	this->listenerSock = listenerSock;
	this->isNotRegisteredListener = (listenerSock == INVALID_SOCKET);
	this->run = false;
	this->siIdentifierSeries = 1;
	this->sf = NULL;
	this->complete = 0;
}

void RequestReaderHandler::switchReaders(SocketInterface* prev, SocketInterface* next) {
	readerSwitchedSocks.enqueue(next);
}

void RequestReaderHandler::registerRead(SocketInterface* si) {
	selector.registerForEvent(si->getDescriptor());
}

void RequestReaderHandler::start() {
	if(run) {
		return;
	}
	if(!run) {
		run = true;
		selector.initialize(listenerSock, -1);
		Thread* pthread = new Thread(&handle, this);
		pthread->execute();
		//Thread* pthread1 = new Thread(&handleTimeouts, this);
		//pthread1->execute();
	}
}

void RequestReaderHandler::stop(std::string ip, int port, bool isSSLEnabled) {
	run = false;
	while(complete<1) {
		Thread::mSleep(500);

		if(isSSLEnabled) {
			SSLClient sc;
			sc.connectionUnresolv(ip, port);
			sc.closeConnection();
		} else {
			Client sc;
			sc.connectionUnresolv(ip, port);
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
	//psi->t1 = Timer::getCurrentTime();
	psi->sockUtil->sel = &selector;
	psi->setIdentifier(siIdentifierSeries++);
	connections.insert(psi->getDescriptor(), psi);
	if(psi->getTimeout()>0)
	{
		//addToTimeoutSocks.push(psi);
	}
	selector.registerForEvent(psi->getDescriptor());
	shi->addOpenRequest(psi);
	psi->onOpen();
}

RequestReaderHandler::~RequestReaderHandler() {
	//stop();
}

void* RequestReaderHandler::handleTimeouts(void* inp) {
	Logger logger = LoggerFactory::getLogger("RequestReaderHandler");
	RequestReaderHandler* ins  = static_cast<RequestReaderHandler*>(inp);
	std::map<int, SocketInterface*>::iterator it;
	while(ins->isActive())
	{
		Timer cdt;

		SocketInterface* asi;
		cdt.start();
		while(ins->remFromTimeoutSocks.try_dequeue(asi) && cdt.elapsedMilliSeconds()<1000)
		{
			ins->connectionsWithTimeouts.erase(asi->getDescriptor());
		}

		cdt.start();
		while(ins->addToTimeoutSocks.try_dequeue(asi) && cdt.elapsedMilliSeconds()<1000)
		{
			ins->connectionsWithTimeouts.insert(asi->getDescriptor(), asi);
		}

		ins->shi->cleanSif(ins->connectionsWithTimeouts);

		if(!ins->connectionsWithTimeouts.empty())
		{
			auto lt = ins->connectionsWithTimeouts.lock_table();
			cuckoohash_map<int, SocketInterface*>::locked_table::iterator it;
			for(it=lt.begin();it!=lt.end();++it)
			{
				/*int tt = (Timer::getTimestamp() - 1203700) + it->second->getTimeout();
				if(it->second->t>=tt)
				{
					logger << "timedout connection " << it->second->getDescriptor() << " " << it->second->identifier << std::endl;
					ins->timedoutSocks.enqueue(it->second);
					ins->connectionsWithTimeouts.erase(it->first);
				}
				else
				{
					it++;
				}*/
			}
		}
		Thread::sSleep(1);
	}
	ins->complete += 1;
	return NULL;
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

		Timer cdt(false);

		/*if(ins->isNotRegisteredListener)
		{
			cdt.start();
			std::vector<SocketInterface*> pds;
			SocketInterface* psi = NULL;
			while(ins->pendingSocks.pop(psi) && cdt.elapsedMilliSeconds()<100)
			{
				if(ins->connections.find(psi->getDescriptor())==ins->connections.end()) {
					ins->addSf(psi);
				} else {
					pds.push_back(psi);
				}
			}
			for (int var = 0; var < (int)pds.size(); ++var) {
				ins->pendingSocks.push(pds.at(var));
				logger << "existing connection " << pds.at(var)->getDescriptor() << std::endl;
			}
		}

		cdt.start();
		SocketInterface* tsi = NULL;
		while(ins->timedoutSocks.pop(tsi) && cdt.elapsedMilliSeconds()<100)
		{
			if(!tsi->isClosed())tsi->close();
			ins->selector.unRegisterForEvent(tsi->getDescriptor());
			ins->connections.erase(tsi->getDescriptor());
			ins->shi->addCloseRequest(tsi);
			delete tsi;
		}*/

		cdt.start();
		SocketInterface* rsi;
		while(ins->readerSwitchedSocks.try_dequeue(rsi))
		{
			logger << "Swicthing protocols.." << std::endl;
			if(ins->connections.contains(rsi->getDescriptor()) && ins->connections.find(rsi->getDescriptor())->getTimeout()>0)
			{
				//ins->remFromTimeoutSocks.push(rsi);
			}
			delete ins->connections.find(rsi->getDescriptor());
			ins->connections.insert(rsi->getDescriptor(), rsi);
			if(rsi->getTimeout()>0)
			{
				//ins->addToTimeoutSocks.push(rsi);
			}
		}
		t.end();
		CommonUtils::tsPoll1 += t.timerNanoSeconds();

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
			SOCKET descriptor = ins->selector.getDescriptor(n);
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
							  /* We have processed all incoming
								 connections. */
							  break;
							}
						}
						SocketInterface* sockIntf = ins->sf(newSocket);
						ins->addSf(sockIntf);
						//logger << "New Sif " << sockIntf->identifier << std::endl;
						CommonUtils::cSocks += 1;
					}
#else
					sin_size = sizeof their_addr;
					SOCKET newSocket = accept(ins->listenerSock, (struct sockaddr *)&(their_addr), &sin_size);
					SocketInterface* sockIntf = ins->sf(newSocket);
					ins->addSf(sockIntf);

					CommonUtils::cSocks += 1;
#endif
				}
				else
				{
					if(!ins->connections.contains(descriptor)) {
						//logger << "IDHAR KAISE AAYA@@@@@@@@" << std::endl;
						ins->selector.unRegisterForEvent(descriptor);
						continue;
					}
					SocketInterface* si = ins->connections.find(descriptor);
					int pending = 1;
					while(pending>0)
					{
						void* context = NULL;
						int reqPos = 0;
						void* request = si->readRequest(context, pending, reqPos);
						if(si->isClosed()) {
							si->onClose();
							ins->selector.unRegisterForEvent(si->getDescriptor());
							ins->connections.erase(si->getDescriptor());
							if(si->allRequestsDone()) {
								//logger << "Delete Sif " << si->identifier << std::endl;
								delete si;
							}
							//ins->remFromTimeoutSocks.push(si);
							//ins->shi->addCloseRequest(si);
							pending = 0;
							break;
						} else if(request!=NULL) {
							//ins->selector.unRegisterForEvent(si->getDescriptor());
							ins->shi->registerServiceRequest(request, si, context, reqPos, ins);
							CommonUtils::cReqs += 1;
						}
					}
				}
			}
		}

		t.end();
		CommonUtils::tsProcess += t.timerNanoSeconds();
	}

	while(ins->shi->run) {
		Thread::mSleep(100);
	}

	SocketInterface* si;
	bool isPendingSocks = false;
	while(ins->pendingSocks.try_dequeue(si))
	{
		isPendingSocks = true;
		ins->shi->donelist.insert(si->identifier, true);
		si->close();
		//logger << "Delete Sif " << si->identifier << std::endl;
		delete si;
	}
	auto lt = ins->connections.lock_table();
	cuckoohash_map<int, SocketInterface*>::locked_table::iterator it;
	for(it=lt.begin();it!=lt.end();++it) {
		if(!isPendingSocks || !ins->shi->donelist.find(it->second->identifier)) {
			it->second->close();
			//logger << "Delete Sif " << it->second->identifier << std::endl;
			delete it->second;
		}
	}
	ins->connections.clear();
	ins->connectionsWithTimeouts.clear();
	si = NULL;
	while(ins->timedoutSocks.try_dequeue(si))
	{
	}
	si = NULL;
	while(ins->readerSwitchedSocks.try_dequeue(si))
	{
	}
	si = NULL;
	while(ins->addToTimeoutSocks.try_dequeue(si))
	{
	}
	si = NULL;
	while(ins->remFromTimeoutSocks.try_dequeue(si))
	{
	}
	Thread::mSleep(500);
	ins->complete += 1;
	return 0;
}
