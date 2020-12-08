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
 * Server.cpp
 *
 *  Created on: Jan 2, 2010
 *      Author: sumeet
 */

#include "Server.h"

#if !defined(OS_MINGW)
void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}
#endif

Server::Server()
{
	runn = false;
	started = false;
	sock = -1;
	mode = 3;
	service = NULL;
	//logger = LoggerFactory::getLogger("Server");
}

Server::Server(const std::string& port, const bool& block, const int& waiting, const Service& serv, int mode)
{
	started = false;
	runn = true;
	//logger = LoggerFactory::getLogger("Server");
	service = serv;
	struct addrinfo hints, *servinfo, *p;
	
	int yes = 1;
	int rv;

	if(mode<1 || mode >3)
		mode = 3;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(0);
	}
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		#ifdef OS_MINGW
		if ((this->sock = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == INVALID_SOCKET)
		#else
		if ((this->sock = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
		#endif
		{
			perror("server: socket");
			continue;
		}
		#ifdef OS_MINGW
			BOOL bOptVal = FALSE;
			if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&bOptVal, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#else
			if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#ifdef HAVE_TCP_QUICKACK
			if (setsockopt(this->sock, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#endif
		#ifdef HAVE_TCP_DEFER_ACCEPT
			int option = 10;
			if (setsockopt(this->sock, IPPROTO_TCP, TCP_DEFER_ACCEPT, &option, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#endif
		#ifdef HAVE_TCP_FASTOPEN
			int option1 = 4096;
			if (setsockopt(this->sock, IPPROTO_TCP, TCP_FASTOPEN, &option1, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#endif
		#endif
		
		#ifdef OS_MINGW
		if (::bind(this->sock, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR) {
		#else
		if (::bind(this->sock, p->ai_addr, p->ai_addrlen) == -1) {
		#endif
			close(this->sock);
			perror("server: bind");
			continue;
		}
		break;
	}
	//int blockMode = 1;
	//ioctl(this->sock, FIONBIO, &blockMode);
	//fcntl(this->sock, F_SETFL, O_NONBLOCK);
	//fcntl (this->sock, F_SETFL, fcntl(this->sock, F_GETFD, 0) | O_NONBLOCK);
	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(0);
	}
	freeaddrinfo(servinfo); // all done with this structure
	if (listen(this->sock, BACKLOGM) == -1)
	{
		perror("listen");
		exit(1);
	}
	#if !defined(OS_MINGW)
	struct sigaction sa;
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}
	#endif
	//logger << ("waiting for connections on " + port + ".....") << std::endl;
	this->mode = mode;
}

Server::~Server() {
	
}

void* Server::servicing(void* arg)
{
	Server* server  = static_cast<Server*>(arg);
	//Service serv = server->service;
	server->lock.lock();
	bool flag = server->runn;
	server->lock.unlock();
	while(flag)
	{
		int new_fd = server->Accept();
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}
		else
		{
			//Thread* pthread = new Thread(serv, &new_fd);
			//pthread->execute();
		}
	}
	return NULL;
}

SOCKET Server::Accept()
{
	socklen_t sin_size = sizeof their_addr;
	SOCKET new_fd = accept(this->sock, (struct sockaddr *)&(this->their_addr), &sin_size);
	return new_fd;
}
int Server::Send(const SOCKET& fd, const std::string& data)
{
	int bytes = send(fd,data.c_str(),data.length(),0);
	if(bytes == -1)
	{
		//logger << "send failed" << std::endl;
	}
	return bytes;
}
int Server::Send(const SOCKET& fd, const std::vector<char>& data)
{
	int bytes = send(fd,&data[0],data.size(),0);
	if(bytes == -1)
	{
		//logger << "send failed" << std::endl;
	}
	return bytes;
}
int Server::Send(const SOCKET& fd, const std::vector<unsigned char>& data)
{
	int bytes = send(fd,(const char*)&data[0],data.size(),0);
	if(bytes == -1)
	{
		//logger << "send failed" << std::endl;
	}
	return bytes;
}
int Server::Send(const SOCKET& fd, char *data)
{
	int bytes = send(fd,data,sizeof data,0);
	if(bytes == -1)
	{
		//logger << "send failed" << std::endl;
	}
	return bytes;
}
int Server::Send(const SOCKET& fd, unsigned char *data)
{
	int bytes = send(fd,(const char*)data,sizeof data,0);
	if(bytes == -1)
	{
		//logger << "send failed" << std::endl;
	}
	return bytes;
}

int Server::Receive(const SOCKET& fd, std::string &data, const int& bytes)
{
	if(bytes==0)
		return -1;
	char buf[bytes];
	memset(buf, 0, sizeof(buf));
	int bytesr = recv(fd, buf, bytes, 0);
	/*string temp;
	std::stringstream ss;
	ss << buf;
	while(getline(ss,temp,'\r'))
	{
		data.append(temp);
	}*/
	std::string temp(buf,buf+bytesr);
	data = temp;
	memset(&buf[0], 0, sizeof(buf));
	return bytesr;
}
int Server::Receive(const SOCKET& fd, std::vector<char>& data, const int& bytes)
{
	if(bytes==0)
		return -1;
	char te[bytes];
	memset(te, 0, sizeof(te));
	int bytesr = Receive(fd,te,bytes);
	data.resize(bytes);
	copy(te,te+bytes,data.begin());
	return bytesr;
}
int Server::Receive(const SOCKET& fd, std::vector<unsigned char>& data, const int& bytes)
{
	if(bytes==0)
		return -1;
	char te[bytes];
	memset(te, 0, sizeof(te));
	int bytesr = Receive(fd,te,bytes);
	data.resize(bytes);
	copy(te,te+bytes,data.begin());
	return bytesr;
}
int Server::Receive(const SOCKET& fd, const char* data, const int& bytes)
{
	if(bytes==0)
		return -1;
	int bytesr = recv(fd, (char*)data, bytes, 0);
	return bytesr;
}
int Server::Receive(const SOCKET& fd, const unsigned char* data, const int& bytes)
{
	if(bytes==0)
		return -1;
	int bytesr = recv(fd, (char*)data, bytes, 0);
	return bytesr;
}
int Server::Receive(const SOCKET& fd, std::vector<std::string>& data, const int& bytes)
{
	if(bytes==0)
		return -1;
	char te[bytes];
	memset(te, 0, sizeof(te));
	std::string temp;
	int bytesr = Receive(fd,te,bytes);
	std::stringstream ss;
	ss << te;
	while(getline(ss,temp,'\n'))
	{
		data.push_back(temp);
		//logger << temp << std::endl;
	}
	memset(&te[0], 0, sizeof(te));
	return bytesr;
}

SOCKET Server::createListener(const int& port, const bool& block, bool isSinglEVH)
{
	SOCKET    sockfd = INVALID_SOCKET;
	int yes = 1, option = 10;
	//int rv;
	//bool bound = false;

	struct sockaddr_in self;
	memset(&self, 0, sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port = htons(port);
	self.sin_addr.s_addr = INADDR_ANY;

	/*---Create streaming socket---*/
	#ifdef OS_MINGW
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	#else
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	#endif
	{
		perror("server: socket");
		return INVALID_SOCKET;
	}

	#ifdef OS_MINGW
		BOOL bOptVal = FALSE;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&bOptVal, sizeof(int)) == -1) {
			perror("setsockopt");
		}
	#else
		#ifdef CYGWIN
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		#else
		if (setsockopt(sockfd, SOL_SOCKET, (isSinglEVH?SO_REUSEADDR | SO_REUSEPORT:SO_REUSEADDR), &yes, sizeof(int)) == -1) {
		#endif
			perror("setsockopt");
		}
	#ifdef HAVE_TCP_QUICKACK
		if (setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
		}
	#endif
	#ifdef HAVE_TCP_DEFER_ACCEPT
		if (setsockopt(sockfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &option, sizeof(int)) == -1) {
			perror("setsockopt");
		}
	#endif
	#ifdef HAVE_TCP_FASTOPEN
		option = 4096;
		if (setsockopt(sockfd, IPPROTO_TCP, TCP_FASTOPEN, &option, sizeof(int)) == -1) {
			perror("setsockopt");
		}
	#endif
	#endif

	#ifdef OS_MINGW
	if (::bind(sockfd, (struct sockaddr*)&self, sizeof(self)) == SOCKET_ERROR) {
	#else
	if (::bind(sockfd, (struct sockaddr*)&self, sizeof(self)) == -1) {
	#endif
		close(sockfd);
		perror("server: bind");
		return INVALID_SOCKET;
	}

	#ifdef OS_MINGW
	if (listen(sockfd, BACKLOGM) == SOCKET_ERROR)
	#else
	if (listen(sockfd, BACKLOGM) == -1)
	#endif
	{
		perror("listen");
		return INVALID_SOCKET;
	}
	#if !defined(OS_MINGW)
	/*struct sigaction sa;
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}*/
	#endif

	if(!block)
	{
		#ifdef OS_MINGW
			u_long iMode = 1;
			ioctlsocket(sockfd, FIONBIO, &iMode);
		#else
			fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
		#endif
	}
	return sockfd;
}

SOCKET Server::createListener(const std::string& ipAddress, const int& port, const bool& block, bool isSinglEVH)
{
	SOCKET    sockfd;
	struct addrinfo hints, *servinfo, *p;
	int yes = 1, option = 10;
	int rv;
	bool bound = false;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	const char *ip_addr = NULL;
	if(ipAddress!="")
		ip_addr = ipAddress.c_str();
	else
		return createListener(port, block, isSinglEVH);

	std::string ports = CastUtil::fromNumber(port);
	if ((rv = getaddrinfo(ip_addr, ports.c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		#ifdef OS_MINGW
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET)
		#else
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		#endif
		{
			perror("server: socket");
			continue;
		}

		#ifdef OS_MINGW
			BOOL bOptVal = FALSE;
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&bOptVal, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#else
		#ifdef CYGWIN
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			#else
			if (setsockopt(sockfd, SOL_SOCKET, (isSinglEVH?SO_REUSEADDR | SO_REUSEPORT:SO_REUSEADDR), &yes, sizeof(int)) == -1) {
			#endif
				perror("setsockopt");
			}
		#ifdef HAVE_TCP_QUICKACK
			if (setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#endif
		#ifdef HAVE_TCP_DEFER_ACCEPT
			if (setsockopt(sockfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &option, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#endif
		#ifdef HAVE_TCP_FASTOPEN
			option = 4096;
			if (setsockopt(sockfd, IPPROTO_TCP, TCP_FASTOPEN, &option, sizeof(int)) == -1) {
				perror("setsockopt");
			}
		#endif
		#endif
		
		#ifdef OS_MINGW
		if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR) {
		#else
		if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		#endif
			close(sockfd);
			perror("server: bind");
			continue;
		} else {
			bound = true;
			break;
		}
	}
	if (!bound)
	{
		fprintf(stderr, "server: failed to bind\n");
		return -1;
	}
	freeaddrinfo(servinfo); // all done with this structure
	#ifdef OS_MINGW
	if (listen(sockfd, BACKLOGM) == SOCKET_ERROR)
	#else
	if (listen(sockfd, BACKLOGM) == -1)
	#endif
	{
		perror("listen");
		return -1;
	}
	#if !defined(OS_MINGW)
	struct sigaction sa;
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}
	#endif

	if(!block)
	{
		#ifdef OS_MINGW
			u_long iMode = 1;
			ioctlsocket(sockfd, FIONBIO, &iMode);
		#else
			fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
		#endif
	}
	return sockfd;
}

void Server::start()
{
	lock.lock();
	if(!started && runn)
	{
		started = true;
		if(mode==1)
		{
			#ifndef OS_MINGW
			if(fork()==0)
			{
				servicing(this);
			}
			#endif
		}
		else if(mode==2)
		{
			//Thread* pthread = new Thread(&servicing, this);
			//pthread->execute();
		}
		else if(mode==3)
		{
			servicing(this);
		}
	}
	lock.unlock();
}

void Server::stop()
{
	lock.lock();
	runn = false;
	lock.unlock();
}

SOCKET Server::getSocket()
{
	return sock;
}
