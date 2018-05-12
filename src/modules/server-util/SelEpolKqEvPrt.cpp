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
}

SelEpolKqEvPrt::~SelEpolKqEvPrt() {
}

void SelEpolKqEvPrt::initialize(const int& timeout)
{
	initialize(-1, timeout);
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
	#if defined(USE_WIN_IOCP)
		initIOCP();
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
	#elif defined USE_EPOLL
		epoll_handle = epoll_create(MAXDESCRIPTORS);
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
		polled_fds->fd = descriptor;
		polled_fds->events = POLLIN | POLLPRI;
		return;
	#endif
	#if !defined(USE_WIN_IOCP)
		if(sockfd>0)registerForEvent(sockfd);
	#endif
}


int SelEpolKqEvPrt::getEvents()
{
	int numEvents = -1;
	#if defined(USE_WIN_IOCP)
		#ifdef OS_MINGW_W64
			IOOverlappedEntry entries[64];
			ULONG nEvents = 0;
			if(!GetQueuedCompletionStatusEx(iocpPort,
						(OVERLAPPED_ENTRY*)entries,
						64,
	                    &nEvents,
						(DWORD)this->timeoutMilis,
						FALSE)) {
				int errCd = WSAGetLastError();
				if(errCd != WAIT_TIMEOUT)
				{
					std::cout << "Error occurred during GetQueuedCompletionStatusEx " << WSAGetLastError() << std::endl;
				}
	           	return -1;
	        }
			psocks.clear();
			for(long i = 0; i < (long)nEvents; i++) {
				DWORD qty;
				DWORD flags;
				if(WSAGetOverlappedResult(entries[i].o->sock, (LPWSAOVERLAPPED)entries[i].o, &qty, FALSE, &flags))
				{
					psocks.push_back(entries[i].o);
				}
			}
			return (int)psocks.size();
		#else
			OVERLAPPED       *pOverlapped = NULL;
			IOOperation *lpContext = NULL;
			DWORD            dwBytesTransfered = 0;
			BOOL bReturn = GetQueuedCompletionStatus(iocpPort,
								&dwBytesTransfered,
								(LPDWORD)&lpContext,
								&pOverlapped,
								(DWORD)this->timeoutMilis);
			if (FALSE == bReturn)
			{
				return -1;
			}
			IOOperation* iops = (IOOperation*)lpContext;
			psocks.clear();
			psocks.push_back(iops);
			return 1;
		#endif
	#elif defined(USE_MINGW_SELECT)
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
	#elif defined USE_EPOLL
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
			numEvents = poll(polled_fds, nfds, &tv);
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

SOCKET SelEpolKqEvPrt::getDescriptor(const SOCKET& index)
{
	#if defined(USE_WIN_IOCP)
		if(psocks.size()>index && index>=0)
		{
			#ifdef OS_MINGW_W64
				l.lock();
				SingleIOOperation* iops = (SingleIOOperation*)psocks.at(index);
				l.unlock();
				iocpRecv(iops->sock, &(iops->o));
				return iops->sock;
			#else
				l.lock();
				IOOperation* iops = (IOOperation*)psocks.at(index);
				l.unlock();
				iocpRecv(iops->sock, iops->o);
				return iops->sock;
			#endif
		}
	#elif defined(USE_MINGW_SELECT)
		int temp = 0;
		l.lock();
		if(FD_ISSET(index, &readfds))
		{
			temp = index;
		}
		l.unlock();
		return temp;
	#elif defined(USE_SELECT)
		int temp = 0;
		l.lock();
		if(FD_ISSET(index%FD_SETSIZE, &readfds[index/FD_SETSIZE]))
		{
			temp = index;
		}
		l.unlock();
		return temp;
	#elif defined USE_EPOLL
		if(index>-1 && index<(int)(sizeof events))
		{
			if ((events[index].events & EPOLLERR) ||
				  (events[index].events & EPOLLHUP) ||
				  (events[index].events & EPOLLRDHUP) ||
				  (!(events[index].events & EPOLLIN)))
			{
				close(events[index].data.fd);
			}
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
		l.lock();
		int temp = polled_fds[index].fd;
		l.unlock();
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

bool SelEpolKqEvPrt::registerForEvent(const SOCKET& descriptor)
{
	//#ifndef USE_WIN_IOCP
		#ifdef OS_MINGW
			u_long iMode = 1;
			ioctlsocket(descriptor, FIONBIO, &iMode);
		#else
			fcntl(descriptor, F_SETFL, fcntl(descriptor, F_GETFD, 0) | O_NONBLOCK);
		#endif
	//#endif

	#if defined(USE_WIN_IOCP)
		#ifdef OS_MINGW_W64
			SingleIOOperation* iops = new SingleIOOperation;
			iops->sock = descriptor;
			memset(&(iops->o), 0, sizeof(OVERLAPPED));
		#else
			IOOperation *iops  = new IOOperation;
			iops->sock = descriptor;
			iops->o = new OVERLAPPED;
			memset(iops->o, 0, sizeof(OVERLAPPED));
		#endif
		if (!addToIOCP(iops)) {
			delete iops;
			return false;
		}
		cntxtMap[descriptor] = iops;
	#elif defined(USE_MINGW_SELECT)
		FD_SET(descriptor, &master);
		if(descriptor > fdMax)
			fdMax = descriptor;
	#elif defined(USE_SELECT)
		FD_SET(descriptor%FD_SETSIZE, &master[descriptor/FD_SETSIZE]);
		if(descriptor > fdMax)
			fdMax = descriptor;
	#elif defined USE_EPOLL
		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		#ifdef USE_EPOLL_LT
			ev.events = EPOLLIN | EPOLLPRI;
		#else
			ev.events = EPOLLIN | EPOLLET;
		#endif
		ev.data.fd = descriptor;
		if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, descriptor, &ev) < 0)
		{
			perror("epoll");
			std::cout << "Error adding to epoll cntl list" << std::endl;
			return false;
		}
	#elif defined USE_KQUEUE
		struct kevent change;
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
		l.lock();
		curfds++;
		nfds++;
		polled_fds = (struct pollfd *)realloc(polled_fds, (nfds+1)*sizeof(struct pollfd));
		(polled_fds+nfds)->fd = descriptor;
		(polled_fds+nfds)->events = POLLIN | POLLPRI;
		l.unlock();
	#endif
	return true;
}

bool SelEpolKqEvPrt::unRegisterForEvent(const SOCKET& descriptor)
{
	if(descriptor<=0)return false;
	#if defined(USE_WIN_IOCP)
		if(cntxtMap.find(descriptor)!=cntxtMap.end()) {
			void* t = cntxtMap[descriptor];
			delete t;
			cntxtMap.erase(descriptor);
			return true;
		}
		return false;
	#elif defined(USE_MINGW_SELECT)
		FD_CLR(descriptor, &master);
		if(fdMax==descriptor)
			fdMax--;
	#elif defined(USE_SELECT)
		FD_CLR(descriptor%FD_SETSIZE, &master[descriptor/FD_SETSIZE]);
		if(fdMax==descriptor)
			fdMax--;
	#elif defined USE_EPOLL
		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		epoll_ctl(epoll_handle, EPOLL_CTL_DEL, descriptor, &ev);
	#elif defined USE_KQUEUE
		struct kevent change;
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

#ifdef USE_WIN_IOCP
//Function to Initialize IOCP
bool SelEpolKqEvPrt::initIOCP()
{
	// Initialize Winsock
	WSADATA wsaData;

	int nResult;
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (NO_ERROR != nResult)
	{
		printf("\nError occurred while executing WSAStartup() %d.\n", WSAGetLastError());
		return false; //error
	}
	else
	{
		printf("\nWSAStartup() successful.\n");
	}
	//Create I/O completion port
	iocpPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );

	if ( NULL == iocpPort)
	{
		printf("\nError occurred while creating IOCP: %d.\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool SelEpolKqEvPrt::addToIOCP(void *p)
{
	//Associate the socket with IOCP
	HANDLE hTemp = NULL;
	SOCKET sock;
	LPWSAOVERLAPPED o;
	#ifdef OS_MINGW_W64
		SingleIOOperation* iops = (SingleIOOperation*)p;
		sock = iops->sock;
		o = &(iops->o);
		hTemp = CreateIoCompletionPort((HANDLE)sock, iocpPort, 0, 0);
	#else
		IOOperation* iops = (IOOperation*)p;
		sock = iops->sock;
		o = iops->o;
		hTemp = CreateIoCompletionPort((HANDLE)sock, iocpPort, (ULONG_PTR)iops, 0);
	#endif
	if (NULL == hTemp)
	{
		printf("\nError occurred while executing CreateIoCompletionPort().\n");
		delete iops;
		return false;
	}
	else
	{
		iocpRecv(sock, o);
	}
	return true;
}

void SelEpolKqEvPrt::iocpRecv(const SOCKET& sock, const LPWSAOVERLAPPED& o) {
	//Get data.
	DWORD dwFlags = MSG_PEEK;
	DWORD dwBytes = 0;

	char buf[2];
	WSABUF dat;
	dat.buf = buf;
	dat.len = 2;
	WSARecv(sock, &dat, 1, &dwBytes, &dwFlags, o, NULL);
}
#endif


void SelEpolKqEvPrt::lock() {
	l.lock();
}

void SelEpolKqEvPrt::unlock() {
	l.unlock();
}


