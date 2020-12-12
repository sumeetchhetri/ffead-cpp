/*
	Copyright 2009-2013, Sumeet Chhetri

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
 * SelEpolKqEvPrt.h
 *
 *  Created on: 30-Dec-2012
 *      Author: sumeetc
 */

#ifndef SELEPOLKQEVPRT_H_
#define SELEPOLKQEVPRT_H_
#include "Compatibility.h"
#include "map"
#include "Mutex.h"
#include "SocketInterface.h"
#ifdef HAVE_SSLINC
#include "SSLHandler.h"
#endif

#define MAXDESCRIPTORS 4096
#define OP_READ     0
#define OP_WRITE    1

#if USE_EVPORT == 1
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <port.h>
#include <poll.h>
#elif USE_EPOLL == 1
	#define USE_EPOLL_ET
	#undef USE_EVPORT
	#undef USE_KQUEUE
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#ifdef USE_EPOLL_ET
		#undef USE_EPOLL_LT
	#endif
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/epoll.h>
#elif USE_KQUEUE == 1
	#undef USE_EVPORT
	#undef USE_EPOLL
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/event.h>
#elif USE_DEVPOLL == 1
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/devpoll.h>
#elif USE_POLL == 1 && !defined(OS_CYGWIN)
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <poll.h>
#elif USE_POLL == 1
	#define USE_SELECT 1
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/select.h>
#elif USE_WIN_IOCP == 1
	#undef USE_WIN_IOCP
	#undef USE_SELECT
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#define USE_EPOLL_LT 1
	#undef USE_EPOLL_ET
	#undef USE_IO_URING
#include "wepoll.h"
#elif USE_MINGW_SELECT == 1
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#elif USE_SELECT == 1
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/select.h>
#elif USE_IO_URING == 1
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#define MAX_CONNECTIONS     MAXDESCRIPTORS
	#define BACKLOG             512
	#define MAX_MESSAGE_LEN     2048
	#define BUFFERS_COUNT       MAX_CONNECTIONS
#include <sys/uio.h>
#include <sys/poll.h>
#include "liburing.h"
#endif

#if !defined(USE_EPOLL) && !defined(USE_KQUEUE) && !defined(USE_WIN_IOCP) && !defined(USE_EVPORT)
#include <libcuckoo/cuckoohash_map.hh>
#endif

enum io_uring_events {
    ACCEPT,
    READ,
    WRITE,
    PROV_BUF
};

enum event_type {
	ACCEPTED,
	READ_READY,
	WRITE_READY,
	CLOSED,
	ON_DATA_READ,
	ON_DATA_WRITE,
};

class SelEpolKqEvPrt;

typedef SocketInterface* (*onEvent) (SelEpolKqEvPrt* ths, SocketInterface* sfd, int type, int fd, char* buf, size_t len, bool isClosed);
typedef bool (*eventLoopContinue) (SelEpolKqEvPrt* ths);

class DummySocketInterface : public SocketInterface {
public:
	DummySocketInterface() {
		closed = true;
	}
	~DummySocketInterface(){}
	std::string getProtocol(void* context){return "";}
	int getTimeout(){return -1;};
	bool readRequest(void* request, void*& context, int& pending, int& reqPos){return false;}
	bool writeResponse(void* req, void* res, void* context, std::string& d, int reqPos){return false;}
	void onOpen(){}
	void onClose(){}
	void addHandler(SocketInterface* handler){}
	int getType(void* context) {return -1;}
	bool isEmbedded(){return true;}
};

class SelEpolKqEvPrt : public EventHandler {
	bool listenerMode;
	int timeoutMilis;
	SOCKET sockfd;
	SOCKET curfds;
	void* context;
	Mutex l;
	DummySocketInterface* dsi;
	#if !defined(USE_EPOLL) && !defined(USE_KQUEUE) && !defined(USE_WIN_IOCP) && !defined(USE_EVPORT)
		libcuckoo::cuckoohash_map<int, void*> connections;
	#endif
	#ifdef USE_MINGW_SELECT
		SOCKET fdMax;
		int fdsetSize;
		fd_set readfds;  // temp file descriptor list for select()
		fd_set master;
	#elif USE_SELECT
		int fdMax, fdsetSize;
		fd_set readfds[1024/FD_SETSIZE];
		fd_set writefds[1024/FD_SETSIZE];
		fd_set master[1024/FD_SETSIZE];
	#elif defined USE_EPOLL
		struct epoll_event events[MAXDESCRIPTORS];
		int epoll_handle;
	#elif defined USE_WIN_IOCP
		struct epoll_event events[MAXDESCRIPTORS];
		HANDLE epoll_handle;
	#elif defined USE_KQUEUE
		int kq;
		struct kevent evlist[MAXDESCRIPTORS];
	#elif defined USE_DEVPOLL
	    int dev_poll_fd;
	    struct pollfd polled_fds[MAXDESCRIPTORS];
	#elif defined USE_EVPORT
	    int port;
	    port_event_t evlist[MAXDESCRIPTORS];
	#elif defined USE_POLL
	    nfds_t nfds;
	    struct pollfd polled_fds[MAXDESCRIPTORS];
	#elif defined USE_IO_URING
	    struct sockaddr_in client_addr;
	    socklen_t client_len;
	    struct io_uring_params params;
	    struct io_uring ring;
	    //char** bufs;
	    //int group_id;
	#endif
public:
	SelEpolKqEvPrt();
	virtual ~SelEpolKqEvPrt();
#if defined(USE_IO_URING) //Not thread safe
	void post_write(SocketInterface* sfd, const std::string& data);
	void post_read(SocketInterface* sfd);
#endif
	void setCtx(void* ctx);
	void* getCtx();
	void loop(eventLoopContinue evlc, onEvent ev);
	void initialize(const int& timeout);
	void initialize(SOCKET sockfd, const int& timeout);
	int getEvents();
	void addListeningSocket(SOCKET sockfd);
	SOCKET getDescriptor(const SOCKET& index, void*& obj, bool& isRead);
	bool isListeningDescriptor(const SOCKET& descriptor);
	bool registerWrite(SocketInterface* obj);
	bool unRegisterWrite(SocketInterface* obj);
	bool registerRead(SocketInterface* obj, const bool& isListeningSock = false, bool epoll_et = true);
	bool unRegisterRead(const SOCKET& descriptor);
	void* getOptData(const int& index);
	void reRegisterServerSock(void* obj);
	bool isInvalidDescriptor(const SOCKET& index);
	void lock();
	void unlock();
};

#endif /* SELEPOLKQEVPRT_H_ */
