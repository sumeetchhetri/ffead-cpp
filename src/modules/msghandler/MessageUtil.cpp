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
 * MessageUtil.cpp
 *
 *  Created on: Oct 1, 2009
 *      Author: sumeet
 */

#include "MessageUtil.h"


void* MessageUtil::get_in_addr(struct sockaddr *sa)
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

std::string MessageUtil::getSubscriber() const
{
	return this->subscriber;
}

void MessageUtil::setSubscriber(const std::string& subscriber)
{
	this->subscriber = subscriber;
}

MessageUtil::MessageUtil(const std::string& file)
{
	logger = LoggerFactory::getLogger("MessageUtil");
	SimpleXmlParser parser("Parser");
	Document doc ;
	parser.readDocument(file, doc);
	Element* dest = doc.getRootElement().getElementByName("destination");
	Element* url = doc.getRootElement().getElementByName("url");
	std::vector<std::string> vec;
	std::string h = url->getText();
	StringUtil::split(vec, h , (":"));
	this->host = vec.at(0);
	this->port = vec.at(1);
	this->destination.setName(dest->getAttribute("name"));
	this->destination.setType(dest->getAttribute("type"));
}

MessageUtil::~MessageUtil()
{

}

bool MessageUtil::sendMessage(const Message& msg)
{
	int sockfd, numbytes;
	char buf[100];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(this->host.c_str(), this->port.c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
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
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	std::string h = msg.toXml();
	bool flag = true;
	if (send(sockfd, h.c_str(), h.length(), 0) == -1)
	{
		logger << "send failed" << std::flush;
		flag = false;
	}
	logger << h << std::flush;

	if ((numbytes = recv(sockfd, buf, 99, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	buf[numbytes] = '\0';
	printf("client: received '%s'\n",buf);
	close(sockfd);
	memset(&buf[0], 0, sizeof(buf));
	return flag;
}

Message MessageUtil::receiveMessage()
{
	int sockfd, numbytes;
	char buf[1024];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(this->host.c_str(), this->port.c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
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
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	std::string h = "GET FROM "+this->destination.getName()+":"+this->destination.getType();
	if(this->destination.getType()=="Topic")
		h += ("-" + this->getSubscriber());
	//bool flag = true;
	if (send(sockfd, h.c_str(), h.length(), 0) == -1)
	{
		logger << "send failed" << std::flush;
		//flag = false;
	}
	logger << h << std::flush;

	if ((numbytes = recv(sockfd, buf, 1024, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	std::string temp,results;
	std::stringstream ss;
	ss << buf;
	while(getline(ss,temp))
	{
		logger << temp << std::flush;
		results += temp;
	}
	logger << "\n\nclient: received" << std::flush;
	logger << results << std::flush;
	Message msg(results);
	buf[numbytes] = '\0';

	close(sockfd);
	memset(&buf[0], 0, sizeof(buf));
	return msg;
}

Destination MessageUtil::getDestination()
{
	return this->destination;
}


void MessageUtil::subscribe(const std::string& subs)
{
	if(this->destination.getType()=="Topic")
	{
		int sockfd, numbytes;
		char buf[1024];
		struct addrinfo hints, *servinfo, *p;
		int rv;
		char s[INET6_ADDRSTRLEN];

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if ((rv = getaddrinfo(this->host.c_str(), this->port.c_str(), &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
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
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);
		printf("client: connecting to %s\n", s);

		freeaddrinfo(servinfo); // all done with this structure

		std::string h = "SUBSCRIBE "+subs+" TO "+this->destination.getName()+":"+this->destination.getType();
		//bool flag = true;
		if (send(sockfd, h.c_str(), h.length(), 0) == -1)
		{
			logger << "send failed" << std::flush;
			//flag = false;
		}
		logger << h << std::flush;

		if ((numbytes = recv(sockfd, buf, 1024, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		std::string temp,results;
		std::stringstream ss;
		ss << buf;
		while(getline(ss,temp))
		{
			logger << temp << std::flush;
			results += temp;
		}
		logger << "\n\nclient: received" << std::flush;
		logger << results << std::flush;
		buf[numbytes] = '\0';
		close(sockfd);
		memset(&buf[0], 0, sizeof(buf));
	}
}

void MessageUtil::unSubscribe(const std::string& subs)
{
	if(this->destination.getType()=="Topic")
	{
		int sockfd, numbytes;
		char buf[1024];
		struct addrinfo hints, *servinfo, *p;
		int rv;
		char s[INET6_ADDRSTRLEN];

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if ((rv = getaddrinfo(this->host.c_str(), this->port.c_str(), &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
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
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);
		printf("client: connecting to %s\n", s);

		freeaddrinfo(servinfo); // all done with this structure

		std::string h = "UNSUBSCRIBE "+subs+" TO "+this->destination.getName()+":"+this->destination.getType();
		//bool flag = true;
		if (send(sockfd, h.c_str(), h.length(), 0) == -1)
		{
			logger << "send failed" << std::flush;
			//flag = false;
		}
		logger << h << std::flush;

		if ((numbytes = recv(sockfd, buf, 1024, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		std::string temp,results;
		std::stringstream ss;
		ss << buf;
		while(getline(ss,temp))
		{
			logger << temp << std::flush;
			results += temp;
		}
		logger << "\n\nclient: received" << std::flush;
		logger << results << std::flush;
		buf[numbytes] = '\0';

		close(sockfd);
		memset(&buf[0], 0, sizeof(buf));
	}
}
