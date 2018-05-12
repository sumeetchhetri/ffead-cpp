/*
 * RequestReaderHandler.h
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#ifndef REQUESTREADERHANDLER_H_
#define REQUESTREADERHANDLER_H_
#include "queue"
#include "string"
#include "SelEpolKqEvPrt.h"
#include "SocketInterface.h"
#include "Thread.h"
#include "Mutex.h"
#include "map"
#include "ReaderSwitchInterface.h"
#include "ServiceHandler.h"
#include "SynchronizedQueue.h"
#include "LoggerFactory.h"
#include "SSLClient.h"
#include "Client.h"
#ifdef HAVE_CXX11
#include "atomic"
#endif


typedef SocketInterface* (*SocketInterfaceFactory) (SOCKET);

class RequestReaderHandler : public ReaderSwitchInterface {
	SynchronizedQueue<SocketInterface*> pendingSocks;
	SynchronizedQueue<SocketInterface*> addToTimeoutSocks;
	SynchronizedQueue<SocketInterface*> remFromTimeoutSocks;
	SynchronizedQueue<SocketInterface*> timedoutSocks;
	SynchronizedQueue<SocketInterface*> readerSwitchedSocks;
	SelEpolKqEvPrt selector;
	std::atomic<bool> run;
	std::atomic<int> complete;
	bool isNotRegisteredListener;
	SOCKET listenerSock;
	ServiceHandler* shi;
	long siIdentifierSeries;
	SocketInterfaceFactory sf;
	std::map<int, SocketInterface*> connections;
	std::map<int, SocketInterface*> connectionsWithTimeouts;
	bool isActive();
	void addSf(SocketInterface* sf);
	static void* handleTimeouts(void* inp);
	static void* handle(void* inp);
public:
	void switchReaders(SocketInterface* prev, SocketInterface* next);
	void registerRead(SocketInterface* sd);
	void start();
	void stop(std::string, int, bool);
	RequestReaderHandler(ServiceHandler* shi, const SOCKET& listenerSock= INVALID_SOCKET);
	void registerSocketInterfaceFactory(const SocketInterfaceFactory& f);
	virtual ~RequestReaderHandler();
};

#endif /* REQUESTREADERHANDLER_H_ */
