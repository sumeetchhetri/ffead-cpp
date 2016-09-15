/*
	Copyright 2009-2012, Sumeet Chhetri 
  
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
#include "XMLSerialize.h"
#include "Thread.h"
#include "map"
#include "LoggerFactory.h"
#include "Constants.h"

#define BACKLOG1 500
#define MAXBUFLEN1 1024

class ComponentHandlerException: public std::exception
{
	public:
		ComponentHandlerException(std::string message, std::string &ret){message="<return:exception>"+message+"</return:exception>";ret=message;}
		virtual ~ComponentHandlerException() throw(){}
		std::string getMessage() const{return message;};
	private:
		std::string message;
};
class ComponentHandler {
	static ComponentHandler* instance;
	Logger logger;
	static void* service(void* arg);
	Server server;
	bool running;
	std::map<std::string,std::string> components;
	static void initComponent();
	static void init();
	ComponentHandler();
	virtual ~ComponentHandler();
public:
	static bool registerComponent(const std::string&);
	static bool unregisterComponent(const std::string&);
	static void trigger(const std::string&);
	static void stop();
};

#endif /* COMPONENTHANDLER_H_ */
