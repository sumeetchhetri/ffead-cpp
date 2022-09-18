/*
	Copyright 2009-2020, Sumeet Chhetri
  
    Licensed under the Apache License, Version 2.0 (const the& "License"); 
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
 * Server.h
 *
 *  Created on: Jan 2, 2010
 *      Author: sumeet
 */

#ifndef SERVER_H_
#define SERVER_H_
#include "Compatibility.h"
#include "vector"
/*HTTPS related*/
#include <unistd.h>
#include <sys/types.h>
#include "string"
#include "Mutex.h"
#include <fcntl.h>
#ifdef HAVE_SSLINC
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "SSLHandler.h"
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include "vector"
#include "concurrentqueue.h"
#include "map"
#ifndef OS_MINGW
#include <netinet/tcp.h>
#endif
#ifdef HAVE_SO_ATTACH_REUSEPORT_CBPF
#include <linux/bpf.h>
#include <linux/filter.h>
#endif
#include "StringUtil.h"
#include "CastUtil.h"
#include "LoggerFactory.h"
#ifdef HAVE_SYSINFO
#include <sys/sysinfo.h>
#endif
#include "blockingconcurrentqueue.h"

/*Fix for Windows Cygwin*///#include <sys/epoll.h>

#define MAXEPOLLSIZES 10000
#define BACKLOGM 500

class Writer;
class BaseSocket;

typedef void (*WriterEvent) (Writer* bs, int type);
typedef int (*cb_into_pv_for_date) ();
typedef int (*cb_into_pv) (const char* hline, size_t hline_len, const char* body, size_t body_len, int fd, void* pv);
typedef void (*cb_reg_ext_fd_pv) (int fd, void* pv);
typedef void (*CleanerFunc) (void* data);

const char gDigitsLut_fcp[200] = {
	'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
	'1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
	'2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
	'3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
	'4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
	'5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
	'6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
	'7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
	'8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
	'9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'
};

static inline int u64toa_fcp(char* buf, uint64_t value) {
	const char* b = buf;
	if (value < 100000000) {
		uint32_t v = (uint32_t)(value);
		if (v < 10000) {
			const uint32_t d1 = (v / 100) << 1;
			const uint32_t d2 = (v % 100) << 1;

			if (v >= 1000)
				*buf++ = gDigitsLut_fcp[d1];
			if (v >= 100)
				*buf++ = gDigitsLut_fcp[d1 + 1];
			if (v >= 10)
				*buf++ = gDigitsLut_fcp[d2];
			*buf++ = gDigitsLut_fcp[d2 + 1];
		}
		else {
			// value = bbbbcccc
			const uint32_t b = v / 10000;
			const uint32_t c = v % 10000;

			const uint32_t d1 = (b / 100) << 1;
			const uint32_t d2 = (b % 100) << 1;

			const uint32_t d3 = (c / 100) << 1;
			const uint32_t d4 = (c % 100) << 1;

			if (value >= 10000000)
				*buf++ = gDigitsLut_fcp[d1];
			if (value >= 1000000)
				*buf++ = gDigitsLut_fcp[d1 + 1];
			if (value >= 100000)
				*buf++ = gDigitsLut_fcp[d2];
			*buf++ = gDigitsLut_fcp[d2 + 1];

			*buf++ = gDigitsLut_fcp[d3];
			*buf++ = gDigitsLut_fcp[d3 + 1];
			*buf++ = gDigitsLut_fcp[d4];
			*buf++ = gDigitsLut_fcp[d4 + 1];
		}
	}
	// *buf = '\0';
	return buf - b;
}

typedef void (*SockWriteRequestF) (Writer* bs, void* arg);

class SockWriteRequest {
	SockWriteRequestF f;
	Writer* bs;
	void* arg;
	void* arg1;
	SockWriteRequest():f(NULL), bs(NULL), arg(NULL), arg1(NULL) {}
	SockWriteRequest(SockWriteRequestF f, Writer* bs, void* arg): f(f), bs(bs), arg(arg), arg1(NULL) {}
	SockWriteRequest(SockWriteRequestF f, Writer* bs, void* arg, void* arg1): f(f), bs(bs), arg(arg), arg1(arg1) {}
	friend class RequestHandler2;
	friend class EventHandler;
};

class EventHandler {
	moodycamel::BlockingConcurrentQueue<SockWriteRequest>* wQ;
	template<typename Func1> void queueWrite(Func1 f, Writer* bs, void* arg) {
		wQ->enqueue(SockWriteRequest(f, bs, arg));
	}
	friend class Writer;
	friend class RequestHandler2;
	friend class RequestReaderHandler;
public:
	virtual bool unRegisterWrite(BaseSocket* obj)=0;
	virtual bool unRegisterRead(const SOCKET& descriptor)=0;
	virtual bool registerWrite(BaseSocket* obj)=0;
	virtual bool registerRead(BaseSocket* obj, const bool& isListeningSock = false, bool epoll_et = true, bool isNonBlocking = false)=0;
#if defined(USE_IO_URING)
	virtual void interrupt_wait()=0;
	virtual void post_write(BaseSocket* sfd, const std::string& data, int off = 0)=0;
	virtual void post_write_2(BaseSocket* sfd, const std::string& data, const std::string& data1)=0;
	virtual void post_write(BaseSocket* sfd, const char* data, int len)=0;
	virtual void post_read(BaseSocket* sfd)=0;
#endif
	EventHandler(): wQ(NULL) {}
	virtual ~EventHandler() {
		if(wQ!=NULL) {
			delete wQ;
			wQ = NULL;
		}
	}
};

class Writer {
	static cb_reg_ext_fd_pv pvregfd;
	static bool isPicoEvAsyncBackendMode;
	void* data;
	CleanerFunc cf;
	static WriterEvent we;
	friend class BaseSocket;
	friend class SocketInterface;
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
	friend class ServerInitUtil;
	friend class LibpqDataSourceImpl;
protected:
	EventHandler* eh;
	std::atomic<int> useCounter;
	long long cqat;
public:
	Writer(): data(NULL), cf(NULL), eh(NULL), cqat(-1) {
		useCounter = 0;
	}
	virtual ~Writer() {}
	virtual void onOpen() {}
	virtual void onClose() {}
	virtual void use() {
		useCounter++;
	}
	virtual void unUse() {
		useCounter--;
	}
	virtual void* getData() {
		return data;
	}
	template<typename Func1>
	void queueWrite(Func1 f, void* arg) {
		eh->queueWrite(f, this, arg);
	}
	template<typename CleanerF>
	void setData(void* data, CleanerF f) {
		if(data!=NULL && f!=NULL) {
			this->data = data;
			this->cf = f;
		}
	}
	virtual int internalWrite(const char* hline, size_t hline_len, const char* body, size_t body_len) {
		return -1;
	}
	virtual int writeData(void* data)=0;
	virtual int type() {
		return 0;
	}
	/*
	 * Please note that this is a one time event registration function
	 * that will intercept all new connections to the server, this
	 * should be used only in cases where there is a single context
	 * being served, multiple context support cannot be provided as
	 * when the connection is opened, we do not know the context/app
	 * being served yet
	 */
	template<typename WriterEventF>
	static void registerWriterEventCallback(WriterEventF nwe) {
		we = nwe;
	}
	static void onWriterEvent(Writer* writer, int type) {
		we(writer, type);
	}
};

typedef void* (*Service)(void*);
class Server {
	//Logger logger;
	SOCKET sock;
	int mode;
	Service service;
	Mutex lock;
	#ifdef OS_MINGW
		struct sockaddr_in their_addr;
	#else
		struct sockaddr_storage their_addr;
	#endif
	
	static void* servicing(void* arg);
	bool runn, started;
public:
	SOCKET getSocket();
	Server();
	Server(const std::string&, const bool&, const int&, const Service&, int);
	//Server(const std::string& port, const int& waiting, const Service& serv);
	virtual ~Server();
	SOCKET Accept();
	int Send(const SOCKET&, const std::string&);
	int Send(const SOCKET&, const std::vector<char>&);
	int Send(const SOCKET&, const std::vector<unsigned char>&);
	int Send(const SOCKET&, char*);
	int Send(const SOCKET&, unsigned char*);
	int Receive(const SOCKET&, std::string&, const int&);
	int Receive(const SOCKET&, std::vector<char>&, const int&);
	int Receive(const SOCKET&, std::vector<unsigned char>&, const int&);
	int Receive(const SOCKET&, const char *data, const int&);
	int Receive(const SOCKET&, const unsigned char *data, const int&);
	int Receive(const SOCKET&, std::vector<std::string>&, const int&);
	void start();
	void stop();

	static void set_cbpf(int socket, int group_size);
	static SOCKET createListener(const std::string& ipAddress, const int& port, const bool& block, bool isSinglEVH);
	static SOCKET createListener(const int& port, const bool& block, bool isSinglEVH);
};

#endif /* SERVER_H_ */
