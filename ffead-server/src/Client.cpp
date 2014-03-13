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
	logger = LoggerFactory::getLogger("Client");
	connected = false;
}

Client::~Client() {
	closeConnection();
}

bool Client::connection(string host,int port)
{
	if(host=="localhost")
	{
		return connectionUnresolv(host, port);
	}

	struct sockaddr_in *remote;
	int tmpres;
	char *ip;

	sockfd = create_tcp_socket();
	ip = get_ip((char*)host.c_str());
	fprintf(stderr, "IP is %s\n", ip);
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;
	tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
	if( tmpres < 0)
	{
		perror("Can't set remote->sin_addr.s_addr");
		return false;
	}
	else if(tmpres == 0)
	{
		fprintf(stderr, "%s is not a valid IP address\n", ip);
		return false;
	}
	remote->sin_port = htons(port);

	if(connect(sockfd, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
		perror("Could not connect");
		connected = false;
	} else {
		connected = true;
	}
	free(remote);
	free(ip);
	
	return connected;
}


bool Client::connectionUnresolv(string host,int port)
{
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    memset(s, 0, INET6_ADDRSTRLEN);
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

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    //printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    return connected;
}

void Client::setSocketBlocking()
{
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
}

void Client::setSocketNonBlocking()
{
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_SYNC);
}

int Client::sendData(string data)
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

string Client::getTextData(string hdrdelm,string cntlnhdr)
{
	int er=-1;
	bool flag = true;
	string alldat;
	int cntlen;
	char buf[MAXBUFLE];
	memset(buf, 0, MAXBUFLE);
	BIO* sbio=BIO_new_socket(sockfd,BIO_NOCLOSE);
	BIO* io=BIO_new(BIO_f_buffer());
	BIO_push(io,sbio);
	while(flag)
	{
		er = BIO_gets(io,buf,MAXBUFLE-1);
		if(er==0)
		{
			if(io!=NULL)BIO_free_all(io);
			//logger << "\nsocket closed before being serviced" <<flush;
			return alldat;
		}
		if(!strcmp(buf,hdrdelm.c_str()))
		{
			string tt(buf, er);
			alldat += tt;
			break;
		}
		string temp(buf, er);
		temp = temp.substr(0,temp.length()-1);
		alldat += (temp + "\n");
		if(temp.find(cntlnhdr)!=string::npos)
		{
			std::string cntle = temp.substr(temp.find(": ")+2);
			cntle = cntle.substr(0,cntle.length()-1);
			try
			{
				cntlen = CastUtil::lexical_cast<int>(cntle);
			}
			catch(...)
			{
				logger << "bad lexical cast" <<endl;
			}
		}
		memset(&buf[0], 0, sizeof(buf));
	}
	while(cntlen>0)
	{
		//logger << "reading conetnt " << cntlen;
		int toRead = cntlen;
		if(cntlen>MAXBUFLE)
			toRead = MAXBUFLE - 1;
		er = BIO_read(io,buf,toRead);
		if(er==0)
		{
			if(io!=NULL)BIO_free_all(io);
			//logger << "\nsocket closed before being serviced" <<flush;
			return alldat;
		}
		string temp(buf, er);
		alldat += temp;
		cntlen -= er;
		memset(&buf[0], 0, sizeof(buf));
	}
	return alldat;
}

int Client::receive(string& buf,int flag)
{
	char buff[MAXBUFLE+1];
	memset(buff, 0, sizeof(buff));
	int t = recv(sockfd, buff, MAXBUFLE, flag);
	buf = buff;
	memset(buff, 0, sizeof(buff));
	return t;
}

int Client::receivelen(string& buf,int len, int flag)
{
	char buff[len+1];
	memset(buff, 0, sizeof(buff));
	int t = recv(sockfd, buff, len, flag);
	buf = buff;
	memset(buff, 0, sizeof(buff));
	return t;
}

int Client::sendlen(string buf,int len)
{
	return send(sockfd, buf.c_str(), len, 0);
}

string Client::getBinaryData(int len, bool isLengthIncluded)
{
	//logger << len;
	string alldat;
	char *buf1 = new char[len];
	memset(buf1, 0, len);
	recv(sockfd, buf1, len, 0);
	for (int var = 0; var < len; ++var) {
		alldat.push_back(buf1[var]);
	}
	delete[] buf1;

	int leng = getLengthCl(alldat, len);
	if(isLengthIncluded)
	{
		leng -= len;
	}
	char *buf = new char[leng];
	memset(buf, 0, leng);
	recv(sockfd, buf, leng, 0);
	for (int var = 0; var < leng; ++var) {
		alldat.push_back(buf[var]);
	}
	delete[] buf;
	return alldat;
}

void Client::closeConnection()
{
	if(!connected)return;
	connected = false;
	close(sockfd);
}

bool Client::isConnected()
{
	return connected && ClientInterface::isConnected(sockfd);
}

string Client::getData()
{
	int numbytes;
	char buf[MAXBUFLE];
	memset(buf, 0, sizeof(buf));
	while ((numbytes = recv(sockfd, buf, MAXBUFLE-1, 0)) == -1)
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
