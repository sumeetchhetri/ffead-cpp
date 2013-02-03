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
	// TODO Auto-generated constructor stub

}

MethodInvoc::~MethodInvoc() {
	// TODO Auto-generated destructor stub
}
MethodInvoc* _methinv_instance = NULL;

void MethodInvoc::init()
{
	if(_methinv_instance==NULL)
	{
		_methinv_instance = new MethodInvoc();
		_methinv_instance->running = false;
	}
}
void* MethodInvoc::service(void* arg)
{
	int fd = *(int*)arg;
	init();
	string methInfo,retValue;
	_methinv_instance->getServer()->Receive(fd,methInfo,1024);
	methInfo =methInfo.substr(0,methInfo.find_last_of(">")+1);
	try
	{
		XmlParser parser("Parser");
		if(methInfo.find("lang=\"c++\"")!=string::npos || methInfo.find("lang='c++'")!=string::npos)
		{
			Document doc = parser.getDocument(methInfo);
			Element message = doc.getRootElement();
			if(message.getTagName()!="method")
			{
				throw new MethodInvokerException("No method Tag\n",retValue);
			}
			if(message.getAttributes().size()<3)
			{
				throw new MethodInvokerException("name,class and lang are mandatory attributes\n",retValue);
			}
			else if(message.getAttribute("name")=="")
			{
				throw new MethodInvokerException("name attribute missing\n",retValue);
			}
			else if(message.getAttribute("className")=="")
			{
				throw new MethodInvokerException("class attribute missing\n",retValue);
			}
			else if(message.getAttribute("lang")=="")
			{
				throw new MethodInvokerException("lang attribute missing\n",retValue);
			}
			if(message.getChildElements().size()!=1)
			{
				throw new MethodInvokerException("message tag should have only one child tag\n",retValue);
			}
			else if(message.getChildElements().at(0).getTagName()!="args")
			{
				throw new MethodInvokerException("message tag should have an args child tag\n",retValue);
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
				if(arg.getTagName()!="argument" || arg.getAttribute("type")=="")
					throw new MethodInvokerException("every argument tag should have a name and type attribute\n",retValue);
				if(arg.getText()=="" && arg.getChildElements().size()==0)
					throw new MethodInvokerException("argument value missing\n",retValue);
				if(arg.getAttribute("type")=="int")
				{
					int *vt = new int;
					*vt = CastUtil::lexical_cast<int>(arg.getText());
					value = vt;
				}
				else if(arg.getAttribute("type")=="float")
				{
					float *vt = new float;
					*vt = CastUtil::lexical_cast<float>(arg.getText());
					value = vt;
				}
				else if(arg.getAttribute("type")=="double")
				{
					double *vt = new double;
					*vt = CastUtil::lexical_cast<double>(arg.getText());
					value = vt;
				}
				else if(arg.getAttribute("type")=="string")
				{
					string *vt = new string;
					*vt = CastUtil::lexical_cast<string>(arg.getText());
					value = vt;
				}
				else if(arg.getAttribute("type")!="")
				{
					Element obj = arg.getChildElements().at(0);
					string objxml = obj.render();
					string objClassName = obj.getTagName();
					value = ser.unSerializeUnknown(objxml,arg.getAttribute("type"));
				}
				argus.push_back(arg.getAttribute("type"));
				valus.push_back(value);
			}
			string className = message.getAttribute("className");
			string returnType = message.getAttribute("returnType");
			string lang = message.getAttribute("lang");
			ClassInfo clas = reflector.getClassInfo(className);
			string methodName = message.getAttribute("name");;
			if(clas.getClassName()=="")
			{
				throw new MethodInvokerException("class does not exist or is not in the library path\n",retValue);
			}
			Method meth = clas.getMethod(methodName,argus);
			if(meth.getMethodName()=="")
			{
				throw new MethodInvokerException("method does not exist for the class or the class does not exist in the library path\n",retValue);
			}
			else
			{
				args argus;
				Constructor ctor = clas.getConstructor(argus);
				void *_temp = reflector.newInstanceGVP(ctor);
				if(returnType=="void" || returnType=="")
				{
					reflector.invokeMethod<void*>(_temp,meth,valus);
					retValue = ("<return:void></return:void>");
				}
				else
				{
					if(returnType=="int")
					{
						int retv = reflector.invokeMethod<int>(_temp,meth,valus);
						retValue = ("<return:int>"+CastUtil::lexical_cast<string>(retv)+"</return:int>");
					}
					else if(returnType=="float")
					{
						float retv = reflector.invokeMethod<float>(_temp,meth,valus);
						retValue = ("<return:float>"+CastUtil::lexical_cast<string>(retv)+"</return:float>");
					}
					else if(returnType=="double")
					{
						double retv = reflector.invokeMethod<double>(_temp,meth,valus);
						retValue = ("<return:double>"+CastUtil::lexical_cast<string>(retv)+"</return:double>");
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
		if(retValue!="")
			_methinv_instance->getServer()->Send(fd,retValue);
		close(fd);
	}
	catch(MethodInvokerException *e)
	{
		_methinv_instance->getServer()->Send(fd,retValue);
		close(fd);
	}
	catch(...)
	{
		retValue = ("<return:exception>XmlParseException occurred</return:exception>");
		_methinv_instance->getServer()->Send(fd,retValue);
		close(fd);
	}
}


void MethodInvoc::trigger(string port)
{
	init();
	if(_methinv_instance->running)
		return;
	_methinv_instance->setServer(new Server(port,false,500,&service,Constants::SYS_FORK_AVAIL));
	_methinv_instance->running = true;
	return;
}
