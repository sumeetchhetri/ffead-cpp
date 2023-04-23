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
#include "Server.h"
#include "Task.h"

class ResponseData {
public:
	std::string _b;
	int oft;
	ResponseData();
	virtual ~ResponseData();
};

typedef void (*CleanerFunc) (void* data);
typedef void (*SockCloseEvent) (void* data);

class BaseSocket: public Writer {
protected:
	//static std::atomic<int> openSocks;
	SOCKET fd;
	std::atomic<bool> closed;
#if defined USE_IO_URING
	char buff[2048];
#else
	char buff[8192];
#endif
	std::string buffer;
	std::string address;
	int io_uring_type;
	bool isBlocking();
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
	friend class HttpServiceHandler;
	friend class LibpqDataSourceImpl;
	friend class RequestHandler2;
	friend class Http11Socket;
	static SockCloseEvent sockCloseFunc;
public:
	BaseSocket();
	BaseSocket(const SOCKET& fd);
	virtual ~BaseSocket();
	int type() {
		return 1;
	}
	int readFrom();
	int readSync();
	int checkReadSync();
	int writeData(void* data) {
		std::string* cont = (std::string*)data;
		return writeDirect(*cont);
	}
	int writeDirect(const std::string& d, int off = 0, bool cont = false);
	int writeDirect(const std::string& h, const std::string& d);
	int writeDirect(const std::string& h, const char* d, size_t len, bool cont = false);
	int writeTo(ResponseData* d);

	bool writeFile(int fdes, int remain_data);
	bool isClosed();
	int getDescriptor();
	std::string getAddress();
	void doneRead();
	virtual bool hasPendingRead() {
		return false;
	}
	bool checkSocketWaitForTimeout(const int& writing, const int& seconds, const int& micros= 0);
	virtual bool flush();
	virtual void closeSocket();

	virtual int getType(void* context){return -1;};
	virtual int getTimeout(){return -1;};
	virtual bool isEmbedded(){return true;};
	virtual bool isSecure();
	virtual int writeWsData(void* d){return -1;}

	virtual int secureWriteDirect(const std::string& d, int off = 0){return -1;};
	virtual int secureWriteDirect(const char* d, size_t len, int off = 0){return -1;};
	virtual int secureWriteTo(ResponseData* d){return -1;};
	virtual int secureReadFrom(){return -1;};
	virtual int secureReadSync(){return -1;};

	virtual bool handle() {
		return false;
	}
};


class BaseSecureSocket: public BaseSocket {
protected:
#ifdef HAVE_SSLINC
	SSL *ssl;
	BIO *io;
#endif
	bool handleRenegotiation();
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
	friend class HttpServiceHandler;
	friend class LibpqDataSourceImpl;
	friend class RequestHandler2;
	friend class Http11Socket;
public:
#ifdef HAVE_SSLINC
	static bool init(const SOCKET& fd, SSL*& ssl, BIO*& io);
#endif
	int secureWriteDirect(const std::string& d, int off = 0);
	int secureWriteDirect(const char* d, size_t len, int off = 0);
	int secureWriteTo(ResponseData* d);
	int secureReadFrom();
	int secureReadSync();
	void closeSocket();
	bool flush();
	bool isSecure();
	BaseSecureSocket();
	BaseSecureSocket(const SOCKET& fd);
	virtual ~BaseSecureSocket();
};

class SocketInterface: public BaseSecureSocket {
	Task* rdTsk;
	Task* srvTsk;
	Task* wrTsk;
	bool http2;
	std::atomic<int> tid;
	std::map<int, ResponseData> wtl;
	std::atomic<int> reqPos;
	std::atomic<int> current;
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
	friend class HttpServiceHandler;
	friend class LibpqDataSourceImpl;
public:
	SocketInterface();
	SocketInterface(const SOCKET& fd);
#ifdef HAVE_SSLINC
	SocketInterface(const SOCKET& fd, SSL* ssl, BIO* io);
#endif
	virtual ~SocketInterface();

	int completeWrite();
	int pushResponse(void* request, void* response, void* context, int reqPos);

	int startRequest();
	int endRequest(int reqPos);
	bool allRequestsDone();
	bool isCurrentRequest(int reqp);
	void writeToBuf(const std::string& d, int reqPos);
	std::string getAlpnProto();
	bool isHttp2();

	virtual std::string getProtocol(void* context)=0;
	virtual bool readRequest(void* request, void*& context, int& pending, int& reqPos)=0;
	virtual bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos)=0;
	virtual void addHandler(SocketInterface* handler)=0;
};

typedef BaseSocket* (*SocketInterfaceFactory) (SOCKET);
typedef void (*CleanSocket) (BaseSocket*);

#endif /* SOCKETINTERFACE_H_ */
