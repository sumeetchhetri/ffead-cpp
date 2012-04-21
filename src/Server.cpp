/*
	Copyright 2010, Sumeet Chhetri 
  
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


void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}


Server::Server(string port,int waiting,Service serv)
{
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

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(0);
	}
	freeaddrinfo(servinfo); // all done with this structure
	if (listen(this->sock, waiting) == -1)
	{
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}
	cout << "waiting for connections on " << port << ".....\n" << flush;

	if(fork()==0)
	{
		struct epoll_event events[MAXEPOLLSIZES];
		int epoll_handle = epoll_create(MAXEPOLLSIZES);
		int curfds =1;
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLPRI;
		ev.data.fd = this->sock;
		if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, this->sock, &ev) < 0)
		{
			fprintf(stderr, "epoll set insertion error: fd=%d\n", this->sock);
			//return -1;
		}
		else
			printf("listener socket to join epoll success!\n");
		while(1)
		{
			int nfds = epoll_wait(epoll_handle, events, curfds,-1);
			if (nfds == -1)
			{
				perror("epoll_wait");
				break;
			}
			for(int n=0;n<nfds;n++)
			{
				if (events[n].data.fd == this->sock)
				{
					int new_fd = this->Accept();
					if (new_fd == -1)
					{
						perror("accept");
						continue;
					}
					else
					{
						fcntl(new_fd, F_SETFL, fcntl(new_fd, F_GETFD, 0) | O_NONBLOCK);
						ev.events = EPOLLIN | EPOLLPRI;
						ev.data.fd = new_fd;
						if (epoll_ctl(epoll_handle, EPOLL_CTL_ADD, new_fd, &ev) < 0)
						{
							perror("epoll");
							//return -1;
						}
					}
				}
				else
				{
					boost::thread m_thread(boost::bind(serv,events[n].data.fd));
				}
			}
		}
	}

}

Server::Server(string port,bool block,int waiting,Service serv,bool tobefork)
{
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
	if (listen(this->sock, waiting) == -1)
	{
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}
	cout << "waiting for connections on " << port << ".....\n" << flush;
	if(tobefork)
	{
		if(fork()==0)
		{
			servicing(serv);
		}
	}
	else
		servicing(serv);
}

Server::~Server() {
	// TODO Auto-generated destructor stub
}

void Server::servicing(Service serv)
{
	while(1)
	{
		int new_fd = this->Accept();
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}
		else
		{
			boost::thread m_thread(boost::bind(serv,new_fd));
		}
	}
}

int Server::Accept()
{
	socklen_t sin_size = sizeof their_addr;
	int new_fd = accept(this->sock, (struct sockaddr *)&(this->their_addr), &sin_size);
	return new_fd;
}
int Server::Send(int fd,string data)
{
	int bytes = send(fd,data.c_str(),data.length(),0);
	if(bytes == -1)
	{
		cout << "send failed" << flush;
	}
	return bytes;
}
int Server::Send(int fd,vector<char> data)
{
	int bytes = send(fd,&data[0],data.size(),0);
	if(bytes == -1)
	{
		cout << "send failed" << flush;
	}
	return bytes;
}
int Server::Send(int fd,vector<unsigned char> data)
{
	int bytes = send(fd,&data[0],data.size(),0);
	if(bytes == -1)
	{
		cout << "send failed" << flush;
	}
	return bytes;
}
int Server::Send(int fd,char *data)
{
	int bytes = send(fd,data,sizeof data,0);
	if(bytes == -1)
	{
		cout << "send failed" << flush;
	}
	return bytes;
}
int Server::Send(int fd,unsigned char *data)
{
	int bytes = send(fd,data,sizeof data,0);
	if(bytes == -1)
	{
		cout << "send failed" << flush;
	}
	return bytes;
}

int Server::Receive(int fd,string &data,int bytes)
{
	if(bytes==0)
		return -1;
	char buf[bytes];
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
int Server::Receive(int fd,vector<char> &data,int bytes)
{
	if(bytes==0)
		return -1;
	char te[bytes];
	int bytesr = Receive(fd,te,bytes);
	data.resize(bytes);
	copy(te,te+bytes,data.begin());
	return bytesr;
}
int Server::Receive(int fd,vector<unsigned char>& data,int bytes)
{
	if(bytes==0)
		return -1;
	char te[bytes];
	int bytesr = Receive(fd,te,bytes);
	data.resize(bytes);
	copy(te,te+bytes,data.begin());
	return bytesr;
}
int Server::Receive(int fd,char data[],int bytes)
{
	if(bytes==0)
		return -1;
	int bytesr = recv(fd, data, bytes, 0);
	return bytesr;
}
int Server::Receive(int fd,unsigned char data[],int bytes)
{
	if(bytes==0)
		return -1;
	int bytesr = recv(fd, data, bytes, 0);
	return bytesr;
}
int Server::Receive(int fd,vector<string>& data,int bytes)
{
	if(bytes==0)
		return -1;
	char te[bytes];
	string temp;
	int bytesr = Receive(fd,te,bytes);
	stringstream ss;
	ss << te;
	while(getline(ss,temp,'\n'))
	{
		data.push_back(temp);
		cout << temp << flush;
	}
	memset(&te[0], 0, sizeof(te));
	return bytesr;
}
