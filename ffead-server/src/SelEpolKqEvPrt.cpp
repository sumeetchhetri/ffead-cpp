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
	logger = LoggerFactory::getLogger("SelEpolKqEvPrt");
}

SelEpolKqEvPrt::~SelEpolKqEvPrt() {
}

void SelEpolKqEvPrt::initialize(int sockfd)
{
	this->sockfd = sockfd;
	curfds = 1;
	#ifdef USE_SELECT
		fdsetSize = MAXDESCRIPTORS/FD_SETSIZE;
		fdMax = sockfd;
		for (int var = 0; var < fdsetSize; ++var) {
			FD_ZERO(&readfds[var]);
			FD_ZERO(&master[var]);
		}
	#elif defined USE_EPOLL
		epoll_handle = epoll_create(MAXDESCRIPTORS);
		memset(&ev, 0, sizeof(ev));
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
	#elif defined USE_DEVPOLL
		nfds=1;
		polled_fds = (struct pollfd *)calloc(1, nfds*sizeof(struct pollfd));
		polled_fds->fd = descriptor;
		polled_fds->events = POLLIN | POLLPRI;
		return;
	#endif
	registerForEvent(sockfd);
}


int SelEpolKqEvPrt::getEvents()
{
	int numEvents = -1;
	#ifdef USE_SELECT
		for (int var = 0; var < fdsetSize; ++var) {
			readfds[var] = master[var];
		}
		numEvents = select(fdMax+1, readfds, NULL, NULL, NULL);
		if(numEvents==-1)
		{
			perror("select()");
		}
		else
		{
			return fdMax+1;
		}
	#elif defined USE_EPOLL
		numEvents = epoll_wait(epoll_handle, events, curfds,-1);
	#elif defined USE_KQUEUE
		numEvents = kevent(kq, NULL, 0, evlist, MAXDESCRIPTORS, NULL);
	#elif defined USE_DEVPOLL
		struct dvpoll pollit;
		pollit.dp_timeout = -1;
		pollit.dp_nfds = curfds;
		pollit.dp_fds = polled_fds;
		numEvents = ioctl(dev_poll_fd, DP_POLL, &pollit);
	#elif defined USE_EVPORT
		uint_t nevents, wevents = 0;
		//uint_t num = 0;
		if (port_getn(port, evlist, 0, &wevents, NULL) < 0) return 0;
		if (0 == wevents) wevents = 1;
		nevents = wevents;
		if (port_getn(port, evlist, (uint_t) MAXDESCRIPTORS, &nevents, NULL) < 0) return 0;
		numEvents = (int)nevents;
	#elif defined USE_POLL
		numEvents = poll(polled_fds,nfds,-1);
		if (numEvents == -1){
			perror ("poll");
			exit(0);
		}
	#endif
	return numEvents;
}

int SelEpolKqEvPrt::getDescriptor(int index)
{
	#ifdef USE_SELECT
		if(FD_ISSET(index%FD_SETSIZE, &readfds[index/FD_SETSIZE]))
		{
			return index;
		}
	#elif defined USE_EPOLL
		if(index>-1 && index<sizeof events)
		{
			return events[index].data.fd;
		}
	#elif defined USE_KQUEUE
		if(index>-1 && index<(int)(sizeof evlist))
		{
			return evlist[index].ident;
		}
	#elif defined USE_DEVPOLL
		return polled_fds[index].fd;
	#elif defined USE_EVPORT
		if(index>-1 && index<sizeof evlist)
		{
			return (int)evlist[index].portev_object;
		}
	#elif defined USE_POLL
		return polled_fds[index].fd;
	#endif
	return -1;
}

bool SelEpolKqEvPrt::isListeningDescriptor(int descriptor)
{
	if(descriptor==sockfd)
	{
		return true;
	}
	return false;
}

bool SelEpolKqEvPrt::registerForEvent(int descriptor)
{
	curfds++;
	fcntl(descriptor, F_SETFL, fcntl(descriptor, F_GETFD, 0) | O_NONBLOCK);
	#ifdef USE_SELECT
		FD_SET(descriptor%FD_SETSIZE, &master[descriptor/FD_SETSIZE]);
		if(descriptor > fdMax)
			fdMax = descriptor;
	#elif defined USE_EPOLL
		ev.events = EPOLLIN | EPOLLPRI;
		ev.data.fd = descriptor;
		if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, descriptor, &ev) < 0)
		{
			perror("epoll");
			logger << "Error adding to epoll cntl list" << endl;
			return false;
		}
	#elif defined USE_KQUEUE
		memset(&change, 0, sizeof(change));
		EV_SET(&change, descriptor, EVFILT_READ, EV_ADD, 0, 0, 0);
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
	#elif defined USE_EVPORT
		if (port_associate(port, PORT_SOURCE_FD, descriptor, POLLIN, NULL) < 0) {
			perror("port_associate");
		}
	#elif defined USE_POLL
		nfds++;
		polled_fds = (struct pollfd *)realloc(polled_fds, (nfds+1)*sizeof(struct pollfd));
		(polled_fds+nfds)->fd = descriptor;
		(polled_fds+nfds)->events = POLLIN | POLLPRI;
	#endif
	return true;
}

bool SelEpolKqEvPrt::unRegisterForEvent(int descriptor)
{
	if(descriptor<=0)return false;
	curfds--;
	#ifdef USE_SELECT
		FD_CLR(descriptor%FD_SETSIZE, &master[descriptor/FD_SETSIZE]);
	#elif defined USE_EPOLL
		epoll_ctl(epoll_handle, EPOLL_CTL_DEL, descriptor, &ev);
	#elif defined USE_KQUEUE
		memset(&change, 0, sizeof(change));
		EV_SET(&change, descriptor, EVFILT_READ, EV_DELETE, 0, 0, 0);
		kevent(kq, &change, 1, NULL, 0, NULL);
	#elif defined USE_DEVPOLL
		struct pollfd poll_fd;
		poll_fd.fd = descriptor;
		poll_fd.events = POLLREMOVE;
		poll_fd.revents = 0;
		if (write(dev_poll_fd, &poll_fd, sizeof(poll_fd)) < 0) {
			perror("devpoll");
			return false;
		}
	#elif defined USE_EVPORT
		/*if (port_dissociate(port, PORT_SOURCE_FD, descriptor) < 0) {
			perror("port_dissociate");
		}*/
	#elif defined USE_POLL
		nfds--;
		memcpy(polled_fds+descriptor,polled_fds+descriptor+1,nfds-descriptor);
		polled_fds = (struct pollfd *)realloc(polled_fds, nfds*sizeof(struct pollfd));
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
