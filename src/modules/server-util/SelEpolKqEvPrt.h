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

#define MAXDESCRIPTORS 1024
#define OP_READ     0
#define OP_WRITE    1
#define MAX_TIMEOUT 10

#if defined(USE_PICOEV)
#ifdef OS_LINUX
	#include <sys/epoll.h>
	#define USE_PICOEV_EPOLL 1
#elif defined(OS_BSD) || defined(APPLE)
	#include <sys/event.h>
	#define USE_PICOEV_KQUEUE 1
#elif defined(USE_WIN_IOCP)
	#include "wepoll.h"
	#define USE_PICOEV_IOCP 1
#elif defined(OS_MINGW) || defined(CYGWIN)
	#define USE_PICOEV_SELECT
#endif
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
	#undef USE_EVPORT
#include "picoev.h"
#elif defined(USE_EVPORT)
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <port.h>
#include <poll.h>
#elif defined(USE_EPOLL)
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
#elif defined(USE_KQUEUE)
	#undef USE_EVPORT
	#undef USE_EPOLL
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/event.h>
#elif defined(USE_DEVPOLL)
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/devpoll.h>
#elif defined(USE_POLL) && !defined(CYGWIN)
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <poll.h>
#elif defined(USE_POLL)
	#define USE_SELECT 1
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/select.h>
#elif defined(USE_WIN_IOCP)
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
#elif defined(USE_MINGW_SELECT)
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_SELECT
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#elif defined(USE_SELECT)
	#undef USE_EPOLL
	#undef USE_KQUEUE
	#undef USE_EVPORT
	#undef USE_DEVPOLL
	#undef USE_POLL
	#undef USE_WIN_IOCP
	#undef USE_IO_URING
#include <sys/select.h>
#elif defined(USE_IO_URING)
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
#include <sys/eventfd.h>
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
    PROV_BUF,
	INTERRUPT
};

enum event_type {
	ACCEPTED,
	READ_READY,
	WRITE_READY,
	CLOSED,
	ON_DATA_READ,
	ON_DATA_WRITE
};

class SelEpolKqEvPrt;

typedef BaseSocket* (*onEvent) (SelEpolKqEvPrt* ths, BaseSocket* sfd, int type, int fd, char* buf, size_t len, bool isClosed);
typedef bool (*eventLoopContinue) (SelEpolKqEvPrt* ths);


class SelEpolKqEvPrt : public EventHandler {
	bool listenerMode;
	int timeoutMilis;
	SOCKET sockfd;
	SOCKET curfds;
	void* context;
	Mutex l;
	BaseSocket* dsi;
	eventLoopContinue elcCb;
	onEvent eCb;
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
	    //Needs higher amount of max locked memory
	    //set ulimit -l
	    //in docker use something like , --ulimit memlock=102400000:102400000
	    struct sockaddr_in client_addr;
	    socklen_t client_len;
	    struct io_uring_params params;
	    struct io_uring ring;
	    //int efd;
		//BaseSocket* efdbs;
		//std::atomic<int> pending;
	    //char** bufs;
	    //int group_id;
	#elif defined(USE_PICOEV)
	    picoev_loop* picoevl;
	    int timeoutsec;
	#endif
public:
	SelEpolKqEvPrt();
	virtual ~SelEpolKqEvPrt();
#if defined(USE_IO_URING) //Not thread safe
	void interrupt_wait();
	void register_interrupt();
	void submit_to_ring();
	void post_write(BaseSocket* sfd, const std::string& data, int off = 0);
	void post_write_2(BaseSocket* sfd, const std::string& data, const std::string& data1);
	void post_write(BaseSocket* sfd, const char* data, int len);
	void post_read(BaseSocket* sfd);
#endif
	void setCtx(void* ctx);
	void* getCtx();
	void loop(eventLoopContinue evlc, onEvent ev, SelEpolKqEvPrt* optSel = NULL);
	void initialize(const int& timeout, eventLoopContinue elcCb = NULL, onEvent eCb = NULL);
	void initialize(SOCKET sockfd, const int& timeout, eventLoopContinue elcCb = NULL, onEvent eCb = NULL);
	int getEvents();
	void addListeningSocket(SOCKET sockfd);
	SOCKET getDescriptor(const SOCKET& index, void*& obj, bool& isRead);
	bool isListeningDescriptor(const SOCKET& descriptor);
	bool registerWrite(BaseSocket* obj);
	bool unRegisterWrite(BaseSocket* obj);
	bool registerRead(BaseSocket* obj, const bool& isListeningSock = false, bool epoll_et = true, bool isNonBlocking = false);
	bool unRegisterRead(const SOCKET& descriptor);
	void* getOptData(const int& index);
	void reRegisterServerSock(void* obj);
	bool isInvalidDescriptor(const SOCKET& index);
	void lock();
	void unlock();
#ifdef USE_PICOEV
	static void picoevAcb(picoev_loop* loop, int fd, int events, void* cb_arg);
	static void picoevRwcb(picoev_loop* loop, int fd, int events, void* cb_arg);
#endif
};

#endif /* SELEPOLKQEVPRT_H_ */
