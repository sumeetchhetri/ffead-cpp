/*
 * RequestHandler2.h
 *
 *  Created on: 25 Aug 2021
 *      Author: sumeetc
 */

#ifndef MODULES_SERVER_UTIL_REQUESTHANDLER2_H_
#define MODULES_SERVER_UTIL_REQUESTHANDLER2_H_
#include "SelEpolKqEvPrt.h"
#include "SocketInterface.h"
#include "Thread.h"
#include "Mutex.h"
#include "map"
#include "ServiceHandler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Client.h"
#include "atomic"
#ifdef HAVE_SSLINC
#include "SSLClient.h"
#include "SSLHandler.h"
#endif

class Http11Socket: public BaseSecureSocket {
	bool isHeadersDone;
	int bytesToRead;
	bool isTeRequest;
	int chunkSize;
	int connKeepAlive;
	int maxReqHdrCnt;
	int maxEntitySize;

	HttpRequest req;
	HttpResponse res;
	std::vector<ResponseData> outbuf;
	bool read();
	friend class RequestHandler2;
public:
	Http11Socket(const SOCKET& fd, const int& chunkSize, const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize);
	virtual ~Http11Socket();
	void handle();
	int getTimeout();
};

typedef Http11Socket* (*SocketInterfaceFactory2) (SOCKET);

class RequestHandler2 {
	SelEpolKqEvPrt selector;
	std::atomic<bool> run;
	std::atomic<int> complete;
	bool isMain;
	bool isSSLEnabled;
	bool isNotRegisteredListener;
	SOCKET listenerSock;
	ServiceHandler* shi;
	SocketInterfaceFactory2 sf;
	std::vector<Http11Socket*> clsdConns;
	bool isActive();
	static void* handle(void* inp);
	static void* handleWrites(void* inp);
	static RequestHandler2* _i;
	httpSockHandle hsh;
	friend class LibpqDataSourceImpl;
	friend class Http11Socket;
public:
	void registerSocketInterfaceFactory(const SocketInterfaceFactory2& f);
	static bool loopContinue(SelEpolKqEvPrt* ths);
	static BaseSocket* loopEventCb(SelEpolKqEvPrt* ths, BaseSocket* sfd, int type, int fd, char* buf, size_t len, bool isClosed);
	static void setInstance(RequestHandler2*);
	static RequestHandler2* getInstance();
	void start(unsigned int cid, bool withWQ);
	void startNL(unsigned int cid, bool withWQ);
	void addListenerSocket(doRegisterListener drl, const SOCKET& listenerSock);
	void stop(std::string, int, bool);
	RequestHandler2(ServiceHandler* shi, const bool& isMain, bool isSSLEnabled, httpSockHandle hsh, const SOCKET& listenerSock = INVALID_SOCKET);
	virtual ~RequestHandler2();
};

#endif /* MODULES_SERVER_UTIL_REQUESTHANDLER2_H_ */
