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
 * SerializeBase.cpp
 *
 *  Created on: 09-Jun-2013
 *      Author: sumeetc
 */

#include "SerializeBase.h"

libcuckoo::cuckoohash_map<std::string, Ser> SerializeBase::_serFMap;
libcuckoo::cuckoohash_map<std::string, SerCont> SerializeBase::_serCFMap;
libcuckoo::cuckoohash_map<std::string, UnSer> SerializeBase::_unserFMap;
libcuckoo::cuckoohash_map<std::string, UnSerCont> SerializeBase::_unserCFMap;
void* SerializeBase::dlib = NULL;

void SerializeBase::init(void* dl) {
	dlib = dl;
}

SerializeBase::~SerializeBase() {
}

bool SerializeBase::isPrimitiveDataType(const std::string& type)
{
	if(type=="short" || type=="short int" || type=="signed short" || type=="signed short int"
			|| type=="unsigned short" || type=="unsigned short int"
			|| type=="signed" || type=="int" || type=="signed int"
			|| type=="unsigned" || type=="unsigned int" || type=="long"
			|| type=="long int" || type=="signed long" || type=="signed long int"
			|| type=="unsigned long" || type=="unsigned long int"
			|| type=="long long" || type=="long long int" || type=="signed long long"
			|| type=="signed long long int" || type=="unsigned long long"
			|| type=="unsigned long long int" || type=="long double" || type=="bool"
			|| type=="float" || type=="double" || type=="string" || type=="std::string"
			|| type=="char" || type=="signed char" || type=="unsigned char"
			|| type=="wchar_t")
	{
		//type = getTypeName(type);
		return true;
	}
	return false;
}

std::string SerializeBase::getTypeName(const std::string& type)
{
	if(type=="short" || type=="short int" || type=="signed short" || type=="signed short int")
	{
		return "short";
	}
	else if(type=="unsigned short" || type=="unsigned short int")
	{
		return "unsigned short";
	}
	else if(type=="signed" || type=="int" || type=="signed int")
	{
		return "int";
	}
	else if(type=="unsigned" || type=="unsigned int")
	{
		return "unsigned int";
	}
	else if(type=="long" || type=="long int" || type=="signed long" || type=="signed long int")
	{
		return "long";
	}
	else if(type=="unsigned long" || type=="unsigned long int")
	{
		return "unsigned long";
	}
	else if(type=="long long" || type=="long long int" || type=="signed long long" || type=="signed long long int")
	{
		return "long long";
	}
	else if(type=="unsigned long long" || type=="unsigned long long int")
	{
		return "unsigned long long";
	}
	else if(type=="long double")
	{
		return "long double";
	}
	return type;
}

void* SerializeBase::getNestedContainer(std::string& className)
{
	std::string container;
	if(className.find("std::vector<")==0 || className.find("vector<")==0)
	{
		container = "std::vector";
		if(className.find("std::vector<")==0)
			StringUtil::replaceFirst(className,"std::vector<","");
		else
			StringUtil::replaceFirst(className,"vector<","");
		return getNewNestedContainer<DummyVec>(container);
	}
	else if(className.find("std::deque<")==0 || className.find("deque<")==0)
	{
		container = "std::deque";
		if(className.find("std::deque<")==0)
			StringUtil::replaceFirst(className,"std::deque<","");
		else
			StringUtil::replaceFirst(className,"deque<","");
		return getNewNestedContainer<DummyDeque>(container);
	}
	else if(className.find("std::list<")==0 || className.find("list<")==0)
	{
		container = "std::list";
		if(className.find("std::list<")==0)
			StringUtil::replaceFirst(className,"std::list<","");
		else
			StringUtil::replaceFirst(className,"list<","");
		return getNewNestedContainer<DummyList>(container);
	}
	else if(className.find("std::set<")==0 || className.find("set<")==0)
	{
		container = "std::set";
		if(className.find("std::set<")==0)
			StringUtil::replaceFirst(className,"std::set<","");
		else
			StringUtil::replaceFirst(className,"set<","");
		return getNewNestedContainer<DummySet>(container);
	}
	else if(className.find("std::multiset<")==0 || className.find("multiset<")==0)
	{
		container = "std::multiset";
		if(className.find("std::multiset<")==0)
			StringUtil::replaceFirst(className,"std::multiset<","");
		else
			StringUtil::replaceFirst(className,"multiset<","");
		return getNewNestedContainer<DummySet>(container);
	}
	else if(className.find("std::queue<")==0 || className.find("queue<")==0)
	{
		container = "std::queue";
		if(className.find("std::queue<")==0)
			StringUtil::replaceFirst(className,"std::queue<","");
		else
			StringUtil::replaceFirst(className,"queue<","");
		return getNewNestedContainer<DummyQueue>(container);
	}
	return NULL;
}

void SerializeBase::addToNestedContainer(void* roott, const std::string& className, const std::string& app, int& lsiz, const std::string& container, void* cont, const int& var, SerializeBase* base)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string cn = className;
	if(((container.find("std::list<")==0 || container.find("list<")==0) && (cn.find("std::list<")!=0 || cn.find("list<")!=0))
			|| container.find("std::list<")==std::string::npos || container.find("list<")==std::string::npos)
	{
		if(cn.find("std::vector<")==0 || cn.find("vector<")==0)
		{
			DummyVec vec;
			void* unservec = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			vec._M_impl = ((DummyVec*)unservec)->_M_impl;
			addValueToNestedContainer(container, vec, cont);
		}
		else if(cn.find("std::deque<")==0 || cn.find("deque<")==0)
		{
			DummyDeque deq;
			void* unserdeq = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			deq._M_impl = ((DummyDeque*)unserdeq)->_M_impl;
			addValueToNestedContainer(container, deq, cont);
		}
		else if(cn.find("std::list<")==0 || cn.find("list<")==0)
		{
			DummyList lis;
			void* unserlis = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			lis._M_impl = ((DummyList*)unserlis)->_M_impl;
			addValueToNestedContainer(container, lis, cont);
		}
		else if(cn.find("std::set<")==0 || cn.find("set<")==0)
		{
			DummySet sett;
			void* unsersett = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			sett._M_t = ((DummySet*)unsersett)->_M_t;
			addValueToNestedContainer(container, sett, cont);
		}
		else if(cn.find("std::multiset<")==0 || cn.find("multiset<")==0)
		{
			DummySet sett;
			void* unsersett = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			sett._M_t = ((DummySet*)unsersett)->_M_t;
			addValueToNestedContainer(container, sett, cont);
		}
	}
	else if((container.find("std::list<")==0 || container.find("list<")==0) && (cn.find("std::list<")==0 || cn.find("list<")==0))
	{
		DummyList lis;
		void* unserlis = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
		lis._M_impl = ((DummyList*)unserlis)->_M_impl;
		DummyList::_List_node_base* temp = lis._M_impl._M_node._M_next;
		addValueToNestedContainer(container, lis, cont);
		while(lsiz-->1)
		{
			temp = temp->_M_next;
		}
		if(temp->_M_next!=&lis._M_impl._M_node) {
			std::list<DummyList>::iterator it = ((std::list<DummyList>*)cont)->begin();
			for (int i = 0; i < var; ++i, ++it) {}
			temp->_M_next = (DummyList::_List_node_base*)&(*it);
			temp->_M_prev = (DummyList::_List_node_base*)&(*it);
		}
	}
}

int SerializeBase::identifySerOption(std::string className) {
	int serOpt = 0;
	StringUtil::trim(className);
	if(className=="void")return -1;
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
	return serOpt;
}

std::string SerializeBase::_handleAllSerialization(int serOpt, std::string className, void *t, const std::string& app, SerializeBase* base, Ser f1, SerCont f2, SerCont f3)
{
	std::string appName = CommonUtils::getAppName(app);
	switch(serOpt) {
		case 0: {
			if(f1!=NULL) return f1(t, base);
			StringUtil::replaceAll(className, "::", "_");
			return _ser(t,className, appName, base);
		}
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17: return base->serializePrimitive(serOpt, className, t);
		case 18: return handleMultiLevelSerialization(t, className, appName, base);

		case 100: {
			if(f2!=NULL) return f2(t, base, "vector");
			StringUtil::replaceFirst(className,"std::vector<","");
			StringUtil::replaceFirst(className,"vector<","");
			std::string vtyp;
			if(className.find(",")!=std::string::npos)
				vtyp = className.substr(0,className.find(","));
			else
				vtyp = className.substr(0,className.find(">"));
			processClassName(vtyp);
			return _serContainer(t, vtyp, appName, "vector", base);
		}
		case 101: return serializevec<std::string>(*(std::vector<std::string>*)t, serOpt - 100, appName, base);
		case 102: return serializevec<char>(*(std::vector<char>*)t, serOpt - 100, appName, base);
		case 103: return serializevec<unsigned char>(*(std::vector<unsigned char>*)t, serOpt - 100, appName, base);
		case 104: return serializevec<int>(*(std::vector<int>*)t, serOpt - 100, appName, base);
		case 105: return serializevec<unsigned int>(*(std::vector<unsigned int>*)t, serOpt - 100, appName, base);
		case 106: return serializevec<short>(*(std::vector<short>*)t, serOpt - 100, appName, base);
		case 107: return serializevec<unsigned short>(*(std::vector<unsigned short>*)t, serOpt - 100, appName, base);
		case 108: return serializevec<long>(*(std::vector<long>*)t, serOpt - 100, appName, base);
		case 109: return serializevec<unsigned long>(*(std::vector<unsigned long>*)t, serOpt - 100, appName, base);
		case 110: return serializevec<long long>(*(std::vector<long long>*)t, serOpt - 100, appName, base);
		case 111: return serializevec<unsigned long long>(*(std::vector<unsigned long long>*)t, serOpt - 100, appName, base);
		case 112: return serializevec<float>(*(std::vector<float>*)t, serOpt - 100, appName, base);
		case 113: return serializevec<double>(*(std::vector<double>*)t, serOpt - 100, appName, base);
		case 114: return serializevec<long double>(*(std::vector<long double>*)t, serOpt - 100, appName, base);
		case 115: {
			std::vector<bool>* bt = (std::vector<bool>*)t;
			int cnt = 0;
			int size = bt->size();
			void* object = base->getSerializableObject();
			base->startContainerSerialization(object, "bool", "std::vector");
			for(bool v: *bt)
			{
				base->addContainerSerializableElement(object, serialize<bool>(v, serOpt, appName, base));
				base->afterAddContainerSerializableElement(object, cnt++, size);
			}
			base->endContainerSerialization(object, "bool", "std::vector");
			std::string ser = base->fromSerializableObjectToString(object);
			base->cleanSerializableObject(object);
			return ser;
		}
		case 116: return serializevec<Date>(*(std::vector<Date>*)t, serOpt - 100, appName, base);

		case 200: {
			if(f2!=NULL) return f2(t, base, "list");
			StringUtil::replaceFirst(className,"std::list<","");
			StringUtil::replaceFirst(className,"list<","");
			std::string vtyp;
			if(className.find(",")!=std::string::npos)
				vtyp = className.substr(0,className.find(","));
			else
				vtyp = className.substr(0,className.find(">"));
			processClassName(vtyp);
			return _serContainer(t, vtyp, appName, "list", base);
		}
		case 201: return serializelist<std::string>(*(std::list<std::string>*)t, serOpt - 200, appName, base);
		case 202: return serializelist<char>(*(std::list<char>*)t, serOpt - 200, appName, base);
		case 203: return serializelist<unsigned char>(*(std::list<unsigned char>*)t, serOpt - 200, appName, base);
		case 204: return serializelist<int>(*(std::list<int>*)t, serOpt - 200, appName, base);
		case 205: return serializelist<unsigned int>(*(std::list<unsigned int>*)t, serOpt - 200, appName, base);
		case 206: return serializelist<short>(*(std::list<short>*)t, serOpt - 200, appName, base);
		case 207: return serializelist<unsigned short>(*(std::list<unsigned short>*)t, serOpt - 200, appName, base);
		case 208: return serializelist<long>(*(std::list<long>*)t, serOpt - 200, appName, base);
		case 209: return serializelist<unsigned long>(*(std::list<unsigned long>*)t, serOpt - 200, appName, base);
		case 210: return serializelist<long long>(*(std::list<long long>*)t, serOpt - 200, appName, base);
		case 211: return serializelist<unsigned long long>(*(std::list<unsigned long long>*)t, serOpt - 200, appName, base);
		case 212: return serializelist<float>(*(std::list<float>*)t, serOpt - 200, appName, base);
		case 213: return serializelist<double>(*(std::list<double>*)t, serOpt - 200, appName, base);
		case 214: return serializelist<long double>(*(std::list<long double>*)t, serOpt - 200, appName, base);
		case 215: {
			std::list<bool>* bt = (std::list<bool>*)t;
			int cnt = 0;
			int size = bt->size();
			void* object = base->getSerializableObject();
			base->startContainerSerialization(object, "bool", "std::list");
			for(bool v: *bt)
			{
				base->addContainerSerializableElement(object, serialize<bool>(v, serOpt, appName, base));
				base->afterAddContainerSerializableElement(object, cnt++, size);
			}
			base->endContainerSerialization(object, "bool", "std::list");
			std::string ser = base->fromSerializableObjectToString(object);
			base->cleanSerializableObject(object);
			return ser;
		}
		case 216: return serializelist<Date>(*(std::list<Date>*)t, serOpt - 200, appName, base);

		case 300: {
			if(f3!=NULL) return f3(t, base, "set");
			StringUtil::replaceFirst(className,"std::set<","");
			StringUtil::replaceFirst(className,"set<","");
			std::string vtyp;
			if(className.find(",")!=std::string::npos)
				vtyp = className.substr(0,className.find(","));
			else
				vtyp = className.substr(0,className.find(">"));
			processClassName(vtyp);
			return _serContainer(t, vtyp, appName, "set", base);
		}
		case 301: return serializeset<std::string>(*(std::set<std::string>*)t, serOpt - 300, appName, base);
		case 302: return serializeset<char>(*(std::set<char>*)t, serOpt - 300, appName, base);
		case 303: return serializeset<unsigned char>(*(std::set<unsigned char>*)t, serOpt - 300, appName, base);
		case 304: return serializeset<int>(*(std::set<int>*)t, serOpt - 300, appName, base);
		case 305: return serializeset<unsigned int>(*(std::set<unsigned int>*)t, serOpt - 300, appName, base);
		case 306: return serializeset<short>(*(std::set<short>*)t, serOpt - 300, appName, base);
		case 307: return serializeset<unsigned short>(*(std::set<unsigned short>*)t, serOpt - 300, appName, base);
		case 308: return serializeset<long>(*(std::set<long>*)t, serOpt - 300, appName, base);
		case 309: return serializeset<unsigned long>(*(std::set<unsigned long>*)t, serOpt - 300, appName, base);
		case 310: return serializeset<long long>(*(std::set<long long>*)t, serOpt - 300, appName, base);
		case 311: return serializeset<unsigned long long>(*(std::set<unsigned long long>*)t, serOpt - 300, appName, base);
		case 312: return serializeset<float>(*(std::set<float>*)t, serOpt - 300, appName, base);
		case 313: return serializeset<double>(*(std::set<double>*)t, serOpt - 300, appName, base);
		case 314: return serializeset<long double>(*(std::set<long double>*)t, serOpt - 300, appName, base);
		case 315: {
			std::set<bool>* bt = (std::set<bool>*)t;
			int cnt = 0;
			int size = bt->size();
			void* object = base->getSerializableObject();
			base->startContainerSerialization(object, "bool", "std::set");
			for(bool v: *bt)
			{
				base->addContainerSerializableElement(object, serialize<bool>(v, serOpt, appName, base));
				base->afterAddContainerSerializableElement(object, cnt++, size);
			}
			base->endContainerSerialization(object, "bool", "std::set");
			std::string ser = base->fromSerializableObjectToString(object);
			base->cleanSerializableObject(object);
			return ser;
		}
		case 316: return serializeset<Date>(*(std::set<Date>*)t, serOpt - 300, appName, base);

		case 400: {
			if(f3!=NULL) return f3(t, base, "multiset");
			StringUtil::replaceFirst(className,"std::multiset<","");
			StringUtil::replaceFirst(className,"multiset<","");
			std::string vtyp;
			if(className.find(",")!=std::string::npos)
				vtyp = className.substr(0,className.find(","));
			else
				vtyp = className.substr(0,className.find(">"));
			processClassName(vtyp);
			return _serContainer(t, vtyp, appName, "multiset", base);
		}
		case 401: return serializemultiset<std::string>(*(std::multiset<std::string>*)t, serOpt - 400, appName, base);
		case 402: return serializemultiset<char>(*(std::multiset<char>*)t, serOpt - 400, appName, base);
		case 403: return serializemultiset<unsigned char>(*(std::multiset<unsigned char>*)t, serOpt - 400, appName, base);
		case 404: return serializemultiset<int>(*(std::multiset<int>*)t, serOpt - 400, appName, base);
		case 405: return serializemultiset<unsigned int>(*(std::multiset<unsigned int>*)t, serOpt - 400, appName, base);
		case 406: return serializemultiset<short>(*(std::multiset<short>*)t, serOpt - 400, appName, base);
		case 407: return serializemultiset<unsigned short>(*(std::multiset<unsigned short>*)t, serOpt - 400, appName, base);
		case 408: return serializemultiset<long>(*(std::multiset<long>*)t, serOpt - 400, appName, base);
		case 409: return serializemultiset<unsigned long>(*(std::multiset<unsigned long>*)t, serOpt - 400, appName, base);
		case 410: return serializemultiset<long long>(*(std::multiset<long long>*)t, serOpt - 400, appName, base);
		case 411: return serializemultiset<unsigned long long>(*(std::multiset<unsigned long long>*)t, serOpt - 400, appName, base);
		case 412: return serializemultiset<float>(*(std::multiset<float>*)t, serOpt - 400, appName, base);
		case 413: return serializemultiset<double>(*(std::multiset<double>*)t, serOpt - 400, appName, base);
		case 414: return serializemultiset<long double>(*(std::multiset<long double>*)t, serOpt - 400, appName, base);
		case 415: {
			std::multiset<bool>* bt = (std::multiset<bool>*)t;
			int cnt = 0;
			int size = bt->size();
			void* object = base->getSerializableObject();
			base->startContainerSerialization(object, "bool", "std::multiset");
			for(bool v: *bt)
			{
				base->addContainerSerializableElement(object, serialize<bool>(v, serOpt, appName, base));
				base->afterAddContainerSerializableElement(object, cnt++, size);
			}
			base->endContainerSerialization(object, "bool", "std::multiset");
			std::string ser = base->fromSerializableObjectToString(object);
			base->cleanSerializableObject(object);
			return ser;
		}
		case 416: return serializemultiset<Date>(*(std::multiset<Date>*)t, serOpt - 400, appName, base);

		case 500: {
			if(f2!=NULL) return f2(t, base, "queue");
			StringUtil::replaceFirst(className,"std::queue<","");
			StringUtil::replaceFirst(className,"queue<","");
			std::string vtyp;
			if(className.find(",")!=std::string::npos)
				vtyp = className.substr(0,className.find(","));
			else
				vtyp = className.substr(0,className.find(">"));
			processClassName(vtyp);
			return _serContainer(t, vtyp, appName, "queue", base);
		}
		case 501: return serializeq<std::string>(*(std::queue<std::string>*)t, serOpt - 500, appName, base);
		case 502: return serializeq<char>(*(std::queue<char>*)t, serOpt - 500, appName, base);
		case 503: return serializeq<unsigned char>(*(std::queue<unsigned char>*)t, serOpt - 500, appName, base);
		case 504: return serializeq<int>(*(std::queue<int>*)t, serOpt - 500, appName, base);
		case 505: return serializeq<unsigned int>(*(std::queue<unsigned int>*)t, serOpt - 500, appName, base);
		case 506: return serializeq<short>(*(std::queue<short>*)t, serOpt - 500, appName, base);
		case 507: return serializeq<unsigned short>(*(std::queue<unsigned short>*)t, serOpt - 500, appName, base);
		case 508: return serializeq<long>(*(std::queue<long>*)t, serOpt - 500, appName, base);
		case 509: return serializeq<unsigned long>(*(std::queue<unsigned long>*)t, serOpt - 500, appName, base);
		case 510: return serializeq<long long>(*(std::queue<long long>*)t, serOpt - 500, appName, base);
		case 511: return serializeq<unsigned long long>(*(std::queue<unsigned long long>*)t, serOpt - 500, appName, base);
		case 512: return serializeq<float>(*(std::queue<float>*)t, serOpt - 500, appName, base);
		case 513: return serializeq<double>(*(std::queue<double>*)t, serOpt - 500, appName, base);
		case 514: return serializeq<long double>(*(std::queue<long double>*)t, serOpt - 500, appName, base);
		case 515: {
			std::queue<bool>* bt = (std::queue<bool>*)t;
			DummyQueue* dptr = (DummyQueue*)bt;
			std::deque<bool>* tt = (std::deque<bool>*)&dptr->dq;
			int cnt = 0;
			int size = tt->size();
			void* object = base->getSerializableObject();
			base->startContainerSerialization(object, "bool", "std::queue");
			for(bool v: *tt)
			{
				base->addContainerSerializableElement(object, serialize<bool>(v, serOpt, appName, base));
				base->afterAddContainerSerializableElement(object, cnt++, size);
			}
			base->endContainerSerialization(object, "bool", "std::queue");
			std::string ser = base->fromSerializableObjectToString(object);
			base->cleanSerializableObject(object);
			return ser;
		}
		case 516: return serializeq<Date>(*(std::queue<Date>*)t, serOpt - 500, appName, base);

		case 600: {
			if(f2!=NULL) return f2(t, base, "deque");
			StringUtil::replaceFirst(className,"std::queue<","");
			StringUtil::replaceFirst(className,"queue<","");
			std::string vtyp;
			if(className.find(",")!=std::string::npos)
				vtyp = className.substr(0,className.find(","));
			else
				vtyp = className.substr(0,className.find(">"));
			processClassName(vtyp);
			return _serContainer(t, vtyp, appName, "queue", base);
		}
		case 601: return serializedq<std::string>(*(std::deque<std::string>*)t, serOpt - 600, appName, base);
		case 602: return serializedq<char>(*(std::deque<char>*)t, serOpt - 600, appName, base);
		case 603: return serializedq<unsigned char>(*(std::deque<unsigned char>*)t, serOpt - 600, appName, base);
		case 604: return serializedq<int>(*(std::deque<int>*)t, serOpt - 600, appName, base);
		case 605: return serializedq<unsigned int>(*(std::deque<unsigned int>*)t, serOpt - 600, appName, base);
		case 606: return serializedq<short>(*(std::deque<short>*)t, serOpt - 600, appName, base);
		case 607: return serializedq<unsigned short>(*(std::deque<unsigned short>*)t, serOpt - 600, appName, base);
		case 608: return serializedq<long>(*(std::deque<long>*)t, serOpt - 600, appName, base);
		case 609: return serializedq<unsigned long>(*(std::deque<unsigned long>*)t, serOpt - 600, appName, base);
		case 610: return serializedq<long long>(*(std::deque<long long>*)t, serOpt - 600, appName, base);
		case 611: return serializedq<unsigned long long>(*(std::deque<unsigned long long>*)t, serOpt - 600, appName, base);
		case 612: return serializedq<float>(*(std::deque<float>*)t, serOpt - 600, appName, base);
		case 613: return serializedq<double>(*(std::deque<double>*)t, serOpt - 600, appName, base);
		case 614: return serializedq<long double>(*(std::deque<long double>*)t, serOpt - 600, appName, base);
		case 615: {
			std::deque<bool>* bt = (std::deque<bool>*)t;
			int cnt = 0;
			int size = bt->size();
			void* object = base->getSerializableObject();
			base->startContainerSerialization(object, "bool", "std::deque");
			for(bool v: *bt)
			{
				base->addContainerSerializableElement(object, serialize<bool>(v, serOpt, appName, base));
				base->afterAddContainerSerializableElement(object, cnt++, size);
			}
			base->endContainerSerialization(object, "bool", "std::deque");
			std::string ser = base->fromSerializableObjectToString(object);
			base->cleanSerializableObject(object);
			return ser;
		}
		case 616: return serializedq<Date>(*(std::deque<Date>*)t, serOpt - 600, appName, base);
	}
	return "";
}

void SerializeBase::processClassAndContainerNames(std::string& className, std::string& container)
{
	StringUtil::replaceAll(container, "std::", "");
	StringUtil::replaceAll(container, "::", "_");
	StringUtil::replaceAll(className, "std::", "");
	StringUtil::replaceAll(className, "::", "_");
	StringUtil::replaceAll(className, "<", "-");
	StringUtil::replaceAll(className, ">", "-");
	StringUtil::replaceAll(className, ",", "-");
	if(className.at(className.length()-1)=='-')
		className = className.substr(0, className.length()-1);
}

void SerializeBase::processClassName(std::string& className)
{
	StringUtil::replaceAll(className, "::", "_");
}

std::string SerializeBase::handleMultiLevelSerialization(void* t, std::string className, const std::string& appName, SerializeBase* base)
{
	if(className.find(",")!=std::string::npos)
	{
		className = className.substr(0, className.find(",")+1);
	}
	void* object = base->getSerializableObject();
	int cnt = 0;
	if(className.find("std::vector<")==0 || className.find("vector<")==0) {
		if(className.find("std::vector<")==0)
			StringUtil::replaceFirst(className,"std::vector<","");
		else
			StringUtil::replaceFirst(className,"vector<","");
		std::string container = "std::vector";
		processClassAndContainerNames(className, container);
		base->startContainerSerialization(object, className, container);
		if(className.find("<")==std::string::npos) {
			int serOpt = identifySerOption("std::vector<"+className+",");
			return _handleAllSerialization(serOpt, "std::vector<"+className+",", t, appName, base, NULL, NULL, NULL);
		} else {
			if(className.find("std::vector<bool")==0 || className.find("vector<bool")==0)
			{
				std::vector<std::vector<bool> >* ptr = (std::vector<std::vector<bool> >*)t;
				std::vector<std::vector<bool> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::vector<bool>* la = (std::vector<bool>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::vector<")==0 || className.find("vector<")==0)
			{
				std::vector<std::vector<Dummy> >* ptr = (std::vector<std::vector<Dummy> >*)t;
				std::vector<std::vector<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::vector<Dummy>* la = (std::vector<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::deque<")==0 || className.find("deque<")==0)
			{
				std::vector<std::deque<Dummy> >* ptr = (std::vector<std::deque<Dummy> >*)t;
				std::vector<std::deque<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::deque<Dummy>* la = (std::deque<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::list<")==0 || className.find("list<")==0)
			{
				std::vector<std::list<Dummy> >* ptr = (std::vector<std::list<Dummy> >*)t;
				std::vector<std::list<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::list<Dummy>* la = (std::list<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::set<")==0 || className.find("set<")==0)
			{
				std::vector<std::set<Dummy> >* ptr = (std::vector<std::set<Dummy> >*)t;
				std::vector<std::set<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::set<Dummy>* la = (std::set<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::multiset<")==0 || className.find("multiset<")==0)
			{
				std::vector<std::multiset<Dummy> >* ptr = (std::vector<std::multiset<Dummy> >*)t;
				std::vector<std::multiset<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::multiset<Dummy>* la = (std::multiset<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
		}
		base->endContainerSerialization(object, className, "std::vector");
	}
	else if(className.find("std::list<")==0 || className.find("list<")==0) {
		if(className.find("std::list<")==0)
			StringUtil::replaceFirst(className,"std::list<","");
		else
			StringUtil::replaceFirst(className,"list<","");
		std::string container = "std::list";
		processClassAndContainerNames(className, container);
		base->startContainerSerialization(object, className, container);
		if(className.find("<")==std::string::npos) {
			int serOpt = identifySerOption("std::vector<"+className+",");
			return _handleAllSerialization(serOpt, "std::list<"+className+",", t, appName, base, NULL, NULL, NULL);
		} else {
			std::list<Dummy>* ptr = (std::list<Dummy>*)t;
			std::list<Dummy>::iterator itls = ptr->begin();
			for(;itls!=ptr->end();++itls) {
				std::list<Dummy>* la = (std::list<Dummy>*)&(*itls);
				std::string serval = handleMultiLevelSerialization(la, className, appName, base);
				base->addContainerSerializableElementMulti(object, serval);
				base->afterAddContainerSerializableElement(object, cnt++, 0);
			}
		}
		base->endContainerSerialization(object, className, "std::list");
	}
	else if(className.find("std::deque<")==0 || className.find("deque<")==0) {
		if(className.find("std::deque<")==0)
			StringUtil::replaceFirst(className,"std::deque<","");
		else
			StringUtil::replaceFirst(className,"deque<","");
		std::string container = "std::deque";
		processClassAndContainerNames(className, container);
		base->startContainerSerialization(object, className, container);
		if(className.find("<")==std::string::npos) {
			int serOpt = identifySerOption("std::vector<"+className+",");
			return _handleAllSerialization(serOpt, "std::deque<"+className+",", t, appName, base, NULL, NULL, NULL);
		} else {
			if(className.find("std::vector<bool")==0 || className.find("vector<bool")==0)
			{
				std::deque<std::vector<bool> >* ptr = (std::deque<std::vector<bool> >*)t;
				std::deque<std::vector<bool> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::vector<bool>* la = (std::vector<bool>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::vector<")==0 || className.find("vector<")==0)
			{
				std::deque<std::vector<Dummy> >* ptr = (std::deque<std::vector<Dummy> >*)t;
				std::deque<std::vector<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::vector<Dummy>* la = (std::vector<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::deque<")==0 || className.find("deque<")==0)
			{
				std::deque<std::deque<Dummy> >* ptr = (std::deque<std::deque<Dummy> >*)t;
				std::deque<std::deque<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::deque<Dummy>* la = (std::deque<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::list<")==0 || className.find("list<")==0)
			{
				std::deque<std::list<Dummy> >* ptr = (std::deque<std::list<Dummy> >*)t;
				std::deque<std::list<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::list<Dummy>* la = (std::list<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::set<")==0 || className.find("set<")==0)
			{
				std::deque<std::set<Dummy> >* ptr = (std::deque<std::set<Dummy> >*)t;
				std::deque<std::set<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::set<Dummy>* la = (std::set<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::multiset<")==0 || className.find("multiset<")==0)
			{
				std::deque<std::multiset<Dummy> >* ptr = (std::deque<std::multiset<Dummy> >*)t;
				std::deque<std::multiset<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					std::multiset<Dummy>* la = (std::multiset<Dummy>*)&(*itls);
					std::string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
		}
		base->endContainerSerialization(object, className, "std::deque");
	}
	else if(className.find("std::queue<")==0 || className.find("queue<")==0) {
		if(className.find("std::queue<")==0)
			StringUtil::replaceFirst(className,"std::queue<","");
		else
			StringUtil::replaceFirst(className,"queue<","");
		std::string container = "std::queue";
		processClassAndContainerNames(className, container);
		base->startContainerSerialization(object, className, container);
		if(className.find("<")==std::string::npos) {
			int serOpt = identifySerOption("std::vector<"+className+",");
			return _handleAllSerialization(serOpt, "std::queue<"+className+",", t, appName, base, NULL, NULL, NULL);
		} else {
			DummyQueue* dptr  = static_cast<DummyQueue*>(t);
			std::deque<std::deque<Dummy> >* ptr = (std::deque<std::deque<Dummy> >*)&dptr->dq;
			std::deque<std::deque<Dummy> >::iterator itls = ptr->begin();
			for(;itls!=ptr->end();++itls) {
				std::deque<std::deque<Dummy> >* la = (std::deque<std::deque<Dummy> >*)&(*itls);
				std::string serval = handleMultiLevelSerialization(la, className, appName, base);
				base->addContainerSerializableElementMulti(object, serval);
				base->afterAddContainerSerializableElement(object, cnt++, 0);
			}
		}
		base->endContainerSerialization(object, className, "std::queue");
	}
	else if(className.find("std::set<")==0 || className.find("set<")==0) {
		if(className.find("std::set<")==0)
			StringUtil::replaceFirst(className,"std::set<","");
		else
			StringUtil::replaceFirst(className,"set<","");
		std::string container = "std::set";
		processClassAndContainerNames(className, container);
		base->startContainerSerialization(object, className, container);
		if(className.find("<")==std::string::npos) {
			int serOpt = identifySerOption("std::vector<"+className+",");
			return _handleAllSerialization(serOpt, "std::set<"+className+",", t, appName, base, NULL, NULL, NULL);
		} else {
			std::set<Dummy>* ptr = (std::set<Dummy>*)t;
			std::set<Dummy>::iterator itls = ptr->begin();
			for(;itls!=ptr->end();++itls) {
				std::set<Dummy>* la = (std::set<Dummy>*)&(*itls);
				std::string serval = handleMultiLevelSerialization(la, className, appName, base);
				base->addContainerSerializableElementMulti(object, serval);
				base->afterAddContainerSerializableElement(object, cnt++, 0);
			}
		}
		base->endContainerSerialization(object, className, "std::set");
	}
	else if(className.find("std::multiset<")==0 || className.find("multiset<")==0) {
		if(className.find("std::multiset<")==0)
			StringUtil::replaceFirst(className,"std::multiset<","");
		else
			StringUtil::replaceFirst(className,"multiset<","");
		std::string container = "std::multiset";
		processClassAndContainerNames(className, container);
		base->startContainerSerialization(object, className, container);
		if(className.find("<")==std::string::npos) {
			int serOpt = identifySerOption("std::vector<"+className+",");
			return _handleAllSerialization(serOpt, "std::multiset<"+className+",", t, appName, base, NULL, NULL, NULL);
		} else {
			std::multiset<Dummy>* ptr = (std::multiset<Dummy>*)t;
			std::multiset<Dummy>::iterator itls = ptr->begin();
			for(;itls!=ptr->end();++itls) {
				std::multiset<Dummy>* la = (std::multiset<Dummy>*)&(*itls);
				std::string serval = handleMultiLevelSerialization(la, className, appName, base);
				base->addContainerSerializableElementMulti(object, serval);
				base->afterAddContainerSerializableElement(object, cnt++, 0);
			}
		}
		base->endContainerSerialization(object, className, "std::multiset");
	}
	std::string ser = base->fromSerializableObjectToString(object);
	base->cleanSerializableObject(object);
	return ser;
}

SerCont SerializeBase::serContFunc(std::string& className, const std::string& appName, const std::string& type)
{
	StringUtil::replaceAll(className, "::", "_");
	std::string methodname = getSerializationMethodName(className,appName,true,type);
	return (SerCont)dlsym(dlib, methodname.c_str());
}

Ser SerializeBase::serFunc(std::string& className, const std::string& appName)
{
	StringUtil::replaceAll(className, "::", "_");
	std::string methodname = getSerializationMethodName(className,appName,true);
	return (Ser)dlsym(dlib, methodname.c_str());
}

std::string SerializeBase::_serContainer(void* t, const std::string& className, const std::string& appName, const std::string& type, SerializeBase* base)
{
	std::string serVal;
	std::string methodname = base->getSerializationMethodName(className,appName,true,type);
	SerCont f;
	if(_serCFMap.contains(methodname)) {
		f = (SerCont)_serCFMap.find(methodname);
	} else {
		f = (SerCont)dlsym(dlib, methodname.c_str());
		_serCFMap.insert(methodname, f);
	}
	if(f!=NULL)
		serVal = f(t, base, type);
	return serVal;
}

std::string SerializeBase::_ser(void* t, const std::string& className, const std::string& appName, SerializeBase* base)
{
	std::string serVal;
	std::string methodname = base->getSerializationMethodName(className,appName,true);
	Ser f;
	if(_serFMap.contains(methodname)) {
		f = (Ser)_serFMap.find(methodname);
	} else {
		f = (Ser)dlsym(dlib, methodname.c_str());
		_serFMap.insert(methodname, f);
	}
	if(f!=NULL)
		serVal = f(t, base);
	return serVal;
}

void* SerializeBase::_handleAllUnSerialization(const std::string& serVal, int serOpt, std::string className, const std::string& app, SerializeBase* base, const bool& isJsonSer, void* serObject)
{
	std::string appName = CommonUtils::getAppName(app);
	void* unserObjectVal = NULL;
	void* intermediateObject = base->getUnserializableObject(serVal);
	if(intermediateObject==NULL || StringUtil::trimCopy(serVal)=="")
	{
		if(serObject==NULL)
			return NULL;
		if(intermediateObject!=NULL) {
			base->cleanUnserializableObject(intermediateObject);
		}
		intermediateObject = serObject;
	}
	std::string unserObjName = base->getUnserializableClassName(intermediateObject, className);
	void* mintermediateObject = intermediateObject;
	int sizet;
	if(unserObjName!="")
	{
		if(isJsonSer && StringUtil::trimCopy(serVal)!="")
		{
			mintermediateObject = base->getValidUnserializableObject(serVal);
		}
	}
	switch(serOpt) {
		case 0: {
			processClassName(className);
			unserObjectVal = _unser(mintermediateObject,className,appName,base);
			break;
		}
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17: {
			unserObjectVal = base->getPrimitiveValue(intermediateObject, serOpt, className);
			break;
		}
		case 18: {
			unserObjectVal = handleMultiLevelUnSerialization(mintermediateObject, className, appName, sizet, base);
			break;
		}

		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
		case 108:
		case 109:
		case 110:
		case 111:
		case 112:
		case 113:
		case 114:
		case 115:
		case 116: {
			unserObjectVal = unserializevec(mintermediateObject,serOpt,appName,sizet,base,className);
			break;
		}

		case 200:
		case 201:
		case 202:
		case 203:
		case 204:
		case 205:
		case 206:
		case 207:
		case 208:
		case 209:
		case 210:
		case 211:
		case 212:
		case 213:
		case 214:
		case 215:
		case 216: {
			unserObjectVal = unserializelist(mintermediateObject,serOpt,appName,sizet,base,className);
			break;
		}

		case 300:
		case 301:
		case 302:
		case 303:
		case 304:
		case 305:
		case 306:
		case 307:
		case 308:
		case 309:
		case 310:
		case 311:
		case 312:
		case 313:
		case 314:
		case 315:
		case 316: {
			unserObjectVal = unserializeset(mintermediateObject,serOpt,appName,sizet,base,className);
			break;
		}

		case 400:
		case 401:
		case 402:
		case 403:
		case 404:
		case 405:
		case 406:
		case 407:
		case 408:
		case 409:
		case 410:
		case 411:
		case 412:
		case 413:
		case 414:
		case 415:
		case 416: {
			unserObjectVal = unserializemultiset(mintermediateObject,serOpt,appName,sizet,base,className);
			break;
		}

		case 500:
		case 501:
		case 502:
		case 503:
		case 504:
		case 505:
		case 506:
		case 507:
		case 508:
		case 509:
		case 510:
		case 511:
		case 512:
		case 513:
		case 514:
		case 515:
		case 516: {
			unserObjectVal = unserializeq(mintermediateObject,serOpt,appName,sizet,base,className);
			break;
		}

		case 600:
		case 601:
		case 602:
		case 603:
		case 604:
		case 605:
		case 606:
		case 607:
		case 608:
		case 609:
		case 610:
		case 611:
		case 612:
		case 613:
		case 614:
		case 615:
		case 616: {
			unserObjectVal = unserializedq(mintermediateObject,serOpt,appName,sizet,base,className);
			break;
		}
	}

	if(unserObjName!="" && isJsonSer && StringUtil::trimCopy(serVal)!="")
	{
		base->cleanValidUnserializableObject(mintermediateObject);
	}
	if(StringUtil::trimCopy(serVal)!="" && intermediateObject!=NULL)
	{
		base->cleanUnserializableObject(intermediateObject);
	}
	return unserObjectVal;
}

void* SerializeBase::handleMultiLevelUnSerialization(void* intermediateObject, std::string className, const std::string& appName, int& size, SerializeBase* base)
{
	if(className.find(",")!=std::string::npos)
	{
		className = className.substr(0, className.find(",")+1);
	}
	size = 0;
	int level = StringUtil::countOccurrences(className, "<") - 1 ;
	void* tv = NULL;
	if(level>0)
	{
		std::string container;
		if(className.find("std::vector<")==0 || className.find("vector<")==0)
		{
			container = "std::vector<";
		}
		else if(className.find("std::deque<")==0 || className.find("deque<")==0)
		{
			container = "std::deque<";
		}
		else if(className.find("std::queue<")==0 || className.find("queue<")==0)
		{
			container = "std::queue<";
		}
		else if(className.find("std::list<")==0 || className.find("list<")==0)
		{
			container = "std::list<";
		}
		else if(className.find("std::set<")==0 || className.find("set<")==0)
		{
			container = "std::set<";
		}
		else if(className.find("std::multiset<")==0 || className.find("multiset<")==0)
		{
			container = "std::multiset<";
		}
		std::vector<int> elesizes;
		tv = getNestedContainer(className);
		int totsiz = base->getContainerSize(intermediateObject);
		for (int var = 0; var < totsiz; var++)
		{
			void* intEleObject = base->getContainerElement(intermediateObject, var);
			size++;
			int lsiz = 0;
			addToNestedContainer(intEleObject, className, appName, lsiz, container, tv, var, base);
			elesizes.push_back(lsiz);
		}
		if((container.find("std::vector<")==0 || container.find("vector<")==0) && elesizes.size()>0
				&& (className.find("std::list<")==0 || className.find("list<")==0))
		{
			int counter = 0;
			std::vector<DummyList>* vdl = (std::vector<DummyList>*)tv;
			std::vector<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}

				DummyList::_List_node_base temp1 = (*it)._M_impl._M_node;
				temp->_M_next = (DummyList::_List_node_base*)&temp1;
				temp->_M_prev = (DummyList::_List_node_base*)&temp1;
			}
		}
		else if((container.find("std::deque<")==0 || container.find("deque<")==0) && elesizes.size()>0
				&& (className.find("std::list<")==0 || className.find("list<")==0))
		{
			int counter = 0;
			std::deque<DummyList>* vdl = (std::deque<DummyList>*)tv;
			std::deque<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}

				DummyList::_List_node_base temp1 = (*it)._M_impl._M_node;
				temp->_M_next = (DummyList::_List_node_base*)&temp1;
				temp->_M_prev = (DummyList::_List_node_base*)&temp1;
			}
		}
		else if((container.find("std::set<")==0 || container.find("set<")==0) && elesizes.size()>0
				&& (className.find("std::list<")==0 || className.find("list<")==0))
		{
			int counter = 0;
			std::set<DummyList>* vdl = (std::set<DummyList>*)tv;
			std::set<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}

				DummyList::_List_node_base temp1 = (*it)._M_impl._M_node;
				temp->_M_next = (DummyList::_List_node_base*)&temp1;
				temp->_M_prev = (DummyList::_List_node_base*)&temp1;
			}
		}
		else if((container.find("std::multiset<")==0 || container.find("multiset<")==0) && elesizes.size()>0
				&& (className.find("std::list<")==0 || className.find("list<")==0))
		{
			int counter = 0;
			std::multiset<DummyList>* vdl = (std::multiset<DummyList>*)tv;
			std::multiset<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}

				DummyList::_List_node_base temp1 = (*it)._M_impl._M_node;
				temp->_M_next = (DummyList::_List_node_base*)&temp1;
				temp->_M_prev = (DummyList::_List_node_base*)&temp1;
			}
		}
		/*else if(container.find("std::queue<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			std::queue<DummyList>* vdl = (std::deque<DummyList>*)tv;
			std::queue<DummyList>::iterator it = vdl->begin();
			for(; it != vdl->end(); ++it) {
				int tmpsiz = elesizes.at(counter);
				DummyList::_List_node_base* temp = (*it)._M_impl._M_node._M_next;
				while(tmpsiz-->1)
				{
					temp = temp->_M_next;
				}

				DummyList::_List_node_base* temp1 = (*it)._M_impl._M_node;
				temp->_M_next = (DummyList::_List_node_base*)&temp1;
				temp->_M_prev = (DummyList::_List_node_base*)&temp1;
			}
		}*/
	}
	else
	{
		int serOpt = identifySerOption(className);
		switch(serOpt) {
			case 100:
			case 101:
			case 102:
			case 103:
			case 104:
			case 105:
			case 106:
			case 107:
			case 108:
			case 109:
			case 110:
			case 111:
			case 112:
			case 113:
			case 114:
			case 115:
			case 116: {
				tv = unserializevec(intermediateObject,serOpt,appName,size,base,className);
				break;
			}

			case 200:
			case 201:
			case 202:
			case 203:
			case 204:
			case 205:
			case 206:
			case 207:
			case 208:
			case 209:
			case 210:
			case 211:
			case 212:
			case 213:
			case 214:
			case 215:
			case 216: {
				tv = unserializelist(intermediateObject,serOpt,appName,size,base,className);
				break;
			}

			case 300:
			case 301:
			case 302:
			case 303:
			case 304:
			case 305:
			case 306:
			case 307:
			case 308:
			case 309:
			case 310:
			case 311:
			case 312:
			case 313:
			case 314:
			case 315:
			case 316: {
				tv = unserializeset(intermediateObject,serOpt,appName,size,base,className);
				break;
			}

			case 400:
			case 401:
			case 402:
			case 403:
			case 404:
			case 405:
			case 406:
			case 407:
			case 408:
			case 409:
			case 410:
			case 411:
			case 412:
			case 413:
			case 414:
			case 415:
			case 416: {
				tv = unserializemultiset(intermediateObject,serOpt,appName,size,base,className);
				break;
			}

			case 500:
			case 501:
			case 502:
			case 503:
			case 504:
			case 505:
			case 506:
			case 507:
			case 508:
			case 509:
			case 510:
			case 511:
			case 512:
			case 513:
			case 514:
			case 515:
			case 516: {
				tv = unserializeq(intermediateObject,serOpt,appName,size,base,className);
				break;
			}

			case 600:
			case 601:
			case 602:
			case 603:
			case 604:
			case 605:
			case 606:
			case 607:
			case 608:
			case 609:
			case 610:
			case 611:
			case 612:
			case 613:
			case 614:
			case 615:
			case 616: {
				tv = unserializedq(intermediateObject,serOpt,appName,size,base,className);
				break;
			}
		}
	}
	return tv;
}

std::string SerializeBase::serializeUnknown(void* t, int serOpt, const std::string& className, const std::string& appName, SerializeBase* base)
{
	return base->serializeUnknownBase(t, serOpt, className, appName);
}

void* SerializeBase::unSerializeUnknown(const std::string& objXml, int serOpt, const std::string& className, const std::string& appName, SerializeBase* base)
{
	return base->unSerializeUnknownBase(objXml, serOpt, className, appName);
}

void* unSerializeUnknown(void* unserObj, const std::string& className, int serOpt, const std::string& appName, SerializeBase* base)
{
	return base->unSerializeUnknownBase(unserObj, serOpt, className, appName);
}

void* SerializeBase::unserializeset(void* unserableObject, int serOpt, const std::string& appName, int &size, SerializeBase* base, const std::string& classN)
{
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;

	serOpt = serOpt - 300;
	switch(serOpt) {
		case 0: {
			processClassName(className);
			return _unserContainer(unserableObject,className,appName,"set",base);
		}
		case 1: t = new std::set<std::string>();break;
		case 2: t = new std::set<char>();break;
		case 3: t = new std::set<unsigned char>();break;
		case 4: t = new std::set<int>();break;
		case 5: t = new std::set<unsigned int>();break;
		case 6: t = new std::set<short>();break;
		case 7: t = new std::set<unsigned short>();break;
		case 8: t = new std::set<long>();break;
		case 9: t = new std::set<unsigned long>();break;
		case 10: t = new std::set<long long>();break;
		case 11: t = new std::set<unsigned long long>();break;
		case 12: t = new std::set<float>();break;
		case 13: t = new std::set<double>();break;
		case 14: t = new std::set<long double>();break;
		case 15: t = new std::set<bool>();break;
		case 16: t = new std::set<Date>();break;
	}

	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, serOpt, var, className, t, "std::set");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializelist(void* unserableObject, int serOpt, const std::string& appName, int &size, SerializeBase* base, const std::string& classN)
{
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;

	serOpt = serOpt - 200;
	switch(serOpt) {
		case 0: {
			processClassName(className);
			return _unserContainer(unserableObject,className,appName,"list",base);
		}
		case 1: t = new std::list<std::string>();break;
		case 2: t = new std::list<char>();break;
		case 3: t = new std::list<unsigned char>();break;
		case 4: t = new std::list<int>();break;
		case 5: t = new std::list<unsigned int>();break;
		case 6: t = new std::list<short>();break;
		case 7: t = new std::list<unsigned short>();break;
		case 8: t = new std::list<long>();break;
		case 9: t = new std::list<unsigned long>();break;
		case 10: t = new std::list<long long>();break;
		case 11: t = new std::list<unsigned long long>();break;
		case 12: t = new std::list<float>();break;
		case 13: t = new std::list<double>();break;
		case 14: t = new std::list<long double>();break;
		case 15: t = new std::list<bool>();break;
		case 16: t = new std::list<Date>();break;
	}

	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, serOpt, var, className, t, "std::list");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializeq(void* unserableObject, int serOpt, const std::string& appName, int &size, SerializeBase* base, const std::string& classN)
{
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;

	serOpt = serOpt - 500;
	switch(serOpt) {
		case 0: {
			processClassName(className);
			return _unserContainer(unserableObject,className,appName,"queue",base);
		}
		case 1: t = new std::queue<std::string>();break;
		case 2: t = new std::queue<char>();break;
		case 3: t = new std::queue<unsigned char>();break;
		case 4: t = new std::queue<int>();break;
		case 5: t = new std::queue<unsigned int>();break;
		case 6: t = new std::queue<short>();break;
		case 7: t = new std::queue<unsigned short>();break;
		case 8: t = new std::queue<long>();break;
		case 9: t = new std::queue<unsigned long>();break;
		case 10: t = new std::queue<long long>();break;
		case 11: t = new std::queue<unsigned long long>();break;
		case 12: t = new std::queue<float>();break;
		case 13: t = new std::queue<double>();break;
		case 14: t = new std::queue<long double>();break;
		case 15: t = new std::queue<bool>();break;
		case 16: t = new std::queue<Date>();break;
	}

	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, serOpt, var, className, t, "std::queue");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializevec(void* unserableObject, int serOpt, const std::string& appName, int &size, SerializeBase* base, const std::string& classN)
{
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;

	serOpt = serOpt - 100;
	switch(serOpt) {
		case 0: {
			processClassName(className);
			return _unserContainer(unserableObject,className,appName,"vector",base);
		}
		case 1: t = new std::vector<std::string>();break;
		case 2: t = new std::vector<char>();break;
		case 3: t = new std::vector<unsigned char>();break;
		case 4: t = new std::vector<int>();break;
		case 5: t = new std::vector<unsigned int>();break;
		case 6: t = new std::vector<short>();break;
		case 7: t = new std::vector<unsigned short>();break;
		case 8: t = new std::vector<long>();break;
		case 9: t = new std::vector<unsigned long>();break;
		case 10: t = new std::vector<long long>();break;
		case 11: t = new std::vector<unsigned long long>();break;
		case 12: t = new std::vector<float>();break;
		case 13: t = new std::vector<double>();break;
		case 14: t = new std::vector<long double>();break;
		case 15: t = new std::vector<bool>();break;
		case 16: t = new std::vector<Date>();break;
	}

	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, serOpt, var, className, t, "std::vector");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializedq(void* unserableObject, int serOpt, const std::string& appName, int &size, SerializeBase* base, const std::string& classN)
{
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;

	serOpt = serOpt - 600;
	switch(serOpt) {
		case 0: {
			processClassName(className);
			return _unserContainer(unserableObject,className,appName,"deque",base);
		}
		case 1: t = new std::deque<std::string>();break;
		case 2: t = new std::deque<char>();break;
		case 3: t = new std::deque<unsigned char>();break;
		case 4: t = new std::deque<int>();break;
		case 5: t = new std::deque<unsigned int>();break;
		case 6: t = new std::deque<short>();break;
		case 7: t = new std::deque<unsigned short>();break;
		case 8: t = new std::deque<long>();break;
		case 9: t = new std::deque<unsigned long>();break;
		case 10: t = new std::deque<long long>();break;
		case 11: t = new std::deque<unsigned long long>();break;
		case 12: t = new std::deque<float>();break;
		case 13: t = new std::deque<double>();break;
		case 14: t = new std::deque<long double>();break;
		case 15: t = new std::deque<bool>();break;
		case 16: t = new std::deque<Date>();break;
	}

	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, serOpt, var, className, t, "std::deque");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializemultiset(void* unserableObject, int serOpt, const std::string& appName, int &size, SerializeBase* base, const std::string& classN)
{
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;

	serOpt = serOpt - 400;
	switch(serOpt) {
		case 0: {
			processClassName(className);
			return _unserContainer(unserableObject,className,appName,"multiset",base);
		}
		case 1: t = new std::multiset<std::string>();break;
		case 2: t = new std::multiset<char>();break;
		case 3: t = new std::multiset<unsigned char>();break;
		case 4: t = new std::multiset<int>();break;
		case 5: t = new std::multiset<unsigned int>();break;
		case 6: t = new std::multiset<short>();break;
		case 7: t = new std::multiset<unsigned short>();break;
		case 8: t = new std::multiset<long>();break;
		case 9: t = new std::multiset<unsigned long>();break;
		case 10: t = new std::multiset<long long>();break;
		case 11: t = new std::multiset<unsigned long long>();break;
		case 12: t = new std::multiset<float>();break;
		case 13: t = new std::multiset<double>();break;
		case 14: t = new std::multiset<long double>();break;
		case 15: t = new std::multiset<bool>();break;
		case 16: t = new std::multiset<Date>();break;
	}

	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, serOpt, var, className, t, "std::multiset");
			}
		}
		return t;
	}
	return NULL;
}


UnSerCont SerializeBase::unSerContFunc(std::string& className, const std::string& appName, const std::string& type)
{
	StringUtil::replaceAll(className, "::", "_");
	std::string methodname = getSerializationMethodName(className,appName,false,type);
	return (UnSerCont)dlsym(dlib, methodname.c_str());
}

UnSer SerializeBase::unSerFunc(std::string& className, const std::string& appName)
{
	StringUtil::replaceAll(className, "::", "_");
	std::string methodname = getSerializationMethodName(className,appName,false);
	return (UnSer)dlsym(dlib, methodname.c_str());
}

void* SerializeBase::_unserContainer(void* unserableObject, const std::string& className, const std::string& appName, const std::string& type, SerializeBase* base)
{
	void* obj = NULL;
	std::string methodname = base->getSerializationMethodName(className,appName,false,type);
	UnSerCont f;
	if(_unserCFMap.contains(methodname)) {
		f = (UnSerCont)_unserCFMap.find(methodname);
	} else {
		f = (UnSerCont)dlsym(dlib, methodname.c_str());
		_unserCFMap.insert(methodname, f);
	}
	if(f!=NULL)
	{
		obj = f(unserableObject, base, type);
	}
	return obj;
}

void* SerializeBase::_unser(void* unserableObject, const std::string& className, const std::string& appName, SerializeBase* base)
{
	void* obj = NULL;
	std::string methodname = base->getSerializationMethodName(className,appName,false);
	UnSer f;
	if(_unserFMap.contains(methodname)) {
		f = (UnSer)_unserFMap.find(methodname);
	} else {
		f = (UnSer)dlsym(dlib, methodname.c_str());
		_unserFMap.insert(methodname, f);
	}
	if(f!=NULL)
	{
		obj = f(unserableObject, base);
	}
	return obj;
}

std::string SerializeBase::getTemplateArg(const std::string& s, std::string& tem)
{
	tem = s;
	bool flag = false;
	int comm = 0;
	do {
		comm = s.find(",", comm+1);
		tem = s.substr(0, comm);
		int to = StringUtil::countOccurrences(tem, "<");
		int tc = StringUtil::countOccurrences(tem, ">");
		flag = tc+1==to || (tc==to && tc==0);
	} while(!flag);

	std::string tfrst;
	if(tem.find("<")!=std::string::npos)
		tfrst = tem.substr(tem.find("<")+1, tem.length());
	else
		tfrst = tem.substr(0, tem.length());
	if(tem.find(",")!=std::string::npos)
		tfrst = tfrst.substr(0, tfrst.find(",")+1);
	StringUtil::trim(tfrst);

	tem = s.substr(tem.length());
	tem = tem.substr(tem.find(",")+1);
	return tfrst;
}

std::string SerializeBase::getSerializationMethodName(const std::string& className, const std::string& appName, const bool& which, const std::string& type)
{
	std::string methodname;
	if(which) {
		methodname = appName + "serialize" + className + (type==""?"":"Container");
	}  else {
		methodname = appName + "unSerialize" + className + (type==""?"":"Container");
	}
	if(type!="") {
		if(type=="set" || type=="std::set" || type=="multiset" || type=="std::multiset") {
			methodname += "SV";
		}
	}
	return methodname;
}

std::string SerializeBase::trySerialize(void* t, int serOpt, std::string& className, const std::string& app) {
	switch(serOpt) {
		case 1: return *(std::string*)t;
		case 2: return CastUtil::fromNumber(*(int*)t);
		case 3: return CastUtil::fromNumber(*(unsigned int*)t);
		case 4: return CastUtil::fromNumber(*(int*)t);
		case 5: return CastUtil::fromNumber(*(unsigned int*)t);
		case 6: return CastUtil::fromNumber(*(short*)t);
		case 7: return CastUtil::fromNumber(*(unsigned short*)t);
		case 8: return CastUtil::fromNumber(*(long*)t);
		case 9: return CastUtil::fromNumber(*(int*)t);
		case 10: return CastUtil::fromNumber(*(long long*)t);
		case 11: return CastUtil::fromNumber(*(int*)t);
		case 12: return CastUtil::fromFloat(*(float*)t);
		case 13: return CastUtil::fromDouble(*(double*)t);
		case 14: return CastUtil::fromLongdouble(*(long double*)t);
		case 15: return CastUtil::fromBool(*(bool*)t);
		case 16: {
			DateFormat formt;
			return formt.format((Date*)t);
		}
	}
	return "";
}
