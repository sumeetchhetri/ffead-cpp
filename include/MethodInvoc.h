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
 * MethodInvoc.h
 *
 *  Created on: Jan 30, 2010
 *      Author: sumeet
 */

#ifndef METHODINVOC_H_
#define METHODINVOC_H_

#include "Server.h"
#include "XmlParser.h"
#include "Reflector.h"
#include "fstream"
#include "Serialize.h"

#define BACKLOGMI 500
#define MAXBUFLEN 1024

using namespace std;
typedef vector<Element> ElementList;
class MethodInvokerException: public exception
{
	public:
		MethodInvokerException(string message,string &ret){message="<return:exception>"+message+"</return:exception>";ret=message;}
		virtual ~MethodInvokerException() throw(){}
		string getMessage() const{return message;};
	private:
		string message;
};
class MethodInvoc {
	string className;
	string methodName;
	string returntype;
	string retByValOrRef;
	map<string,string> arguments;
	bool startTranx;
	bool endTranx;
	Server *server;
	bool running;
	static void init();
	static void* service(void* arg);

public:
	MethodInvoc();
	virtual ~MethodInvoc();
	static void trigger(string);
	Server* getServer(){return this->server;}
	void setServer(Server *server){this->server = server;}
};

#endif /* METHODINVOC_H_ */
/*
<method className="ExampleClass" methodName="exampleMethod" returntype="string" retByValOrRef="value" startTranx="true">
    <arguments>
        <arg type="string">asdfsdf</arg>
        <arg type="SomeClass"><SomeClass><id type=""><></SomeClass></arg>
        <arg type="string">asdfsdf</arg>
    </arguments>
</method>
*/
