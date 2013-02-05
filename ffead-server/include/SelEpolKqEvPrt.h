/*
	Copyright 2009-2013, Sumeet Chhetri

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
 * SelEpolKqEvPrt.h
 *
 *  Created on: 30-Dec-2012
 *      Author: sumeetc
 */

#ifndef SELEPOLKQEVPRT_H_
#define SELEPOLKQEVPRT_H_
#include <sys/types.h>
#include "AppDefines.h"
#include "Logger.h"
#include "cstring"
#include <fcntl.h>
#include <sys/time.h>
#define MAXDESCRIPTORS 1024

class SelEpolKqEvPrt {
	Logger logger;
	int mode;
	int sockfd;
	int curfds;
	#ifdef USE_SELECT
		fd_set master;    // master file descriptor list
		fd_set read_fds;  // temp file descriptor list for select()
		int fdmax;
	#endif
	#ifdef USE_EPOLL
		struct epoll_event ev;
		struct epoll_event events[MAXDESCRIPTORS];
		int epoll_handle;
	#endif
	#ifdef USE_KQUEUE
		int kq;
		struct kevent evlist[MAXDESCRIPTORS];
	    struct kevent change;
	#endif
	#ifdef USE_DEVPOLL
	    int dev_poll_fd;
	    struct pollfd polled_fds[MAXDESCRIPTORS];
	#endif
	#ifdef USE_EVPORT
	    int port;
	    port_event_t evlist[MAXDESCRIPTORS];
	#endif
	#ifdef USE_POLL
	    nfds_t nfds;
	    struct pollfd *polled_fds;
	#endif
public:
	SelEpolKqEvPrt();
	virtual ~SelEpolKqEvPrt();
	void initialize(int sockfd);
	int getEvents();
	int getDescriptor(int index);
	bool isListeningDescriptor(int descriptor);
	bool registerForEvent(int descriptor);
	bool unRegisterForEvent(int descriptor);
	void reRegisterServerSock();
};

#endif /* SELEPOLKQEVPRT_H_ */
