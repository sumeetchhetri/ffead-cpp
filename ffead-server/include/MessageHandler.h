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
 * MessageHandler.h
 *
 *  Created on: Sep 27, 2009
 *      Author: sumeet
 */

#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "Thread.h"
#include "Message.h"
#include "Exception.h"
#include <iostream>
#include <fstream>
#include "StringUtil.h"
#include "vector"
#include "algorithm"
#include "MQueue.h"
#include "Server.h"
#include "string"
#include "Logger.h"
#include "Constants.h"
#include "AMEFResources.h"


#define MAXBUFLEN 1024
#define BACKLOG 10
typedef vector<unsigned char> Cont;
using namespace std;
class MessageHandler {
	string path;
	Logger logger;
public:
	MessageHandler(string);
	~MessageHandler()
	{
	}
	static void trigger(string,string);
	Server* getServer(){return this->server;}
	void setServer(Server *server){this->server = server;}
private:
	Server *server;
	bool running;
	static void* service(void* arg);
	static void init(string);
	Message readMessageFromQ(string fileName, bool erase);
	void writeMessageToQ(Message msg,string fileName);
	bool tempUnSubscribe(string subs,string fileName);
	Message readMessageFromT(string fileName,string subs);
	void writeMessageToT(Message msg,string fileName);
	void subscribe(string subs,string fileName);
	void unSubscribe(string subs,string fileName);
};

#endif /* MESSAGEHANDLER_H_ */
