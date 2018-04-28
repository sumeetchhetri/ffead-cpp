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
#include "Reflector.h"

std::map<std::string, ClassInfo> Reflector::_ciMap;
ClassInfo Reflector::nullclass;

Reflector::Reflector()
{
	closedlib = true;
	dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		throw "Cannot load reflection shared library";
	}
	dlibinstantiated = true;
}

Reflector::Reflector(void* dlib)
{
	if(dlib == NULL)
	{
		throw "Cannot load reflection shared library";
	}
	closedlib = false;
	this->dlib = dlib;
	dlibinstantiated = false;
}

Reflector::~Reflector()
{
	if(dlibinstantiated && closedlib)
	{
		dlclose(dlib);
	}
}
void Reflector::cleanUp()
{
	for (int var=0;var<(int)objectT.size();var++)
	{
		if(objectT.at(var)=="string" || objectT.at(var)=="int" || objectT.at(var)=="long"
			|| objectT.at(var)=="double" || objectT.at(var)=="float" || objectT.at(var)=="bool"
				|| objectT.at(var)=="char" || objectT.at(var)=="long long")
			delete objects.at(var);
		else
		{
			//destroy(objects.at(var),objectT.at(var));
			delete objects.at(var);
		}
	}
	objectT.clear();
	objects.clear();
}

const ClassInfo& Reflector::getClassInfo(const std::string& cs, const std::string& app)
{
	std::string className = cs;
	std::string appName = CommonUtils::getAppName(app);
	StringUtil::replaceAll(className, "::", "_");
	std::string ca = appName +"-"+ className;
	if(_ciMap.find(ca)!=_ciMap.end()) {
		return _ciMap[ca];
	}
	std::string methodname = appName + "_"+className;
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef ClassInfo (*RfPtr) ();
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		_ciMap[ca] = f();
		return _ciMap[ca];
	}
	else
		return nullclass;
}

void Reflector::destroy(void* instance, std::string className, const std::string& app) {
	StringUtil::trim(className);
	std::string appName = CommonUtils::getAppName(app);
	if(className=="short" || className=="short int" || className=="signed short" || className=="signed short int"
			|| className=="unsigned short" || className=="unsigned short int"
			|| className=="signed" || className=="int" || className=="signed int"
			|| className=="unsigned" || className=="unsigned int" || className=="long"
			|| className=="long int" || className=="signed long" || className=="signed long int"
			|| className=="unsigned long" || className=="unsigned long int"
			|| className=="long long" || className=="long long int" || className=="signed long long"
			|| className=="signed long long int" || className=="unsigned long long"
			|| className=="unsigned long long int" || className=="long double" || className=="bool"
			|| className=="float" || className=="double" || className=="string" || className=="std::string"
			|| className=="char" || className=="signed char" || className=="unsigned char"
			|| className=="wchar_t")
	{
		if(className=="char")
		{
			delete (char*)instance;
		}
		else if(className=="unsigned char")
		{
			delete (unsigned char*)instance;
		}
		else if(className=="int")
		{
			delete (int*)instance;
		}
		else if(className=="unsigned int")
		{
			delete (unsigned int*)instance;
		}
		else if(className=="short")
		{
			delete (short*)instance;
		}
		else if(className=="unsigned short")
		{
			delete (unsigned short*)instance;
		}
		else if(className=="long")
		{
			delete (long*)instance;
		}
		else if(className=="unsigned long")
		{
			delete (unsigned long*)instance;
		}
		else if(className=="long long")
		{
			delete (long long*)instance;
		}
		else if(className=="unsigned long long")
		{
			delete (unsigned long long*)instance;
		}
		else if(className=="double")
		{
			delete (double*)instance;
		}
		else if(className=="long double")
		{
			delete (long double*)instance;
		}
		else if(className=="float")
		{
			delete (float*)instance;
		}
		else if(className=="bool")
		{
			delete (bool*)instance;
		}
		else if(className=="string" || className=="std::string")
		{
			delete (std::string*)instance;
		}
	}
	else if(className=="Date")
	{
		delete (Date*)instance;
	}
	else if(className=="BinaryData")
	{
		delete (BinaryData*)instance;
	}
	else if(className.find("std::vector<std::string,")!=std::string::npos || className.find("std::vector<std::string>")!=std::string::npos
		|| className.find("vector<std::string,")!=std::string::npos || className.find("vector<std::string>")!=std::string::npos
		|| className.find("std::vector<string,")!=std::string::npos || className.find("std::vector<string>")!=std::string::npos
		|| className.find("vector<string,")!=std::string::npos || className.find("vector<string>")!=std::string::npos)
	{
		delete (std::vector<std::string>*)instance;
	}
	else if(className.find("std::vector<char,")!=std::string::npos || className.find("std::vector<char>")!=std::string::npos
		|| className.find("vector<char,")!=std::string::npos || className.find("vector<char>")!=std::string::npos)
	{
		delete (std::vector<char>*)instance;
	}
	else if(className.find("std::vector<unsigned char,")!=std::string::npos || className.find("std::vector<unsigned char>")!=std::string::npos
		|| className.find("vector<unsigned char,")!=std::string::npos || className.find("vector<unsigned char>")!=std::string::npos)
	{
		delete (std::vector<unsigned char>*)instance;
	}
	else if(className.find("std::vector<int,")!=std::string::npos || className.find("std::vector<int>")!=std::string::npos
		|| className.find("vector<int,")!=std::string::npos || className.find("vector<int>")!=std::string::npos)
	{
		delete (std::vector<int>*)instance;
	}
	else if(className.find("std::vector<short,")!=std::string::npos || className.find("std::vector<short>")!=std::string::npos
		|| className.find("vector<short,")!=std::string::npos || className.find("vector<short>")!=std::string::npos)
	{
		delete (std::vector<short>*)instance;
	}
	else if(className.find("std::vector<long,")!=std::string::npos || className.find("std::vector<long>")!=std::string::npos
		|| className.find("vector<long,")!=std::string::npos || className.find("vector<long>")!=std::string::npos)
	{
		delete (std::vector<long>*)instance;
	}
	else if(className.find("std::vector<long long,")!=std::string::npos || className.find("std::vector<long long>")!=std::string::npos
		|| className.find("vector<long long,")!=std::string::npos || className.find("vector<long long>")!=std::string::npos)
	{
		delete (std::vector<long long>*)instance;
	}
	else if(className.find("std::vector<unsigned int,")!=std::string::npos || className.find("std::vector<unsigned int>")!=std::string::npos
		|| className.find("vector<unsigned int,")!=std::string::npos || className.find("vector<unsigned int>")!=std::string::npos)
	{
		delete (std::vector<unsigned int>*)instance;
	}
	else if(className.find("std::vector<unsigned short,")!=std::string::npos || className.find("std::vector<unsigned short>")!=std::string::npos
		|| className.find("vector<unsigned short,")!=std::string::npos || className.find("vector<unsigned short>")!=std::string::npos)
	{
		delete (std::vector<unsigned short>*)instance;
	}
	else if(className.find("std::vector<unsigned long,")!=std::string::npos || className.find("std::vector<unsigned long>")!=std::string::npos
		|| className.find("vector<unsigned long,")!=std::string::npos || className.find("vector<unsigned long>")!=std::string::npos)
	{
		delete (std::vector<unsigned long>*)instance;
	}
	else if(className.find("std::vector<unsigned long long,")!=std::string::npos || className.find("std::vector<unsigned long long>")!=std::string::npos
		|| className.find("vector<unsigned long long,")!=std::string::npos || className.find("vector<unsigned long long>")!=std::string::npos)
	{
		delete (std::vector<unsigned long long>*)instance;
	}
	else if(className.find("std::vector<double,")!=std::string::npos || className.find("std::vector<double>")!=std::string::npos
		|| className.find("vector<double,")!=std::string::npos || className.find("vector<double>")!=std::string::npos)
	{
		delete (std::vector<double>*)instance;
	}
	else if(className.find("std::vector<long double,")!=std::string::npos || className.find("std::vector<long double>")!=std::string::npos
		|| className.find("vector<long double,")!=std::string::npos || className.find("vector<long double>")!=std::string::npos)
	{
		delete (std::vector<long double>*)instance;
	}
	else if(className.find("std::vector<float,")!=std::string::npos || className.find("std::vector<float>")!=std::string::npos
		|| className.find("vector<float,")!=std::string::npos || className.find("vector<float>")!=std::string::npos)
	{
		delete (std::vector<float>*)instance;
	}
	else if(className.find("std::vector<bool,")!=std::string::npos || className.find("std::vector<bool>")!=std::string::npos
		|| className.find("vector<bool,")!=std::string::npos || className.find("vector<bool>")!=std::string::npos)
	{
		delete (std::vector<bool>*)instance;
	}
	else if(className.find("std::vector<")!=std::string::npos || className.find("vector<")!=std::string::npos)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		StringUtil::replaceFirst(className,"vector<","");
		std::string vtyp;
		if(className.find(",")!=std::string::npos)
			vtyp = className.substr(0,className.find(","));
		else
			vtyp = className.substr(0,className.find(">"));
		StringUtil::replaceAll(vtyp, "::", "_");
		std::string appName = CommonUtils::getAppName(app);
		destroyContainer(instance, vtyp, "std::vector");
	}
	else if(className.find("std::list<std::string,")!=std::string::npos || className.find("std::list<std::string>")!=std::string::npos
		|| className.find("list<std::string,")!=std::string::npos || className.find("list<std::string>")!=std::string::npos
		|| className.find("std::list<string,")!=std::string::npos || className.find("std::list<string>")!=std::string::npos
		|| className.find("list<string,")!=std::string::npos || className.find("list<string>")!=std::string::npos)
	{
		delete (std::list<std::string>*)instance;
	}
	else if(className.find("std::list<char,")!=std::string::npos || className.find("std::list<char>")!=std::string::npos
		|| className.find("list<char,")!=std::string::npos || className.find("list<char>")!=std::string::npos)
	{
		delete (std::list<char>*)instance;
	}
	else if(className.find("std::list<unsigned char,")!=std::string::npos || className.find("std::list<unsigned char>")!=std::string::npos
		|| className.find("list<unsigned char,")!=std::string::npos || className.find("list<unsigned char>")!=std::string::npos)
	{
		delete (std::list<unsigned char>*)instance;
	}
	else if(className.find("std::list<int,")!=std::string::npos || className.find("std::list<int>")!=std::string::npos
		|| className.find("list<int,")!=std::string::npos || className.find("list<int>")!=std::string::npos)
	{
		delete (std::list<int>*)instance;
	}
	else if(className.find("std::list<short,")!=std::string::npos || className.find("std::list<short>")!=std::string::npos
		|| className.find("list<short,")!=std::string::npos || className.find("list<short>")!=std::string::npos)
	{
		delete (std::list<short>*)instance;
	}
	else if(className.find("std::list<long,")!=std::string::npos || className.find("std::list<long>")!=std::string::npos
		|| className.find("list<long,")!=std::string::npos || className.find("list<long>")!=std::string::npos)
	{
		delete (std::list<long>*)instance;
	}
	else if(className.find("std::list<long long,")!=std::string::npos || className.find("std::list<long long>")!=std::string::npos
		|| className.find("list<long long,")!=std::string::npos || className.find("list<long long>")!=std::string::npos)
	{
		delete (std::list<long long>*)instance;
	}
	else if(className.find("std::list<unsigned int,")!=std::string::npos || className.find("std::list<unsigned int>")!=std::string::npos
		|| className.find("list<unsigned int,")!=std::string::npos || className.find("list<unsigned int>")!=std::string::npos)
	{
		delete (std::list<unsigned int>*)instance;
	}
	else if(className.find("std::list<unsigned short,")!=std::string::npos || className.find("std::list<unsigned short>")!=std::string::npos
		|| className.find("list<unsigned short,")!=std::string::npos || className.find("list<unsigned short>")!=std::string::npos)
	{
		delete (std::list<unsigned short>*)instance;
	}
	else if(className.find("std::list<unsigned long,")!=std::string::npos || className.find("std::list<unsigned long>")!=std::string::npos
		|| className.find("list<unsigned long,")!=std::string::npos || className.find("list<unsigned long>")!=std::string::npos)
	{
		delete (std::list<unsigned long>*)instance;
	}
	else if(className.find("std::list<unsigned long long,")!=std::string::npos || className.find("std::list<unsigned long long>")!=std::string::npos
		|| className.find("list<unsigned long long,")!=std::string::npos || className.find("list<unsigned long long>")!=std::string::npos)
	{
		delete (std::list<unsigned long long>*)instance;
	}
	else if(className.find("std::list<double,")!=std::string::npos || className.find("std::list<double>")!=std::string::npos
		|| className.find("list<double,")!=std::string::npos || className.find("list<double>")!=std::string::npos)
	{
		delete (std::list<double>*)instance;
	}
	else if(className.find("std::list<long double,")!=std::string::npos || className.find("std::list<long double>")!=std::string::npos
		|| className.find("list<long double,")!=std::string::npos || className.find("list<long double>")!=std::string::npos)
	{
		delete (std::list<long double>*)instance;
	}
	else if(className.find("std::list<float,")!=std::string::npos || className.find("std::list<float>")!=std::string::npos
		|| className.find("list<float,")!=std::string::npos || className.find("list<float>")!=std::string::npos)
	{
		delete (std::list<float>*)instance;
	}
	else if(className.find("std::list<bool,")!=std::string::npos || className.find("std::list<bool>")!=std::string::npos
		|| className.find("list<bool,")!=std::string::npos || className.find("list<bool>")!=std::string::npos)
	{
		delete (std::list<bool>*)instance;
	}
	else if(className.find("std::list<")!=std::string::npos || className.find("list<")!=std::string::npos)
	{
		StringUtil::replaceFirst(className,"std::list<","");
		StringUtil::replaceFirst(className,"list<","");
		std::string vtyp;
		if(className.find(",")!=std::string::npos)
			vtyp = className.substr(0,className.find(","));
		else
			vtyp = className.substr(0,className.find(">"));
		StringUtil::replaceAll(vtyp, "::", "_");
		std::string appName = CommonUtils::getAppName(app);
		destroyContainer(instance, vtyp, "std::list");
	}
	else if(className.find("std::set<std::string,")!=std::string::npos || className.find("std::set<std::string>")!=std::string::npos
		|| className.find("set<std::string,")!=std::string::npos || className.find("set<std::string>")!=std::string::npos
		|| className.find("std::set<string,")!=std::string::npos || className.find("std::set<string>")!=std::string::npos
		|| className.find("set<string,")!=std::string::npos || className.find("set<string>")!=std::string::npos)
	{
		delete (std::set<std::string>*)instance;
	}
	else if(className.find("std::set<char,")!=std::string::npos || className.find("std::set<char>")!=std::string::npos
		|| className.find("set<char,")!=std::string::npos || className.find("set<char>")!=std::string::npos)
	{
		delete (std::set<char>*)instance;
	}
	else if(className.find("std::set<unsigned char,")!=std::string::npos || className.find("std::set<unsigned char>")!=std::string::npos
		|| className.find("set<unsigned char,")!=std::string::npos || className.find("set<unsigned char>")!=std::string::npos)
	{
		delete (std::set<unsigned char>*)instance;
	}
	else if(className.find("std::set<int,")!=std::string::npos || className.find("std::set<int>")!=std::string::npos
		|| className.find("set<int,")!=std::string::npos || className.find("set<int>")!=std::string::npos)
	{
		delete (std::set<int>*)instance;
	}
	else if(className.find("std::set<short,")!=std::string::npos || className.find("std::set<short>")!=std::string::npos
		|| className.find("set<short,")!=std::string::npos || className.find("set<short>")!=std::string::npos)
	{
		delete (std::set<short>*)instance;
	}
	else if(className.find("std::set<long,")!=std::string::npos || className.find("std::set<long>")!=std::string::npos
		|| className.find("set<long,")!=std::string::npos || className.find("set<long>")!=std::string::npos)
	{
		delete (std::set<long>*)instance;
	}
	else if(className.find("std::set<long long,")!=std::string::npos || className.find("std::set<long long>")!=std::string::npos
		|| className.find("set<long long,")!=std::string::npos || className.find("set<long long>")!=std::string::npos)
	{
		delete (std::set<long long>*)instance;
	}
	else if(className.find("std::set<unsigned int,")!=std::string::npos || className.find("std::set<unsigned int>")!=std::string::npos
		|| className.find("set<unsigned int,")!=std::string::npos || className.find("set<unsigned int>")!=std::string::npos)
	{
		delete (std::set<unsigned int>*)instance;
	}
	else if(className.find("std::set<unsigned short,")!=std::string::npos || className.find("std::set<unsigned short>")!=std::string::npos
		|| className.find("set<unsigned short,")!=std::string::npos || className.find("set<unsigned short>")!=std::string::npos)
	{
		delete (std::set<unsigned short>*)instance;
	}
	else if(className.find("std::set<unsigned long,")!=std::string::npos || className.find("std::set<unsigned long>")!=std::string::npos
		|| className.find("set<unsigned long,")!=std::string::npos || className.find("set<unsigned long>")!=std::string::npos)
	{
		delete (std::set<unsigned long>*)instance;
	}
	else if(className.find("std::set<unsigned long long,")!=std::string::npos || className.find("std::set<unsigned long long>")!=std::string::npos
		|| className.find("set<unsigned long long,")!=std::string::npos || className.find("set<unsigned long long>")!=std::string::npos)
	{
		delete (std::set<unsigned long long>*)instance;
	}
	else if(className.find("std::set<double,")!=std::string::npos || className.find("std::set<double>")!=std::string::npos
		|| className.find("set<double,")!=std::string::npos || className.find("set<double>")!=std::string::npos)
	{
		delete (std::set<double>*)instance;
	}
	else if(className.find("std::set<long double,")!=std::string::npos || className.find("std::set<long double>")!=std::string::npos
		|| className.find("set<long double,")!=std::string::npos || className.find("set<long double>")!=std::string::npos)
	{
		delete (std::set<long double>*)instance;
	}
	else if(className.find("std::set<float,")!=std::string::npos || className.find("std::set<float>")!=std::string::npos
		|| className.find("set<float,")!=std::string::npos || className.find("set<float>")!=std::string::npos)
	{
		delete (std::set<float>*)instance;
	}
	else if(className.find("std::set<bool,")!=std::string::npos || className.find("std::set<bool>")!=std::string::npos
		|| className.find("set<bool,")!=std::string::npos || className.find("set<bool>")!=std::string::npos)
	{
		delete (std::set<bool>*)instance;
	}
	else if(className.find("std::set<")!=std::string::npos || className.find("set<")!=std::string::npos)
	{
		StringUtil::replaceFirst(className,"std::set<","");
		StringUtil::replaceFirst(className,"set<","");
		std::string vtyp;
		if(className.find(",")!=std::string::npos)
			vtyp = className.substr(0,className.find(","));
		else
			vtyp = className.substr(0,className.find(">"));
		StringUtil::replaceAll(vtyp, "::", "_");
		std::string appName = CommonUtils::getAppName(app);
		destroyContainer(instance, vtyp, "std::set");
	}
	else if(className.find("std::multiset<std::string,")!=std::string::npos || className.find("std::multiset<std::string>")!=std::string::npos
		|| className.find("multiset<std::string,")!=std::string::npos || className.find("multiset<std::string>")!=std::string::npos
		|| className.find("std::multiset<string,")!=std::string::npos || className.find("std::multiset<string>")!=std::string::npos
		|| className.find("multiset<string,")!=std::string::npos || className.find("multiset<string>")!=std::string::npos)
	{
		delete (std::multiset<std::string>*)instance;
	}
	else if(className.find("std::multiset<char,")!=std::string::npos || className.find("std::multiset<char>")!=std::string::npos
		|| className.find("multiset<char,")!=std::string::npos || className.find("multiset<char>")!=std::string::npos)
	{
		delete (std::multiset<char>*)instance;
	}
	else if(className.find("std::multiset<unsigned char,")!=std::string::npos || className.find("std::multiset<unsigned char>")!=std::string::npos
		|| className.find("multiset<unsigned char,")!=std::string::npos || className.find("multiset<unsigned char>")!=std::string::npos)
	{
		delete (std::multiset<unsigned char>*)instance;
	}
	else if(className.find("std::multiset<int,")!=std::string::npos || className.find("std::multiset<int>")!=std::string::npos
		|| className.find("multiset<int,")!=std::string::npos || className.find("multiset<int>")!=std::string::npos)
	{
		delete (std::multiset<int>*)instance;
	}
	else if(className.find("std::multiset<short,")!=std::string::npos || className.find("std::multiset<short>")!=std::string::npos
		|| className.find("multiset<short,")!=std::string::npos || className.find("multiset<short>")!=std::string::npos)
	{
		delete (std::multiset<short>*)instance;
	}
	else if(className.find("std::multiset<long,")!=std::string::npos || className.find("std::multiset<long>")!=std::string::npos
		|| className.find("multiset<long,")!=std::string::npos || className.find("multiset<long>")!=std::string::npos)
	{
		delete (std::multiset<long>*)instance;
	}
	else if(className.find("std::multiset<long long,")!=std::string::npos || className.find("std::multiset<long long>")!=std::string::npos
		|| className.find("multiset<long long,")!=std::string::npos || className.find("multiset<long long>")!=std::string::npos)
	{
		delete (std::multiset<long long>*)instance;
	}
	else if(className.find("std::multiset<unsigned int,")!=std::string::npos || className.find("std::multiset<unsigned int>")!=std::string::npos
		|| className.find("multiset<unsigned int,")!=std::string::npos || className.find("multiset<unsigned int>")!=std::string::npos)
	{
		delete (std::multiset<unsigned int>*)instance;
	}
	else if(className.find("std::multiset<unsigned short,")!=std::string::npos || className.find("std::multiset<unsigned short>")!=std::string::npos
		|| className.find("multiset<unsigned short,")!=std::string::npos || className.find("multiset<unsigned short>")!=std::string::npos)
	{
		delete (std::multiset<unsigned short>*)instance;
	}
	else if(className.find("std::multiset<unsigned long,")!=std::string::npos || className.find("std::multiset<unsigned long>")!=std::string::npos
		|| className.find("multiset<unsigned long,")!=std::string::npos || className.find("multiset<unsigned long>")!=std::string::npos)
	{
		delete (std::multiset<unsigned long>*)instance;
	}
	else if(className.find("std::multiset<unsigned long long,")!=std::string::npos || className.find("std::multiset<unsigned long long>")!=std::string::npos
		|| className.find("multiset<unsigned long long,")!=std::string::npos || className.find("multiset<unsigned long long>")!=std::string::npos)
	{
		delete (std::multiset<unsigned long long>*)instance;
	}
	else if(className.find("std::multiset<double,")!=std::string::npos || className.find("std::multiset<double>")!=std::string::npos
		|| className.find("multiset<double,")!=std::string::npos || className.find("multiset<double>")!=std::string::npos)
	{
		delete (std::multiset<double>*)instance;
	}
	else if(className.find("std::multiset<long double,")!=std::string::npos || className.find("std::multiset<long double>")!=std::string::npos
		|| className.find("multiset<long double,")!=std::string::npos || className.find("multiset<long double>")!=std::string::npos)
	{
		delete (std::multiset<long double>*)instance;
	}
	else if(className.find("std::multiset<float,")!=std::string::npos || className.find("std::multiset<float>")!=std::string::npos
		|| className.find("multiset<float,")!=std::string::npos || className.find("multiset<float>")!=std::string::npos)
	{
		delete (std::multiset<float>*)instance;
	}
	else if(className.find("std::multiset<bool,")!=std::string::npos || className.find("std::multiset<bool>")!=std::string::npos
		|| className.find("multiset<bool,")!=std::string::npos || className.find("multiset<bool>")!=std::string::npos)
	{
		delete (std::multiset<bool>*)instance;
	}
	else if(className.find("std::multiset<")!=std::string::npos || className.find("multiset<")!=std::string::npos)
	{
		StringUtil::replaceFirst(className,"std::multiset<","");
		StringUtil::replaceFirst(className,"multiset<","");
		std::string vtyp;
		if(className.find(",")!=std::string::npos)
			vtyp = className.substr(0,className.find(","));
		else
			vtyp = className.substr(0,className.find(">"));
		StringUtil::replaceAll(vtyp, "::", "_");
		std::string appName = CommonUtils::getAppName(app);
		destroyContainer(instance, vtyp, "std::multiset");
	}
	else if(className.find("std::queue<std::string,")!=std::string::npos || className.find("std::queue<std::string>")!=std::string::npos
		|| className.find("queue<std::string,")!=std::string::npos || className.find("queue<std::string>")!=std::string::npos
		|| className.find("std::queue<string,")!=std::string::npos || className.find("std::queue<string>")!=std::string::npos
		|| className.find("queue<string,")!=std::string::npos || className.find("queue<string>")!=std::string::npos)
	{
		delete (std::queue<std::string>*)instance;
	}
	else if(className.find("std::queue<char,")!=std::string::npos || className.find("std::queue<char>")!=std::string::npos
		|| className.find("queue<char,")!=std::string::npos || className.find("queue<char>")!=std::string::npos)
	{
		delete (std::queue<char>*)instance;
	}
	else if(className.find("std::queue<unsigned char,")!=std::string::npos || className.find("std::queue<unsigned char>")!=std::string::npos
		|| className.find("queue<unsigned char,")!=std::string::npos || className.find("queue<unsigned char>")!=std::string::npos)
	{
		delete (std::queue<unsigned char>*)instance;
	}
	else if(className.find("std::queue<int,")!=std::string::npos || className.find("std::queue<int>")!=std::string::npos
		|| className.find("queue<int,")!=std::string::npos || className.find("queue<int>")!=std::string::npos)
	{
		delete (std::queue<int>*)instance;
	}
	else if(className.find("std::queue<short,")!=std::string::npos || className.find("std::queue<short>")!=std::string::npos
		|| className.find("queue<short,")!=std::string::npos || className.find("queue<short>")!=std::string::npos)
	{
		delete (std::queue<short>*)instance;
	}
	else if(className.find("std::queue<long,")!=std::string::npos || className.find("std::queue<long>")!=std::string::npos
		|| className.find("queue<long,")!=std::string::npos || className.find("queue<long>")!=std::string::npos)
	{
		delete (std::queue<long>*)instance;
	}
	else if(className.find("std::queue<long long,")!=std::string::npos || className.find("std::queue<long long>")!=std::string::npos
		|| className.find("queue<long long,")!=std::string::npos || className.find("queue<long long>")!=std::string::npos)
	{
		delete (std::queue<long long>*)instance;
	}
	else if(className.find("std::queue<unsigned int,")!=std::string::npos || className.find("std::queue<unsigned int>")!=std::string::npos
		|| className.find("queue<unsigned int,")!=std::string::npos || className.find("queue<unsigned int>")!=std::string::npos)
	{
		delete (std::queue<unsigned int>*)instance;
	}
	else if(className.find("std::queue<unsigned short,")!=std::string::npos || className.find("std::queue<unsigned short>")!=std::string::npos
		|| className.find("queue<unsigned short,")!=std::string::npos || className.find("queue<unsigned short>")!=std::string::npos)
	{
		delete (std::queue<unsigned short>*)instance;
	}
	else if(className.find("std::queue<unsigned long,")!=std::string::npos || className.find("std::queue<unsigned long>")!=std::string::npos
		|| className.find("queue<unsigned long,")!=std::string::npos || className.find("queue<unsigned long>")!=std::string::npos)
	{
		delete (std::queue<unsigned long>*)instance;
	}
	else if(className.find("std::queue<unsigned long long,")!=std::string::npos || className.find("std::queue<unsigned long long>")!=std::string::npos
		|| className.find("queue<unsigned long long,")!=std::string::npos || className.find("queue<unsigned long long>")!=std::string::npos)
	{
		delete (std::queue<unsigned long long>*)instance;
	}
	else if(className.find("std::queue<double,")!=std::string::npos || className.find("std::queue<double>")!=std::string::npos
		|| className.find("queue<double,")!=std::string::npos || className.find("queue<double>")!=std::string::npos)
	{
		delete (std::queue<double>*)instance;
	}
	else if(className.find("std::queue<long double,")!=std::string::npos || className.find("std::queue<long double>")!=std::string::npos
		|| className.find("queue<long double,")!=std::string::npos || className.find("queue<long double>")!=std::string::npos)
	{
		delete (std::queue<long double>*)instance;
	}
	else if(className.find("std::queue<float,")!=std::string::npos || className.find("std::queue<float>")!=std::string::npos
		|| className.find("queue<float,")!=std::string::npos || className.find("queue<float>")!=std::string::npos)
	{
		delete (std::queue<float>*)instance;
	}
	else if(className.find("std::queue<bool,")!=std::string::npos || className.find("std::queue<bool>")!=std::string::npos
		|| className.find("queue<bool,")!=std::string::npos || className.find("queue<bool>")!=std::string::npos)
	{
		delete (std::queue<bool>*)instance;
	}
	else if(className.find("std::queue<")!=std::string::npos || className.find("queue<")!=std::string::npos)
	{
		StringUtil::replaceFirst(className,"std::queue<","");
		StringUtil::replaceFirst(className,"queue<","");
		std::string vtyp;
		if(className.find(",")!=std::string::npos)
			vtyp = className.substr(0,className.find(","));
		else
			vtyp = className.substr(0,className.find(">"));
		StringUtil::replaceAll(vtyp, "::", "_");
		std::string appName = CommonUtils::getAppName(app);
		destroyContainer(instance, vtyp, "std::queue");
	}
	else if(className.find("std::deque<std::string,")!=std::string::npos || className.find("std::deque<std::string>")!=std::string::npos
		|| className.find("deque<std::string,")!=std::string::npos || className.find("deque<std::string>")!=std::string::npos
		|| className.find("std::deque<string,")!=std::string::npos || className.find("std::deque<string>")!=std::string::npos
		|| className.find("deque<string,")!=std::string::npos || className.find("deque<string>")!=std::string::npos)
	{
		delete (std::deque<std::string>*)instance;
	}
	else if(className.find("std::deque<char,")!=std::string::npos || className.find("std::deque<char>")!=std::string::npos
		|| className.find("deque<char,")!=std::string::npos || className.find("deque<char>")!=std::string::npos)
	{
		delete (std::deque<char>*)instance;
	}
	else if(className.find("std::deque<unsigned char,")!=std::string::npos || className.find("std::deque<unsigned char>")!=std::string::npos
		|| className.find("deque<unsigned char,")!=std::string::npos || className.find("deque<unsigned char>")!=std::string::npos)
	{
		delete (std::deque<unsigned char>*)instance;
	}
	else if(className.find("std::deque<int,")!=std::string::npos || className.find("std::deque<int>")!=std::string::npos
		|| className.find("deque<int,")!=std::string::npos || className.find("deque<int>")!=std::string::npos)
	{
		delete (std::deque<int>*)instance;
	}
	else if(className.find("std::deque<short,")!=std::string::npos || className.find("std::deque<short>")!=std::string::npos
		|| className.find("deque<short,")!=std::string::npos || className.find("deque<short>")!=std::string::npos)
	{
		delete (std::deque<short>*)instance;
	}
	else if(className.find("std::deque<long,")!=std::string::npos || className.find("std::deque<long>")!=std::string::npos
		|| className.find("deque<long,")!=std::string::npos || className.find("deque<long>")!=std::string::npos)
	{
		delete (std::deque<long>*)instance;
	}
	else if(className.find("std::deque<long long,")!=std::string::npos || className.find("std::deque<long long>")!=std::string::npos
		|| className.find("deque<long long,")!=std::string::npos || className.find("deque<long long>")!=std::string::npos)
	{
		delete (std::deque<long long>*)instance;
	}
	else if(className.find("std::deque<unsigned int,")!=std::string::npos || className.find("std::deque<unsigned int>")!=std::string::npos
		|| className.find("deque<unsigned int,")!=std::string::npos || className.find("deque<unsigned int>")!=std::string::npos)
	{
		delete (std::deque<unsigned int>*)instance;
	}
	else if(className.find("std::deque<unsigned short,")!=std::string::npos || className.find("std::deque<unsigned short>")!=std::string::npos
		|| className.find("deque<unsigned short,")!=std::string::npos || className.find("deque<unsigned short>")!=std::string::npos)
	{
		delete (std::deque<unsigned short>*)instance;
	}
	else if(className.find("std::deque<unsigned long,")!=std::string::npos || className.find("std::deque<unsigned long>")!=std::string::npos
		|| className.find("deque<unsigned long,")!=std::string::npos || className.find("deque<unsigned long>")!=std::string::npos)
	{
		delete (std::deque<unsigned long>*)instance;
	}
	else if(className.find("std::deque<unsigned long long,")!=std::string::npos || className.find("std::deque<unsigned long long>")!=std::string::npos
		|| className.find("deque<unsigned long long,")!=std::string::npos || className.find("deque<unsigned long long>")!=std::string::npos)
	{
		delete (std::deque<unsigned long long>*)instance;
	}
	else if(className.find("std::deque<double,")!=std::string::npos || className.find("std::deque<double>")!=std::string::npos
		|| className.find("deque<double,")!=std::string::npos || className.find("deque<double>")!=std::string::npos)
	{
		delete (std::deque<double>*)instance;
	}
	else if(className.find("std::deque<long double,")!=std::string::npos || className.find("std::deque<long double>")!=std::string::npos
		|| className.find("deque<long double,")!=std::string::npos || className.find("deque<long double>")!=std::string::npos)
	{
		delete (std::deque<long double>*)instance;
	}
	else if(className.find("std::deque<float,")!=std::string::npos || className.find("std::deque<float>")!=std::string::npos
		|| className.find("deque<float,")!=std::string::npos || className.find("deque<float>")!=std::string::npos)
	{
		delete (std::deque<float>*)instance;
	}
	else if(className.find("std::deque<bool,")!=std::string::npos || className.find("std::deque<bool>")!=std::string::npos
		|| className.find("deque<bool,")!=std::string::npos || className.find("deque<bool>")!=std::string::npos)
	{
		delete (std::deque<bool>*)instance;
	}
	else if(className.find("std::deque<")!=std::string::npos || className.find("deque<")!=std::string::npos)
	{
		StringUtil::replaceFirst(className,"std::deque<","");
		StringUtil::replaceFirst(className,"deque<","");
		std::string vtyp;
		if(className.find(",")!=std::string::npos)
			vtyp = className.substr(0,className.find(","));
		else
			vtyp = className.substr(0,className.find(">"));
		StringUtil::replaceAll(vtyp, "::", "_");
		std::string appName = CommonUtils::getAppName(app);
		destroyContainer(instance, vtyp, "std::deque");
	}
	else
	{
		ClassInfo ci = getClassInfo(className, app);
		void *mkr = dlsym(dlib, ci.getDestRefName().c_str());
		typedef void (*RfPtr) (void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			f(instance);
		}
		delete instance;
	}
}
