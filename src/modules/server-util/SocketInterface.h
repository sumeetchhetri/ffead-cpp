/*
 * SocketInterface.h
 *
 *  Created on: 03-Dec-2014
 *      Author: sumeetc
 */

#ifndef SOCKETINTERFACE_H_
#define SOCKETINTERFACE_H_
#include "Compatibility.h"
#include "vector"
/*HTTPS related*/
#include <unistd.h>
#include <sys/types.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "string"
#include "Mutex.h"
#include <fcntl.h>
#include "SSLHandler.h"
#include "Timer.h"
#include <sys/stat.h>
#include <fcntl.h>
#if defined(OS_DARWIN) || defined(OS_BSD)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#elif defined(IS_SENDFILE)
#include <sys/sendfile.h>
#endif
#include "vector"
#include <libcuckoo/cuckoohash_map.hh>
#include "concurrentqueue.h"
#include "map"

class SocketInterface;

class ResponseData {
	std::string _b;
	int oft;
	bool done;
	friend class SocketInterface;
};

class EventHandler {
public:
	virtual bool unRegisterWrite(SocketInterface* obj)=0;
	virtual bool registerWrite(SocketInterface* obj)=0;
	virtual ~EventHandler(){}
};

class SocketInterface {
	static std::atomic<int> openSocks;
	EventHandler* eh;
	SSL *ssl;
	BIO *io;
	SOCKET fd;
	std::atomic<bool> closed;
	bool http2;
	Logger logger;
	std::string buffer;
	std::atomic<int> tid;
	Mutex m;
	Mutex wm;
	std::map<int, ResponseData*> wtl;
	//cuckoohash_map<int, ResponseData*> wtl;
	std::atomic<int> reqPos;
	std::atomic<int> current;
	std::string address;
	bool isBlocking();
	bool handleRenegotiation();
	void init(const SOCKET& fd);
	friend class RequestReaderHandler;
	friend class ServiceHandler;
	friend class HandlerRequest;
	friend class HttpServiceTask;
	friend class HttpReadTask;
	friend class SelEpolKqEvPrt;
	friend class CHServer;
	friend class Http2Handler;
	friend class Http11Handler;
	friend class Http11WebSocketHandler;
	friend class CommonUtils;
	friend class DummySocketInterface;
public:
	bool completeWrite();
	int pushResponse(void* request, void* response, void* context, int reqPos);
	static bool init(const SOCKET& fd, SSL*& ssl, BIO*& io, Logger& logger);
	int startRequest();
	int endRequest(int reqPos);
	bool allRequestsDone();
	bool isCurrentRequest(int reqp);
	void writeTo(const std::string& d, int reqPos);
	int writeTo(ResponseData* d);
	bool writeFile(int fdes, int remain_data);
	bool isClosed();
	int readFrom();
	int getDescriptor();
	std::string getAddress();
	std::string getAlpnProto();
	bool isHttp2();
	SocketInterface();
	SocketInterface(const SOCKET& fd, SSL* ssl, BIO* io);
	virtual ~SocketInterface();
	bool flush();
	void closeSocket();
	bool checkSocketWaitForTimeout(const int& writing, const int& seconds, const int& micros= 0);
	virtual std::string getProtocol(void* context)=0;
	virtual int getTimeout()=0;
	virtual void* readRequest(void*& context, int& pending, int& reqPos)=0;
	virtual bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos)=0;
	virtual void onOpen()=0;
	virtual void onClose()=0;
	virtual void addHandler(SocketInterface* handler)=0;
};

#endif /* SOCKETINTERFACE_H_ */
