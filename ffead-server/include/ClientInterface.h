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
 * ClientInterface.h
 *
 *  Created on: Dec 23, 2012
 *      Author: sumeet
 */

#ifndef CLIENT_INTERFACE_H_
#define CLIENT_INTERFACE_H_
#include "iostream"
#include "sstream"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "string"
#include "cstring"
#include <signal.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include "CastUtil.h"
#include "LoggerFactory.h"
#include "cstring"
using namespace std;

#define MAXBUFLE 32768
class ClientInterface
{
public:
	virtual bool connection(string,int)=0;
	virtual void closeConnection()=0;
	virtual bool connectionUnresolv(string host,int port)=0;
	virtual bool isConnected()=0;
	virtual int sendData(string)=0;
	virtual string getBinaryData(int,bool)=0;
	virtual string getTextData(string hdrdelm,string cntlnhdr)=0;
	int getLengthCl(string header,int size);
	void *get_in_addr(struct sockaddr *sa);
	int create_tcp_socket();
	static char* get_ip(char *host);
	static bool isConnected(int fd);
	virtual ~ClientInterface(){}
};

#endif /* CLIENT_INTERFACE_H_ */
