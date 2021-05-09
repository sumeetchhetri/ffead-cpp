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
 * ClientInterface.cpp
 *
 *  Created on: Dec 24, 2010
 *      Author: sumeet
 */

#include "ClientInterface.h"

int ClientInterface::getLengthCl(const std::string& header, const int& size)
{
	int totsize = header[size-1] & 0xff;
	for (int var = 0; var < size-1; var++)
	{
		totsize |= ((header[var] & 0xff) << (size-1-var)*8);
	}
	return totsize;
}

void* ClientInterface::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	#if !defined(OS_MINGW)
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
	#else
		return NULL;
	#endif
}

SOCKET ClientInterface::create_tcp_socket()
{
	SOCKET sock;
	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		perror("Can't create TCP socket");
		exit(1);
	}
	return sock;
}

char* ClientInterface::get_ip(char *host)
{
	struct hostent *hent;
	int iplen = 15; //XXX.XXX.XXX.XXX
	char *ip = (char *)malloc(iplen+1);
	memset(ip, 0, iplen+1);
	if((hent = gethostbyname(host)) == NULL)
	{
		perror("Can't get IP");
		return ip;
	}
	if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL)
	{
		perror("Can't resolve host");
		//return NULL;
	}
	return ip;
}

bool ClientInterface::isReady(const SOCKET& fd, int mode) {
	/*char c;
	if (recv(fd, &c, 1, MSG_DONTWAIT | MSG_PEEK) == 0) {
		return false;
	}
	return true;*/
	/*#ifdef OS_MINGW
		u_long iMode = 1;
		ioctlsocket(fd, FIONBIO, &iMode);
	#else
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
	#endif*/

	fd_set rset, wset;
	struct timeval tv = {0, 100};
	int rc;

	/* Guard against closed socket */
	if (fd < 0)
	{
		return false;
	}

	/* Construct the arguments to select */
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	wset = rset;

	/* See if the socket is ready */
	switch (mode)
	{
		case 0:
			rc = select(fd+1, &rset, NULL, NULL, &tv);
			break;
		case 1:
			rc = select(fd+1, NULL, &wset, NULL, &tv);
			break;
		case 2:
			rc = select(fd+1, &rset, &wset, NULL, &tv);
			break;
	}
	FD_CLR(fd, &rset);

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	/* Return SOCKET_TIMED_OUT on timeout, SOCKET_OPERATION_OK
	otherwise
	(when we are able to write or when there's something to
	read) */
	return rc <= 0 ? false : true;
}

void ClientInterface::setSocketBlocking(const SOCKET& sockfd)
{
	#ifdef OS_MINGW
		u_long bMode = 0;
		ioctlsocket(sockfd, FIONBIO, &bMode);
	#else
		fcntl(sockfd, F_SETFL, O_SYNC);
	#endif
}

void ClientInterface::setSocketNonBlocking(const SOCKET& sockfd)
{
	#ifdef OS_MINGW
		u_long iMode = 1;
		ioctlsocket(sockfd, FIONBIO, &iMode);
	#else
		fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
	#endif
}
