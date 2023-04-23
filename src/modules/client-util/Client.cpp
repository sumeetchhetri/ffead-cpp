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
 * Client.cpp
 *
 *  Created on: Mar 27, 2010
 *      Author: sumeet
 */

#include "Client.h"

Client::Client() {
	//logger = LoggerFactory::getLogger("Client");
	connected = false;
	sockfd = -1;
}

Client::~Client() {
	closeConnection();
}

int Client::conn(const std::string& host, const int& port) {
	int sockfd = create_tcp_socket();

	struct sockaddr_in *remote;
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;

	if(host!="localhost" && host!="0.0.0.0" && host!="127.0.0.1") {
		char* ip = get_ip((char*)host.c_str());
		fprintf(stderr, "IP is %s\n", ip);
		int tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
		if( tmpres < 0)
		{
			free(remote);
			perror("Can't set remote->sin_addr.s_addr");
			return false;
		}
		else if(tmpres == 0)
		{
			free(remote);
			fprintf(stderr, "%s is not a valid IP address\n", ip);
			return false;
		}
		remote->sin_addr.s_addr = inet_addr(ip);
		remote->sin_port = htons(port);
		free(ip);

		if(connect(sockfd, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0 && (errno != EINPROGRESS)) {
			perror("Could not connect");
		} else {
		}

		free(remote);
	} else {
		struct addrinfo hints, *servinfo, *p;
		int rv;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		std::string ports = CastUtil::lexical_cast<std::string>(port);

		if ((rv = getaddrinfo(host.c_str(), ports.c_str(), &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				//perror("client: connect");
				continue;
			}

			break;
		}

		if (p == NULL) {
			perror("client: failed to connect\n");
			return false;
		}

		freeaddrinfo(servinfo);
	}
	return sockfd;
}

bool Client::connection(const std::string& host, const int& port)
{
	sockfd = create_tcp_socket();

	struct sockaddr_in *remote;
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;

	if(host!="localhost" && host!="0.0.0.0" && host!="127.0.0.1") {
		char* ip = get_ip((char*)host.c_str());
		fprintf(stderr, "IP is %s\n", ip);
		int tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
		if( tmpres < 0)
		{
			free(remote);
			perror("Can't set remote->sin_addr.s_addr");
			return false;
		}
		else if(tmpres == 0)
		{
			free(remote);
			fprintf(stderr, "%s is not a valid IP address\n", ip);
			return false;
		}
		remote->sin_addr.s_addr = inet_addr(ip);
		remote->sin_port = htons(port);
		free(ip);

		if(connect(sockfd, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0 && (errno != EINPROGRESS)) {
			perror("Could not connect");
			connected = false;
		} else {
			connected = true;
		}

		free(remote);
	} else {
		struct addrinfo hints, *servinfo, *p;
		int rv;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		std::string ports = CastUtil::lexical_cast<std::string>(port);

		if ((rv = getaddrinfo(host.c_str(), ports.c_str(), &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				//perror("client: connect");
				continue;
			}

			break;
		}

		if (p == NULL) {
			perror("client: failed to connect\n");
			return false;
		}

		freeaddrinfo(servinfo);
	}

	connected = true;

	int error = 0;
	socklen_t len = sizeof (error);
#if defined(OS_MINGW)
	int retval = getsockopt (sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
#else
	int retval = getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
#endif
	if (retval != 0) {
	    /* there was a problem getting the error code */
	    fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
	    connected = false;
	}

	if (error != 0) {
	    /* socket has a non zero error status */
	    fprintf(stderr, "socket error: %s\n", strerror(error));
	    connected = false;
	}

	return connected;
}

bool Client::connectionNB(const std::string& host, const int& port)
{
	connection(host, port);

	setSocketNonBlocking(sockfd);

	return connected;
}

/*bool Client::connectionUnresolv(const std::string& host, const int& port)
{
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    memset(s, 0, INET6_ADDRSTRLEN);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    std::string sport = CastUtil::fromNumber(port);
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

        fcntl(sockfd, F_SETFL, O_NONBLOCK);
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 && (errno != EINPROGRESS)) {
        	closesocket(sockfd);
            perror("client: connect");
            connected = false;
            continue;
        } else {
        	connected = true;
        }
        break;
    }

    if (p == NULL) {
    	connected = false;
        fprintf(stderr, "client: failed to connect\n");
        return false;
    }

    //inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
    //        s, sizeof s);
    //printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    return connected;
}*/

int Client::sendData(std::string data)
{
	int sent = 0;
	while(data.length()>0)
	{
		int tmpres = send(sockfd, data.c_str(), data.length(), 0);
		if(tmpres == -1){
			perror("Can't send data");
		}
		data = data.substr(tmpres);
	}
	return sent;
}

std::string Client::getHttpData(const std::string& hdrdelm, const std::string& cntlnhdr, bool connected, std::string& buffer) {
	int cntlen = 0;
	std::string alldat;
	bool isTE = false;
	std::string tehdr = "transfer-encoding: chunked";
	if(connected) {
		if(buffer.find(hdrdelm)!=std::string::npos) {
			alldat = buffer.substr(0, buffer.find(hdrdelm)+4);
			buffer = buffer.substr(buffer.find(hdrdelm)+4);
		}
		std::string ltemp = StringUtil::toLowerCopy(alldat);
		std::string cntlnhdr1 = cntlnhdr + ": ";
		size_t ps = ltemp.find(cntlnhdr1);
		if(ps!=std::string::npos)
		{
			std::string cntle = alldat.substr(ps+cntlnhdr1.length(), ltemp.find("\r\n", ps));
			StringUtil::trim(cntle);
			try
			{
				cntlen = CastUtil::toInt(cntle);
			}
			catch(const std::exception& e)
			{
				//logger << "bad lexical cast" <<std::endl;
			}
		}
		else if(ltemp.find(tehdr)!=std::string::npos)
		{
			isTE = true;
		}
	} else {
		return alldat;
	}
	while(isTE && buffer.find("\r\n")!=std::string::npos)
	{
		std::string len = buffer.substr(0, buffer.find("\r\n"));
		buffer = buffer.substr(buffer.find("\r\n")+2);
		int bytesToRead = (int)StringUtil::fromHEX(len);
		if(bytesToRead==0)
		{
			return alldat;
		}
		alldat += buffer.substr(0, bytesToRead);
		buffer = buffer.substr(bytesToRead);
	}
	if(cntlen>0)
	{
		alldat += buffer.substr(0, cntlen);
		buffer = buffer.substr(cntlen);
	}
	return alldat;
}

std::string Client::getTextData(const std::string& hdrdelm, const std::string& cntlnhdr)
{
	getData();
	return getHttpData(hdrdelm, cntlnhdr, connected, buffer);
}

bool Client::isReady(int mode) {
	return ClientInterface::isReady(sockfd, mode);
}

int Client::receive(std::string& buf, const int& flag)
{
	char buff[MAXBUFLE+1];
	memset(buff, 0, sizeof(buff));
	int t = recv(sockfd, buff, MAXBUFLE, flag);
	buf = buff;
	memset(buff, 0, sizeof(buff));
	return t;
}

int Client::receivelen(std::string& buf, const int& len, const int& flag)
{
	char buff[len+1];
	memset(buff, 0, sizeof(buff));
	int t = recv(sockfd, buff, len, flag);
	buf = buff;
	memset(buff, 0, sizeof(buff));
	return t;
}

int Client::sendlen(const std::string& buf, const int& len)
{
	return send(sockfd, buf.c_str(), len, 0);
}

std::string Client::getBinaryData(const int& len, const bool& isLengthIncluded)
{
	//logger << len;
	std::string alldat;
	char buf1[len];
	memset(buf1, 0, len);
	recv(sockfd, buf1, len, 0);
	for (int var = 0; var < len; ++var) {
		alldat.push_back(buf1[var]);
	}

	int leng = getLengthCl(alldat, len);
	if(isLengthIncluded)
	{
		leng -= len;
	}
	char buf[leng];
	memset(buf, 0, leng);
	recv(sockfd, buf, leng, 0);
	for (int var = 0; var < leng; ++var) {
		alldat.push_back(buf[var]);
	}
	return alldat;
}

void Client::closeConnection()
{
	if(!connected)return;
	connected = false;
	closesocket(sockfd);
}

bool Client::isConnected()
{
	return connected && ClientInterface::isReady(sockfd, 2);
}

std::string Client::getData()
{
	int numbytes;
	char buf[MAXBUFLE];
	memset(buf, 0, sizeof(buf));
	while ((numbytes = recv(sockfd, buf, MAXBUFLE, 0)) > 0)
	{
		buffer.append(buf, numbytes);
		memset(buf, 0, sizeof(buf));
	}
	if(numbytes==0)
	{
		connected = false;
		closesocket(sockfd);
	}
	return buffer;
}
