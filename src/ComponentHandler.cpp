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
 * ComponentHandler.cpp
 *
 *  Created on: Mar 26, 2010
 *      Author: sumeet
 */

#include "ComponentHandler.h"

ComponentHandler* _cmp_instance = NULL;

ComponentHandler::ComponentHandler()
{
	logger = Logger::getLogger("ComponentHandler");
}

ComponentHandler::~ComponentHandler()
{

}

void ComponentHandler::init()
{
	if(_cmp_instance==NULL)
	{
		_cmp_instance = new ComponentHandler();
		_cmp_instance->running = false;
	}
}

void ComponentHandler::service(int fd)
{
	init();
	string methInfo,retValue;
	_cmp_instance->getServer()->Receive(fd,methInfo,1024);
	methInfo =methInfo.substr(0,methInfo.find_last_of(">")+1);
	_cmp_instance->logger << methInfo << flush;
	try
	{
		XmlParser parser("Parser");
		_cmp_instance->logger << "\nBean call parsed successfully\n" << flush;
		if(methInfo.find("lang=\"c++\"")!=string::npos || methInfo.find("lang='c++'")!=string::npos)
		{
			Document doc = parser.getDocument(methInfo);
			Element message = doc.getRootElement();
			if(message.getTagName()!="service")
			{
				throw new ComponentHandlerException("No service Tag\n",retValue);
			}
			if(message.getAttributes().size()<4)
			{
				throw new ComponentHandlerException("name,beanName,returnType and lang are mandatory attributes\n",retValue);
			}
			else if(message.getAttribute("name")=="")
			{
				throw new ComponentHandlerException("name attribute missing\n",retValue);
			}
			else if(message.getAttribute("returnType")=="")
			{
				throw new ComponentHandlerException("returnType attribute missing\n",retValue);
			}
			else if(message.getAttribute("beanName")=="")
			{
				throw new ComponentHandlerException("beanName attribute missing\n",retValue);
			}
			else if(message.getAttribute("lang")=="")
			{
				throw new ComponentHandlerException("lang attribute missing\n",retValue);
			}
			if(message.getChildElements().size()!=1)
			{
				throw new ComponentHandlerException("message tag should have only one child tag\n",retValue);
			}
			else if(message.getChildElements().at(0).getTagName()!="args")
			{
				throw new ComponentHandlerException("message tag should have an args child tag\n",retValue);
			}
			Serialize ser;
			Reflector reflector;
			args argus;
			vals valus;
			ElementList argts = message.getChildElements().at(0).getChildElements();
			for (unsigned var = 0; var < argts.size();  var++)
			{
				void *value = NULL;
				Element arg = argts.at(var);
				if(arg.getTagName()!="argument")
					throw new ComponentHandlerException("Invalid Tag, only argument tag allowed\n",retValue);
				else if(arg.getAttribute("type")=="")
					throw new ComponentHandlerException("every argument tag should have a type attribute\n",retValue);
				if(arg.getText()=="" && arg.getChildElements().size()==0)
					throw new ComponentHandlerException("argument value missing\n",retValue);
				if(arg.getAttribute("type")!="")
				{
					Element obj = arg.getChildElements().at(0);
					string objxml = obj.render();
					string objClassName = obj.getTagName();
					value = ser.unSerializeUnknown(objxml,arg.getAttribute("type"));
				}
				argus.push_back(arg.getAttribute("type"));
				valus.push_back(value);
			}
			string className = "Component_"+message.getAttribute("beanName");
			_cmp_instance->logger << "\nBean class = " << className << "\n" << flush;
			string returnType = message.getAttribute("returnType");
			string lang = message.getAttribute("lang");
			ClassInfo clas = reflector.getClassInfo(className);
			string methodName = message.getAttribute("name");
			_cmp_instance->logger << "\nBean service = " << methodName << "\n" << flush;
			if(clas.getClassName()=="")
			{
				throw new ComponentHandlerException("bean does not exist or is not regsitered\n",retValue);
			}
			Method meth = clas.getMethod(methodName,argus);
			if(meth.getMethodName()=="")
			{
				throw new ComponentHandlerException("service does not exist for the bean or the bean does not exist or is not regsitered\n\n",retValue);
			}
			else
			{
				_cmp_instance->logger << "\nGot Bean service " << methodName << "\n" << flush;
				args argus;
				Constructor ctor = clas.getConstructor(argus);
				void *_temp = reflector.newInstanceGVP(ctor);
				_cmp_instance->logger << "\nGot Bean " << _temp << "\n" << flush;
				if(returnType=="void" || returnType=="")
				{
					_cmp_instance->logger << "\nVoid return " << "\n" << flush;
					reflector.invokeMethod<void*>(_temp,meth,valus);
					retValue = ("<return:void></return:void>");
				}
				else
				{
					_cmp_instance->logger << "\nReturn type = "<< returnType << "\n" << flush;
					if(returnType=="int")
					{
						int retv = reflector.invokeMethod<int>(_temp,meth,valus);
						retValue = ("<return:int>"+boost::lexical_cast<string>(retv)+"</return:int>");
					}
					else if(returnType=="float")
					{
						float retv = reflector.invokeMethod<float>(_temp,meth,valus);
						retValue = ("<return:float>"+boost::lexical_cast<string>(retv)+"</return:float>");
					}
					else if(returnType=="double")
					{
						double retv = reflector.invokeMethod<double>(_temp,meth,valus);
						retValue = ("<return:double>"+boost::lexical_cast<string>(retv)+"</return:double>");
					}
					else if(returnType=="string")
					{
						string retv = reflector.invokeMethod<string>(_temp,meth,valus);
						retValue = ("<return:string>"+retv+"</return:string>");
					}
					else if(returnType!="")
					{
						void* retobj = reflector.invokeMethodUnknownReturn(_temp,meth,valus);
						string oxml = ser.serializeUnknown(retobj,returnType);
						retValue = ("<return:"+returnType+">"+oxml+"</return:"+returnType+">");
					}
				}

			}
		}
		else
		{
			retValue = "<return:exception>This is a C++ daemon</return:exception>";
		}
		_cmp_instance->logger << "\nSending data = "<< retValue << "\n" << flush;
		if(retValue!="")
			_cmp_instance->getServer()->Send(fd,retValue);
		//close(fd);
	}
	catch(ComponentHandlerException *e)
	{
		_cmp_instance->logger << e->getMessage() << flush;
		_cmp_instance->getServer()->Send(fd,retValue);
		close(fd);
	}
	catch(...)
	{
		_cmp_instance->logger << "exception occurred" << flush;
		retValue = ("<return:exception>XmlParseException occurred</return:exception>");
		_cmp_instance->getServer()->Send(fd,retValue);
		close(fd);
	}
}

void ComponentHandler::initComponent()
{

}

bool ComponentHandler::registerComponent(string name)
{
	init();
	if(_cmp_instance->components.find(name)!=_cmp_instance->components.end())
	{
		return false;
	}
	else
	{
		_cmp_instance->components[name] = "";
		return true;
	}
}

bool ComponentHandler::unregisterComponent(string name)
{
	init();
	map<string,string>::iterator it = _cmp_instance->components.find(name);
	if(it!=_cmp_instance->components.end())
	{
		_cmp_instance->components.erase(it);
		return true;
	}
	else
		return false;
}


void ComponentHandler::trigger(string port)
{
	init();
	if(_cmp_instance->running)
		return;
	_cmp_instance->setServer(new Server(port,false,500,&service,Constants::SYS_FORK_AVAIL));
	//_cmp_instance->getServer() = new Server(port,500,&service);
	_cmp_instance->running = true;
	return;
}

