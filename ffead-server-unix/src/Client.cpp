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
 * Client.cpp
 *
 *  Created on: Mar 27, 2010
 *      Author: sumeet
 */

#include "Client.h"

Client::Client() {
	connected = false;
}

Client::~Client() {
	// TODO Auto-generated destructor stub
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr1(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool Client::connection(string host,int port)
{
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    string sport = CastUtil::lexical_cast<string>(port);
    if ((rv = getaddrinfo(host.c_str(), sport.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return false;
    }

    inet_ntop(p->ai_family, get_in_addr1((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    /*if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);

    close(sockfd);*/
    connected = true;
    return true;
}

int Client::sendData(string data)
{
	int bytes = send(sockfd,data.c_str(),data.length(),0);
	if(bytes == -1)
	{

	}
	return bytes;
}

string Client::getData()
{
	int numbytes;
	char buf[MAXDATASIZE];
	while ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
	{
		//perror("recv");
		if(errno!=EAGAIN)
			return "";
		//exit(1);
	}
	if(numbytes==0)
	{
		connected = false;
		close(sockfd);
		return "";
	}
	string data(buf,buf+numbytes);
	memset(&buf[0], 0, sizeof(buf));
	return data;
}

void Client::closeConnection()
{
	connected = false;
	close(sockfd);
}

bool Client::isConnected()
{
	int numbytes;
	char buf[1];
	if ((numbytes = recv(sockfd, buf, 1, MSG_PEEK)) == 0)
	{
		return false;
	}
	return connected;
}
