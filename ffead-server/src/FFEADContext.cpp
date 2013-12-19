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
 * FFEADContext.cpp
 *
 *  Created on: Oct 17, 2010
 *      Author: root
 */

#include "FFEADContext.h"

FFEADContext::FFEADContext()
{
	logger = LoggerFactory::getLogger("FFEADContext");
}

FFEADContext::FFEADContext(string depFile, string appName)
{
	logger = LoggerFactory::getLogger("FFEADContext");
	XmlParser parser("Parser");
	Element root = parser.getDocument(depFile).getRootElement();
	ElementList eles = root.getChildElements();
	if(eles.size()>0 && root.getTagName()=="beans")
	{
		for (unsigned int var = 0; var < eles.size(); var++)
		{
			Element ele = eles.at(var);
			if(ele.getTagName()=="bean")
			{
				Bean bean;
				bean.appName = appName;
				bean.name = ele.getAttribute("name");
				bean.value = ele.getAttribute("value");
				bean.inbuilt = ele.getAttribute("inbuilt");
				if(ele.getAttribute("bean")!="")
					throw "Invalid attribute";
				bean.bean = ele.getAttribute("bean");
				bean.clas = ele.getAttribute("class");
				bean.intfType = ele.getAttribute("intfType");
				bean.injectAs = ele.getAttribute("injectAs");
				bean.scope = ele.getAttribute("scope");
				bean.realbean = true;
				ElementList eleeles = ele.getChildElements();
				if(eleeles.size()==0)
				{

				}
				else
				{
					for (unsigned int var1 = 0; var1 < eleeles.size(); var1++)
					{
						Element ele1 = eleeles.at(var1);
						if(ele1.getTagName()=="inject")
						{
							if(ele1.getAttribute("bean")!="")
							{
								bean.injs.push_back(appName+ele1.getAttribute("bean"));
								if(ele1.getAttribute("name")!="")
									bean.names.push_back(appName+ele1.getAttribute("name"));
								else
									bean.names.push_back(appName+ele1.getAttribute("bean"));
								bean.types.push_back(ele1.getAttribute("intfType"));
							}
							else
							{
								Bean beanc;
								beanc.appName = appName;
								beanc.name = ele1.getAttribute("name");
								beanc.value = ele1.getAttribute("value");
								beanc.inbuilt = ele1.getAttribute("inbuilt");
								beanc.clas = ele1.getAttribute("class");
								beanc.intfType = ele1.getAttribute("intfType");
								bean.scope = ele1.getAttribute("scope");
								beanc.realbean = false;
								injbns[beanc.appName+beanc.name] = beanc;
								bean.injs.push_back(beanc.appName+beanc.name);
								bean.names.push_back(beanc.appName+beanc.name);
							}
						}
						else
						{
							throw "Invalid tag";
						}
					}
				}
				beans[bean.appName+bean.name] = bean;
			}
			else
			{
				throw "Invalid tag";
			}
		}
	}
}

FFEADContext::~FFEADContext()
{
	this->clear();
}

void* FFEADContext::getBean(Bean bean)
{
	Reflector reflector;
	void *_temp = NULL;
	string type;
	if(bean.inbuilt!="" && bean.value!="")
	{
		type = bean.inbuilt;
	}
	else if(bean.inbuilt!="" && bean.value=="")
	{
		throw "Invalid value for inbuilt type";
	}
	else
	{
		type = bean.clas;
	}
	if(StringUtil::toLowerCopy(bean.scope)!="prototype")
	{
		if(objects.find(bean.appName+type)!=objects.end())
		{
			return objects[bean.appName+type];
		}
	}
	if(bean.inbuilt!="" && bean.value!="")
	{
		if(bean.inbuilt=="string")
		{
			string *in = new string(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="int")
		{
			int *in = new int;
			*in = CastUtil::lexical_cast<int>(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="flaot")
		{
			float *in = new float;
			*in = CastUtil::lexical_cast<float>(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="double")
		{
			double *in = new double;
			*in = CastUtil::lexical_cast<double>(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="long")
		{
			long *in = new long;
			*in = CastUtil::lexical_cast<long>(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="bool")
		{
			bool *in = new bool;
			if(bean.value=="true")
				*in = true;
			else if(bean.value=="false")
				*in = false;
			_temp = in;
		}
		else if(bean.inbuilt=="char")
		{
			char *in = new char;
			*in = bean.value[0];
			_temp = in;
		}
	}
	else if(bean.injectAs=="" || bean.injs.size()==0)
	{
		args argus;
		ClassInfo clas = reflector.getClassInfo(bean.clas, bean.appName);
		Constructor ctor = clas.getConstructor(argus);
		_temp = reflector.newInstanceGVP(ctor, bean.appName);
	}
	else if(bean.injectAs=="prop")
	{
		args argus;
		vals valus;
		ClassInfo clas = reflector.getClassInfo(bean.clas, bean.appName);
		Constructor ctor = clas.getConstructor(argus);
		_temp = reflector.newInstanceGVP(ctor, bean.appName);
		for (unsigned int var = 0; var < bean.injs.size(); var++)
		{
			Bean beanc = injbns[bean.injs.at(var)];
			if(beanc.name=="")
				beanc = beans[bean.injs.at(var)];
			string methodName("set");
			methodName += AfcUtil::camelCased(bean.names.at(var));
			if(beanc.inbuilt!="")
				argus.push_back(beanc.inbuilt);
			else if(beanc.clas!="")
				argus.push_back(beanc.clas);
			else
				throw "Invalid or no bean type defined";
			Method meth = clas.getMethod(methodName,argus);
			void *value = getBean(beanc);
			valus.push_back(value);
			reflector.invokeMethod<void*>(_temp,meth,valus,bean.appName);
			valus.clear();
			argus.clear();
		}
	}
	else if(bean.injectAs=="cons")
	{
		args argus;
		vals valus;
		ClassInfo clas = reflector.getClassInfo(bean.clas, bean.appName);
		for (unsigned int var = 0; var < bean.injs.size(); var++)
		{
			Bean beanc = injbns[bean.injs.at(var)];
			if(beanc.name=="")
				beanc = beans[bean.injs.at(var)];
			if(beanc.inbuilt!="")
				argus.push_back(beanc.inbuilt);
			else if(beanc.clas!="")
				argus.push_back(beanc.clas);
			else
				throw "Invalid or no bean type defined";
			void *value = getBean(beanc);
			valus.push_back(value);
		}
		Constructor ctor = clas.getConstructor(argus);
		_temp = reflector.newInstanceGVP(ctor,valus, bean.appName);
	}
	else if(bean.injectAs=="intf")
	{
		args argus;
		vals valus;
		ClassInfo clas = reflector.getClassInfo(bean.clas, bean.appName);
		Constructor ctor = clas.getConstructor(argus);
		_temp = reflector.newInstanceGVP(ctor, bean.appName);
		for (unsigned int var = 0; var < bean.injs.size(); var++)
		{
			Bean beanc = injbns[bean.injs.at(var)];
			if(beanc.name=="")
				beanc = beans[bean.injs.at(var)];
			string methodName("set");
			methodName += AfcUtil::camelCased(bean.names.at(var));
			if(bean.types.at(var)!="")
				argus.push_back(bean.types.at(var));
			else
				throw "Invalid or no bean type defined";
			Method meth = clas.getMethod(methodName,argus);
			void *value = getBean(beanc);
			valus.push_back(value);
			reflector.invokeMethod<void*>(_temp,meth,valus, bean.appName);
			valus.clear();
			argus.clear();
		}
	}
	if(StringUtil::toLowerCopy(bean.scope)!="prototype")
	{
		objects[bean.appName+type] = _temp;
	}
	return _temp;
}

void* FFEADContext::getBean(string beanName, string appName)
{
	if(beanName!="" && beans.find(appName+beanName)!=beans.end())
	{
		Bean bean = beans[appName+beanName];
		return getBean(bean);
	}
	return NULL;
}


void FFEADContext::clear(string appName)
{
	if(cleared)
		return;
	map<string,void*>::iterator objectsIter;
	for (objectsIter=objects.begin();objectsIter != objects.end();objectsIter++)
	{
		if(objectsIter->first=="string" || objectsIter->first=="int" || objectsIter->first=="long"
			|| objectsIter->first=="double" || objectsIter->first=="float" || objectsIter->first=="bool"
				|| objectsIter->first=="char")
			delete objectsIter->second;
		else
		{
			Reflector reflector;
			reflector.destroy(objectsIter->second,objectsIter->first, appName);
		}
	}
	cleared = true;
}

void FFEADContext::addBean(Bean bean)
{
	if(bean.name!="" && beans.find(bean.appName+bean.name)==beans.end())
		beans[bean.appName+bean.name] = bean;
}

void FFEADContext::initializeAllSingletonBeans()
{
	map<string,Bean>::iterator beanIter;
	logger << "Initializing singleton beans..." << endl;
	for (beanIter=beans.begin();beanIter!=beans.end();beanIter++)
	{
		Bean bean = beanIter->second;
		string type;
		if(bean.inbuilt!="" && bean.value!="")
		{
			type = bean.inbuilt;
		}
		else
		{
			type = bean.clas;
		}
		if(StringUtil::toLowerCopy(bean.scope)!="prototype" && objects.find(type)==objects.end())
		{
			logger << ("Initializing Bean [appName = "+bean.appName+", name = "+bean.name+ ", class = "+bean.clas+ ", value = 0x" + StringUtil::toHEX((long long)getBean(bean))) << endl;
		}
	}
}

Bean::Bean(string name,string value,string type,string scope,bool isInbuilt,string appName)
{
	this->name = name;
	this->value = value;
	if(isInbuilt)
		this->inbuilt = type;
	else
		this->clas = type;
	this->scope = scope;
	this->realbean = true;
	this->appName = appName;
}

Bean::Bean()
{
	this->appName = "default";
}

Bean::~Bean()
{

}
