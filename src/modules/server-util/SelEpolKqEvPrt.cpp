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
 * SelEpolKqEvPrt.cpp
 *
 *  Created on: 30-Dec-2012
 *      Author: sumeetc
 */

#include "SelEpolKqEvPrt.h"

SelEpolKqEvPrt::SelEpolKqEvPrt() {
	listenerMode = true;
	sockfd = -1;
	curfds = 0;
	timeoutMilis = -1;
	elcCb = NULL;
	eCb = NULL;
#if defined(USE_EPOLL)
	epoll_handle = -1;
#elif defined(USE_WIN_IOCP)
	epoll_handle = NULL;
#elif defined(USE_PICOEV)
	picoevl = NULL;
	timeoutsec = 1;
#elif defined(USE_KQUEUE)
	kq = -1;
#endif
	dsi = NULL;
	context = NULL;
}

SelEpolKqEvPrt::~SelEpolKqEvPrt() {
	if(dsi!=NULL) {
		delete dsi;
	}
#if defined USE_IO_URING
	//if(efdbs!=NULL) {
	//	delete efdbs;
	//}
	/*for (int i = 0; i < BUFFERS_COUNT; i++ )
	{
	    free(bufs[i]);
	}
	free(bufs);*/
#endif
}

void SelEpolKqEvPrt::initialize(const int& timeout, eventLoopContinue elcCb, onEvent eCb)
{
	this->elcCb = elcCb;
	this->eCb = eCb;
	this->timeoutMilis = timeout;
	curfds = 1;
	#if defined(USE_PICOEV)
		picoev_init(MAXDESCRIPTORS);
		timeoutsec = timeout/1000;
		if(timeout<=0) {
			timeoutsec = 1000;
		}
		picoevl = picoev_create_loop(MAX_TIMEOUT);
		picoevl->arg = this;
	#elif defined(USE_MINGW_SELECT)
		FD_ZERO(&readfds);
		FD_ZERO(&master);
	#elif defined(USE_SELECT)
		fdsetSize = 1024/FD_SETSIZE;
		for (int var = 0; var < fdsetSize; ++var) {
			FD_ZERO(&readfds[var]);
			FD_ZERO(&master[var]);
		}
	#elif defined USE_EPOLL || defined USE_WIN_IOCP
		epoll_handle = epoll_create1(0);
	#elif defined USE_KQUEUE
		kq = kqueue();
		if (kq == -1)
		{
			perror("kqueue");
		}
	#elif defined USE_DEVPOLL
		if((dev_poll_fd = open("/dev/poll", O_RDWR)) <0)
		{
			perror("devpoll");
		}
		if (fcntl(dev_poll_fd, F_SETFD, FD_CLOEXEC) < 0)
		{
			perror("devpoll fcntl");
		}
		for(int i=1;i<MAXDESCRIPTORS;i++) {
			polled_fds[i].fd = -1;
			polled_fds[i].revents = 0;
		}
	#elif defined USE_EVPORT
		if ((port = port_create()) < 0) {
			perror("port_create");
		}
	#elif defined USE_POLL
		nfds = 1;
		for(int i=0;i<MAXDESCRIPTORS;i++) {
			polled_fds[i].fd = -1;
			polled_fds[i].revents = 0;
		}
	#elif defined USE_IO_URING
		memset(&params, 0, sizeof(params));

		if (io_uring_queue_init_params(2048, &ring, &params) < 0) {
			perror("io_uring_init_failed...\n");
			return;
		}

		// check if IORING_FEAT_FAST_POLL is supported
		if (!(params.features & IORING_FEAT_FAST_POLL)) {
			printf("IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n");
			return;
		}
		/*efd = eventfd(0, O_NONBLOCK);
		if (efd < 0) {
			perror("eventfd");
			exit(0);
		}

		efdbs = new BaseSocket();
		efdbs->fd = efd;
		register_interrupt();

		pending = 0;*/

		// check if buffer selection is supported
		/*struct io_uring_probe *probe;
		probe = io_uring_get_probe_ring(&ring);
		if (!probe || !io_uring_opcode_supported(probe, IORING_OP_PROVIDE_BUFFERS)) {
			printf("Buffer select not supported, skipping...\n");
			return;
		}
		free(probe);

		bufs = (char**) calloc(BUFFERS_COUNT, sizeof(char*));
		for(int i=0;i<BUFFERS_COUNT;i++)
		{
			bufs[i] = (char*) calloc(MAX_MESSAGE_LEN, sizeof(char));
			memset(bufs[i], 0, MAX_MESSAGE_LEN);
		}
		group_id = 1881;

		struct io_uring_cqe* cqe;
		struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
		io_uring_prep_provide_buffers(sqe, bufs, MAX_MESSAGE_LEN, BUFFERS_COUNT, group_id, 0);

		io_uring_submit(&ring);
		io_uring_wait_cqe(&ring, &cqe);
		if (cqe->res < 0) {
			printf("cqe->res = %d\n", cqe->res);
			return;
		}
		io_uring_cqe_seen(&ring, cqe);*/
	#endif
}

void SelEpolKqEvPrt::addListeningSocket(SOCKET sockfd) {
	this->sockfd = sockfd;
	if(sockfd<=0)
	{
		listenerMode = false;
		sockfd = 0;
	}
	else
	{
		listenerMode = true;
	}
	#if defined(USE_MINGW_SELECT)
		fdMax = sockfd;
	#elif defined(USE_SELECT)
		fdMax = sockfd;
	#endif
	dsi = new BaseSocket();
	dsi->fd = sockfd;
	#ifdef USE_IO_URING
	    client_len = sizeof(client_addr);
	    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	    io_uring_prep_accept(sqe, dsi->fd, (sockaddr*)&client_addr, &client_len, 0);
	    io_uring_sqe_set_flags(sqe, 0);
	    dsi->io_uring_type = ACCEPT;
	    io_uring_sqe_set_data(sqe, dsi);
	    io_uring_submit(&ring);
	    return;
	#endif
	if(sockfd>0) registerRead(dsi, true);
}

void SelEpolKqEvPrt::initialize(SOCKET sockfd, const int& timeout, eventLoopContinue elcCb, onEvent eCb)
{
	this->elcCb = elcCb;
	this->eCb = eCb;
	this->timeoutMilis = timeout;
	this->sockfd = sockfd;
	if(sockfd<=0)
	{
		listenerMode = false;
		sockfd = 0;
	}
	else
	{
		listenerMode = true;
	}
	curfds = 1;
	#if defined(USE_PICOEV)
		picoev_init(MAXDESCRIPTORS);
		timeoutsec = timeout/1000;
		if(timeout<=0) {
			timeoutsec = 1000;
		}
		picoevl = picoev_create_loop(MAX_TIMEOUT);
		picoevl->arg = this;
	#elif defined(USE_MINGW_SELECT)
		fdMax = sockfd;
		FD_ZERO(&readfds);
		FD_ZERO(&master);
	#elif defined(USE_SELECT)
		fdsetSize = 1024/FD_SETSIZE;
		fdMax = sockfd;
		for (int var = 0; var < fdsetSize; ++var) {
			FD_ZERO(&readfds[var]);
			FD_ZERO(&master[var]);
		}
	#elif defined USE_EPOLL || defined USE_WIN_IOCP
		epoll_handle = epoll_create1(0);
	#elif defined USE_KQUEUE
		kq = kqueue();
		if (kq == -1)
		{
			perror("kqueue");
		}
	#elif defined USE_DEVPOLL
		if((dev_poll_fd = open("/dev/poll", O_RDWR)) <0)
		{
			perror("devpoll");
		}
		if (fcntl(dev_poll_fd, F_SETFD, FD_CLOEXEC) < 0)
		{
			perror("devpoll fcntl");
		}
		for(int i=1;i<MAXDESCRIPTORS;i++) {
			polled_fds[i].fd = -1;
			polled_fds[i].revents = 0;
		}
	#elif defined USE_EVPORT
		if ((port = port_create()) < 0) {
			perror("port_create");
		}
	#elif defined USE_POLL
		nfds = 1;
		for(int i=0;i<MAXDESCRIPTORS;i++) {
			polled_fds[i].fd = -1;
			polled_fds[i].revents = 0;
		}
	#endif
	dsi = new BaseSocket();
	dsi->fd = sockfd;
	#ifdef USE_IO_URING
		memset(&params, 0, sizeof(params));

		if (io_uring_queue_init_params(2048, &ring, &params) < 0) {
			perror("io_uring_init_failed...\n");
			return;
		}

		// check if IORING_FEAT_FAST_POLL is supported
		if (!(params.features & IORING_FEAT_FAST_POLL)) {
			printf("IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n");
			return;
		}

		/*efd = eventfd(0, O_NONBLOCK);
		if (efd < 0) {
			perror("eventfd");
			exit(0);
		}

		efdbs = new BaseSocket();
		efdbs->fd = efd;
		register_interrupt();

		pending = 0;*/

		// check if buffer selection is supported
		/*struct io_uring_probe *probe;
		probe = io_uring_get_probe_ring(&ring);
		if (!probe || !io_uring_opcode_supported(probe, IORING_OP_PROVIDE_BUFFERS)) {
			printf("Buffer select not supported, skipping...\n");
			return;
		}
		free(probe);

		bufs = (char**) calloc(BUFFERS_COUNT, sizeof(char*));
		for(int i=0;i<BUFFERS_COUNT;i++)
		{
			bufs[i] = (char*) calloc(MAX_MESSAGE_LEN, sizeof(char));
			memset(bufs[i], 0, MAX_MESSAGE_LEN);
		}
		group_id = 1881;

		struct io_uring_cqe* cqe;
		struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
		io_uring_prep_provide_buffers(sqe, bufs, MAX_MESSAGE_LEN, BUFFERS_COUNT, group_id, 0);

		io_uring_submit(&ring);
		io_uring_wait_cqe(&ring, &cqe);
		if (cqe->res < 0) {
			printf("cqe->res = %d\n", cqe->res);
			return;
		}
		io_uring_cqe_seen(&ring, cqe);*/

		client_len = sizeof(client_addr);
		struct io_uring_sqe *sqe1 = io_uring_get_sqe(&ring);
		io_uring_prep_accept(sqe1, dsi->fd, (sockaddr*)&client_addr, &client_len, 0);
		io_uring_sqe_set_flags(sqe1, 0);
		dsi->io_uring_type = ACCEPT;
		io_uring_sqe_set_data(sqe1, dsi);
		io_uring_submit(&ring);
		return;
	#endif
	if(sockfd>0) registerRead(dsi, true);
}


int SelEpolKqEvPrt::getEvents()
{
	int numEvents = -1;
	#if defined(USE_MINGW_SELECT)
		readfds = master;
		if(timeoutMilis>1)
		{
			struct timeval tv;
			tv.tv_sec = (timeoutMilis/1000);
			tv.tv_usec = (timeoutMilis%1000)*1000;
			numEvents = select(fdMax+1, &readfds, NULL, NULL, &tv);
		}
		else
		{
			numEvents = select(fdMax+1, &readfds, NULL, NULL, NULL);
		}
		if(numEvents==-1)
		{
			perror("select()");
		}
		else
		{
			if(fdMax>0)
				return fdMax+1;
		}
	#elif defined(USE_SELECT)
		for (int var = 0; var < fdsetSize; ++var) {
			readfds[var] = master[var];
			writefds[var] = master[var];
		}
		if(timeoutMilis>1)
		{
			struct timeval tv;
			tv.tv_sec = (timeoutMilis/1000);
			tv.tv_usec = (timeoutMilis%1000)*1000;
			numEvents = select(fdMax+1, readfds, NULL, NULL, &tv);
		}
		else
		{
			numEvents = select(fdMax+1, readfds, NULL, NULL, NULL);
		}
		if(numEvents==-1)
		{
			perror("select()");
		}
		else
		{
			if(fdMax>0)
				return fdMax+1;
		}
	#elif defined USE_EPOLL || defined USE_WIN_IOCP
		numEvents = epoll_wait(epoll_handle, events, MAXDESCRIPTORS, timeoutMilis);
	#elif defined USE_KQUEUE
		if(timeoutMilis>1)
		{
			struct timespec tv;
			tv.tv_sec = (timeoutMilis/1000);
			tv.tv_nsec = (timeoutMilis%1000)*1000000;
			numEvents = kevent(kq, NULL, 0, evlist, MAXDESCRIPTORS, &tv);
		}
		else
		{
			numEvents = kevent(kq, NULL, 0, evlist, MAXDESCRIPTORS, NULL);
		}
	#elif defined USE_DEVPOLL
		struct dvpoll pollit;
		pollit.dp_timeout = timeoutMilis>1?timeoutMilis:-1;
		pollit.dp_nfds = curfds;
		pollit.dp_fds = polled_fds;
		numEvents = ioctl(dev_poll_fd, DP_POLL, &pollit);
	#elif defined USE_EVPORT
		uint_t nevents = 1, wevents = 0;
		if(timeoutMilis>1)
		{
			struct timespec tv;
			tv.tv_sec = (timeoutMilis/1000);
			tv.tv_nsec = (timeoutMilis%1000)*1000000;
			//uint_t num = 0;
			if (port_getn(port, evlist, 0, &wevents, &tv) < 0) return 0;
			if (0 == wevents) wevents = 1;
			nevents = wevents;
			if (port_getn(port, evlist, (uint_t) MAXDESCRIPTORS, &nevents, &tv) < 0) return 0;
		}
		else
		{
			//uint_t num = 0;
			if (port_getn(port, evlist, 0, &wevents, NULL) < 0) return 0;
			if (0 == wevents) wevents = 1;
			nevents = wevents;
			if (port_getn(port, evlist, (uint_t) MAXDESCRIPTORS, &nevents, NULL) < 0) return 0;
		}
		numEvents = (int)nevents;
	#elif defined USE_POLL
		if(timeoutMilis>1)
		{
			numEvents = poll(polled_fds, curfds+1, timeoutMilis);
		}
		else
		{
			numEvents = poll(polled_fds, curfds+1, -1);
		}
		if (numEvents == -1) {
			perror ("poll");
		} else {
			return curfds+1;
		}
	#elif defined(USE_PICOEV)
		picoev_loop_once(picoevl, timeoutMilis/1000);
		numEvents = -1;
	#endif
	return numEvents;
}

bool SelEpolKqEvPrt::registerWrite(BaseSocket* obj) {
#if defined USE_EPOLL || defined USE_WIN_IOCP
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	#ifdef USE_EPOLL_LT
		ev.events = EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
	#else
		ev.events = EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET;
	#endif
	ev.data.ptr = obj;
	if (epoll_ctl(epoll_handle, EPOLL_CTL_MOD, obj->fd, &ev) < 0)
	{
		//perror("epoll");
		//std::cout << "Error adding to epoll cntl list" << std::endl;
		return false;
	}
#elif defined USE_KQUEUE
	struct kevent change;
	memset(&change, 0, sizeof(change));
	EV_SET(&change, obj->fd, EVFILT_READ, EV_DELETE, 0, 0, obj);
	kevent(kq, &change, 1, NULL, 0, NULL);
	memset(&change, 0, sizeof(change));
	EV_SET(&change, obj->fd, EVFILT_WRITE, EV_ADD, 0, 0, obj);
	kevent(kq, &change, 1, NULL, 0, NULL);
#endif
	return true;
}

bool SelEpolKqEvPrt::unRegisterWrite(BaseSocket* obj) {
#if defined USE_EPOLL || defined USE_WIN_IOCP
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	#ifdef USE_EPOLL_LT
		ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
	#else
		ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET;
	#endif
	ev.data.ptr = obj;
	if (epoll_ctl(epoll_handle, EPOLL_CTL_MOD, obj->fd, &ev) < 0)
	{
		//perror("epoll");
		//std::cout << "Error adding to epoll cntl list" << std::endl;
		return false;
	}
#elif defined USE_KQUEUE
	struct kevent change;
	memset(&change, 0, sizeof(change));
	EV_SET(&change, obj->fd, EVFILT_WRITE, EV_DELETE, 0, 0, obj);
	kevent(kq, &change, 1, NULL, 0, NULL);
	memset(&change, 0, sizeof(change));
	EV_SET(&change, obj->fd, EVFILT_READ, EV_ADD, 0, 0, obj);
	kevent(kq, &change, 1, NULL, 0, NULL);
#endif
	return true;
}

SOCKET SelEpolKqEvPrt::getDescriptor(const SOCKET& index, void*& obj, bool& isRead)
{
	isRead = true;
	#if defined(USE_MINGW_SELECT)
		int temp = 0;
		if(FD_ISSET(index, &readfds))
		{
			temp = index;
			obj = connections.find(temp);
			return temp;
		}
	#elif defined(USE_SELECT)
		int temp = 0;
		if(FD_ISSET(index%FD_SETSIZE, &readfds[index/FD_SETSIZE]))
		{
			temp = index;
			obj = connections.find(temp);
			return temp;
		}
	#elif defined USE_EPOLL || defined USE_WIN_IOCP
		if(index>-1 && index<(int)(sizeof events))
		{
			BaseSocket* p = (BaseSocket*)events[index].data.ptr;
			if ((events[index].events & EPOLLERR) ||
				  (events[index].events & EPOLLHUP) ||
				  (events[index].events & EPOLLRDHUP))
			{
				p->closeSocket();
			}
			else if (events[index].events & EPOLLOUT)
			{
				isRead = false;
			}
			obj = p;
			return p->fd;
		}
	#elif defined USE_KQUEUE
		if(index>-1 && index<(int)(sizeof evlist))
		{
			obj = evlist[index].udata;
			return evlist[index].ident;
		}
	#elif defined USE_DEVPOLL || defined USE_POLL
		if(polled_fds[index].fd>0) {
			if (polled_fds[index].revents & POLLIN) {
				polled_fds[index].revents = 0;
				obj = connections.find(polled_fds[index].fd);
				return polled_fds[index].fd;
			} else if((polled_fds[index].revents & POLLERR) || (polled_fds[index].revents & POLLHUP)) {
				obj = connections.find(polled_fds[index].fd);
				((BaseSocket*)obj)->closeSocket();
				int descriptor = polled_fds[index].fd;
				polled_fds[index].fd = -1;
				polled_fds[index].revents = 0;
				return descriptor;
			}
		}
	#elif defined USE_EVPORT
		if(index>-1 && index<(int)sizeof evlist)
		{
			if(evlist[index].portev_events & POLLIN) {
				obj = evlist[index].portev_user;
				return (int)evlist[index].portev_object;
			} else if((evlist[index].portev_events & POLLERR) || (evlist[index].portev_events & POLLHUP)) {
				obj = evlist[index].portev_user;
				((BaseSocket*)obj)->closeSocket();
				return (int)evlist[index].portev_object;
			}
		}
	#endif
	return -1;
}

bool SelEpolKqEvPrt::isListeningDescriptor(const SOCKET& descriptor)
{
	if(listenerMode && descriptor==sockfd)
	{
		return true;
	}
	return false;
}

bool SelEpolKqEvPrt::registerRead(BaseSocket* obj, const bool& isListeningSock, bool epoll_et, bool isNonBlocking)
{
	#ifdef USE_IO_URING
		return true;
	#endif
	SOCKET descriptor = obj->fd;
	if(!isNonBlocking) {
#ifdef OS_MINGW
		u_long iMode = 1;
		ioctlsocket(descriptor, FIONBIO, &iMode);
#else
#ifndef HAVE_ACCEPT4
		fcntl(descriptor, F_SETFL, fcntl(descriptor, F_GETFD, 0) | O_NONBLOCK);
#else
		if(isListeningSock) {
			fcntl(descriptor, F_SETFL, fcntl(descriptor, F_GETFD, 0) | O_NONBLOCK);
		} else if(!isListeningSock) {
#ifdef HAVE_SSLINC
			if(SSLHandler::getInstance()->getIsSSL()) {
				fcntl(descriptor, F_SETFL, fcntl(descriptor, F_GETFD, 0) | O_NONBLOCK);
			}
#endif
		}
#endif
		int i = 1;
		setsockopt(descriptor, IPPROTO_TCP, TCP_NODELAY, (void *)&i, sizeof(i));
		//setsockopt(descriptor, IPPROTO_TCP, TCP_CORK, (void *)&i, sizeof(i));
#endif
	}

	#ifdef USE_PICOEV
		picoev_add(picoevl, descriptor, PICOEV_READ, isListeningSock?0:obj->getTimeout(), isListeningSock?picoevAcb:picoevRwcb, obj);
	#elif defined(USE_MINGW_SELECT)
		FD_SET(descriptor, &master);
		if(descriptor > fdMax)
			fdMax = descriptor;
		connections.insert(descriptor, obj);
	#elif defined(USE_SELECT)
		FD_SET(descriptor%FD_SETSIZE, &master[descriptor/FD_SETSIZE]);
		if(descriptor > fdMax)
			fdMax = descriptor;
		connections.insert(descriptor, obj);
	#elif defined USE_EPOLL || defined USE_WIN_IOCP
		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		if(!epoll_et) {
			ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
		} else {
			#if defined(EPOLLEXCLUSIVE)
				//if(isListeningSock) {
				//	ev.events = EPOLLIN | EPOLLEXCLUSIVE;
				//} else {
					#ifdef USE_EPOLL_LT
						ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
					#else
						ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET;
					#endif
				//}
			#else
				#ifdef USE_EPOLL_LT
					ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
				#else
					ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET;
				#endif
			#endif
		}
		ev.data.ptr = obj;
		if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, descriptor, &ev) < 0)
		{
			perror("epoll");
			//std::cout << "Error adding to epoll cntl list" << std::endl;
			return false;
		}
	#elif defined USE_KQUEUE
		struct kevent change;
		memset(&change, 0, sizeof(change));
		EV_SET(&change, descriptor, EVFILT_READ, EV_ADD, 0, 0, obj);
		if (kevent(kq, &change, 1, NULL, 0, NULL) < 0)
		{
			perror("kevent");
			//std::cout << "Error adding to kqueue cntl list" << std::endl;
			return false;
		}
	#elif defined USE_DEVPOLL
		if(isListeningSock) {
			polled_fds[0].fd = descriptor;
			polled_fds[0].events = POLLIN | POLLPRI | POLLERR | POLLHUP;
			polled_fds[0].revents = 0;
			curfds = 0;
			if (write(dev_poll_fd, &polled_fds[0], sizeof(poll_fd)) < 0) {
				perror("devpoll");
				return false;
			}
		} else {
			bool found = false;
			for(int i=1;i<MAXDESCRIPTORS;i++) {
				if(polled_fds[i].fd<0) {
					polled_fds[i].fd = descriptor;
					polled_fds[i].events = POLLIN | POLLPRI | POLLERR | POLLHUP;
					polled_fds[i].revents = 0;
					found = true;
					if(i>curfds) {
						curfds = i;
					}
					if (write(dev_poll_fd, &polled_fds[i], sizeof(poll_fd)) < 0) {
						perror("devpoll");
						return false;
					}
					break;
				}
			}
			if(!found) {
				perror("too many clients");
			}
		}
		connections.insert(descriptor, obj);
	#elif defined USE_EVPORT
		if (port_associate(port, PORT_SOURCE_FD, descriptor, POLLIN | POLLERR | POLLHUP, (void*)obj) < 0) {
			perror("port_associate");
		}
	#elif defined USE_POLL
		if(isListeningSock) {
			polled_fds[0].fd = descriptor;
			polled_fds[0].events = POLLIN | POLLPRI | POLLERR | POLLHUP;
			curfds = 0;
		} else {
			bool found = false;
			for(int i=1;i<MAXDESCRIPTORS;i++) {
				if(polled_fds[i].fd<0) {
					polled_fds[i].fd = descriptor;
					polled_fds[i].events = POLLIN | POLLPRI | POLLERR | POLLHUP;
					found = true;
					if(i>curfds) {
						curfds = i;
					}
					break;
				}
			}
			if(!found) {
				perror("too many clients");
			}
		}
		connections.insert(descriptor, obj);
	#endif
	return true;
}

void* SelEpolKqEvPrt::getOptData(const int& index) {
	#if defined USE_EPOLL || defined USE_WIN_IOCP
		return events[index].data.ptr;
	#elif defined USE_KQUEUE
		return evlist[index].udata;
	#elif USE_EVPORT
		return evlist[index].portev_user;
	#endif
	return NULL;
}

bool SelEpolKqEvPrt::unRegisterRead(const SOCKET& descriptor)
{
	#ifdef USE_IO_URING
		return true;
	#endif
	if(descriptor<=0)return false;
	#if defined(USE_MINGW_SELECT)
		//connections.erase(descriptor);
		FD_CLR(descriptor, &master);
		if(fdMax==descriptor)
			fdMax--;
	#elif defined(USE_SELECT)
		//connections.erase(descriptor);
		FD_CLR(descriptor%FD_SETSIZE, &master[descriptor/FD_SETSIZE]);
		if(fdMax==descriptor)
			fdMax--;
	#elif defined USE_EPOLL || defined USE_WIN_IOCP
		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		epoll_ctl(epoll_handle, EPOLL_CTL_DEL, descriptor, &ev);
	#elif defined USE_KQUEUE
		struct kevent change;
		memset(&change, 0, sizeof(change));
		EV_SET(&change, descriptor, EVFILT_READ, EV_DELETE, 0, 0, 0);
		kevent(kq, &change, 1, NULL, 0, NULL);
	#elif defined USE_DEVPOLL
		//connections.erase(descriptor);
		for(int i=1;i<MAXDESCRIPTORS;i++) {
			if(polled_fds[i].fd==descriptor) {
				polled_fds[i].fd = -1;
				polled_fds[i].events = POLLREMOVE;
				polled_fds[i].revents = 0;
				if (write(dev_poll_fd, &polled_fds[i], sizeof(poll_fd)) < 0) {
					perror("devpoll");
					return false;
				}
				break;
			}
		}
	#elif defined USE_EVPORT
		if (port_dissociate(port, PORT_SOURCE_FD, descriptor) < 0) {
			perror("port_dissociate");
		}
	#elif defined USE_POLL
		//connections.erase(descriptor);
		//l.lock();
		for(int i=1;i<MAXDESCRIPTORS;i++) {
			if(polled_fds[i].fd==descriptor) {
				polled_fds[i].fd = -1;
				polled_fds[i].revents = 0;
				break;
			}
		}
		//l.unlock();
	#elif defined(USE_PICOEV)
		picoev_del(picoevl, descriptor);
	#endif
	return true;
}

void SelEpolKqEvPrt::reRegisterServerSock(void* obj)
{
	#ifdef USE_EVPORT
		if (port_associate(port, PORT_SOURCE_FD, sockfd, POLLIN, obj) < 0) {
			perror("port_associate");
		}
	#endif
}

#ifdef USE_PICOEV
void SelEpolKqEvPrt::picoevAcb(picoev_loop* loop, int descriptor, int events, void* cb_arg) {
	SelEpolKqEvPrt* ths = (SelEpolKqEvPrt*)loop->arg;

	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	while (true) {
		sin_size = sizeof their_addr;
#ifdef HAVE_ACCEPT4
#ifdef HAVE_SSLINC
		SOCKET newSocket = accept4(descriptor, (struct sockaddr *)&(their_addr), &sin_size,
				SSLHandler::getInstance()->getIsSSL()?0:SOCK_NONBLOCK);
#else
		SOCKET newSocket = accept4(descriptor, (struct sockaddr *)&(their_addr), &sin_size, SOCK_NONBLOCK);
#endif
#else
		SOCKET newSocket = accept(descriptor, (struct sockaddr *)&(their_addr), &sin_size);
#endif
		if(newSocket < 0)
		{
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			{
				break;
			}
		}
		BaseSocket* sifd = ths->eCb(ths, (BaseSocket*)cb_arg, ACCEPTED, newSocket, NULL, -1, false);
		picoev_add(loop, newSocket, PICOEV_READ, sifd->getTimeout(), picoevRwcb, sifd);
	}
	ths->reRegisterServerSock(cb_arg);
}

void SelEpolKqEvPrt::picoevRwcb(picoev_loop* loop, int descriptor, int events, void* cb_arg) {
	SelEpolKqEvPrt* ths = (SelEpolKqEvPrt*)loop->arg;
	BaseSocket* sock = (BaseSocket*)cb_arg;
	if ((events & PICOEV_TIMEOUT) != 0) {
		if(sock->getTimeout()>0) {
			sock->closeSocket();
			picoev_del(loop, descriptor);
		}
	} else if ((events & PICOEV_READ) != 0) {
		picoev_set_timeout(loop, descriptor, 10);
		ths->eCb(ths, sock, READ_READY, descriptor, NULL, -1, false);
	}
}
#endif

void SelEpolKqEvPrt::loop(eventLoopContinue evlc, onEvent ev, SelEpolKqEvPrt* optSel) {
	BaseSocket df;
	df.io_uring_type = PROV_BUF;

	if(evlc!=NULL) {
		this->elcCb = evlc;
	}
	if(ev!=NULL) {
		this->eCb = ev;
	}

#ifdef USE_IO_URING
	if(dsi==NULL) {
		sleep(1);
	}
#endif

	while (elcCb(this)) {
#ifdef USE_IO_URING
		struct io_uring_cqe *cqe;

		int ret = io_uring_wait_cqe(&ring, &cqe);
		if (ret < 0) {
			//fprintf(stderr, "io_uring_wait_cqe");
			continue;
		}
		BaseSocket* udata = (BaseSocket*)io_uring_cqe_get_data(cqe);
		/*if (cqe->res < 0) {
			fprintf(stderr, "Async request failed: %s for event: %d\n", strerror(-cqe->res), udata->io_uring_type);
			exit(1);
		}*/

		//io_uring_submit_and_wait(&ring, 1);
		//unsigned head;
		//unsigned count = 0;

		// go through all CQEs
		//io_uring_for_each_cqe(&ring, head, cqe) {
		//	++count;
			//BaseSocket* udata = (BaseSocket*)io_uring_cqe_get_data(cqe);

		if (cqe->res == -ENOBUFS) {
			fprintf(stdout, "bufs in automatic buffer selection empty, this should not happen...\n");
			fflush(stdout);
			return;
		} else if (udata->io_uring_type == PROV_BUF) {
			if (cqe->res < 0) {
				printf("cqe->res = %d\n", cqe->res);
				return;
			}
		} else if (udata->io_uring_type == INTERRUPT) {
			register_interrupt();
		} else if (udata->io_uring_type == ACCEPT) {
			int sock_conn_fd = cqe->res;
			// only read when there is no error, >= 0
			if (sock_conn_fd >= 0) {
				BaseSocket* sifd = ev(this, udata, ACCEPTED, sock_conn_fd, NULL, -1, false);

				struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
				io_uring_prep_recv(sqe, sock_conn_fd, sifd->buff, MAX_MESSAGE_LEN, 0);
				//io_uring_sqe_set_flags(sqe, IOSQE_BUFFER_SELECT);
				//sqe->buf_group = group_id;
				sifd->io_uring_type = READ;
				io_uring_sqe_set_data(sqe, sifd);
				//add_socket_read(&ring, sock_conn_fd, group_id, MAX_MESSAGE_LEN, IOSQE_BUFFER_SELECT);
			}

			// new connected client; read data from socket and re-add accept to monitor for new connections
			struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
			io_uring_prep_accept(sqe, udata->fd, (sockaddr*)&client_addr, &client_len, 0);
			io_uring_sqe_set_flags(sqe, 0);
			udata->io_uring_type = ACCEPT;
			io_uring_sqe_set_data(sqe, udata);
			submit_to_ring();
			//add_accept(&ring, sock_listen_fd, (struct sockaddr *)&client_addr, &client_len, 0);
		} else if (udata->io_uring_type == READ) {
			int bytes_read = cqe->res;
			if (cqe->res <= 0) {
				// connection closed or error
				shutdown(udata->fd, SHUT_RDWR);
				eCb(this, udata, CLOSED, udata->fd, NULL, -1, true);
			} else {
				//fprintf(stdout, "sock data read....\n");
				// bytes have been read into bufs, now add write to socket sqe
				//int bid = cqe->flags >> 16;
				//udata->io_uring_bid = bid;
				eCb(this, udata, ON_DATA_READ, udata->fd, udata->buff, bytes_read, false);
				//add_socket_write(&ring, conn_i.fd, bid, bytes_read, 0);
			}
		} else if (udata->io_uring_type == WRITE) {
			//fprintf(stdout, "sock data written....\n");
			// write has been completed, first re-add the buffer
			/*struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
			io_uring_prep_provide_buffers(sqe, bufs[udata->io_uring_bid], MAX_MESSAGE_LEN, 1, group_id, udata->io_uring_bid);
			io_uring_sqe_set_data(sqe, &df);*/
			//add_provide_buf(&ring, conn_i.bid, group_id);
			//pending--;
			// add a new read for the existing connection
			//struct io_uring_sqe *sqe1 = io_uring_get_sqe(&ring);
			//io_uring_prep_recv(sqe1, udata->fd, udata->buff, MAX_MESSAGE_LEN, 0);
			//io_uring_sqe_set_flags(sqe1, IOSQE_BUFFER_SELECT);
			//sqe1->buf_group = group_id;
			//udata->io_uring_type = READ;
			//io_uring_sqe_set_data(sqe1, udata);
			//submit_to_ring();
			eCb(this, udata, ON_DATA_WRITE, udata->fd, NULL, -1, false);
			//add_socket_read(&ring, conn_i.fd, group_id, MAX_MESSAGE_LEN, IOSQE_BUFFER_SELECT);
		}
		//}
		io_uring_cqe_seen(&ring, cqe);
		//io_uring_cq_advance(&ring, 1);
#else
		int num = getEvents();
#ifdef USE_PICOEV
		continue;
#endif
		if (num<=0)
		{
			if(num==-1) {
				//print errors
			}
			continue;
		}

		struct sockaddr_storage their_addr;
		socklen_t sin_size;
		for(int n=0;n<num;n++)
		{
			void* vsi = NULL;
			bool isRead = true;
			SOCKET descriptor = getDescriptor(n, vsi, isRead);
			if(descriptor!=-1)
			{
				BaseSocket* udata = (BaseSocket*)vsi;
				if(isListeningDescriptor(descriptor))
				{
					while (true) {
						sin_size = sizeof their_addr;
#ifdef HAVE_ACCEPT4
#ifdef HAVE_SSLINC
						SOCKET newSocket = accept4(descriptor, (struct sockaddr *)&(their_addr), &sin_size,
								SSLHandler::getInstance()->getIsSSL()?0:SOCK_NONBLOCK);
#else
						SOCKET newSocket = accept4(descriptor, (struct sockaddr *)&(their_addr), &sin_size, SOCK_NONBLOCK);
#endif
#else
						SOCKET newSocket = accept(descriptor, (struct sockaddr *)&(their_addr), &sin_size);
#endif
						if(newSocket < 0)
						{
							if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
							{
								break;
							}
						}
						BaseSocket* sifd = ev(this, udata, ACCEPTED, newSocket, NULL, -1, false);
						if(optSel!=NULL) {
							optSel->registerRead(sifd);
						} else {
							registerRead(sifd);
						}
					}
					reRegisterServerSock(udata);
				}
				else
				{
					if(READ_READY) {
#if defined(USE_SELECT) || defined(USE_MINGW_SELECT) || defined(USE_POLL) || defined(USE_DEVPOLL)
						unRegisterRead(descriptor);
#endif
					}
					eCb(this, udata, isRead?READ_READY:WRITE_READY, descriptor, NULL, -1, false);
				}
			}
		}
#endif
	}
}

#if defined USE_IO_URING
void SelEpolKqEvPrt::register_interrupt() {
	/*struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_recv(sqe, efdbs->fd, efdbs->buff, 1, 0);
	efdbs->io_uring_type = INTERRUPT;
	io_uring_sqe_set_data(sqe, &efd);
	submit_to_ring();*/
}
void SelEpolKqEvPrt::interrupt_wait() {
	/*if(pending>0) {
		eventfd_write(efd, 0);
	}*/
}
void SelEpolKqEvPrt::submit_to_ring() {
	l.lock();
	io_uring_submit(&ring);
	l.unlock();
}
void SelEpolKqEvPrt::post_write(BaseSocket* sifd, const std::string& data, int off) {
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_send(sqe, sifd->fd, &data[off], data.length()-off, 0);
	io_uring_sqe_set_flags(sqe, 0);
	sifd->io_uring_type = WRITE;
	io_uring_sqe_set_data(sqe, sifd);
	io_uring_submit(&ring);
}
void SelEpolKqEvPrt::post_write_2(BaseSocket* sifd, const std::string& data, const std::string& data1) {
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_send(sqe, sifd->fd, &data[0], data.length(), 0);
	io_uring_sqe_set_flags(sqe, IOSQE_IO_LINK);
	sifd->io_uring_type = WRITE;
	io_uring_sqe_set_data(sqe, sifd);
	struct io_uring_sqe *sqe1 = io_uring_get_sqe(&ring);
	io_uring_prep_send(sqe1, sifd->fd, &data1[0], data1.length(), 0);
	io_uring_sqe_set_flags(sqe1, IOSQE_IO_LINK);
	sifd->io_uring_type = WRITE;
	io_uring_sqe_set_data(sqe1, sifd);
	io_uring_submit(&ring);
}
void SelEpolKqEvPrt::post_write(BaseSocket* sifd, const char* data, int len) {
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_send(sqe, sifd->fd, data, len, 0);
	io_uring_sqe_set_flags(sqe, 0);
	sifd->io_uring_type = WRITE;
	io_uring_sqe_set_data(sqe, sifd);
	io_uring_submit(&ring);
}
void SelEpolKqEvPrt::post_read(BaseSocket* sifd) {
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_recv(sqe, sifd->fd, sifd->buff, MAX_MESSAGE_LEN, 0);
	//io_uring_sqe_set_flags(sqe, IOSQE_BUFFER_SELECT);
	//sqe->buf_group = group_id;
	sifd->io_uring_type = READ;
	io_uring_sqe_set_data(sqe, sifd);
	io_uring_submit(&ring);
}
#endif

void SelEpolKqEvPrt::lock() {
	l.lock();
}

void SelEpolKqEvPrt::unlock() {
	l.unlock();
}

void SelEpolKqEvPrt::setCtx(void *ctx) {
	this->context = ctx;
}

void* SelEpolKqEvPrt::getCtx() {
	return context;
}
