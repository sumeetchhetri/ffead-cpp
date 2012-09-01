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
 * FFEADContext.cpp
 *
 *  Created on: Oct 17, 2010
 *      Author: root
 */

#include "FFEADContext.h"

FFEADContext::FFEADContext(string depFile)
{
	XmlParser parser("Parser");
	Element root = parser.getDocument(depFile).getRootElement();
	ElementList eles = root.getChildElements();
	vector<string> beanchk;
	if(eles.size()>0 && root.getTagName()=="beans")
	{
		for (unsigned int var = 0; var < eles.size(); var++)
		{
			Element ele = eles.at(var);
			if(ele.getTagName()=="bean")
			{
				Bean bean;
				bean.name = ele.getAttribute("name");
				bean.value = ele.getAttribute("value");
				bean.inbuilt = ele.getAttribute("inbuilt");
				if(ele.getAttribute("bean")!="")
					throw "Invalid attribute";
				bean.bean = ele.getAttribute("bean");
				bean.clas = ele.getAttribute("class");
				bean.intfType = ele.getAttribute("intfType");
				bean.injectAs = ele.getAttribute("injectAs");
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
								beanchk.push_back(ele1.getAttribute("bean"));
								bean.injs.push_back(ele1.getAttribute("bean"));
								if(ele1.getAttribute("name")!="")
									bean.names.push_back(ele1.getAttribute("name"));
								else
									bean.names.push_back(ele1.getAttribute("bean"));
								bean.types.push_back(ele1.getAttribute("intfType"));
							}
							else
							{
								Bean beanc;
								beanc.name = ele1.getAttribute("name");
								beanc.value = ele1.getAttribute("value");
								beanc.inbuilt = ele1.getAttribute("inbuilt");
								beanc.clas = ele1.getAttribute("class");
								beanc.intfType = ele1.getAttribute("intfType");
								beanc.realbean = false;
								injbns[beanc.name] = beanc;
								bean.injs.push_back(beanc.name);
								bean.names.push_back(beanc.name);
							}
						}
						else
						{
							throw "Invalid tag";
						}
					}
				}
				beans[bean.name] = bean;
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
	void *_temp = NULL;
	string type;
	if(bean.inbuilt!="" && bean.value!="")
	{
		type = bean.inbuilt;
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
	else if(bean.inbuilt!="" && bean.value=="")
	{
		throw "Invalid value for inbuilt type";
	}
	else if(bean.injectAs=="" || bean.injs.size()==0)
	{
		Reflector reflector;
		args argus;
		ClassInfo clas = reflector.getClassInfo(bean.clas);
		Constructor ctor = clas.getConstructor(argus);
		_temp = reflector.newInstanceGVP(ctor);
		type = bean.clas;
	}
	else if(bean.injectAs=="prop")
	{
		type = bean.clas;
		Reflector reflector;
		args argus;
		vals valus;
		ClassInfo clas = reflector.getClassInfo(bean.clas);
		Constructor ctor = clas.getConstructor(argus);
		_temp = reflector.newInstanceGVP(ctor);
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
			reflector.invokeMethod<void*>(_temp,meth,valus);
			valus.clear();
			argus.clear();
		}
	}
	else if(bean.injectAs=="cons")
	{
		type = bean.clas;
		Reflector reflector;
		args argus;
		vals valus;
		ClassInfo clas = reflector.getClassInfo(bean.clas);
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
		_temp = reflector.newInstanceGVP(ctor,valus);
	}
	else if(bean.injectAs=="intf")
	{
		type = bean.clas;
		Reflector reflector;
		args argus;
		vals valus;
		ClassInfo clas = reflector.getClassInfo(bean.clas);
		Constructor ctor = clas.getConstructor(argus);
		_temp = reflector.newInstanceGVP(ctor);
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
			reflector.invokeMethod<void*>(_temp,meth,valus);
			valus.clear();
			argus.clear();
		}
	}
	objects[type] = _temp;
	return _temp;
}

void* FFEADContext::getBean(string beanName)
{
	Bean bean = beans[beanName];
	return getBean(bean);
}


void FFEADContext::clear()
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
			reflector.destroy(objectsIter->second,objectsIter->first);
		}
	}
	cleared = true;
}
