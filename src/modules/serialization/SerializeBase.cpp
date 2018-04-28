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
	}
	else if(className.find("std::deque<")==0 || className.find("deque<")==0)
	{
		container = "std::deque";
		if(className.find("std::deque<")==0)
			StringUtil::replaceFirst(className,"std::deque<","");
		else
			StringUtil::replaceFirst(className,"deque<","");
	}
	else if(className.find("std::list<")==0 || className.find("list<")==0)
	{
		container = "std::list";
		if(className.find("std::list<")==0)
			StringUtil::replaceFirst(className,"std::list<","");
		else
			StringUtil::replaceFirst(className,"list<","");
	}
	else if(className.find("std::set<")==0 || className.find("set<")==0)
	{
		container = "std::set";
		if(className.find("std::set<")==0)
			StringUtil::replaceFirst(className,"std::set<","");
		else
			StringUtil::replaceFirst(className,"set<","");
	}
	else if(className.find("std::multiset<")==0 || className.find("multiset<")==0)
	{
		container = "std::multiset";
		if(className.find("std::multiset<")==0)
			StringUtil::replaceFirst(className,"std::multiset<","");
		else
			StringUtil::replaceFirst(className,"multiset<","");
	}
	else if(className.find("std::queue<")==0 || className.find("queue<")==0)
	{
		container = "std::queue";
		if(className.find("std::queue<")==0)
			StringUtil::replaceFirst(className,"std::queue<","");
		else
			StringUtil::replaceFirst(className,"queue<","");
	}

	if(className.find("std::vector<")==0 || className.find("vector<")==0)
	{
		return getNewNestedContainer<DummyVec>(container);
	}
	else if(className.find("std::deque<")==0 || className.find("deque<")==0)
	{
		return getNewNestedContainer<DummyDeque>(container);
	}
	else if(className.find("std::list<")==0 || className.find("list<")==0)
	{
		return getNewNestedContainer<DummyList>(container);
	}
	else if(className.find("std::set<")==0 || className.find("set<")==0)
	{
		return getNewNestedContainer<DummySet>(container);
	}
	else if(className.find("std::multiset<")==0 || className.find("multiset<")==0)
	{
		return getNewNestedContainer<DummySet>(container);
	}
	else if(className.find("std::queue<")==0 || className.find("queue<")==0)
	{
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

std::string SerializeBase::_handleAllSerialization(std::string className, void *t, const std::string& app, SerializeBase* base)
{
	StringUtil::trim(className);
	std::string appName = CommonUtils::getAppName(app);
	std::string serVal;
	if(className.find(",")!=std::string::npos)
	{
		className = className.substr(0, className.find(",")+1);
	}
	int level = StringUtil::countOccurrences(className, "<");
	if(isPrimitiveDataType(className))
	{
		serVal = base->serializePrimitive(className, t);
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		std::string dateval = formt.format(*(Date*)t);
		serVal = base->serializePrimitive(className, &dateval);
	}
	else if(className=="BinaryData")
	{
		std::string binaryData = BinaryData::serilaize(*(BinaryData*)t);
		serVal = base->serializePrimitive(className, &binaryData);
	}
	else if(level>1)
	{
		serVal = handleMultiLevelSerialization(t, className, appName, base);
	}
	else if(className.find("std::vector<std::string,")!=std::string::npos || className.find("std::vector<std::string>")!=std::string::npos
		|| className.find("vector<std::string,")!=std::string::npos || className.find("vector<std::string>")!=std::string::npos
		|| className.find("std::vector<string,")!=std::string::npos || className.find("std::vector<string>")!=std::string::npos
		|| className.find("vector<string,")!=std::string::npos || className.find("vector<string>")!=std::string::npos)
	{
		std::vector<std::string> *tt = (std::vector<std::string>*)t;
		serVal = serializevec<std::string>(*tt, appName, base);
	}
	else if(className.find("std::vector<char,")!=std::string::npos || className.find("std::vector<char>")!=std::string::npos
		|| className.find("vector<char,")!=std::string::npos || className.find("vector<char>")!=std::string::npos)
	{
		std::vector<char> *tt = (std::vector<char>*)t;
		serVal = serializevec<char>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned char,")!=std::string::npos || className.find("std::vector<unsigned char>")!=std::string::npos
		|| className.find("vector<unsigned char,")!=std::string::npos || className.find("vector<unsigned char>")!=std::string::npos)
	{
		std::vector<unsigned char> *tt = (std::vector<unsigned char>*)t;
		serVal = serializevec<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::vector<int,")!=std::string::npos || className.find("std::vector<int>")!=std::string::npos
		|| className.find("vector<int,")!=std::string::npos || className.find("vector<int>")!=std::string::npos)
	{
		std::vector<int> *tt = (std::vector<int>*)t;
		serVal = serializevec<int>(*tt, appName, base);
	}
	else if(className.find("std::vector<short,")!=std::string::npos || className.find("std::vector<short>")!=std::string::npos
		|| className.find("vector<short,")!=std::string::npos || className.find("vector<short>")!=std::string::npos)
	{
		std::vector<short> *tt = (std::vector<short>*)t;
		serVal = serializevec<short>(*tt, appName, base);
	}
	else if(className.find("std::vector<long,")!=std::string::npos || className.find("std::vector<long>")!=std::string::npos
		|| className.find("vector<long,")!=std::string::npos || className.find("vector<long>")!=std::string::npos)
	{
		std::vector<long> *tt = (std::vector<long>*)t;
		serVal = serializevec<long>(*tt, appName, base);
	}
	else if(className.find("std::vector<long long,")!=std::string::npos || className.find("std::vector<long long>")!=std::string::npos
		|| className.find("vector<long long,")!=std::string::npos || className.find("vector<long long>")!=std::string::npos)
	{
		std::vector<long long> *tt = (std::vector<long long>*)t;
		serVal = serializevec<long long>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned int,")!=std::string::npos || className.find("std::vector<unsigned int>")!=std::string::npos
		|| className.find("vector<unsigned int,")!=std::string::npos || className.find("vector<unsigned int>")!=std::string::npos)
	{
		std::vector<unsigned int> *tt = (std::vector<unsigned int>*)t;
		serVal = serializevec<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned short,")!=std::string::npos || className.find("std::vector<unsigned short>")!=std::string::npos
		|| className.find("vector<unsigned short,")!=std::string::npos || className.find("vector<unsigned short>")!=std::string::npos)
	{
		std::vector<unsigned short> *tt = (std::vector<unsigned short>*)t;
		serVal = serializevec<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned long,")!=std::string::npos || className.find("std::vector<unsigned long>")!=std::string::npos
		|| className.find("vector<unsigned long,")!=std::string::npos || className.find("vector<unsigned long>")!=std::string::npos)
	{
		std::vector<unsigned long> *tt = (std::vector<unsigned long>*)t;
		serVal = serializevec<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned long long,")!=std::string::npos || className.find("std::vector<unsigned long long>")!=std::string::npos
		|| className.find("vector<unsigned long long,")!=std::string::npos || className.find("vector<unsigned long long>")!=std::string::npos)
	{
		std::vector<unsigned long long> *tt = (std::vector<unsigned long long>*)t;
		serVal = serializevec<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::vector<double,")!=std::string::npos || className.find("std::vector<double>")!=std::string::npos
		|| className.find("vector<double,")!=std::string::npos || className.find("vector<double>")!=std::string::npos)
	{
		std::vector<double> *tt = (std::vector<double>*)t;
		serVal = serializevec<double>(*tt, appName, base);
	}
	else if(className.find("std::vector<long double,")!=std::string::npos || className.find("std::vector<long double>")!=std::string::npos
		|| className.find("vector<long double,")!=std::string::npos || className.find("vector<long double>")!=std::string::npos)
	{
		std::vector<long double> *tt = (std::vector<long double>*)t;
		serVal = serializevec<long double>(*tt, appName, base);
	}
	else if(className.find("std::vector<float,")!=std::string::npos || className.find("std::vector<float>")!=std::string::npos
		|| className.find("vector<float,")!=std::string::npos || className.find("vector<float>")!=std::string::npos)
	{
		std::vector<float> *tt = (std::vector<float>*)t;
		serVal = serializevec<float>(*tt, appName, base);
	}
	else if(className.find("std::vector<bool,")!=std::string::npos || className.find("std::vector<bool>")!=std::string::npos
		|| className.find("vector<bool,")!=std::string::npos || className.find("vector<bool>")!=std::string::npos)
	{
		std::vector<bool> *tt = (std::vector<bool>*)t;
		serVal = serializevec<bool>(*tt, appName, base);
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
		processClassName(vtyp);
		serVal = _serContainer(t, vtyp, appName, "vector", base);
	}
	else if(className.find("std::list<std::string,")!=std::string::npos || className.find("std::list<std::string>")!=std::string::npos
		|| className.find("list<std::string,")!=std::string::npos || className.find("list<std::string>")!=std::string::npos
		|| className.find("std::list<string,")!=std::string::npos || className.find("std::list<string>")!=std::string::npos
		|| className.find("list<string,")!=std::string::npos || className.find("list<string>")!=std::string::npos)
	{
		std::list<std::string> *tt = (std::list<std::string>*)t;
		serVal = serializelist<std::string>(*tt, appName, base);
	}
	else if(className.find("std::list<char,")!=std::string::npos || className.find("std::list<char>")!=std::string::npos
		|| className.find("list<char,")!=std::string::npos || className.find("list<char>")!=std::string::npos)
	{
		std::list<char> *tt = (std::list<char>*)t;
		serVal = serializelist<char>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned char,")!=std::string::npos || className.find("std::list<unsigned char>")!=std::string::npos
		|| className.find("list<unsigned char,")!=std::string::npos || className.find("list<unsigned char>")!=std::string::npos)
	{
		std::list<unsigned char> *tt = (std::list<unsigned char>*)t;
		serVal = serializelist<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::list<int,")!=std::string::npos || className.find("std::list<int>")!=std::string::npos
		|| className.find("list<int,")!=std::string::npos || className.find("list<int>")!=std::string::npos)
	{
		std::list<int> *tt = (std::list<int>*)t;
		serVal = serializelist<int>(*tt, appName, base);
	}
	else if(className.find("std::list<long,")!=std::string::npos || className.find("std::list<long>")!=std::string::npos
		|| className.find("list<long,")!=std::string::npos || className.find("list<long>")!=std::string::npos)
	{
		std::list<long> *tt = (std::list<long>*)t;
		serVal = serializelist<long>(*tt, appName, base);
	}
	else if(className.find("std::list<short,")!=std::string::npos || className.find("std::list<short>")!=std::string::npos
		|| className.find("list<short,")!=std::string::npos || className.find("list<short>")!=std::string::npos)
	{
		std::list<short> *tt = (std::list<short>*)t;
		serVal = serializelist<short>(*tt, appName, base);
	}
	else if(className.find("std::list<long long,")!=std::string::npos || className.find("std::list<long long>")!=std::string::npos
		|| className.find("list<long long,")!=std::string::npos || className.find("list<long long>")!=std::string::npos)
	{
		std::list<long long> *tt = (std::list<long long>*)t;
		serVal = serializelist<long long>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned int,")!=std::string::npos || className.find("std::list<unsigned int>")!=std::string::npos
		|| className.find("list<unsigned int,")!=std::string::npos || className.find("list<unsigned int>")!=std::string::npos)
	{
		std::list<unsigned int> *tt = (std::list<unsigned int>*)t;
		serVal = serializelist<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned long,")!=std::string::npos || className.find("std::list<unsigned long>")!=std::string::npos
		|| className.find("list<unsigned long,")!=std::string::npos || className.find("list<unsigned long>")!=std::string::npos)
	{
		std::list<unsigned long> *tt = (std::list<unsigned long>*)t;
		serVal = serializelist<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned short,")!=std::string::npos || className.find("std::list<unsigned short>")!=std::string::npos
		|| className.find("list<unsigned short,")!=std::string::npos || className.find("list<unsigned short>")!=std::string::npos)
	{
		std::list<unsigned short> *tt = (std::list<unsigned short>*)t;
		serVal = serializelist<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned long long,")!=std::string::npos || className.find("std::list<unsigned long long>")!=std::string::npos
		|| className.find("list<unsigned long long,")!=std::string::npos || className.find("list<unsigned long long>")!=std::string::npos)
	{
		std::list<unsigned long long> *tt = (std::list<unsigned long long>*)t;
		serVal = serializelist<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::list<double,")!=std::string::npos || className.find("std::list<double>")!=std::string::npos
		|| className.find("list<double,")!=std::string::npos || className.find("list<double>")!=std::string::npos)
	{
		std::list<double> *tt = (std::list<double>*)t;
		serVal = serializelist<double>(*tt, appName, base);
	}
	else if(className.find("std::list<long double,")!=std::string::npos || className.find("std::list<long double>")!=std::string::npos
		|| className.find("list<long double,")!=std::string::npos || className.find("list<long double>")!=std::string::npos)
	{
		std::list<long double> *tt = (std::list<long double>*)t;
		serVal = serializelist<long double>(*tt, appName, base);
	}
	else if(className.find("std::list<float,")!=std::string::npos || className.find("std::list<float>")!=std::string::npos
		|| className.find("list<float,")!=std::string::npos || className.find("list<float>")!=std::string::npos)
	{
		std::list<float> *tt = (std::list<float>*)t;
		serVal = serializelist<float>(*tt, appName, base);
	}
	else if(className.find("std::list<bool,")!=std::string::npos || className.find("std::list<bool>")!=std::string::npos
		|| className.find("list<bool,")!=std::string::npos || className.find("list<bool>")!=std::string::npos)
	{
		std::list<bool> *tt = (std::list<bool>*)t;
		serVal = serializelist<bool>(*tt, appName, base);
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
		processClassName(vtyp);
		serVal = _serContainer(t, vtyp, appName, "list", base);
	}
	else if(className.find("std::set<std::string,")!=std::string::npos || className.find("std::set<std::string>")!=std::string::npos
		|| className.find("set<std::string,")!=std::string::npos || className.find("set<std::string>")!=std::string::npos
		|| className.find("std::set<string,")!=std::string::npos || className.find("std::set<string>")!=std::string::npos
		|| className.find("set<string,")!=std::string::npos || className.find("set<string>")!=std::string::npos)
	{
		std::set<std::string> *tt = (std::set<std::string>*)t;
		serVal = serializeset<std::string>(*tt, appName, base);
	}
	else if(className.find("std::set<char,")!=std::string::npos || className.find("std::set<char>")!=std::string::npos
		|| className.find("set<char,")!=std::string::npos || className.find("set<char>")!=std::string::npos)
	{
		std::set<char> *tt = (std::set<char>*)t;
		serVal = serializeset<char>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned char,")!=std::string::npos || className.find("std::set<unsigned char>")!=std::string::npos
		|| className.find("set<unsigned char,")!=std::string::npos || className.find("set<unsigned char>")!=std::string::npos)
	{
		std::set<unsigned char> *tt = (std::set<unsigned char>*)t;
		serVal = serializeset<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::set<int,")!=std::string::npos || className.find("std::set<int>")!=std::string::npos
		|| className.find("set<int,")!=std::string::npos || className.find("set<int>")!=std::string::npos)
	{
		std::set<int> *tt = (std::set<int>*)t;
		serVal = serializeset<int>(*tt, appName, base);
	}
	else if(className.find("std::set<short,")!=std::string::npos || className.find("std::set<short>")!=std::string::npos
		|| className.find("set<short,")!=std::string::npos || className.find("set<short>")!=std::string::npos)
	{
		std::set<short> *tt = (std::set<short>*)t;
		serVal = serializeset<short>(*tt, appName, base);
	}
	else if(className.find("std::set<long,")!=std::string::npos || className.find("std::set<long>")!=std::string::npos
		|| className.find("set<long,")!=std::string::npos || className.find("set<long>")!=std::string::npos)
	{
		std::set<long> *tt = (std::set<long>*)t;
		serVal = serializeset<long>(*tt, appName, base);
	}
	else if(className.find("std::set<long long,")!=std::string::npos || className.find("std::set<long long>")!=std::string::npos
		|| className.find("set<long long,")!=std::string::npos || className.find("set<long long>")!=std::string::npos)
	{
		std::set<long long> *tt = (std::set<long long>*)t;
		serVal = serializeset<long long>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned int,")!=std::string::npos || className.find("std::set<unsigned int>")!=std::string::npos
		|| className.find("set<unsigned int,")!=std::string::npos || className.find("set<unsigned int>")!=std::string::npos)
	{
		std::set<unsigned int> *tt = (std::set<unsigned int>*)t;
		serVal = serializeset<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned short,")!=std::string::npos || className.find("std::set<unsigned short>")!=std::string::npos
		|| className.find("set<unsigned short,")!=std::string::npos || className.find("set<unsigned short>")!=std::string::npos)
	{
		std::set<unsigned short> *tt = (std::set<unsigned short>*)t;
		serVal = serializeset<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned long,")!=std::string::npos || className.find("std::set<unsigned long>")!=std::string::npos
		|| className.find("set<unsigned long,")!=std::string::npos || className.find("set<unsigned long>")!=std::string::npos)
	{
		std::set<unsigned long> *tt = (std::set<unsigned long>*)t;
		serVal = serializeset<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned long long,")!=std::string::npos || className.find("std::set<unsigned long long>")!=std::string::npos
		|| className.find("set<unsigned long long,")!=std::string::npos || className.find("set<unsigned long long>")!=std::string::npos)
	{
		std::set<unsigned long long> *tt = (std::set<unsigned long long>*)t;
		serVal = serializeset<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::set<long double,")!=std::string::npos || className.find("std::set<long double>")!=std::string::npos
		|| className.find("set<long double,")!=std::string::npos || className.find("set<long double>")!=std::string::npos)
	{
		std::set<long double> *tt = (std::set<long double>*)t;
		serVal = serializeset<long double>(*tt, appName, base);
	}
	else if(className.find("std::set<double,")!=std::string::npos || className.find("std::set<double>")!=std::string::npos
		|| className.find("set<double,")!=std::string::npos || className.find("set<double>")!=std::string::npos)
	{
		std::set<double> *tt = (std::set<double>*)t;
		serVal = serializeset<double>(*tt, appName, base);
	}
	else if(className.find("std::set<float,")!=std::string::npos || className.find("std::set<float>")!=std::string::npos
		|| className.find("set<float,")!=std::string::npos || className.find("set<float>")!=std::string::npos)
	{
		std::set<float> *tt = (std::set<float>*)&t;
		serVal = serializeset<float>(*tt, appName, base);
	}
	else if(className.find("std::set<bool,")!=std::string::npos || className.find("std::set<bool>")!=std::string::npos
		|| className.find("set<bool,")!=std::string::npos || className.find("set<bool>")!=std::string::npos)
	{
		std::set<bool> *tt = (std::set<bool>*)&t;
		serVal = serializeset<bool>(*tt, appName, base);
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
		processClassName(vtyp);
		serVal = _serContainer(t, vtyp, appName, "set", base);
	}
	else if(className.find("std::multiset<std::string,")!=std::string::npos || className.find("std::multiset<std::string>")!=std::string::npos
		|| className.find("multiset<std::string,")!=std::string::npos || className.find("multiset<std::string>")!=std::string::npos
		|| className.find("std::multiset<string,")!=std::string::npos || className.find("std::multiset<string>")!=std::string::npos
		|| className.find("multiset<string,")!=std::string::npos || className.find("multiset<string>")!=std::string::npos)
	{
		std::multiset<std::string> *tt = (std::multiset<std::string>*)t;
		serVal = serializemultiset<std::string>(*tt, appName, base);
	}
	else if(className.find("std::multiset<char,")!=std::string::npos || className.find("std::multiset<char>")!=std::string::npos
		|| className.find("multiset<char,")!=std::string::npos || className.find("multiset<char>")!=std::string::npos)
	{
		std::multiset<char> *tt = (std::multiset<char>*)t;
		serVal = serializemultiset<char>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned char,")!=std::string::npos || className.find("std::multiset<unsigned char>")!=std::string::npos
		|| className.find("multiset<unsigned char,")!=std::string::npos || className.find("multiset<unsigned char>")!=std::string::npos)
	{
		std::multiset<unsigned char> *tt = (std::multiset<unsigned char>*)t;
		serVal = serializemultiset<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::multiset<int,")!=std::string::npos || className.find("std::multiset<int>")!=std::string::npos
		|| className.find("multiset<int,")!=std::string::npos || className.find("multiset<int>")!=std::string::npos)
	{
		std::multiset<int> *tt = (std::multiset<int>*)t;
		serVal = serializemultiset<int>(*tt, appName, base);
	}
	else if(className.find("std::multiset<long,")!=std::string::npos || className.find("std::multiset<long>")!=std::string::npos
		|| className.find("multiset<long,")!=std::string::npos || className.find("multiset<long>")!=std::string::npos)
	{
		std::multiset<long> *tt = (std::multiset<long>*)t;
		serVal = serializemultiset<long>(*tt, appName, base);
	}
	else if(className.find("std::multiset<long long,")!=std::string::npos || className.find("std::multiset<long long>")!=std::string::npos
		|| className.find("multiset<long long,")!=std::string::npos || className.find("multiset<long long>")!=std::string::npos)
	{
		std::multiset<long long> *tt = (std::multiset<long long>*)t;
		serVal = serializemultiset<long long>(*tt, appName, base);
	}
	else if(className.find("std::multiset<short,")!=std::string::npos || className.find("std::multiset<short>")!=std::string::npos
		|| className.find("multiset<short,")!=std::string::npos || className.find("multiset<short>")!=std::string::npos)
	{
		std::multiset<short> *tt = (std::multiset<short>*)t;
		serVal = serializemultiset<short>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned int,")!=std::string::npos || className.find("std::multiset<unsigned int>")!=std::string::npos
		|| className.find("multiset<unsigned int,")!=std::string::npos || className.find("multiset<unsigned int>")!=std::string::npos)
	{
		std::multiset<unsigned int> *tt = (std::multiset<unsigned int>*)t;
		serVal = serializemultiset<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned long,")!=std::string::npos || className.find("std::multiset<unsigned long>")!=std::string::npos
		|| className.find("multiset<unsigned long,")!=std::string::npos || className.find("multiset<unsigned long>")!=std::string::npos)
	{
		std::multiset<unsigned long> *tt = (std::multiset<unsigned long>*)t;
		serVal = serializemultiset<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned long long,")!=std::string::npos || className.find("std::multiset<unsigned long long>")!=std::string::npos
		|| className.find("multiset<unsigned long long,")!=std::string::npos || className.find("multiset<unsigned long long>")!=std::string::npos)
	{
		std::multiset<unsigned long long> *tt = (std::multiset<unsigned long long>*)t;
		serVal = serializemultiset<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned short,")!=std::string::npos || className.find("std::multiset<unsigned short>")!=std::string::npos
		|| className.find("multiset<unsigned short,")!=std::string::npos || className.find("multiset<unsigned short>")!=std::string::npos)
	{
		std::multiset<unsigned short> *tt = (std::multiset<unsigned short>*)t;
		serVal = serializemultiset<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::multiset<double,")!=std::string::npos || className.find("std::multiset<double>")!=std::string::npos
		|| className.find("multiset<double,")!=std::string::npos || className.find("multiset<double>")!=std::string::npos)
	{
		std::multiset<double> *tt = (std::multiset<double>*)t;
		serVal = serializemultiset<double>(*tt, appName, base);
	}
	else if(className.find("std::multiset<long double,")!=std::string::npos || className.find("std::multiset<long double>")!=std::string::npos
		|| className.find("multiset<long double,")!=std::string::npos || className.find("multiset<long double>")!=std::string::npos)
	{
		std::multiset<long double> *tt = (std::multiset<long double>*)t;
		serVal = serializemultiset<long double>(*tt, appName, base);
	}
	else if(className.find("std::multiset<float,")!=std::string::npos || className.find("std::multiset<float>")!=std::string::npos
		|| className.find("multiset<float,")!=std::string::npos || className.find("multiset<float>")!=std::string::npos)
	{
		std::multiset<float> *tt = (std::multiset<float>*)t;
		serVal = serializemultiset<float>(*tt, appName, base);
	}
	else if(className.find("std::multiset<bool,")!=std::string::npos || className.find("std::multiset<bool>")!=std::string::npos
		|| className.find("multiset<bool,")!=std::string::npos || className.find("multiset<bool>")!=std::string::npos)
	{
		std::multiset<bool> *tt = (std::multiset<bool>*)t;
		serVal = serializemultiset<bool>(*tt, appName, base);
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
		processClassName(vtyp);
		serVal = _serContainer(t, vtyp, appName, "multiset", base);
	}
	else if(className.find("std::queue<std::string,")!=std::string::npos || className.find("std::queue<std::string>")!=std::string::npos
		|| className.find("queue<std::string,")!=std::string::npos || className.find("queue<std::string>")!=std::string::npos
		|| className.find("std::queue<string,")!=std::string::npos || className.find("std::queue<string>")!=std::string::npos
		|| className.find("queue<string,")!=std::string::npos || className.find("queue<string>")!=std::string::npos)
	{
		std::queue<std::string> *tt = (std::queue<std::string>*)t;
		serVal = serializeq<std::string>(*tt, appName, base);
	}
	else if(className.find("std::queue<char,")!=std::string::npos || className.find("std::queue<char>")!=std::string::npos
		|| className.find("queue<char,")!=std::string::npos || className.find("queue<char>")!=std::string::npos)
	{
		std::queue<char> *tt = (std::queue<char>*)t;
		serVal = serializeq<char>(*tt, appName, base);
	}
	else if(className.find("std::queue<unsigned char,")!=std::string::npos || className.find("std::queue<unsigned char>")!=std::string::npos
		|| className.find("queue<unsigned char,")!=std::string::npos || className.find("queue<unsigned char>")!=std::string::npos)
	{
		std::queue<unsigned char> *tt = (std::queue<unsigned char>*)t;
		serVal = serializeq<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::queue<int,")!=std::string::npos || className.find("std::queue<int>")!=std::string::npos
		|| className.find("queue<int,")!=std::string::npos || className.find("queue<int>")!=std::string::npos)
	{
		std::queue<int> *tt = (std::queue<int>*)t;
		serVal = serializeq<int>(*tt, appName, base);
	}
	else if(className.find("std::queue<short,")!=std::string::npos || className.find("std::queue<short>")!=std::string::npos
		|| className.find("queue<short,")!=std::string::npos || className.find("queue<short>")!=std::string::npos)
	{
		std::queue<short> *tt = (std::queue<short>*)t;
		serVal = serializeq<short>(*tt, appName, base);
	}
	else if(className.find("std::queue<long,")!=std::string::npos || className.find("std::queue<long>")!=std::string::npos
		|| className.find("queue<long,")!=std::string::npos || className.find("queue<long>")!=std::string::npos)
	{
		std::queue<long> *tt = (std::queue<long>*)t;
		serVal = serializeq<long>(*tt, appName, base);
	}
	else if(className.find("std::queue<long long,")!=std::string::npos || className.find("std::queue<long long>")!=std::string::npos
		|| className.find("queue<long long,")!=std::string::npos || className.find("queue<long long>")!=std::string::npos)
	{
		std::queue<long long> *tt = (std::queue<long long>*)t;
		serVal = serializeq<long long>(*tt, appName, base);
	}
	else if(className.find("std::queue<unsigned int,")!=std::string::npos || className.find("std::queue<unsigned int>")!=std::string::npos
		|| className.find("queue<unsigned int,")!=std::string::npos || className.find("queue<unsigned int>")!=std::string::npos)
	{
		std::queue<unsigned int> *tt = (std::queue<unsigned int>*)t;
		serVal = serializeq<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::queue<unsigned short,")!=std::string::npos || className.find("std::queue<unsigned short>")!=std::string::npos
		|| className.find("queue<unsigned short,")!=std::string::npos || className.find("queue<unsigned short>")!=std::string::npos)
	{
		std::queue<unsigned short> *tt = (std::queue<unsigned short>*)t;
		serVal = serializeq<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::queue<unsigned long,")!=std::string::npos || className.find("std::queue<unsigned long>")!=std::string::npos
		|| className.find("queue<unsigned long,")!=std::string::npos || className.find("queue<unsigned long>")!=std::string::npos)
	{
		std::queue<unsigned long> *tt = (std::queue<unsigned long>*)t;
		serVal = serializeq<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::queue<unsigned long long,")!=std::string::npos || className.find("std::queue<unsigned long long>")!=std::string::npos
		|| className.find("queue<unsigned long long,")!=std::string::npos || className.find("queue<unsigned long long>")!=std::string::npos)
	{
		std::queue<unsigned long long> *tt = (std::queue<unsigned long long>*)t;
		serVal = serializeq<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::queue<long double,")!=std::string::npos || className.find("std::queue<long double>")!=std::string::npos
		|| className.find("queue<long double,")!=std::string::npos || className.find("queue<long double>")!=std::string::npos)
	{
		std::queue<long double> *tt = (std::queue<long double>*)t;
		serVal = serializeq<long double>(*tt, appName, base);
	}
	else if(className.find("std::queue<double,")!=std::string::npos || className.find("std::queue<double>")!=std::string::npos
		|| className.find("queue<double,")!=std::string::npos || className.find("queue<double>")!=std::string::npos)
	{
		std::queue<double> *tt = (std::queue<double>*)t;
		serVal = serializeq<double>(*tt, appName, base);
	}
	else if(className.find("std::queue<float,")!=std::string::npos || className.find("std::queue<float>")!=std::string::npos
		|| className.find("queue<float,")!=std::string::npos || className.find("queue<float>")!=std::string::npos)
	{
		std::queue<float> *tt = (std::queue<float>*)t;
		serVal = serializeq<float>(*tt, appName, base);
	}
	else if(className.find("std::queue<bool,")!=std::string::npos || className.find("std::queue<bool>")!=std::string::npos
		|| className.find("queue<bool,")!=std::string::npos || className.find("queue<bool>")!=std::string::npos)
	{
		std::queue<bool> *tt = (std::queue<bool>*)t;
		serVal = serializeq<bool>(*tt, appName, base);
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
		processClassName(vtyp);
		serVal = _serContainer(t, vtyp, appName, "queue", base);
	}
	else if(className.find("std::deque<std::string,")!=std::string::npos || className.find("std::deque<std::string>")!=std::string::npos
		|| className.find("deque<std::string,")!=std::string::npos || className.find("deque<std::string>")!=std::string::npos
		|| className.find("std::deque<string,")!=std::string::npos || className.find("std::deque<string>")!=std::string::npos
		|| className.find("deque<string,")!=std::string::npos || className.find("deque<string>")!=std::string::npos)
	{
		std::deque<std::string> *tt = (std::deque<std::string>*)t;
		serVal = serializedq<std::string>(*tt, appName, base);
	}
	else if(className.find("std::deque<char,")!=std::string::npos || className.find("std::deque<char>")!=std::string::npos
		|| className.find("deque<char,")!=std::string::npos || className.find("deque<char>")!=std::string::npos)
	{
		std::deque<char> *tt = (std::deque<char>*)t;
		serVal = serializedq<char>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned char,")!=std::string::npos || className.find("std::deque<unsigned char>")!=std::string::npos
		|| className.find("deque<unsigned char,")!=std::string::npos || className.find("deque<unsigned char>")!=std::string::npos)
	{
		std::deque<unsigned char> *tt = (std::deque<unsigned char>*)t;
		serVal = serializedq<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::deque<int,")!=std::string::npos || className.find("std::deque<int>")!=std::string::npos
		|| className.find("deque<int,")!=std::string::npos || className.find("deque<int>")!=std::string::npos)
	{
		std::deque<int> *tt = (std::deque<int>*)t;
		serVal = serializedq<int>(*tt, appName, base);
	}
	else if(className.find("std::deque<long,")!=std::string::npos || className.find("std::deque<long>")!=std::string::npos
		|| className.find("deque<long,")!=std::string::npos || className.find("deque<long>")!=std::string::npos)
	{
		std::deque<long> *tt = (std::deque<long>*)t;
		serVal = serializedq<long>(*tt, appName, base);
	}
	else if(className.find("std::deque<short,")!=std::string::npos || className.find("std::deque<short>")!=std::string::npos
		|| className.find("deque<short,")!=std::string::npos || className.find("deque<short>")!=std::string::npos)
	{
		std::deque<short> *tt = (std::deque<short>*)t;
		serVal = serializedq<short>(*tt, appName, base);
	}
	else if(className.find("std::deque<long long,")!=std::string::npos || className.find("std::deque<long long>")!=std::string::npos
		|| className.find("deque<long long,")!=std::string::npos || className.find("deque<long long>")!=std::string::npos)
	{
		std::deque<long long> *tt = (std::deque<long long>*)t;
		serVal = serializedq<long long>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned int,")!=std::string::npos || className.find("std::deque<unsigned int>")!=std::string::npos
		|| className.find("deque<unsigned int,")!=std::string::npos || className.find("deque<unsigned int>")!=std::string::npos)
	{
		std::deque<unsigned int> *tt = (std::deque<unsigned int>*)t;
		serVal = serializedq<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned short,")!=std::string::npos || className.find("std::deque<unsigned short>")!=std::string::npos
		|| className.find("deque<unsigned short,")!=std::string::npos || className.find("deque<unsigned short>")!=std::string::npos)
	{
		std::deque<unsigned short> *tt = (std::deque<unsigned short>*)t;
		serVal = serializedq<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned long,")!=std::string::npos || className.find("std::deque<unsigned long>")!=std::string::npos
		|| className.find("deque<unsigned long,")!=std::string::npos || className.find("deque<unsigned long>")!=std::string::npos)
	{
		std::deque<unsigned long> *tt = (std::deque<unsigned long>*)t;
		serVal = serializedq<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned long long,")!=std::string::npos || className.find("std::deque<unsigned long long>")!=std::string::npos
		|| className.find("deque<unsigned long long,")!=std::string::npos || className.find("deque<unsigned long long>")!=std::string::npos)
	{
		std::deque<unsigned long long> *tt = (std::deque<unsigned long long>*)t;
		serVal = serializedq<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::deque<long double,")!=std::string::npos || className.find("std::deque<long double>")!=std::string::npos
		|| className.find("deque<long double,")!=std::string::npos || className.find("deque<long double>")!=std::string::npos)
	{
		std::deque<long double> *tt = (std::deque<long double>*)t;
		serVal = serializedq<long double>(*tt, appName, base);
	}
	else if(className.find("std::deque<double,")!=std::string::npos || className.find("std::deque<double>")!=std::string::npos
		|| className.find("deque<double,")!=std::string::npos || className.find("deque<double>")!=std::string::npos)
	{
		std::deque<double> *tt = (std::deque<double>*)t;
		serVal = serializedq<double>(*tt, appName, base);
	}
	else if(className.find("std::deque<float,")!=std::string::npos || className.find("std::deque<float>")!=std::string::npos
		|| className.find("deque<float,")!=std::string::npos || className.find("deque<float>")!=std::string::npos)
	{
		std::deque<float> *tt = (std::deque<float>*)t;
		serVal = serializedq<float>(*tt, appName, base);
	}
	else if(className.find("std::deque<bool,")!=std::string::npos || className.find("std::deque<bool>")!=std::string::npos
		|| className.find("deque<bool,")!=std::string::npos || className.find("deque<bool>")!=std::string::npos)
	{
		std::deque<bool> *tt = (std::deque<bool>*)t;
		serVal = serializedq<bool>(*tt, appName, base);
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
		processClassName(vtyp);
		serVal = _serContainer(t, vtyp, appName, "deque", base);
	}
	else
	{
		processClassName(className);
		return _ser(t,className,appName,base);
	}
	return serVal;
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

std::string SerializeBase::handleMultiLevelSerialization(void* t, std::string className, const std::string& app, SerializeBase* base)
{
	std::string appName = CommonUtils::getAppName(app);
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
			std::string serval = _handleAllSerialization("std::vector<"+className+",", t, appName, base);
			return serval;
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
			std::string serval = _handleAllSerialization("std::list<"+className+",", t, appName, base);
			return serval;
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
			std::string serval = _handleAllSerialization("std::deque<"+className+",", t, appName, base);
			return serval;
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
			std::string serval = _handleAllSerialization("std::queue<"+className+",", t, appName, base);
			return serval;
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
			std::string serval = _handleAllSerialization("std::set<"+className+",", t, appName, base);
			return serval;
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
			std::string serval = _handleAllSerialization("std::multiset<"+className+",", t, appName, base);
			return serval;
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

std::string SerializeBase::_serContainer(void* t, const std::string& className, const std::string& app, const std::string& type, SerializeBase* base)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string serVal;
	std::string libName = INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		exit(-1);
	}
	std::string methodname = base->getSerializationMethodName(className,appName,true,type);
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef std::string (*RfPtr) (void*, SerializeBase*, const std::string&);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		serVal = f(t, base, type);
	return serVal;
}

std::string SerializeBase::_ser(void* t, const std::string& className, const std::string& app, SerializeBase* base)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string serVal;
	std::string libName = INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		exit(-1);
	}
	std::string methodname = base->getSerializationMethodName(className,appName,true);
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef std::string (*RfPtr) (void*, SerializeBase*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		serVal = f(t, base);
	return serVal;
}

void* SerializeBase::_handleAllUnSerialization(const std::string& serVal, std::string className, const std::string& app, SerializeBase* base, const bool& isJsonSer, void* serObject)
{
	std::string appName = CommonUtils::getAppName(app);
	if(className.find(",")!=std::string::npos)
	{
		className = className.substr(0, className.find(",")+1);
	}
	void* unserObjectVal = NULL;
	void* intermediateObject = base->getUnserializableObject(serVal);
	if(intermediateObject==NULL || StringUtil::trimCopy(serVal)=="")
	{
		if(serObject==NULL)
			return NULL;
		base->cleanUnserializableObject(intermediateObject);
		intermediateObject = serObject;
	}
	StringUtil::trim(className);
	std::string unserObjName = base->getUnserializableClassName(intermediateObject, className);
	if(isPrimitiveDataType(className) || className=="BinaryData" || className=="Date")
	{
		unserObjectVal = base->getPrimitiveValue(intermediateObject, className);
	}
	else if(unserObjName!="")
	{
		void* mintermediateObject = intermediateObject;
		int sizet;
		int level = StringUtil::countOccurrences(className, "<");
		if(isJsonSer && StringUtil::trimCopy(serVal)!="")
		{
			mintermediateObject = base->getValidUnserializableObject(serVal);
		}
		if(level>1)
		{
			int size;
			unserObjectVal = handleMultiLevelUnSerialization(mintermediateObject, className, appName, size, base);
		}
		else if(unserObjName.find("std::vector<")==0 || unserObjName.find("vector<")==0 || unserObjName.find("vector-")==0 || unserObjName.find("std::vector-")==0)
		{
			unserObjectVal = unserializevec(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::set<")==0 || unserObjName.find("set<")==0 || unserObjName.find("set-")==0 || unserObjName.find("std::set-")==0)
		{
			unserObjectVal = unserializeset(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::multiset<")==0 || unserObjName.find("multiset<")==0 || unserObjName.find("multiset-")==0 || unserObjName.find("std::multiset-")==0)
		{
			unserObjectVal = unserializemultiset(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::list<")==0 || unserObjName.find("list<")==0 || unserObjName.find("list-")==0 || unserObjName.find("std::list-")==0)
		{
			unserObjectVal = unserializelist(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::queue<")==0 || unserObjName.find("queue<")==0 || unserObjName.find("queue-")==0 || unserObjName.find("std::queue-")==0)
		{
			unserObjectVal = unserializeq(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::deque<")==0 || unserObjName.find("deque<")==0 || unserObjName.find("deque-")==0 || unserObjName.find("std::deque-")==0)
		{
			unserObjectVal = unserializedq(mintermediateObject,appName,sizet,base,className);
		}
		else
		{
			processClassName(className);
			unserObjectVal = _unser(mintermediateObject,className,appName,base);
		}
		if(isJsonSer && StringUtil::trimCopy(serVal)!="")
		{
			base->cleanValidUnserializableObject(mintermediateObject);
		}
	}
	if(StringUtil::trimCopy(serVal)!="" && intermediateObject!=NULL)
	{
		base->cleanUnserializableObject(intermediateObject);
	}
	return unserObjectVal;
}

void* SerializeBase::handleMultiLevelUnSerialization(void* intermediateObject, std::string className, const std::string& app, int& size, SerializeBase* base)
{
	std::string appName = CommonUtils::getAppName(app);
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
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
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
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
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
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
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
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
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
				temp->_M_next = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
				temp->_M_prev = (DummyList::_List_node_base*)&(*it)._M_impl._M_node;
			}
		}*/
	}
	else
	{
		if(className.find("std::vector<")==0 || className.find("vector<")==0)
		{
			tv = unserializevec(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::list<")==0 || className.find("list<")==0)
		{
			tv = unserializelist(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::deque<")==0 || className.find("deque<")==0)
		{
			tv = unserializedq(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::set<")==0 || className.find("set<")==0)
		{
			tv = unserializeset(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::multiset<")==0 || className.find("multiset<")==0)
		{
			tv = unserializemultiset(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::queue<")==0 || className.find("queue<")==0)
		{
			tv = unserializeq(intermediateObject, appName, size, base, className);
		}
	}
	return tv;
}

std::string SerializeBase::serializeUnknown(void* t, const std::string& className, const std::string& appName, SerializeBase* base)
{
	return base->serializeUnknownBase(t, className, appName);
}

void* SerializeBase::unSerializeUnknown(const std::string& objXml, const std::string& className, const std::string& appName, SerializeBase* base)
{
	return base->unSerializeUnknownBase(objXml, className, appName);
}

void* unSerializeUnknown(void* unserObj, const std::string& className, const std::string& appName, SerializeBase* base)
{
	return base->unSerializeUnknownBase(unserObj, className, appName);
}

void* SerializeBase::unserializeset(void* unserableObject, const std::string& app, int &size, SerializeBase* base, const std::string& classN)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::set<std::string>();
	else if(className=="int")
		t = new std::set<int>();
	else if(className=="short")
		t = new std::set<short>();
	else if(className=="long")
		t = new std::set<long>();
	else if(className=="long long")
		t = new std::set<long long>();
	else if(className=="long double")
		t = new std::set<long double>();
	else if(className=="unsigned int")
		t = new std::set<unsigned int>();
	else if(className=="unsigned short")
		t = new std::set<unsigned short>();
	else if(className=="unsigned long")
		t = new std::set<unsigned long>();
	else if(className=="unsigned long long")
		t = new std::set<unsigned long long>();
	else if(className=="float")
		t = new std::set<float>();
	else if(className=="double")
		t = new std::set<double>();
	else if(className=="bool")
		t = new std::set<bool>();
	else if(className=="char")
		t = new std::set<char>();
	else if(className=="unsigned char")
		t = new std::set<unsigned char>();
	else
	{
		processClassName(className);
		return _unserContainer(unserableObject,className,appName,"set",base);
	}
	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, var, className, t, "std::set");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializelist(void* unserableObject, const std::string& app, int &size, SerializeBase* base, const std::string& classN)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::list<std::string>();
	else if(className=="int")
		t = new std::list<int>();
	else if(className=="short")
		t = new std::list<short>();
	else if(className=="long")
		t = new std::list<long>();
	else if(className=="long long")
		t = new std::list<long long>();
	else if(className=="long double")
		t = new std::list<long double>();
	else if(className=="unsigned int")
		t = new std::list<unsigned int>();
	else if(className=="unsigned short")
		t = new std::list<unsigned short>();
	else if(className=="unsigned long")
		t = new std::list<unsigned long>();
	else if(className=="unsigned long long")
		t = new std::list<unsigned long long>();
	else if(className=="float")
		t = new std::list<float>();
	else if(className=="double")
		t = new std::list<double>();
	else if(className=="bool")
		t = new std::list<bool>();
	else if(className=="char")
		t = new std::list<char>();
	else if(className=="unsigned char")
		t = new std::list<unsigned char>();
	else
	{
		processClassName(className);
		return _unserContainer(unserableObject,className,appName,"list",base);
	}
	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, var, className, t, "std::list");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializeq(void* unserableObject, const std::string& app, int &size, SerializeBase* base, const std::string& classN)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::queue<std::string>();
	else if(className=="int")
		t = new std::queue<int>();
	else if(className=="short")
		t = new std::queue<short>();
	else if(className=="long")
		t = new std::queue<long>();
	else if(className=="long long")
		t = new std::queue<long long>();
	else if(className=="long double")
		t = new std::queue<long double>();
	else if(className=="unsigned int")
		t = new std::queue<unsigned int>();
	else if(className=="unsigned short")
		t = new std::queue<unsigned short>();
	else if(className=="unsigned long")
		t = new std::queue<unsigned long>();
	else if(className=="unsigned long long")
		t = new std::queue<unsigned long long>();
	else if(className=="float")
		t = new std::queue<float>();
	else if(className=="double")
		t = new std::queue<double>();
	else if(className=="bool")
		t = new std::queue<bool>();
	else if(className=="char")
		t = new std::queue<char>();
	else if(className=="unsigned char")
		t = new std::queue<unsigned char>();
	else
	{
		processClassName(className);
		return _unserContainer(unserableObject,className,appName,"queue",base);
	}
	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, var, className, t, "std::queue");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializevec(void* unserableObject, const std::string& app, int &size, SerializeBase* base, const std::string& classN)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::vector<std::string>();
	else if(className=="int")
		t = new std::vector<int>();
	else if(className=="short")
		t = new std::vector<short>();
	else if(className=="long")
		t = new std::vector<long>();
	else if(className=="long long")
		t = new std::vector<long long>();
	else if(className=="long double")
		t = new std::vector<long double>();
	else if(className=="unsigned int")
		t = new std::vector<unsigned int>();
	else if(className=="unsigned short")
		t = new std::vector<unsigned short>();
	else if(className=="unsigned long")
		t = new std::vector<unsigned long>();
	else if(className=="unsigned long long")
		t = new std::vector<unsigned long long>();
	else if(className=="float")
		t = new std::vector<float>();
	else if(className=="double")
		t = new std::vector<double>();
	else if(className=="bool")
		t = new std::vector<bool>();
	else if(className=="char")
		t = new std::vector<char>();
	else if(className=="unsigned char")
		t = new std::vector<unsigned char>();
	else
	{
		processClassName(className);
		return _unserContainer(unserableObject,className,appName,"vector",base);
	}
	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, var, className, t, "std::vector");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializedq(void* unserableObject, const std::string& app, int &size, SerializeBase* base, const std::string& classN)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::deque<std::string>();
	else if(className=="int")
		t = new std::deque<int>();
	else if(className=="short")
		t = new std::deque<short>();
	else if(className=="long")
		t = new std::deque<long>();
	else if(className=="long long")
		t = new std::deque<long long>();
	else if(className=="long double")
		t = new std::deque<long double>();
	else if(className=="unsigned int")
		t = new std::deque<unsigned int>();
	else if(className=="unsigned short")
		t = new std::deque<unsigned short>();
	else if(className=="unsigned long")
		t = new std::deque<unsigned long>();
	else if(className=="unsigned long long")
		t = new std::deque<unsigned long long>();
	else if(className=="float")
		t = new std::deque<float>();
	else if(className=="double")
		t = new std::deque<double>();
	else if(className=="bool")
		t = new std::deque<bool>();
	else if(className=="char")
		t = new std::deque<char>();
	else if(className=="unsigned char")
		t = new std::deque<unsigned char>();
	else
	{
		processClassName(className);
		return _unserContainer(unserableObject,className,appName,"deque",base);
	}
	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, var, className, t, "std::deque");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::unserializemultiset(void* unserableObject, const std::string& app, int &size, SerializeBase* base, const std::string& classN)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string className = base->getConatinerElementClassName(unserableObject, classN);
	StringUtil::trim(className);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::multiset<std::string>();
	else if(className=="int")
		t = new std::multiset<int>();
	else if(className=="short")
		t = new std::multiset<short>();
	else if(className=="long")
		t = new std::multiset<long>();
	else if(className=="long long")
		t = new std::multiset<long long>();
	else if(className=="long double")
		t = new std::multiset<long double>();
	else if(className=="unsigned int")
		t = new std::multiset<unsigned int>();
	else if(className=="unsigned short")
		t = new std::multiset<unsigned short>();
	else if(className=="unsigned long")
		t = new std::multiset<unsigned long>();
	else if(className=="unsigned long long")
		t = new std::multiset<unsigned long long>();
	else if(className=="float")
		t = new std::multiset<float>();
	else if(className=="double")
		t = new std::multiset<double>();
	else if(className=="bool")
		t = new std::multiset<bool>();
	else if(className=="char")
		t = new std::multiset<char>();
	else if(className=="unsigned char")
		t = new std::multiset<unsigned char>();
	else
	{
		processClassName(className);
		return _unserContainer(unserableObject,className,appName,"multiset",base);
	}
	if(t!=NULL)
	{
		if(unserableObject!=NULL)
		{
			int totsize = base->getContainerSize(unserableObject);
			for (int var = 0; var < totsize; var++)
			{
				size++;
				base->addPrimitiveElementToContainer(unserableObject, var, className, t, "std::multiset");
			}
		}
		return t;
	}
	return NULL;
}

void* SerializeBase::_unserContainer(void* unserableObject, const std::string& className, const std::string& app, const std::string& type, SerializeBase* base)
{
	std::string appName = CommonUtils::getAppName(app);
	void* obj = NULL;
	std::string libName = INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		exit(-1);
	}
	std::string methodname = base->getSerializationMethodName(className,appName,false,type);
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (void*, SerializeBase*, const std::string&);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(unserableObject, base, type);
	}
	return obj;
}

void* SerializeBase::_unser(void* unserableObject, const std::string& className, const std::string& app, SerializeBase* base)
{
	std::string appName = CommonUtils::getAppName(app);
	void* obj = NULL;
	std::string libName = INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		exit(-1);
	}
	std::string methodname = base->getSerializationMethodName(className,appName,false);
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (void*, SerializeBase*);
	RfPtr f = (RfPtr)mkr;
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

std::string SerializeBase::getSerializationMethodName(const std::string& className, const std::string& app, const bool& which, const std::string& type)
{
	std::string appName = CommonUtils::getAppName(app);
	std::string methodname;
	if(which)
		methodname = appName + "serialize" + className + (type==""?"":"Container");
	else
		methodname = appName + "unSerialize" + className + (type==""?"":"Container");
	if(type=="set" || type=="std::set" || type=="multiset" || type=="std::multiset") {
		methodname += "SV";
	}
	return methodname;
}
