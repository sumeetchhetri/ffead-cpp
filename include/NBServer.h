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
 * NBServer.h
 *
 *  Created on: Jan 2, 2010
 *      Author: sumeet
 */

#ifndef NBSERVER_H_
#define NBSERVER_H_
#include "AppDefines.h"
#include "iostream"
#include <unistd.h>
#include <errno.h>
#include "mingw.h"
#if !defined(OS_MINGW)
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#endif

#include <signal.h>
#include "string"
#include "cstring"
#include <stdio.h>
#include <stdlib.h>
#include "vector"
#include "sstream"
#include <fcntl.h>

/*Fix for Windows Cygwin*///#include <sys/epoll.h>

#include "Thread.h"
#include "LoggerFactory.h"
#include "SelEpolKqEvPrt.h"
#define MAXEPOLLSIZES 10000
#define BACKLOGM 500
using namespace std;

typedef void* (*Service)(void*);
class NBServer {
	Logger logger;
	SOCKET sock;
	int mode;
	Service service;
	Mutex lock;
	#ifdef OS_MINGW
		struct sockaddr_in their_addr;
	#else
		struct sockaddr_storage their_addr;
	#endif
	static void* servicing(void* arg);
	bool runn, started;
	SelEpolKqEvPrt selEpolKqEvPrtHandler;
public:
	SOCKET getSocket();
	NBServer();
	NBServer(string,int,Service);
	virtual ~NBServer();
	int Accept();
	int Send(SOCKET,string);
	int Send(SOCKET,vector<char>);
	int Send(SOCKET,vector<unsigned char>);
	int Send(SOCKET,char*);
	int Send(SOCKET,unsigned char*);
	int Receive(SOCKET,string&,int);
	int Receive(SOCKET,vector<char>&,int);
	int Receive(SOCKET,vector<unsigned char>&,int);
	int Receive(SOCKET,char *data,int);
	int Receive(SOCKET,unsigned char *data,int);
	int Receive(SOCKET,vector<string>&,int);
	void start();
	void stop();
};

#endif /* NBSERVER_H_ */
