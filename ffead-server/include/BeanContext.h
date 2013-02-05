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
 * BeanContext.h
 *
 *  Created on: Mar 27, 2010
 *      Author: sumeet
 */

#ifndef BeanContext_H_
#define BeanContext_H_
#include "CastUtil.h"
#include "Client.h"
#include "Reflector.h"
#include "Serialize.h"
#include "Object.h"
#include "XmlParser.h"
#include "Logger.h"

class BeanContext {
	string host;
	int port;
	Client client;
	Logger logger;
public:
	BeanContext();
	BeanContext(string host,int port);
	virtual ~BeanContext();
	string getHost() const;
	void setHost(string host);
	int getPort() const;
	void setPort(int port);
	void* lookup(string);
	void* invoke(string name,vector<Object> args,string,string);
};
class RemoteComponentInt {
	BeanContext cntxt;
public:
	BeanContext getContext();
	RemoteComponentInt();
	virtual ~RemoteComponentInt();
	void setBeanContext(BeanContext cntxt);
};
#endif /* BeanContext_H_ */
