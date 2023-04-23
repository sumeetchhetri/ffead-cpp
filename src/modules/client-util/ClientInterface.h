/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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
#include "AppDefines.h"
#include "Compatibility.h"
#if !defined(OS_MINGW)
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include "iostream"
#include "sstream"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "string"
#include "cstring"
#include <signal.h>
#ifdef HAVE_SSLINC
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif
#include "CastUtil.h"
#include "LoggerFactory.h"
#include "cstring"



#define MAXBUFLE 8192
class ClientInterface
{
public:
	virtual bool connection(const std::string&, const int&)=0;
	virtual bool connectionNB(const std::string&, const int&)=0;
	virtual void closeConnection()=0;
	//virtual bool connectionUnresolv(const std::string& host, const int& port)=0;
	virtual bool isConnected()=0;
	virtual int sendData(std::string)=0;
	virtual std::string getBinaryData(const int&, const bool&)=0;
	virtual std::string getTextData(const std::string& hdrdelm, const std::string& cntlnhdr)=0;
	virtual bool isReady(int mode)=0;
	int getLengthCl(const std::string& header, const int& size);
	void *get_in_addr(struct sockaddr *sa);
	static SOCKET create_tcp_socket();
	static char* get_ip(char *host);
	static bool isReady(const SOCKET& fd, int mode);
	virtual ~ClientInterface(){}
	void setSocketBlocking(const SOCKET& sockfd);
	void setSocketNonBlocking(const SOCKET& sockfd);
};

#endif /* CLIENT_INTERFACE_H_ */
