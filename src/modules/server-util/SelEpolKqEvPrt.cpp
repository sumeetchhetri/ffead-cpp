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
#if defined USE_EPOLL
	epoll_handle = -1;
#elif defined USE_WIN_IOCP
	epoll_handle = NULL;
#endif
#if USE_KQUEUE == 1
	kq = -1;
#endif
	dsi = NULL;
}

SelEpolKqEvPrt::~SelEpolKqEvPrt() {
	if(dsi!=NULL) {
		delete dsi;
	}
}

void SelEpolKqEvPrt::initialize(const int& timeout)
{
	this->timeoutMilis = timeout;
	curfds = 1;
	#if defined(USE_MINGW_SELECT)
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
	#elif defined USE_EVPORT
		if ((port = port_create()) < 0) {
			perror("port_create");
		}
	#elif defined USE_POLL
		nfds=1;
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
	#elif defined USE_POLL
		polled_fds = (struct pollfd *)calloc(1, nfds*sizeof(struct pollfd));
		polled_fds->fd = sockfd;
		polled_fds->events = POLLIN | POLLPRI;
		return;
	#endif
	dsi = new DummySocketInterface();
	dsi->fd = sockfd;
	if(sockfd>0)registerRead(dsi, true);
}

void SelEpolKqEvPrt::initialize(SOCKET sockfd, const int& timeout)
{
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
	#if defined(USE_MINGW_SELECT)
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
	#elif defined USE_EVPORT
		if ((port = port_create()) < 0) {
			perror("port_create");
		}
	#elif defined USE_POLL
		nfds=1;
		polled_fds = (struct pollfd *)calloc(1, nfds*sizeof(struct pollfd));
		polled_fds->fd = sockfd;
		polled_fds->events = POLLIN | POLLPRI;
		return;
	#endif
	dsi = new DummySocketInterface();
	dsi->fd = sockfd;
	if(sockfd>0)registerRead(dsi, true);
}


int SelEpolKqEvPrt::getEvents()
{
	int numEvents = -1;
	#if defined(USE_MINGW_SELECT)
	    l.lock();
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
		l.unlock();
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
		l.lock();
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
		l.unlock();
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
		pollit.dp_timeout = timeoutMilis;
		pollit.dp_nfds = curfds;
		pollit.dp_fds = polled_fds;
		numEvents = ioctl(dev_poll_fd, DP_POLL, &pollit);
	#elif defined USE_EVPORT
		uint_t nevents, wevents = 0;
		if(timeoutMilis>1)
		{
			struct timespec tv
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
		l.lock();
		if(timeoutMilis>1)
		{
			struct timespec tv;
			tv.tv_sec = (timeoutMilis/1000);
			tv.tv_nsec = (timeoutMilis%1000)*1000000;
			numEvents = poll(polled_fds, nfds, timeoutMilis);
		}
		else
		{
			numEvents = poll(polled_fds, nfds, NULL);
		}
		l.unlock();
		if (numEvents == -1){
			perror ("poll");
			exit(0);
		}
	#endif
	return numEvents;
}

bool SelEpolKqEvPrt::registerWrite(SocketInterface* obj) {
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

bool SelEpolKqEvPrt::unRegisterWrite(SocketInterface* obj) {
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
		l.lock();
		if(FD_ISSET(index, &readfds))
		{
			temp = index;
		}
		l.unlock();
		obj = connections.find(temp);
		return temp;
	#elif defined(USE_SELECT)
		int temp = 0;
		l.lock();
		if(FD_ISSET(index%FD_SETSIZE, &readfds[index/FD_SETSIZE]))
		{
			temp = index;
		}
		l.unlock();
		obj = connections.find(temp);
		return temp;
	#elif defined USE_EPOLL || defined USE_WIN_IOCP
		if(index>-1 && index<(int)(sizeof events))
		{
			SocketInterface* p = (SocketInterface*)events[index].data.ptr;
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
	#elif defined USE_DEVPOLL
		obj = connections.find(polled_fds[index].fd);
		return polled_fds[index].fd;
	#elif defined USE_EVPORT
		if(index>-1 && index<sizeof evlist)
		{
			obj = connections.find((int)evlist[index].portev_object);
			return (int)evlist[index].portev_object;
		}
	#elif defined USE_POLL
		l.lock();
		int temp = polled_fds[index].fd;
		l.unlock();
		obj = connections.find(temp);
		return temp;
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

bool SelEpolKqEvPrt::registerRead(SocketInterface* obj, const bool& isListeningSock)
{
	SOCKET descriptor = obj->fd;
	#ifdef OS_MINGW
		u_long iMode = 1;
		ioctlsocket(descriptor, FIONBIO, &iMode);
	#else
#ifndef HAVE_ACCEPT4
		fcntl(descriptor, F_SETFL, fcntl(descriptor, F_GETFD, 0) | O_NONBLOCK);
#else
		if(isListeningSock) {
			fcntl(descriptor, F_SETFL, fcntl(descriptor, F_GETFD, 0) | O_NONBLOCK);
		}
#endif
		int i = 1;
		setsockopt(descriptor, IPPROTO_TCP, TCP_NODELAY, (void *)&i, sizeof(i));
		//setsockopt(descriptor, IPPROTO_TCP, TCP_CORK, (void *)&i, sizeof(i));
	#endif

	#if defined(USE_MINGW_SELECT)
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
		kevent(kq, &change, 1, NULL, 0, NULL);
	#elif defined USE_DEVPOLL
		struct pollfd poll_fd;
		poll_fd.fd = descriptor;
		poll_fd.events = POLLIN;
		poll_fd.revents = 0;
		if (write(dev_poll_fd, &poll_fd, sizeof(poll_fd)) < 0) {
			perror("devpoll");
			return false;
		}
		connections.insert(descriptor, obj);
	#elif defined USE_EVPORT
		if (port_associate(port, PORT_SOURCE_FD, descriptor, POLLIN, NULL) < 0) {
			perror("port_associate");
		}
		connections.insert(descriptor, obj);
	#elif defined USE_POLL
		l.lock();
		curfds++;
		nfds++;
		polled_fds = (struct pollfd *)realloc(polled_fds, (nfds+1)*sizeof(struct pollfd));
		(polled_fds+nfds)->fd = descriptor;
		(polled_fds+nfds)->events = POLLIN | POLLPRI;
		l.unlock();
		connections.insert(descriptor, obj);
	#endif
	return true;
}

void* SelEpolKqEvPrt::getOptData(const int& index) {
	#if defined USE_EPOLL || defined USE_WIN_IOCP
		return events[index].data.ptr;
	#elif defined USE_KQUEUE
		return evlist[index].udata;
	#endif
	return NULL;
}

bool SelEpolKqEvPrt::unRegisterRead(const SOCKET& descriptor)
{
	if(descriptor<=0)return false;
	#if defined(USE_MINGW_SELECT)
		connections.erase(descriptor);
		FD_CLR(descriptor, &master);
		if(fdMax==descriptor)
			fdMax--;
	#elif defined(USE_SELECT)
		connections.erase(descriptor);
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
		connections.erase(descriptor);
		struct pollfd poll_fd;
		poll_fd.fd = descriptor;
		poll_fd.events = POLLREMOVE;
		poll_fd.revents = 0;
		if (write(dev_poll_fd, &poll_fd, sizeof(poll_fd)) < 0) {
			perror("devpoll");
			return false;
		}
	#elif defined USE_EVPORT
		connections.erase(descriptor);
		/*if (port_dissociate(port, PORT_SOURCE_FD, descriptor) < 0) {
			perror("port_dissociate");
		}*/
	#elif defined USE_POLL
		connections.erase(descriptor);
		l.lock();
		curfds--;
		nfds--;
		memcpy(polled_fds+descriptor,polled_fds+descriptor+1,nfds-descriptor);
		polled_fds = (struct pollfd *)realloc(polled_fds, nfds*sizeof(struct pollfd));
		l.unlock();
	#endif
	return true;
}

void SelEpolKqEvPrt::reRegisterServerSock()
{
	#ifdef USE_EVPORT
		if (port_associate(port, PORT_SOURCE_FD, sockfd, POLLIN, NULL) < 0) {
			perror("port_associate");
		}
	#endif
}

void SelEpolKqEvPrt::lock() {
	l.lock();
}

void SelEpolKqEvPrt::unlock() {
	l.unlock();
}
