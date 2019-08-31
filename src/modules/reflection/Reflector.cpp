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

cuckoohash_map<std::string, ClassInfo*> Reflector::_ciMap;
ClassInfo Reflector::nullclass;

Reflector::Reflector()
{
	dlib = NULL;
}

Reflector::Reflector(void* dlib)
{
	if(dlib == NULL)
	{
		throw std::runtime_error("Cannot load reflection shared library");
	}
	this->dlib = dlib;
}

Reflector::~Reflector()
{
	if(_ciMap.size()>0) {
		auto lt = _ciMap.lock_table();
		cuckoohash_map<std::string, ClassInfo*>::locked_table::iterator it;
		for(it=lt.begin();it!=lt.end();++it) {
			delete it->second;
		}
	}
}
void Reflector::cleanUp()
{
}

ClassInfo* Reflector::getClassInfo(const std::string& cs, const std::string& app)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string ca = appName +"_"+ cs;
	if(cs.find("::")!=std::string::npos) {
		StringUtil::replaceAll(ca, "::", "_");
	}
	if(_ciMap.contains(ca)) {
		return _ciMap.find(ca);
	}
	void *mkr = dlsym(dlib, ca.c_str());
	typedef ClassInfo (*RfPtr) ();
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		ClassInfo* t = new ClassInfo();
		*t = f();
		_ciMap.insert(ca, t);
		return t;
	}
	else
		return &nullclass;
}

void Reflector::destroy(void* instance, std::string className, const std::string& app) {
	int serOpt = 0;
	StringUtil::trim(className);
	if(className=="void")delete instance; return;
	if(className.find(",")!=std::string::npos)
	{
		className = className.substr(0, className.find(",")+1);
	}
	int level = StringUtil::countOccurrences(className, "<");
	if(className=="std::string" || className=="string")serOpt = 1;
	else if(className=="char")serOpt = 2;
	else if(className=="unsigned char")serOpt = 3;
	else if(className=="int")serOpt = 4;
	else if(className=="unsigned int")serOpt = 5;
	else if(className=="short")serOpt = 6;
	else if(className=="unsigned short")serOpt = 7;
	else if(className=="long")serOpt = 8;
	else if(className=="unsigned long")serOpt = 9;
	else if(className=="long long")serOpt = 10;
	else if(className=="unsigned long long")serOpt = 11;
	else if(className=="float")serOpt = 12;
	else if(className=="double")serOpt = 13;
	else if(className=="long double")serOpt = 14;
	else if(className=="bool")serOpt = 15;
	else if(className=="Date")serOpt = 16;
	else if(className=="BinaryData")serOpt = 17;
	else if(level>1)
	{
		serOpt = 18;
	}
	else if(className=="std::ifstream")serOpt = 19;
	else if(className.find("std::vector<")!=std::string::npos || className.find("vector<")!=std::string::npos ||
			className.find("std::list<")!=std::string::npos || className.find("list<")!=std::string::npos ||
			className.find("std::set<")!=std::string::npos || className.find("set<")!=std::string::npos ||
			className.find("std::multiset<")!=std::string::npos || className.find("multiset<")!=std::string::npos ||
			className.find("std::queue<")!=std::string::npos || className.find("queue<")!=std::string::npos ||
			className.find("std::deque<")!=std::string::npos || className.find("deque<")!=std::string::npos)
	{
		std::string cc = className.substr(0, className.find("<"));
		if(cc.find("std::")==0) cc = cc.substr(5);

		std::string ic = className.substr(className.find("<")+1);
		if(ic.find(",")!=std::string::npos) ic = ic.substr(0, ic.find(","));
		else ic = ic.substr(0, ic.find(">"));

		int icsO = 0;
		if(ic=="std::string" || ic=="string")icsO = 1;
		else if(ic=="char")icsO = 2;
		else if(ic=="unsigned char")icsO = 3;
		else if(ic=="int")icsO = 4;
		else if(ic=="unsigned int")icsO = 5;
		else if(ic=="short")icsO = 6;
		else if(ic=="unsigned short")icsO = 7;
		else if(ic=="long")icsO = 8;
		else if(ic=="unsigned long")icsO = 9;
		else if(ic=="long long")icsO = 10;
		else if(ic=="unsigned long long")icsO = 11;
		else if(ic=="float")icsO = 12;
		else if(ic=="double")icsO = 13;
		else if(ic=="long double")icsO = 14;
		else if(ic=="bool")icsO = 15;
		else if(ic=="Date")icsO = 16;
		else if(ic=="BinaryData")icsO = 17;
		else if(ic=="std::ifstream")icsO = 19;

		if(cc=="vector") serOpt = 100 + icsO;
		else if(cc=="list") serOpt = 200 + icsO;
		else if(cc=="set") serOpt = 300 + icsO;
		else if(cc=="multiset") serOpt = 400 + icsO;
		else if(cc=="queue") serOpt = 500 + icsO;
		else if(cc=="deque") serOpt = 600 + icsO;
	}
	destroy(serOpt, instance, className, app);
}

void Reflector::destroy(int serOpt, void* instance, std::string className, const std::string& app) {
	std::string appName = CommonUtils::getAppName(app);
	switch(serOpt) {
		case 0: {
			ClassInfo* ci = getClassInfo(className, app);
			if(ci->getDestRefName()!="") {
				void *mkr = dlsym(dlib, ci->getDestRefName().c_str());
				typedef void (*RfPtr) (void*);
				RfPtr f = (RfPtr)mkr;
				if(f!=NULL)
				{
					f(instance);
				}
			}
			delete instance;
			break;
		}
		case 1: delete (std::string*)instance; break;
		case 2: delete (char*)instance; break;
		case 3: delete (unsigned char*)instance; break;
		case 4: delete (int*)instance; break;
		case 5: delete (unsigned int*)instance; break;
		case 6: delete (short*)instance; break;
		case 7: delete (unsigned short*)instance; break;
		case 8: delete (long*)instance; break;
		case 9: delete (unsigned long*)instance; break;
		case 10: delete (long long*)instance; break;
		case 11: delete (unsigned long long*)instance; break;
		case 12: delete (float*)instance; break;
		case 13: delete (double*)instance; break;
		case 14: delete (long double*)instance; break;
		case 15: delete (bool*)instance; break;
		case 16: delete (Date*)instance; break;
		case 17: delete (BinaryData*)instance; break;
		case 18: delete instance; break;

		case 100: {
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
			break;
		}
		case 101: destroyNestedContainer<std::string>("std::vector", instance); break;
		case 102: destroyNestedContainer<char>("std::vector", instance); break;
		case 103: destroyNestedContainer<unsigned char>("std::vector", instance); break;
		case 104: destroyNestedContainer<int>("std::vector", instance); break;
		case 105: destroyNestedContainer<unsigned int>("std::vector", instance); break;
		case 106: destroyNestedContainer<short>("std::vector", instance); break;
		case 107: destroyNestedContainer<unsigned short>("std::vector", instance); break;
		case 108: destroyNestedContainer<long>("std::vector", instance); break;
		case 109: destroyNestedContainer<unsigned long>("std::vector", instance); break;
		case 110: destroyNestedContainer<long long>("std::vector", instance); break;
		case 111: destroyNestedContainer<unsigned long long>("std::vector", instance); break;
		case 112: destroyNestedContainer<float>("std::vector", instance); break;
		case 113: destroyNestedContainer<double>("std::vector", instance); break;
		case 114: destroyNestedContainer<long double>("std::vector", instance); break;
		case 115: destroyNestedContainer<bool>("std::vector", instance); break;
		case 116: destroyNestedContainer<Date>("std::vector", instance); break;

		case 200: {
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
			break;
		}
		case 201: destroyNestedContainer<std::string>("std::list", instance); break;
		case 202: destroyNestedContainer<char>("std::list", instance); break;
		case 203: destroyNestedContainer<unsigned char>("std::list", instance); break;
		case 204: destroyNestedContainer<int>("std::list", instance); break;
		case 205: destroyNestedContainer<unsigned int>("std::list", instance); break;
		case 206: destroyNestedContainer<short>("std::list", instance); break;
		case 207: destroyNestedContainer<unsigned short>("std::list", instance); break;
		case 208: destroyNestedContainer<long>("std::list", instance); break;
		case 209: destroyNestedContainer<unsigned long>("std::list", instance); break;
		case 210: destroyNestedContainer<long long>("std::list", instance); break;
		case 211: destroyNestedContainer<unsigned long long>("std::list", instance); break;
		case 212: destroyNestedContainer<float>("std::list", instance); break;
		case 213: destroyNestedContainer<double>("std::list", instance); break;
		case 214: destroyNestedContainer<long double>("std::list", instance); break;
		case 215: destroyNestedContainer<bool>("std::list", instance); break;
		case 216: destroyNestedContainer<Date>("std::list", instance); break;

		case 300: {
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
			break;
		}
		case 301: destroyNestedContainerSV<std::string>("std::set", instance); break;
		case 302: destroyNestedContainerSV<char>("std::set", instance); break;
		case 303: destroyNestedContainerSV<unsigned char>("std::set", instance); break;
		case 304: destroyNestedContainerSV<int>("std::set", instance); break;
		case 305: destroyNestedContainerSV<unsigned int>("std::set", instance); break;
		case 306: destroyNestedContainerSV<short>("std::set", instance); break;
		case 307: destroyNestedContainerSV<unsigned short>("std::set", instance); break;
		case 308: destroyNestedContainerSV<long>("std::set", instance); break;
		case 309: destroyNestedContainerSV<unsigned long>("std::set", instance); break;
		case 310: destroyNestedContainerSV<long long>("std::set", instance); break;
		case 311: destroyNestedContainerSV<unsigned long long>("std::set", instance); break;
		case 312: destroyNestedContainerSV<float>("std::set", instance); break;
		case 313: destroyNestedContainerSV<double>("std::set", instance); break;
		case 314: destroyNestedContainerSV<long double>("std::set", instance); break;
		case 315: destroyNestedContainerSV<bool>("std::set", instance); break;
		case 316: destroyNestedContainerSV<Date>("std::set", instance); break;

		case 400: {
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
			break;
		}
		case 401: destroyNestedContainerSV<std::string>("std::multiset", instance); break;
		case 402: destroyNestedContainerSV<char>("std::multiset", instance); break;
		case 403: destroyNestedContainerSV<unsigned char>("std::multiset", instance); break;
		case 404: destroyNestedContainerSV<int>("std::multiset", instance); break;
		case 405: destroyNestedContainerSV<unsigned int>("std::multiset", instance); break;
		case 406: destroyNestedContainerSV<short>("std::multiset", instance); break;
		case 407: destroyNestedContainerSV<unsigned short>("std::multiset", instance); break;
		case 408: destroyNestedContainerSV<long>("std::multiset", instance); break;
		case 409: destroyNestedContainerSV<unsigned long>("std::multiset", instance); break;
		case 410: destroyNestedContainerSV<long long>("std::multiset", instance); break;
		case 411: destroyNestedContainerSV<unsigned long long>("std::multiset", instance); break;
		case 412: destroyNestedContainerSV<float>("std::multiset", instance); break;
		case 413: destroyNestedContainerSV<double>("std::multiset", instance); break;
		case 414: destroyNestedContainerSV<long double>("std::multiset", instance); break;
		case 415: destroyNestedContainerSV<bool>("std::multiset", instance); break;
		case 416: destroyNestedContainerSV<Date>("std::multiset", instance); break;

		case 500: {
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
			break;
		}
		case 501: destroyNestedContainer<std::string>("std::queue", instance); break;
		case 502: destroyNestedContainer<char>("std::queue", instance); break;
		case 503: destroyNestedContainer<unsigned char>("std::queue", instance); break;
		case 504: destroyNestedContainer<int>("std::queue", instance); break;
		case 505: destroyNestedContainer<unsigned int>("std::queue", instance); break;
		case 506: destroyNestedContainer<short>("std::queue", instance); break;
		case 507: destroyNestedContainer<unsigned short>("std::queue", instance); break;
		case 508: destroyNestedContainer<long>("std::queue", instance); break;
		case 509: destroyNestedContainer<unsigned long>("std::queue", instance); break;
		case 510: destroyNestedContainer<long long>("std::queue", instance); break;
		case 511: destroyNestedContainer<unsigned long long>("std::queue", instance); break;
		case 512: destroyNestedContainer<float>("std::queue", instance); break;
		case 513: destroyNestedContainer<double>("std::queue", instance); break;
		case 514: destroyNestedContainer<long double>("std::queue", instance); break;
		case 515: destroyNestedContainer<bool>("std::queue", instance); break;
		case 516: destroyNestedContainer<Date>("std::queue", instance); break;

		case 600: {
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
			break;
		}
		case 601: destroyNestedContainer<std::string>("std::deque", instance); break;
		case 602: destroyNestedContainer<char>("std::deque", instance); break;
		case 603: destroyNestedContainer<unsigned char>("std::deque", instance); break;
		case 604: destroyNestedContainer<int>("std::deque", instance); break;
		case 605: destroyNestedContainer<unsigned int>("std::deque", instance); break;
		case 606: destroyNestedContainer<short>("std::deque", instance); break;
		case 607: destroyNestedContainer<unsigned short>("std::deque", instance); break;
		case 608: destroyNestedContainer<long>("std::deque", instance); break;
		case 609: destroyNestedContainer<unsigned long>("std::deque", instance); break;
		case 610: destroyNestedContainer<long long>("std::deque", instance); break;
		case 611: destroyNestedContainer<unsigned long long>("std::deque", instance); break;
		case 612: destroyNestedContainer<float>("std::deque", instance); break;
		case 613: destroyNestedContainer<double>("std::deque", instance); break;
		case 614: destroyNestedContainer<long double>("std::deque", instance); break;
		case 615: destroyNestedContainer<bool>("std::deque", instance); break;
		case 616: destroyNestedContainer<Date>("std::deque", instance); break;
	}
}
