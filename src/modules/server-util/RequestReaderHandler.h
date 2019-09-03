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
#include "ServiceHandler.h"
#include "LoggerFactory.h"
#include "SSLClient.h"
#include "Client.h"
#include "concurrentqueue.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "atomic"

typedef SocketInterface* (*SocketInterfaceFactory) (SOCKET);

class RequestReaderHandler {
	SelEpolKqEvPrt selector;
	std::atomic<bool> run;
	std::atomic<int> complete;
	bool isMain;
	bool isNotRegisteredListener;
	SOCKET listenerSock;
	ServiceHandler* shi;
	long siIdentifierSeries;
	SocketInterfaceFactory sf;
	std::vector<SocketInterface*> clsdConns;
	bool isActive();
	void addSf(SocketInterface* sf);
	static void* handle(void* inp);
public:
	void start(unsigned int cid);
	void stop(std::string, int, bool);
	RequestReaderHandler(ServiceHandler* shi, const bool& isMain, const SOCKET& listenerSock = INVALID_SOCKET);
	void registerSocketInterfaceFactory(const SocketInterfaceFactory& f);
	virtual ~RequestReaderHandler();
};

#endif /* REQUESTREADERHANDLER_H_ */
