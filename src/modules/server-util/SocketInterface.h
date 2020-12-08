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
#include "vector"
#include "concurrentqueue.h"
#include "map"
#include "Task.h"
#ifndef OS_MINGW
#include <netinet/tcp.h>
#endif

class SocketInterface;

class ResponseData {
public:
	std::string _b;
	int oft;
	ResponseData();
	virtual ~ResponseData();
};

class EventHandler {
public:
	virtual bool unRegisterWrite(SocketInterface* obj)=0;
	virtual bool unRegisterRead(const SOCKET& descriptor)=0;
	virtual bool registerWrite(SocketInterface* obj)=0;
	virtual bool registerRead(SocketInterface* obj, const bool& isListeningSock = false)=0;
#if defined(USE_IO_URING)
	virtual void post_write(SocketInterface* sfd, const std::string& data)=0;
	virtual void post_read(SocketInterface* sfd)=0;
#endif
	virtual ~EventHandler(){}
};

class SocketInterface {
	Task* rdTsk;
	Task* srvTsk;
	Task* wrTsk;
	static std::atomic<int> openSocks;
	EventHandler* eh;
	SSL *ssl;
	BIO *io;
	SOCKET fd;
	std::atomic<bool> closed;
	bool http2;
	int io_uring_type;
	//int io_uring_bid;
	Logger logger;
	char buff[2048];
	std::string buffer;
	std::atomic<int> tid;
	std::atomic<int> useCounter;
	//Mutex m;
	//Mutex wm;
	std::map<int, ResponseData> wtl;
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
	friend class HttpWriteTask;
	friend class SelEpolKqEvPrt;
	friend class CHServer;
	friend class Http2Handler;
	friend class Http11Handler;
	friend class Http11WebSocketHandler;
	friend class CommonUtils;
	friend class DummySocketInterface;
	friend class HttpServiceHandler;
	friend class LibpqDataSourceImpl;
public:
	int completeWrite();
	int pushResponse(void* request, void* response, void* context, int reqPos);
	static bool init(const SOCKET& fd, SSL*& ssl, BIO*& io, Logger& logger);
	int startRequest();
	int endRequest(int reqPos);
	bool allRequestsDone();
	bool isCurrentRequest(int reqp);
	void writeTo(const std::string& d, int reqPos);
	int writeTo(ResponseData* d);
	int writeDirect(const std::string& d, int off = 0);
	int writeWsData(void* d);
	bool writeFile(int fdes, int remain_data);
	bool isClosed();
	virtual int readFrom();
	int getDescriptor();
	std::string getAddress();
	std::string getAlpnProto();
	bool isHttp2();
	SocketInterface();
	SocketInterface(const SOCKET& fd, SSL* ssl, BIO* io);
	virtual ~SocketInterface();
	bool flush();
	void closeSocket();
	void doneRead();
	bool checkSocketWaitForTimeout(const int& writing, const int& seconds, const int& micros= 0);
	virtual std::string getProtocol(void* context)=0;
	virtual int getType(void* context)=0;
	virtual int getTimeout()=0;
	virtual bool readRequest(void* request, void*& context, int& pending, int& reqPos)=0;
	virtual bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos)=0;
	virtual void onOpen()=0;
	virtual void onClose()=0;
	virtual void addHandler(SocketInterface* handler)=0;
	virtual bool isEmbedded()=0;
	virtual bool hasPendingRead();
	void use();
	void unUse();
};

#endif /* SOCKETINTERFACE_H_ */
