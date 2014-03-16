/*
	Copyright 2009-2012, Sumeet Chhetri
  
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
 * NBServer.cpp
 *
 *  Created on: Jan 2, 2010
 *      Author: sumeet
 */

#include "NBServer.h"


void sigchld_handlernb(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

NBServer::NBServer()
{
	runn = false;
	started = false;
}

NBServer::NBServer(string port,int waiting,Service serv)
{
	started = false;
	runn = true;

	logger = LoggerFactory::getLogger("NBServer");

	service = serv;
	struct addrinfo hints, *servinfo, *p;
	struct sigaction sa;
	int yes=1;
	int rv;

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
		if ((this->sock = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}
		if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(this->sock, p->ai_addr, p->ai_addrlen) == -1) {
			close(this->sock);
			perror("server: bind");
			continue;
		}
		break;
	}

	fcntl (this->sock, F_SETFL, fcntl(this->sock, F_GETFD, 0) | O_NONBLOCK);

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
	sa.sa_handler = sigchld_handlernb; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}
	logger << ("waiting for connections on " + port + ".....") << endl;
}

NBServer::~NBServer() {
	// TODO Auto-generated destructor stub
}

void* NBServer::servicing(void* arg)
{
	NBServer* server = (NBServer*)arg;
	Service serv = server->service;
	server->lock.lock();
	bool flag = server->runn;
	server->lock.unlock();

	server->selEpolKqEvPrtHandler.initialize(server->sock);

	int new_fd, nfds;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

	while(flag)
	{
		nfds = server->selEpolKqEvPrtHandler.getEvents();
		if (nfds == -1)
		{
			perror("event handler main process");
			if(errno==EBADF)
				server->logger << "\nInavlid fd" <<flush;
			else if(errno==EFAULT)
				server->logger << "\nThe memory area pointed to by events is not accessible" <<flush;
			else if(errno==EINTR)
				server->logger << "\ncall was interrupted by a signal handler before any of the requested events occurred" <<flush;
			else if(errno==EINVAL)
				server->logger << "not a poll file descriptor, or maxevents is less than or equal to zero" << endl;
			else
				server->logger << "\nnot an epoll file descriptor" <<flush;
		}
		for(int n=0;n<nfds;n++)
		{
			int descriptor = server->selEpolKqEvPrtHandler.getDescriptor(n);
			if (descriptor == server->sock)
			{
				new_fd = -1;
				sin_size = sizeof their_addr;
				new_fd = accept(server->sock, (struct sockaddr *)&their_addr, &sin_size);
				if (new_fd == -1)
				{
					perror("accept");
					continue;
				}
				else
				{
					server->selEpolKqEvPrtHandler.reRegisterServerSock();
					server->selEpolKqEvPrtHandler.registerForEvent(new_fd);
				}
			}
			else if (descriptor!=-1)
			{
				server->selEpolKqEvPrtHandler.unRegisterForEvent(descriptor);
				char buf[10];
				memset(buf, 0, sizeof(buf));
				int err;
				if((err=recv(descriptor,buf,10,MSG_PEEK))==0)
				{
					close(descriptor);
					continue;
				}

				fcntl(descriptor, F_SETFL, O_SYNC);

				Thread pthread(serv, &descriptor);
				pthread.execute();
			}
		}
	}
	return NULL;
}

int NBServer::Accept()
{
	return selEpolKqEvPrtHandler.getEvents();
}
int NBServer::Send(int fd,string data)
{
	int bytes = send(fd,data.c_str(),data.length(),0);
	if(bytes == -1)
	{
		logger << "send failed" << endl;
	}
	return bytes;
}
int NBServer::Send(int fd,vector<char> data)
{
	int bytes = send(fd,&data[0],data.size(),0);
	if(bytes == -1)
	{
		logger << "send failed" << endl;
	}
	return bytes;
}
int NBServer::Send(int fd,vector<unsigned char> data)
{
	int bytes = send(fd,&data[0],data.size(),0);
	if(bytes == -1)
	{
		logger << "send failed" << endl;
	}
	return bytes;
}
int NBServer::Send(int fd,char *data)
{
	int bytes = send(fd,data,sizeof data,0);
	if(bytes == -1)
	{
		logger << "send failed" << endl;
	}
	return bytes;
}
int NBServer::Send(int fd,unsigned char *data)
{
	int bytes = send(fd,data,sizeof data,0);
	if(bytes == -1)
	{
		logger << "send failed" << endl;
	}
	return bytes;
}

int NBServer::Receive(int fd,string &data,int bytes)
{
	if(bytes==0)
		return -1;
	char buf[bytes];
	memset(buf, 0, sizeof(buf));
	int bytesr = recv(fd, buf, bytes, 0);
	/*string temp;
	stringstream ss;
	ss << buf;
	while(getline(ss,temp,'\r'))
	{
		data.append(temp);
	}*/
	string temp(buf,buf+bytesr);
	data = temp;
	memset(&buf[0], 0, sizeof(buf));
	return bytesr;
}
int NBServer::Receive(int fd,vector<char> &data,int bytes)
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
int NBServer::Receive(int fd,vector<unsigned char>& data,int bytes)
{
	if(bytes==0)
		return -1;
	char te[bytes];
	int bytesr = Receive(fd,te,bytes);
	data.resize(bytes);
	copy(te,te+bytes,data.begin());
	return bytesr;
}
int NBServer::Receive(int fd,char data[],int bytes)
{
	if(bytes==0)
		return -1;
	int bytesr = recv(fd, data, bytes, 0);
	return bytesr;
}
int NBServer::Receive(int fd,unsigned char data[],int bytes)
{
	if(bytes==0)
		return -1;
	int bytesr = recv(fd, data, bytes, 0);
	return bytesr;
}
int NBServer::Receive(int fd,vector<string>& data,int bytes)
{
	if(bytes==0)
		return -1;
	char te[bytes];
	memset(te, 0, sizeof(te));
	string temp;
	int bytesr = Receive(fd,te,bytes);
	stringstream ss;
	ss << te;
	while(getline(ss,temp,'\n'))
	{
		data.push_back(temp);
		//logger << temp << endl;
	}
	memset(&te[0], 0, sizeof(te));
	return bytesr;
}

void NBServer::start()
{
	lock.lock();
	if(!started && runn)
	{
		started = true;
		Thread servicing_thread(&servicing, this);
		servicing_thread.execute();
	}
	lock.unlock();
}

void NBServer::stop()
{
	lock.lock();
	runn = false;
	lock.unlock();
}

int NBServer::getSocket()
{
	return sock;
}
