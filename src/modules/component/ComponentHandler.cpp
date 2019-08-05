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
 * ComponentHandler.cpp
 *
 *  Created on: Mar 26, 2010
 *      Author: sumeet
 */

#include "ComponentHandler.h"

ComponentHandler* ComponentHandler::instance = NULL;

ComponentHandler::ComponentHandler()
{
	running = false;
	logger = LoggerFactory::getLogger("ComponentHandler");
}

ComponentHandler::~ComponentHandler()
{

}

void ComponentHandler::init()
{
	if(instance==NULL)
	{
		instance = new ComponentHandler();
		instance->running = false;
	}
}

void* ComponentHandler::service(void* arg)
{
	int fd = *(int*)arg;
	init();
	std::string methInfo,retValue;
	instance->server.Receive(fd,methInfo,1024);
	methInfo =methInfo.substr(0,methInfo.find_last_of(">")+1);
	instance->logger << ("Component method " +  methInfo) << std::endl;
	try
	{
		XmlParser parser("Parser");
		instance->logger << "Bean call parsed successfully" << std::endl;
		if(methInfo.find("lang=\"c++\"")!=std::string::npos || methInfo.find("lang='c++'")!=std::string::npos)
		{
			Document doc;
			parser.parse(methInfo, doc);
			const Element& message = doc.getRootElement();
			if(message.getTagName()!="service")
			{
				throw ComponentHandlerException("No service Tag\n",retValue);
			}
			if(message.getAttributes().size()<4)
			{
				throw ComponentHandlerException("name,beanName,returnType and lang are mandatory attributes\n",retValue);

			}
			else if(message.getAttribute("name")=="")
			{
				throw ComponentHandlerException("name attribute missing\n",retValue);
			}
			else if(message.getAttribute("returnType")=="")
			{
				throw ComponentHandlerException("returnType attribute missing\n",retValue);
			}
			else if(message.getAttribute("beanName")=="")
			{
				throw ComponentHandlerException("beanName attribute missing\n",retValue);
			}
			else if(message.getAttribute("lang")=="")
			{
				throw ComponentHandlerException("lang attribute missing\n",retValue);
			}
			if(message.getChildElements().size()!=1)
			{
				throw ComponentHandlerException("message tag should have only one child tag\n",retValue);
			}
			else if(message.getChildElements().at(0).getTagName()!="args")
			{
				throw ComponentHandlerException("message tag should have an args child tag\n",retValue);
			}
			XMLSerialize ser;
			Reflector reflector;
			args argus;
			vals valus;
			ElementList argts = message.getChildElements().at(0).getChildElements();
			for (unsigned var = 0; var < argts.size();  var++)
			{
				void *value = NULL;
				if(argts.at(var).getTagName()!="argument")
				{
					throw ComponentHandlerException("Invalid Tag, only argument tag allowed\n",retValue);

				}
				else if(argts.at(var).getAttribute("type")=="")
				{
					throw ComponentHandlerException("every argument tag should have a type attribute\n",retValue);

				}
				if(argts.at(var).getText()=="" && argts.at(var).getChildElements().size()==0)
				{
					throw ComponentHandlerException("argument value missing\n",retValue);

				}
				if(argts.at(var).getAttribute("type")!="")
				{
					std::string objxml = argts.at(var).getChildElements().at(0).render(); // @suppress("Invalid arguments")
					std::string objClassName = argts.at(var).getChildElements().at(0).getTagName();
					value = ser.unSerializeUnknown(objxml, argts.at(var).getAttribute("type"));
				}
				argus.push_back(argts.at(var).getAttribute("type"));
				valus.push_back(value);
			}
			std::string className = "Component_"+message.getAttribute("beanName");
			instance->logger << ("Bean class = " + className) << std::endl;
			std::string returnType = message.getAttribute("returnType");
			std::string lang = message.getAttribute("lang");
			ClassInfo* clas = reflector.getClassInfo(className);
			std::string methodName = message.getAttribute("name");
			instance->logger << ("Bean service = " + methodName) << std::endl;
			if(clas->getClassName()=="")
			{
				throw ComponentHandlerException("bean does not exist or is not regsitered\n",retValue);
			}
			Method meth = clas->getMethod(methodName,argus);
			if(meth.getMethodName()=="")
			{
				throw ComponentHandlerException("service does not exist for the bean or the bean does not exist or is not regsitered\n\n",retValue);

			}
			else
			{
				instance->logger << ("Got Bean service " + methodName) << std::endl;
				args argus;
				Constructor ctor = clas->getConstructor(argus);
				void *_temp = reflector.newInstanceGVP(ctor);
				instance->logger << ("Got Bean") << std::endl;
				if(returnType=="void" || returnType=="")
				{
					instance->logger << "Void return" << std::endl;
					reflector.invokeMethod<void*>(_temp,meth,valus,true);
					retValue = ("<return:void></return:void>");
				}
				else
				{
					instance->logger << ("Return type = " + returnType) << std::endl;
					if(returnType=="int")
					{
						int retv = reflector.invokeMethod<int>(_temp,meth,valus,true);
						retValue = ("<return:int>"+CastUtil::lexical_cast<std::string>(retv)+"</return:int>");
					}
					else if(returnType=="long")
					{
						long retv = reflector.invokeMethod<long>(_temp,meth,valus,true);
						retValue = ("<return:long>"+CastUtil::lexical_cast<std::string>(retv)+"</return:long>");
					}
					else if(returnType=="long long")
					{
						long long retv = reflector.invokeMethod<long long>(_temp,meth,valus,true);
						retValue = ("<return:longlong>"+CastUtil::lexical_cast<std::string>(retv)+"</return:longlong>");
					}
					else if(returnType=="float")
					{
						float retv = reflector.invokeMethod<float>(_temp,meth,valus,true);
						retValue = ("<return:float>"+CastUtil::lexical_cast<std::string>(retv)+"</return:float>");
					}
					else if(returnType=="double")
					{
						double retv = reflector.invokeMethod<double>(_temp,meth,valus,true);
						retValue = ("<return:double>"+CastUtil::lexical_cast<std::string>(retv)+"</return:double>");
					}
					else if(returnType=="string")
					{
						std::string retv = reflector.invokeMethod<std::string>(_temp,meth,valus,true);
						retValue = ("<return:string>"+retv+"</return:string>");
					}
					else if(returnType!="")
					{
						void* retobj = reflector.invokeMethodUnknownReturn(_temp,meth,valus,true);
						std::string oxml = ser.serializeUnknown(retobj,returnType);
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
		//instance->logger << "\nSending data = "<< retValue << "\n" << std::endl;
		if(retValue!="")
			instance->server.Send(fd,retValue);
		//close(fd);
	}
	catch(const ComponentHandlerException& e)
	{
		instance->logger << e.getMessage() << std::endl;
		instance->server.Send(fd,retValue);
		close(fd);
	}
	catch(const std::exception& e)
	{
		instance->logger << "Component exception occurred" << std::endl;
		retValue = ("<return:exception>XmlParseException occurred</return:exception>");
		instance->server.Send(fd,retValue);
		close(fd);
	}
	return NULL;
}

void ComponentHandler::initComponent()
{

}

bool ComponentHandler::registerComponent(const std::string& name)
{
	init();
	if(instance->components.find(name)!=instance->components.end())
	{
		return false;
	}
	else
	{
		instance->components[name] = "";
		return true;
	}
}

bool ComponentHandler::unregisterComponent(const std::string& name)
{
	init();
	std::map<std::string,std::string>::iterator it = instance->components.find(name);
	if(it!=instance->components.end())
	{
		instance->components.erase(it);
		return true;
	}
	else
		return false;
}


void ComponentHandler::trigger(const std::string& port)
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

void ComponentHandler::stop()
{
	if(instance!=NULL) {
		instance->server.stop();
		delete instance;
	}
}
