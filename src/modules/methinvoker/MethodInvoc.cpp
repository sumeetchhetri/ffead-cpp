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
 * MethodInvoc.cpp
 *
 *  Created on: Jan 30, 2010
 *      Author: sumeet
 */

#include "MethodInvoc.h"

MethodInvoc::MethodInvoc() {
	startTranx = false;
	endTranx = false;
	running = false;
}

MethodInvoc::~MethodInvoc() {
	
}
MethodInvoc* MethodInvoc::instance = NULL;

void MethodInvoc::init()
{
	if(instance==NULL)
	{
		instance = new MethodInvoc();
		instance->running = false;
	}
}
void* MethodInvoc::service(void* arg)
{
	int fd = *(int*)arg;
	init();
	std::string methInfo,retValue;
	instance->server.Receive(fd,methInfo,1024);
	methInfo =methInfo.substr(0,methInfo.find_last_of(">")+1);
	try
	{
		XmlParser parser("Parser");
		if(methInfo.find("lang=\"c++\"")!=std::string::npos || methInfo.find("lang='c++'")!=std::string::npos)
		{
			Document doc;
			parser.parse(methInfo, doc);
			const Element& message = doc.getRootElement();
			if(message.getTagName()!="method")
			{
				throw MethodInvokerException("No method Tag\n",retValue);
			}
			if(message.getAttributes().size()<3)
			{
				throw MethodInvokerException("name,class and lang are mandatory attributes\n",retValue);
			}
			else if(message.getAttribute("name")=="")
			{
				throw MethodInvokerException("name attribute missing\n",retValue);
			}
			else if(message.getAttribute("className")=="")
			{
				throw MethodInvokerException("class attribute missing\n",retValue);
			}
			else if(message.getAttribute("lang")=="")
			{
				throw MethodInvokerException("lang attribute missing\n",retValue);
			}
			if(message.getChildElements().size()!=1)
			{
				throw MethodInvokerException("message tag should have only one child tag\n",retValue);
			}
			else if(message.getChildElements().at(0).getTagName()!="args")
			{
				throw MethodInvokerException("message tag should have an args child tag\n",retValue);
			}
			XMLSerialize ser;
			Reflector reflector;
			args argus;
			vals valus;
			ElementList argts = message.getChildElements().at(0).getChildElements();
			for (unsigned var = 0; var < argts.size();  var++)
			{
				void *value = NULL;
				Element* arg = argts.at(var);
				if(arg->getTagName()!="argument" || arg->getAttribute("type")=="")
					throw MethodInvokerException("every argument tag should have a name and type attribute\n",retValue);
				if(arg->getText()=="" && arg->getChildElements().size()==0)
					throw MethodInvokerException("argument value missing\n",retValue);
				if(arg->getAttribute("type")=="int")
				{
					int *vt = new int;
					*vt = CastUtil::toInt(arg->getText());
					value = vt;
				}
				else if(arg->getAttribute("type")=="long")
				{
					long *vt = new long;
					*vt = CastUtil::toLong(arg->getText());
					value = vt;
				}
				else if(arg->getAttribute("type")=="long long")
				{
					long long *vt = new long long;
					*vt = CastUtil::toLonglong(arg->getText());
					value = vt;
				}
				else if(arg->getAttribute("type")=="float")
				{
					float *vt = new float;
					*vt = CastUtil::toFloat(arg->getText());
					value = vt;
				}
				else if(arg->getAttribute("type")=="double")
				{
					double *vt = new double;
					*vt = CastUtil::toDouble(arg->getText());
					value = vt;
				}
				else if(arg->getAttribute("type")=="string")
				{
					std::string *vt = new std::string;
					*vt = arg->getText();
					value = vt;
				}
				else if(arg->getAttribute("type")!="")
				{
					Element* obj = arg->getChildElements().at(0);
					std::string objxml = obj->render();
					std::string objClassName = obj->getTagName();
					int serOpt = SerializeBase::identifySerOption(arg->getAttribute("type"));
					value = ser.unSerializeUnknown(objxml,serOpt,arg->getAttribute("type"));
				}
				argus.push_back(arg->getAttribute("type"));
				valus.push_back(value);
			}
			std::string className = message.getAttribute("className");
			std::string returnType = message.getAttribute("returnType");
			std::string lang = message.getAttribute("lang");
			ClassInfo* clas = reflector.getClassInfo(className);
			std::string methodName = message.getAttribute("name");;
			if(clas->getClassName()=="")
			{
				throw MethodInvokerException("class does not exist or is not in the library path\n",retValue);
			}
			Method meth = clas->getMethod(methodName,argus);
			if(meth.getMethodName()=="")
			{
				throw MethodInvokerException("method does not exist for the class or the class does not exist in the library path\n",retValue);
			}
			else
			{
				args argus;
				Constructor ctor = clas->getConstructor(argus);
				void *_temp = reflector.newInstanceGVP(ctor);
				if(returnType=="void" || returnType=="")
				{
					reflector.invokeMethodGVP(_temp,meth,valus,true);
					retValue = ("<return:void></return:void>");
				}
				else
				{
					if(returnType=="int")
					{
						int retv;
						reflector.invokeMethod<int>(&retv,_temp,meth,valus,true);
						retValue = ("<return:int>"+CastUtil::fromNumber(retv)+"</return:int>");
					}
					else if(returnType=="long")
					{
						long retv;
						reflector.invokeMethod<long>(&retv,_temp,meth,valus,true);
						retValue = ("<return:long>"+CastUtil::fromNumber(retv)+"</return:long>");
					}
					else if(returnType=="long long")
					{
						long long retv;
						reflector.invokeMethod<long long>(&retv,_temp,meth,valus,true);
						retValue = ("<return:longlong>"+CastUtil::fromNumber(retv)+"</return:longlong>");
					}
					else if(returnType=="float")
					{
						float retv;
						reflector.invokeMethod<float>(&retv,_temp,meth,valus,true);
						retValue = ("<return:float>"+CastUtil::fromFloat(retv)+"</return:float>");
					}
					else if(returnType=="double")
					{
						double retv;
						reflector.invokeMethod<double>(&retv,_temp,meth,valus,true);
						retValue = ("<return:double>"+CastUtil::fromDouble(retv)+"</return:double>");
					}
					else if(returnType=="string")
					{
						std::string retv;
						reflector.invokeMethod<std::string>(&retv,_temp,meth,valus,true);
						retValue = ("<return:string>"+retv+"</return:string>");
					}
					else if(returnType!="")
					{
						void* retobj = reflector.invokeMethodUnknownReturn(_temp,meth,valus,true);
						int serOpt = SerializeBase::identifySerOption(returnType);
						std::string oxml = ser.serializeUnknown(retobj,serOpt,returnType);
						retValue = ("<return:"+returnType+">"+oxml+"</return:"+returnType+">");
					}
				}
				reflector.destroy(_temp, className);
			}
		}
		else
		{
			retValue = "<return:exception>This is a C++ daemon</return:exception>";
		}
		if(retValue!="")
			instance->server.Send(fd,retValue);
		close(fd);
	}
	catch(MethodInvokerException *e)
	{
		instance->server.Send(fd,retValue);
		close(fd);
	}
	catch(const std::exception& e)
	{
		retValue = ("<return:exception>XmlParseException occurred</return:exception>");
		instance->server.Send(fd,retValue);
		close(fd);
	}
	return NULL;
}


void MethodInvoc::trigger(const std::string& port)
{
	init();
	if(instance->running)
		return;
	Server serv(port,false,500,&service,2);
	instance->server = serv;
	instance->server.start();
	instance->running = true;
	return;
}

void MethodInvoc::stop()
{
	if(instance!=NULL) {
		instance->server.stop();
		delete instance;
	}
}
