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
 * BeanContext.cpp
 *
 *  Created on: Mar 27, 2010
 *      Author: sumeet
 */

#include "BeanContext.h"

RemoteComponentInt::RemoteComponentInt() {
	// TODO Auto-generated constructor stub

}

RemoteComponentInt::~RemoteComponentInt() {
	// TODO Auto-generated destructor stub
}

void RemoteComponentInt::setBeanContext(BeanContext cntxt)
{
	this->cntxt = cntxt;;
}
BeanContext RemoteComponentInt::getContext()
{
	return this->cntxt;
}


BeanContext::BeanContext() {

}

BeanContext::BeanContext(string host,int port) {
	this->setHost(host);
	this->setPort(port);
}

BeanContext::~BeanContext() {
	// TODO Auto-generated destructor stub
}
string BeanContext::getHost() const
{
	return host;
}

void BeanContext::setHost(string host)
{
	this->host = host;
}

int BeanContext::getPort() const
{
	return port;
}

void BeanContext::setPort(int port)
{
	this->port = port;
}

void* BeanContext::lookup(string cmpName)
{
	void *_temp = NULL;
	if(!client.isConnected())
		client.connection(host,port);
	if(client.isConnected())
	{
		Reflector ref;
		string classn;
		classn = "Component_"+cmpName+"_Remote";
		ClassInfo clas = ref.getClassInfo(classn);
		args argus;
		Constructor ctor = clas.getConstructor(argus);
		_temp = ref.newInstanceGVP(ctor);
		RemoteComponentInt* intf = (RemoteComponentInt*)_temp;
		intf->setBeanContext(*this);
	}
	return _temp;
}

void* BeanContext::invoke(string name,vector<Object> args,string bname,string rettyp)
{
	void* retval = NULL;
	if(client.isConnected())
	{
		Serialize ser;
		string argus;
		if(args.size()>0)
		{
			for (unsigned int var = 0; var < args.size(); ++var)
			{
				argus += "<argument type=\""+rettyp+"\">"+ser.serializeUnknown(args.at(var).getVoidPointer(),rettyp)+"</argument>";
			}
		}
		string call = "<service name=\""+name+"\" beanName=\""+bname+"\" lang=\"c++\" returnType=\""+rettyp+"\"><args>"+argus+"</args></service>";
		client.sendData(call);
		call = "";
		while((call=client.getData())=="")
		{
		}
		cout << call << flush;
		XmlParser parser("Parser");
		Document doc = parser.getDocument(call);
		Element message = doc.getRootElement();
		if(message.getTagName().find("<return:exception>")==string::npos)
		{
			string tag = message.getTagName();
			boost::replace_first(tag,"return:","");
			message.setTagName(tag);
			call = message.render();
			cout << call << flush;
			retval = ser.unSerializeUnknown(call,tag);
		}
		else
		{
			throw "Exception occurred";
		}
		cout << retval << flush;
	}
	else
	{
		throw "Invalid BeanContext";
	}
	return retval;
}
