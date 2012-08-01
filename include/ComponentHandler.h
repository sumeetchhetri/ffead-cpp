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
 * ComponentHandler.h
 *
 *  Created on: Mar 26, 2010
 *      Author: sumeet
 */

#ifndef COMPONENTHANDLER_H_
#define COMPONENTHANDLER_H_
#include "string"
#include "Reflector.h"
#include "Component.h"
#include "Server.h"
#include "XmlParser.h"
#include "Reflector.h"
#include "fstream"
#include "Serialize.h"
#include <boost/thread/thread.hpp>
#include "map"
#include "Logger.h"
#include "Constants.h"

#define BACKLOG1 500
#define MAXBUFLEN1 1024
using namespace std;
class ComponentHandlerException: public exception
{
	public:
		ComponentHandlerException(string message,string &ret){message="<return:exception>"+message+"</return:exception>";ret=message;}
		virtual ~ComponentHandlerException() throw(){}
		string getMessage() const{return message;};
	private:
		string message;
};
class ComponentHandler{
	Logger logger;
	static void service(int fd);
	Server *server;
	bool running;
	map<string,string> components;
	static void initComponent();
	static void init();
	ComponentHandler();
	virtual ~ComponentHandler();
public:
	static bool registerComponent(string);
	static bool unregisterComponent(string);
	static void trigger(string);
	Server* getServer(){return this->server;}
	void setServer(Server *server){this->server = server;}
};

#endif /* COMPONENTHANDLER_H_ */
