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
 * Client.h
 *
 *  Created on: Mar 27, 2010
 *      Author: sumeet
 */

#ifndef CLIENT_H_
#define CLIENT_H_
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
#include "boost/lexical_cast.hpp"
#include "cstring"
using namespace std;
#define MAXDATASIZE 100
class Client {
	bool connected;
	int sockfd;
public:
	Client();
	virtual ~Client();
	bool connection(string,int);
	int sendData(string);
	string getData();
	void closeConnection();
	bool isConnected();
};

#endif /* CLIENT_H_ */
