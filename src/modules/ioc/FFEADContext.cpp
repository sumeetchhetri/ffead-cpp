/*
	Copyright 2009-2020, Sumeet Chhetri 
  
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
	cleared = false;
	reflector = NULL;
	logger = LoggerFactory::getLogger("FFEADContext");
}

FFEADContext::FFEADContext(const std::string& depFile, const std::string& appName)
{
	reflector = NULL;
	cleared = false;
	logger = LoggerFactory::getLogger("FFEADContext");
	SimpleXmlParser parser("Parser");
	Document doc;
	parser.readDocument(depFile, doc);
	const Element& root = doc.getRootElement();
	ElementList eles = root.getChildElements();
	if(eles.size()>0 && root.getTagName()=="beans")
	{
		for (unsigned int var = 0; var < eles.size(); var++)
		{
			Element* ele = &(eles.at(var));
			if(ele->getTagName()=="bean")
			{
				Bean bean;
				bean.appName = appName;
				bean.name = ele->getAttribute("name");
				bean.value = ele->getAttribute("value");
				bean.inbuilt = ele->getAttribute("inbuilt");
				if(ele->getAttribute("bean")!="")
					throw std::runtime_error("Invalid attribute");
				bean.bean = ele->getAttribute("bean");
				bean.clas = ele->getAttribute("class");
				bean.intfType = ele->getAttribute("intfType");
				bean.injectAs = ele->getAttribute("injectAs");
				bean.scope = StringUtil::toLowerCopy(ele->getAttribute("scope"));
				bean.realbean = true;
				ElementList eleeles = ele->getChildElements();
				if(eleeles.size()==0)
				{

				}
				else
				{
					for (unsigned int var1 = 0; var1 < eleeles.size(); var1++)
					{
						Element* ele1 = &(eleeles.at(var1));
						if(ele1->getTagName()=="inject")
						{
							if(ele1->getAttribute("bean")!="")
							{
								bean.injs.push_back(appName+ele1->getAttribute("bean"));
								if(ele1->getAttribute("name")!="")
									bean.names.push_back(appName+ele1->getAttribute("name"));
								else
									bean.names.push_back(appName+ele1->getAttribute("bean"));
								bean.types.push_back(ele1->getAttribute("intfType"));
							}
							else
							{
								Bean beanc;
								beanc.appName = appName;
								beanc.name = ele1->getAttribute("name");
								beanc.value = ele1->getAttribute("value");
								beanc.inbuilt = ele1->getAttribute("inbuilt");
								beanc.clas = ele1->getAttribute("class");
								beanc.intfType = ele1->getAttribute("intfType");
								bean.scope = StringUtil::toLowerCopy(ele1->getAttribute("scope"));
								beanc.realbean = false;
								injbns[beanc.appName+beanc.name] = beanc;
								bean.injs.push_back(beanc.appName+beanc.name);
								bean.names.push_back(beanc.appName+beanc.name);
							}
						}
						else
						{
							throw std::runtime_error("Invalid tag");
						}
					}
				}
				beans[bean.appName+bean.name] = bean;
			}
			else
			{
				throw std::runtime_error("Invalid tag");
			}
		}
	}
}

FFEADContext::~FFEADContext()
{
	//this->clearAllSingletonBeans();
}

void* FFEADContext::getBean(const Bean& bean)
{
	void *_temp = NULL;
	std::string type;
	if(bean.inbuilt!="" && bean.value!="")
	{
		type = bean.inbuilt;
	}
	else if(bean.inbuilt!="" && bean.value=="")
	{
		throw std::runtime_error("Invalid value for inbuilt type");
	}
	else
	{
		type = bean.clas;
	}
	std::string k = bean.name+";"+type;
	if(StringUtil::toLowerCopy(bean.scope)!="prototype")
	{
		std::string _k = bean.appName + k;
		if(objects.find(_k)!=objects.end())
		{
			return objects[_k];
		}
	}
	if(bean.inbuilt!="" && bean.value!="")
	{
		if(bean.inbuilt=="string" || bean.inbuilt=="std::string")
		{
			std::string *in = new std::string(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="int")
		{
			int *in = new int;
			*in = CastUtil::toInt(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="float")
		{
			float *in = new float;
			*in = CastUtil::toFloat(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="double")
		{
			double *in = new double;
			*in = CastUtil::toDouble(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="long")
		{
			long *in = new long;
			*in = CastUtil::toLong(bean.value);
			_temp = in;
		}
		else if(bean.inbuilt=="long long")
		{
			long long *in = new long long;
			*in = CastUtil::toLonglong(bean.value);
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
		ClassInfo* clas = reflector->getClassInfo(bean.clas, bean.appName);
		const Constructor& ctor = clas->getConstructor(argus);
		_temp = reflector->newInstanceGVP(ctor);
	}
	else if(bean.injectAs=="prop")
	{
		args argus;
		vals valus;
		ClassInfo* clas = reflector->getClassInfo(bean.clas, bean.appName);
		const Constructor& ctor = clas->getConstructor(argus);
		_temp = reflector->newInstanceGVP(ctor);
		for (unsigned int var = 0; var < bean.injs.size(); var++)
		{
			Bean& beanc = injbns[bean.injs.at(var)];
			if(beanc.name=="")
				beanc = beans[bean.injs.at(var)];
			std::string methodName("set");
			methodName += StringUtil::capitalizedCopy(bean.names.at(var));
			if(beanc.inbuilt!="")
				argus.push_back(beanc.inbuilt);
			else if(beanc.clas!="")
				argus.push_back(beanc.clas);
			else
				throw std::runtime_error("Invalid or no bean type defined");
			Method meth = clas->getMethod(methodName,argus);
			void *value = getBean(beanc);
			valus.push_back(value);
			reflector->invokeMethodGVP(_temp,meth,valus);
			valus.clear();
			argus.clear();
		}
	}
	else if(bean.injectAs=="cons")
	{
		args argus;
		vals valus;
		ClassInfo* clas = reflector->getClassInfo(bean.clas, bean.appName);
		for (unsigned int var = 0; var < bean.injs.size(); var++)
		{
			Bean& beanc = injbns[bean.injs.at(var)];
			if(beanc.name=="")
				beanc = beans[bean.injs.at(var)];
			if(beanc.inbuilt!="")
				argus.push_back(beanc.inbuilt);
			else if(beanc.clas!="")
				argus.push_back(beanc.clas);
			else
				throw std::runtime_error("Invalid or no bean type defined");
			void *value = getBean(beanc);
			valus.push_back(value);
		}
		const Constructor& ctor = clas->getConstructor(argus);
		_temp = reflector->newInstanceGVP(ctor,valus);
	}
	else if(bean.injectAs=="intf")
	{
		args argus;
		vals valus;
		ClassInfo* clas = reflector->getClassInfo(bean.clas, bean.appName);
		const Constructor& ctor = clas->getConstructor(argus);
		_temp = reflector->newInstanceGVP(ctor);
		for (unsigned int var = 0; var < bean.injs.size(); var++)
		{
			Bean& beanc = injbns[bean.injs.at(var)];
			if(beanc.name=="")
				beanc = beans[bean.injs.at(var)];
			std::string methodName("set");
			methodName += StringUtil::capitalizedCopy(bean.names.at(var));
			if(bean.types.at(var)!="")
				argus.push_back(bean.types.at(var));
			else
				throw std::runtime_error("Invalid or no bean type defined");
			Method meth = clas->getMethod(methodName,argus);
			void *value = getBean(beanc);
			valus.push_back(value);
			reflector->invokeMethodGVP(_temp,meth,valus);
			valus.clear();
			argus.clear();
		}
	}
	return _temp;
}

void* FFEADContext::getBean(const std::string& beanName, const std::string& appName)
{
	if(beanName!="" && beans.find(appName+beanName)!=beans.end())
	{
		Bean& bean = beans[appName+beanName];
		return getBean(bean);
	}
	return NULL;
}

void* FFEADContext::getBean(const std::string& beanName, std::string_view appName)
{
	std::string an = std::string(appName);
	if(beanName!="" && beans.find(an+beanName)!=beans.end())
	{
		Bean& bean = beans[an+beanName];
		return getBean(bean);
	}
	return NULL;
}

void FFEADContext::release(void* instance, const std::string& beanName, const std::string& appName)
{
	if(beanName!="" && beans.find(appName+beanName)!=beans.end())
	{
		Bean& bean = beans[appName+beanName];
		if(bean.scope=="prototype")
		{
			std::string type;
			if(bean.inbuilt!="" && bean.value!="")
			{
				type = bean.inbuilt;
			}
			else
			{
				type = bean.clas;
			}
			reflector->destroy(instance, type, appName);
		}
	}
}

void FFEADContext::release(void* instance, const std::string& beanName, std::string_view appName)
{
	std::string an = std::string(appName);
	if(beanName!="" && beans.find(an+beanName)!=beans.end())
	{
		Bean& bean = beans[an+beanName];
		if(bean.scope=="prototype")
		{
			std::string type;
			if(bean.inbuilt!="" && bean.value!="")
			{
				type = bean.inbuilt;
			}
			else
			{
				type = bean.clas;
			}
			reflector->destroy(instance, type, appName);
		}
	}
}

void FFEADContext::clear(const std::string& appName)
{
	std::map<std::string, void*>::iterator it;
	for(it=objects.begin();it!=objects.end();++it) {
		std::string k = StringUtil::replaceFirstCopy(it->first, appName, "");
		k = k.substr(k.find(";")+1);
		reflector->destroy(it->second, k, appName);
	}
}

void FFEADContext::addBean(Bean& bean)
{
	StringUtil::trim(bean.name);
	if(bean.name=="") {
#ifdef HAVE_LIBUUID
		uuid_t idt;
		uuid_generate(idt);
		std::string ids;
		for(int i=0;i<16;i++){
			ids.push_back(idt[i]);
		}
		bean.name = ids;
#elif HAVE_BSDUUIDINC
		uuid_t idt;
		uint32_t status;
		uuid_create(&idt, &status);
		std::string ids((const char *) &idt, sizeof(idt));
		bean.name = ids;

#else
		bean.name = CastUtil::fromNumber(Timer::getCurrentTime());
#endif
	}
	if(bean.name!="" && beans.find(bean.appName+bean.name)==beans.end())
		beans[bean.appName+bean.name] = bean;
}

void FFEADContext::clearAllSingletonBeans(const std::map<std::string, bool, std::less<> >& servingContexts)
{
	if(cleared)
		return;
	std::map<std::string, bool>::const_iterator it;
	for (it=servingContexts.begin();it!=servingContexts.end();it++)
	{
		clear(it->first);
	}
	cleared = true;
}

void FFEADContext::initializeAllSingletonBeans(const std::map<std::string, bool, std::less<> >& servingContexts, Reflector* reflector)
{
	this->reflector = reflector;
	std::map<std::string,Bean>::iterator beanIter;
	logger << "Initializing singleton beans..." << std::endl;
	for (beanIter=beans.begin();beanIter!=beans.end();beanIter++)
	{
		Bean& bean = beanIter->second;
		std::string type;
		if(bean.inbuilt!="" && bean.value!="")
		{
			type = bean.inbuilt;
		}
		else
		{
			type = bean.clas;
		}
		std::string _k = bean.appName + bean.name + ";" + type;
		if(servingContexts.find(bean.appName)!=servingContexts.end() && bean.scope!="prototype" && objects.find(_k)==objects.end())
		{
			std::string scappName = bean.appName;
			StringUtil::replaceAll(scappName, "-", "_");
			RegexUtil::replace(scappName, "[^a-zA-Z0-9_]+", "");
			CommonUtils::setAppName(scappName);
			void* si = getBean(bean);
			if(bean.scope!="prototype")
			{
				objects[_k] = si;
			}
			ClassInfo* clas = reflector->getClassInfo(bean.clas, bean.appName);
			clas->si = si;
			if(clas->getClassName()!="") {
				if(clas->getConstructors().size()>0) {
					ctorMap cm = clas->getConstructors();
					ctorMap::iterator ci;
					for(ci=cm.begin();ci!=cm.end();++ci) {
						void *mkr = dlsym(reflector->dlib, ci->second.getRefName().c_str());
						NewInst f = (NewInst)mkr;
						if(f!=NULL) {
							if(ci->second.getArgumentTypes().size()==0) {
								clas->f = f;
							}
							ci->second.f = f;
							clas->addConstructor(ci->second);
						}
					}
				}

				if(clas->getMethods().size()>0) {
					methMap cm = clas->getMethods();
					methMap::iterator ci;
					for(ci=cm.begin();ci!=cm.end();++ci) {
						void *mkr = dlsym(reflector->dlib, ci->second.getRefName().c_str());
						GetMeth f = (GetMeth)mkr;
						if(f!=NULL) {
							ci->second.f = f;
							clas->addMethod(ci->second);
						}
					}
				}

				if(clas->getFields().size()>0) {
					fldMap cm = clas->getFields();
					fldMap::iterator ci;
					for(ci=cm.begin();ci!=cm.end();++ci) {
						void *mkr = dlsym(reflector->dlib, ci->second.getRefName().c_str());
						GetFld f = (GetFld)mkr;
						if(f!=NULL) {
							ci->second.f = f;
							clas->addField(ci->second);
						}
					}
				}

				if(bean.isController) {
					contInsMap[bean.name+bean.appName] = clas;
				}
			}
			logger << ("Initializing Bean [appName = "+bean.appName+", name = "+bean.name+ ", class = "+bean.clas+ ", value = 0x" + StringUtil::toHEX((long long)si)) << std::endl;
		}
	}
}

Reflector* FFEADContext::getReflector()
{
	return reflector;
}

Bean::Bean(const std::string& name, const std::string& value, const std::string& type, const std::string& scope, const bool& isInbuilt, bool isController, const std::string& appName)
{
	this->isController = isController;
	this->name = name;
	this->value = value;
	if(isInbuilt)
		this->inbuilt = type;
	else
		this->clas = type;
	this->scope = StringUtil::toLowerCopy(scope);
	this->realbean = true;
	this->appName = appName;
}

Bean::Bean()
{
	this->isController = false;
	this->realbean = false;
	this->appName = "default";
}

Bean::~Bean()
{
}
