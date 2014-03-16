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
	// TODO Auto-generated destructor stub
}

string SerializeBase::demangle(const char *mangled)
{
	int status;	char *demangled;
	using namespace abi;
	demangled = __cxa_demangle(mangled, NULL, 0, &status);
	string s(demangled);
	free(demangled);
	return s;
}
string SerializeBase::getClassName(void* instance)
{
	const char *mangled = typeid(instance).name();
	return demangle(mangled);
}

bool SerializeBase::isPrimitiveDataType(string type)
{
	StringUtil::trim(type);
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
		type = getTypeName(type);
		return true;
	}
	return false;
}

string SerializeBase::getTypeName(string type)
{
	StringUtil::trim(type);
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

void* SerializeBase::getNestedContainer(string& className)
{
	string container;
	if(className.find("std::vector<")==0)
	{
		container = "std::vector";
		StringUtil::replaceFirst(className,"std::vector<","");
	}
	else if(className.find("std::deque<")==0)
	{
		container = "std::deque";
		StringUtil::replaceFirst(className,"std::deque<","");
	}
	else if(className.find("std::list<")==0)
	{
		container = "std::list";
		StringUtil::replaceFirst(className,"std::list<","");
	}
	else if(className.find("std::set<")==0)
	{
		container = "std::set";
		StringUtil::replaceFirst(className,"std::set<","");
	}
	else if(className.find("std::multiset<")==0)
	{
		container = "std::multiset";
		StringUtil::replaceFirst(className,"std::multiset<","");
	}
	else if(className.find("std::queue<")==0)
	{
		container = "std::queue";
		StringUtil::replaceFirst(className,"std::queue<","");
	}

	string cn = className;
	if(className.find("std::vector<")==0)
	{
		return getNewNestedContainer<DummyVec>(container);
	}
	else if(className.find("std::deque<")==0)
	{
		return getNewNestedContainer<DummyDeque>(container);
	}
	else if(className.find("std::list<")==0)
	{
		return getNewNestedContainer<DummyList>(container);
	}
	else if(className.find("std::set<")==0)
	{
		return getNewNestedContainer<DummySet>(container);
	}
	else if(className.find("std::multiset<")==0)
	{
		return getNewNestedContainer<DummySet>(container);
	}
	else if(className.find("std::queue<")==0)
	{
		return getNewNestedContainer<DummyQueue>(container);
	}
	return NULL;
}

void SerializeBase::addToNestedContainer(void* roott, string className, string appName, int& lsiz, string container, void* cont, int var, SerializeBase* base)
{
	string cn = className;
	if(container.find("std::list<")==string::npos || (container.find("std::list<")==0 && cn.find("std::list<")!=0))
	{
		if(cn.find("std::vector<")==0)
		{
			DummyVec vec;
			void* unservec = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			vec._M_impl = ((DummyVec*)unservec)->_M_impl;
			addValueToNestedContainer(container, vec, cont);
		}
		else if(cn.find("std::deque<")==0)
		{
			DummyDeque deq;
			void* unserdeq = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			deq._M_impl = ((DummyDeque*)unserdeq)->_M_impl;
			addValueToNestedContainer(container, deq, cont);
		}
		else if(cn.find("std::list<")==0)
		{
			DummyList lis;
			void* unserlis = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			lis._M_impl = ((DummyList*)unserlis)->_M_impl;
			addValueToNestedContainer(container, lis, cont);
		}
		else if(cn.find("std::set<")==0)
		{
			DummySet sett;
			void* unsersett = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			sett._M_t = ((DummySet*)unsersett)->_M_t;
			addValueToNestedContainer(container, sett, cont);
		}
		else if(cn.find("std::multiset<")==0)
		{
			DummySet sett;
			void* unsersett = handleMultiLevelUnSerialization(roott, className, appName, lsiz, base);
			sett._M_t = ((DummySet*)unsersett)->_M_t;
			addValueToNestedContainer(container, sett, cont);
		}
	}
	else if(container.find("std::list<")==0 && cn.find("std::list<")==0)
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
			list<DummyList>::iterator it = ((list<DummyList>*)cont)->begin();
			for (int i = 0; i < var; ++i, ++it) {}
			temp->_M_next = (DummyList::_List_node_base*)&(*it);
			temp->_M_prev = (DummyList::_List_node_base*)&(*it);
		}
	}
}

string SerializeBase::_handleAllSerialization(string className, void *t, string appName, SerializeBase* base)
{
	string serVal;
	int level = StringUtil::countOccurrences(className, "<");
	if(isPrimitiveDataType(className))
	{
		serVal = base->serializePrimitive(className, t);
	}
	else if(className=="Date")
	{
		DateFormat formt("yyyy-mm-dd hh:mi:ss");
		string dateval = formt.format(*(Date*)t);
		serVal = base->serializePrimitive(className, &dateval);
	}
	else if(className=="BinaryData")
	{
		string binaryData = BinaryData::serilaize(*(BinaryData*)t);
		serVal = base->serializePrimitive(className, &binaryData);
	}
	else if(level>1)
	{
		serVal = handleMultiLevelSerialization(t, className, appName, base);
	}
	else if(className.find("std::vector<std::string,")!=string::npos || className.find("std::vector<string,")!=string::npos)
	{
		vector<string> *tt = (vector<string>*)t;
		serVal = serializevec<string>(*tt, appName, base);
	}
	else if(className.find("std::vector<char,")!=string::npos)
	{
		vector<char> *tt = (vector<char>*)t;
		serVal = serializevec<char>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned char,")!=string::npos)
	{
		vector<unsigned char> *tt = (vector<unsigned char>*)t;
		serVal = serializevec<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::vector<int,")!=string::npos)
	{
		vector<int> *tt = (vector<int>*)t;
		serVal = serializevec<int>(*tt, appName, base);
	}
	else if(className.find("std::vector<short,")!=string::npos)
	{
		vector<short> *tt = (vector<short>*)t;
		serVal = serializevec<short>(*tt, appName, base);
	}
	else if(className.find("std::vector<long,")!=string::npos)
	{
		vector<long> *tt = (vector<long>*)t;
		serVal = serializevec<long>(*tt, appName, base);
	}
	else if(className.find("std::vector<long long,")!=string::npos)
	{
		vector<long long> *tt = (vector<long long>*)t;
		serVal = serializevec<long long>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned int,")!=string::npos)
	{
		vector<unsigned int> *tt = (vector<unsigned int>*)t;
		serVal = serializevec<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned short,")!=string::npos)
	{
		vector<unsigned short> *tt = (vector<unsigned short>*)t;
		serVal = serializevec<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned long,")!=string::npos)
	{
		vector<unsigned long> *tt = (vector<unsigned long>*)t;
		serVal = serializevec<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::vector<unsigned long long,")!=string::npos)
	{
		vector<unsigned long long> *tt = (vector<unsigned long long>*)t;
		serVal = serializevec<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::vector<double,")!=string::npos)
	{
		vector<double> *tt = (vector<double>*)t;
		serVal = serializevec<double>(*tt, appName, base);
	}
	else if(className.find("std::vector<long double,")!=string::npos)
	{
		vector<long double> *tt = (vector<long double>*)t;
		serVal = serializevec<long double>(*tt, appName, base);
	}
	else if(className.find("std::vector<float,")!=string::npos)
	{
		vector<float> *tt = (vector<float>*)t;
		serVal = serializevec<float>(*tt, appName, base);
	}
	else if(className.find("std::vector<bool,")!=string::npos)
	{
		vector<bool> *tt = (vector<bool>*)t;
		serVal = serializevec<bool>(*tt, appName, base);
	}
	else if(className.find("std::vector<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::vector<","");
		string vtyp = className.substr(0,className.find(","));
		serVal = _serContainer(t, vtyp, appName, "Vec", base);
	}
	else if(className.find("std::list<std::string,")!=string::npos || className.find("std::list<string,")!=string::npos)
	{
		list<string> *tt = (list<string>*)t;
		serVal = serializelist<string>(*tt, appName, base);
	}
	else if(className.find("std::list<char,")!=string::npos)
	{
		list<char> *tt = (list<char>*)t;
		serVal = serializelist<char>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned char,")!=string::npos)
	{
		list<unsigned char> *tt = (list<unsigned char>*)t;
		serVal = serializelist<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::list<int,")!=string::npos)
	{
		list<int> *tt = (list<int>*)t;
		serVal = serializelist<int>(*tt, appName, base);
	}
	else if(className.find("std::list<long,")!=string::npos)
	{
		list<long> *tt = (list<long>*)t;
		serVal = serializelist<long>(*tt, appName, base);
	}
	else if(className.find("std::list<short,")!=string::npos)
	{
		list<short> *tt = (list<short>*)t;
		serVal = serializelist<short>(*tt, appName, base);
	}
	else if(className.find("std::list<long long,")!=string::npos)
	{
		list<long long> *tt = (list<long long>*)t;
		serVal = serializelist<long long>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned int,")!=string::npos)
	{
		list<unsigned int> *tt = (list<unsigned int>*)t;
		serVal = serializelist<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned long,")!=string::npos)
	{
		list<unsigned long> *tt = (list<unsigned long>*)t;
		serVal = serializelist<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned short,")!=string::npos)
	{
		list<unsigned short> *tt = (list<unsigned short>*)t;
		serVal = serializelist<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::list<unsigned long long,")!=string::npos)
	{
		list<unsigned long long> *tt = (list<unsigned long long>*)t;
		serVal = serializelist<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::list<double,")!=string::npos)
	{
		list<double> *tt = (list<double>*)t;
		serVal = serializelist<double>(*tt, appName, base);
	}
	else if(className.find("std::list<long double,")!=string::npos)
	{
		list<long double> *tt = (list<long double>*)t;
		serVal = serializelist<long double>(*tt, appName, base);
	}
	else if(className.find("std::list<float,")!=string::npos)
	{
		list<float> *tt = (list<float>*)t;
		serVal = serializelist<float>(*tt, appName, base);
	}
	else if(className.find("std::list<bool,")!=string::npos)
	{
		list<bool> *tt = (list<bool>*)t;
		serVal = serializelist<bool>(*tt, appName, base);
	}
	else if(className.find("std::list<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::list<","");
		string vtyp = className.substr(0,className.find(","));
		serVal = _serContainer(t, vtyp, appName, "Lis", base);
	}
	else if(className.find("std::set<std::string,")!=string::npos || className.find("std::set<string,")!=string::npos)
	{
		set<string> *tt = (set<string>*)t;
		serVal = serializeset<string>(*tt, appName, base);
	}
	else if(className.find("std::set<char,")!=string::npos)
	{
		set<char> *tt = (set<char>*)t;
		serVal = serializeset<char>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned char,")!=string::npos)
	{
		set<unsigned char> *tt = (set<unsigned char>*)t;
		serVal = serializeset<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::set<int,")!=string::npos)
	{
		set<int> *tt = (set<int>*)t;
		serVal = serializeset<int>(*tt, appName, base);
	}
	else if(className.find("std::set<short,")!=string::npos)
	{
		set<short> *tt = (set<short>*)t;
		serVal = serializeset<short>(*tt, appName, base);
	}
	else if(className.find("std::set<long,")!=string::npos)
	{
		set<long> *tt = (set<long>*)t;
		serVal = serializeset<long>(*tt, appName, base);
	}
	else if(className.find("std::set<long long,")!=string::npos)
	{
		set<long long> *tt = (set<long long>*)t;
		serVal = serializeset<long long>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned int,")!=string::npos)
	{
		set<unsigned int> *tt = (set<unsigned int>*)t;
		serVal = serializeset<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned short,")!=string::npos)
	{
		set<unsigned short> *tt = (set<unsigned short>*)t;
		serVal = serializeset<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned long,")!=string::npos)
	{
		set<unsigned long> *tt = (set<unsigned long>*)t;
		serVal = serializeset<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::set<unsigned long long,")!=string::npos)
	{
		set<unsigned long long> *tt = (set<unsigned long long>*)t;
		serVal = serializeset<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::set<long double,")!=string::npos)
	{
		set<long double> *tt = (set<long double>*)t;
		serVal = serializeset<long double>(*tt, appName, base);
	}
	else if(className.find("std::set<double,")!=string::npos)
	{
		set<double> *tt = (set<double>*)t;
		serVal = serializeset<double>(*tt, appName, base);
	}
	else if(className.find("std::set<float,")!=string::npos)
	{
		set<float> *tt = (set<float>*)&t;
		serVal = serializeset<float>(*tt, appName, base);
	}
	else if(className.find("std::set<bool,")!=string::npos)
	{
		set<bool> *tt = (set<bool>*)&t;
		serVal = serializeset<bool>(*tt, appName, base);
	}
	else if(className.find("std::set<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::set<","");
		string vtyp = className.substr(0,className.find(","));
		serVal = _serContainer(t, vtyp, appName, "Set", base);
	}
	else if(className.find("std::multiset<std::string,")!=string::npos || className.find("std::multiset<string,")!=string::npos)
	{
		multiset<string> *tt = (multiset<string>*)t;
		serVal = serializemultiset<string>(*tt, appName, base);
	}
	else if(className.find("std::multiset<char,")!=string::npos)
	{
		multiset<char> *tt = (multiset<char>*)t;
		serVal = serializemultiset<char>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned char,")!=string::npos)
	{
		multiset<unsigned char> *tt = (multiset<unsigned char>*)t;
		serVal = serializemultiset<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::multiset<int,")!=string::npos)
	{
		multiset<int> *tt = (multiset<int>*)t;
		serVal = serializemultiset<int>(*tt, appName, base);
	}
	else if(className.find("std::multiset<long,")!=string::npos)
	{
		multiset<long> *tt = (multiset<long>*)t;
		serVal = serializemultiset<long>(*tt, appName, base);
	}
	else if(className.find("std::multiset<long long,")!=string::npos)
	{
		multiset<long long> *tt = (multiset<long long>*)t;
		serVal = serializemultiset<long long>(*tt, appName, base);
	}
	else if(className.find("std::multiset<short,")!=string::npos)
	{
		multiset<short> *tt = (multiset<short>*)t;
		serVal = serializemultiset<short>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned int,")!=string::npos)
	{
		multiset<unsigned int> *tt = (multiset<unsigned int>*)t;
		serVal = serializemultiset<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned long,")!=string::npos)
	{
		multiset<unsigned long> *tt = (multiset<unsigned long>*)t;
		serVal = serializemultiset<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::multiset<long long,")!=string::npos)
	{
		multiset<unsigned long long> *tt = (multiset<unsigned long long>*)t;
		serVal = serializemultiset<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::multiset<unsigned short,")!=string::npos)
	{
		multiset<unsigned short> *tt = (multiset<unsigned short>*)t;
		serVal = serializemultiset<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::multiset<double,")!=string::npos)
	{
		multiset<double> *tt = (multiset<double>*)t;
		serVal = serializemultiset<double>(*tt, appName, base);
	}
	else if(className.find("std::multiset<long double,")!=string::npos)
	{
		multiset<long double> *tt = (multiset<long double>*)t;
		serVal = serializemultiset<long double>(*tt, appName, base);
	}
	else if(className.find("std::multiset<float,")!=string::npos)
	{
		multiset<float> *tt = (multiset<float>*)t;
		serVal = serializemultiset<float>(*tt, appName, base);
	}
	else if(className.find("std::multiset<bool,")!=string::npos)
	{
		multiset<bool> *tt = (multiset<bool>*)t;
		serVal = serializemultiset<bool>(*tt, appName, base);
	}
	else if(className.find("std::multiset<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::multiset<","");
		string vtyp = className.substr(0,className.find(","));
		serVal = _serContainer(t, vtyp, appName, "MulSet", base);
	}
	else if(className.find("std::queue<std::string,")!=string::npos || className.find("std::queue<string,")!=string::npos)
	{
		std::queue<string> *tt = (std::queue<string>*)t;
		serVal = serializeq<string>(*tt, appName, base);
	}
	else if(className.find("std::queue<char,")!=string::npos)
	{
		std::queue<char> *tt = (std::queue<char>*)t;
		serVal = serializeq<char>(*tt, appName, base);
	}
	else if(className.find("std::queue<unsigned char,")!=string::npos)
	{
		std::queue<unsigned char> *tt = (std::queue<unsigned char>*)t;
		serVal = serializeq<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::queue<int,")!=string::npos)
	{
		std::queue<int> *tt = (std::queue<int>*)t;
		serVal = serializeq<int>(*tt, appName, base);
	}
	else if(className.find("std::queue<short,")!=string::npos)
	{
		std::queue<short> *tt = (std::queue<short>*)t;
		serVal = serializeq<short>(*tt, appName, base);
	}
	else if(className.find("std::queue<long,")!=string::npos)
	{
		std::queue<long> *tt = (std::queue<long>*)t;
		serVal = serializeq<long>(*tt, appName, base);
	}
	else if(className.find("std::std::queue<long long,")!=string::npos)
	{
		std::queue<long long> *tt = (std::queue<long long>*)t;
		serVal = serializeq<long long>(*tt, appName, base);
	}
	else if(className.find("std::std::queue<unsigned int,")!=string::npos)
	{
		std::queue<unsigned int> *tt = (std::queue<unsigned int>*)t;
		serVal = serializeq<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::std::queue<unsigned short,")!=string::npos)
	{
		std::queue<unsigned short> *tt = (std::queue<unsigned short>*)t;
		serVal = serializeq<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::std::queue<unsigned long,")!=string::npos)
	{
		std::queue<unsigned long> *tt = (std::queue<unsigned long>*)t;
		serVal = serializeq<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::std::queue<unsigned long long,")!=string::npos)
	{
		std::queue<unsigned long long> *tt = (std::queue<unsigned long long>*)t;
		serVal = serializeq<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::std::queue<long double,")!=string::npos)
	{
		std::queue<long double> *tt = (std::queue<long double>*)t;
		serVal = serializeq<long double>(*tt, appName, base);
	}
	else if(className.find("std::queue<double,")!=string::npos)
	{
		std::queue<double> *tt = (std::queue<double>*)t;
		serVal = serializeq<double>(*tt, appName, base);
	}
	else if(className.find("std::queue<float,")!=string::npos)
	{
		std::queue<float> *tt = (std::queue<float>*)t;
		serVal = serializeq<float>(*tt, appName, base);
	}
	else if(className.find("std::queue<bool,")!=string::npos)
	{
		std::queue<bool> *tt = (std::queue<bool>*)t;
		serVal = serializeq<bool>(*tt, appName, base);
	}
	else if(className.find("std::queue<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::queue<","");
		string vtyp = className.substr(0,className.find(","));
		serVal = _serContainer(t, vtyp, appName, "Q", base);
	}
	else if(className.find("std::deque<std::string,")!=string::npos || className.find("std::deque<string,")!=string::npos)
	{
		deque<string> *tt = (deque<string>*)t;
		serVal = serializedq<string>(*tt, appName, base);
	}
	else if(className.find("std::deque<char,")!=string::npos)
	{
		deque<char> *tt = (deque<char>*)t;
		serVal = serializedq<char>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned char,")!=string::npos)
	{
		deque<unsigned char> *tt = (deque<unsigned char>*)t;
		serVal = serializedq<unsigned char>(*tt, appName, base);
	}
	else if(className.find("std::deque<int,")!=string::npos)
	{
		deque<int> *tt = (deque<int>*)t;
		serVal = serializedq<int>(*tt, appName, base);
	}
	else if(className.find("std::deque<long,")!=string::npos)
	{
		deque<long> *tt = (deque<long>*)t;
		serVal = serializedq<long>(*tt, appName, base);
	}
	else if(className.find("std::deque<short,")!=string::npos)
	{
		deque<short> *tt = (deque<short>*)t;
		serVal = serializedq<short>(*tt, appName, base);
	}
	else if(className.find("std::deque<long long,")!=string::npos)
	{
		deque<long long> *tt = (deque<long long>*)t;
		serVal = serializedq<long long>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned int,")!=string::npos)
	{
		deque<unsigned int> *tt = (deque<unsigned int>*)t;
		serVal = serializedq<unsigned int>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned short,")!=string::npos)
	{
		deque<unsigned short> *tt = (deque<unsigned short>*)t;
		serVal = serializedq<unsigned short>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned long,")!=string::npos)
	{
		deque<unsigned long> *tt = (deque<unsigned long>*)t;
		serVal = serializedq<unsigned long>(*tt, appName, base);
	}
	else if(className.find("std::deque<unsigned long long,")!=string::npos)
	{
		deque<unsigned long long> *tt = (deque<unsigned long long>*)t;
		serVal = serializedq<unsigned long long>(*tt, appName, base);
	}
	else if(className.find("std::deque<long double,")!=string::npos)
	{
		deque<long double> *tt = (deque<long double>*)t;
		serVal = serializedq<long double>(*tt, appName, base);
	}
	else if(className.find("std::deque<double,")!=string::npos)
	{
		deque<double> *tt = (deque<double>*)t;
		serVal = serializedq<double>(*tt, appName, base);
	}
	else if(className.find("std::deque<float,")!=string::npos)
	{
		deque<float> *tt = (deque<float>*)t;
		serVal = serializedq<float>(*tt, appName, base);
	}
	else if(className.find("std::deque<bool,")!=string::npos)
	{
		deque<bool> *tt = (deque<bool>*)t;
		serVal = serializedq<bool>(*tt, appName, base);
	}
	else if(className.find("std::deque<")!=string::npos)
	{
		StringUtil::replaceFirst(className,"std::deque<","");
		string vtyp = className.substr(0,className.find(","));
		serVal = _serContainer(t, vtyp, appName, "Dq", base);
	}
	else
	{
		return _ser(t,className,appName,base);
	}
	return serVal;
}

string SerializeBase::handleMultiLevelSerialization(void* t, string className, string appName, SerializeBase* base)
{
	void* object = base->getSerializableObject();
	int cnt = 0;
	if(className.find("std::vector<")==0) {
		StringUtil::replaceFirst(className,"std::vector<","");
		base->startContainerSerialization(object, className, "std::vector");
		if(className.find("<")==string::npos) {
			string serval = _handleAllSerialization("std::vector<"+className+",", t, appName, base);
			return serval;
		} else {
			if(className.find("std::vector<bool")==0)
			{
				vector<vector<bool> >* ptr = (vector<vector<bool> >*)t;
				vector<vector<bool> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					vector<bool>* la = (vector<bool>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::vector<")==0)
			{
				vector<vector<Dummy> >* ptr = (vector<vector<Dummy> >*)t;
				vector<vector<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					vector<Dummy>* la = (vector<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::deque<")==0)
			{
				vector<deque<Dummy> >* ptr = (vector<deque<Dummy> >*)t;
				vector<deque<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					deque<Dummy>* la = (deque<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::list<")==0)
			{
				vector<list<Dummy> >* ptr = (vector<list<Dummy> >*)t;
				vector<list<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					list<Dummy>* la = (list<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::set<")==0)
			{
				vector<set<Dummy> >* ptr = (vector<set<Dummy> >*)t;
				vector<set<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					set<Dummy>* la = (set<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::multiset<")==0)
			{
				vector<multiset<Dummy> >* ptr = (vector<multiset<Dummy> >*)t;
				vector<multiset<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					multiset<Dummy>* la = (multiset<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
		}
		base->endContainerSerialization(object, className, "std::vector");
	}
	else if(className.find("std::list<")==0) {
		StringUtil::replaceFirst(className,"std::list<","");
		base->startContainerSerialization(object, className, "std::list");
		if(className.find("<")==string::npos) {
			string serval = _handleAllSerialization("std::list<"+className+",", t, appName, base);
			return serval;
		} else {
			list<Dummy>* ptr = (list<Dummy>*)t;
			list<Dummy>::iterator itls = ptr->begin();
			for(;itls!=ptr->end();++itls) {
				list<Dummy>* la = (list<Dummy>*)&(*itls);
				string serval = handleMultiLevelSerialization(la, className, appName, base);
				base->addContainerSerializableElementMulti(object, serval);
				base->afterAddContainerSerializableElement(object, cnt++, 0);
			}
		}
		base->endContainerSerialization(object, className, "std::list");
	}
	else if(className.find("std::deque<")==0) {
		StringUtil::replaceFirst(className,"std::deque<","");
		base->startContainerSerialization(object, className, "std::deque");
		if(className.find("<")==string::npos) {
			string serval = _handleAllSerialization("std::deque<"+className+",", t, appName, base);
			return serval;
		} else {
			if(className.find("std::vector<bool")==0)
			{
				deque<vector<bool> >* ptr = (deque<vector<bool> >*)t;
				deque<vector<bool> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					vector<bool>* la = (vector<bool>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::vector<")==0)
			{
				deque<vector<Dummy> >* ptr = (deque<vector<Dummy> >*)t;
				deque<vector<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					vector<Dummy>* la = (vector<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::deque<")==0)
			{
				deque<deque<Dummy> >* ptr = (deque<deque<Dummy> >*)t;
				deque<deque<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					deque<Dummy>* la = (deque<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::list<")==0)
			{
				deque<list<Dummy> >* ptr = (deque<list<Dummy> >*)t;
				deque<list<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					list<Dummy>* la = (list<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::set<")==0)
			{
				deque<set<Dummy> >* ptr = (deque<set<Dummy> >*)t;
				deque<set<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					set<Dummy>* la = (set<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
			else if(className.find("std::multiset<")==0)
			{
				deque<multiset<Dummy> >* ptr = (deque<multiset<Dummy> >*)t;
				deque<multiset<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					multiset<Dummy>* la = (multiset<Dummy>*)&(*itls);
					string serval = handleMultiLevelSerialization(la, className, appName, base);
					base->addContainerSerializableElementMulti(object, serval);
					base->afterAddContainerSerializableElement(object, cnt++, 0);
				}
			}
		}
		base->endContainerSerialization(object, className, "std::deque");
	}
	else if(className.find("std::queue<")==0) {
		StringUtil::replaceFirst(className,"std::queue<","");
		base->startContainerSerialization(object, className, "std::queue");
		if(className.find("<")==string::npos) {
			string serval = _handleAllSerialization("std::queue<"+className+",", t, appName, base);
			return serval;
		} else {
			DummyQueue* dptr = (DummyQueue*)t;
			deque<deque<Dummy> >* ptr = (deque<deque<Dummy> >*)&dptr->dq;
			deque<deque<Dummy> >::iterator itls = ptr->begin();
			for(;itls!=ptr->end();++itls) {
				deque<deque<Dummy> >* la = (deque<deque<Dummy> >*)&(*itls);
				string serval = handleMultiLevelSerialization(la, className, appName, base);
				base->addContainerSerializableElementMulti(object, serval);
				base->afterAddContainerSerializableElement(object, cnt++, 0);
			}
		}
		base->endContainerSerialization(object, className, "std::queue");
	}
	else if(className.find("std::set<")==0) {
		StringUtil::replaceFirst(className,"std::set<","");
		base->startContainerSerialization(object, className, "std::set");
		if(className.find("<")==string::npos) {
			string serval = _handleAllSerialization("std::set<"+className+",", t, appName, base);
			return serval;
		} else {
			set<Dummy>* ptr = (set<Dummy>*)t;
			set<Dummy>::iterator itls = ptr->begin();
			for(;itls!=ptr->end();++itls) {
				set<Dummy>* la = (set<Dummy>*)&(*itls);
				string serval = handleMultiLevelSerialization(la, className, appName, base);
				base->addContainerSerializableElementMulti(object, serval);
				base->afterAddContainerSerializableElement(object, cnt++, 0);
			}
		}
		base->endContainerSerialization(object, className, "std::set");
	}
	else if(className.find("std::multiset<")==0) {
		StringUtil::replaceFirst(className,"std::multiset<","");
		base->startContainerSerialization(object, className, "std::multiset");
		if(className.find("<")==string::npos) {
			string serval = _handleAllSerialization("std::multiset<"+className+",", t, appName, base);
			return serval;
		} else {
			multiset<Dummy>* ptr = (multiset<Dummy>*)t;
			multiset<Dummy>::iterator itls = ptr->begin();
			for(;itls!=ptr->end();++itls) {
				multiset<Dummy>* la = (multiset<Dummy>*)&(*itls);
				string serval = handleMultiLevelSerialization(la, className, appName, base);
				base->addContainerSerializableElementMulti(object, serval);
				base->afterAddContainerSerializableElement(object, cnt++, 0);
			}
		}
		base->endContainerSerialization(object, className, "std::multiset");
	}
	string ser = base->fromSerializableObjectToString(object);
	base->cleanSerializableObject(object);
	return ser;
}

string SerializeBase::_serContainer(void* t, string className, string appName, string type, SerializeBase* base)
{
	StringUtil::replaceAll(className, "::", "_");
	string serVal;
	string libName = INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = base->getSerializationMethodName(className,appName,true,type);
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*, SerializeBase*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		serVal = f(t, base);
	return serVal;
}

string SerializeBase::_ser(void* t, string className, string appName, SerializeBase* base)
{
	StringUtil::replaceAll(className, "::", "_");
	string serVal;
	string libName = INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = base->getSerializationMethodName(className,appName,true);
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef string (*RfPtr) (void*, SerializeBase*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
		serVal = f(t, base);
	return serVal;
}

void* SerializeBase::_handleAllUnSerialization(string serVal, string className, string appName, SerializeBase* base, bool isJsonSer, void* serObject)
{
	void* unserObjectVal = NULL;
	void* intermediateObject = base->getUnserializableObject(serVal);
	if(intermediateObject==NULL || StringUtil::trimCopy(serVal)=="")
	{
		if(serObject==NULL)
			return NULL;
		base->cleanUnserializableObject(intermediateObject);
		intermediateObject = serObject;
	}
	string unserObjName = base->getUnserializableClassName(intermediateObject, className);
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
		else if(unserObjName.find("std::vector<")==0 || unserObjName.find("vector-")==0)
		{
			unserObjectVal = unserializevec(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::set<")==0 || unserObjName.find("set-")==0)
		{
			unserObjectVal = unserializeset(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::multiset<")==0 || unserObjName.find("multiset-")==0)
		{
			unserObjectVal = unserializemultiset(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::list<")==0 || unserObjName.find("list-")==0)
		{
			unserObjectVal = unserializelist(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::queue<")==0 || unserObjName.find("queue-")==0)
		{
			unserObjectVal = unserializeq(mintermediateObject,appName,sizet,base,className);
		}
		else if(unserObjName.find("std::deque<")==0 || unserObjName.find("deque-")==0)
		{
			unserObjectVal = unserializedq(mintermediateObject,appName,sizet,base,className);
		}
		else
		{
			unserObjectVal = _unser(mintermediateObject,className,appName,base);
		}
		if(isJsonSer && StringUtil::trimCopy(serVal)!="")
		{
			base->cleanValidUnserializableObject(mintermediateObject);
		}
	}
	if(StringUtil::trimCopy(serVal)!="" && serObject!=NULL)
	{
		base->cleanUnserializableObject(intermediateObject);
	}
	return unserObjectVal;
}

void* SerializeBase::handleMultiLevelUnSerialization(void* intermediateObject, string className, string appName, int& size, SerializeBase* base)
{
	size = 0;
	int level = StringUtil::countOccurrences(className, "<") - 1 ;
	void* tv = NULL;
	if(level>0)
	{
		string container;
		if(className.find("std::vector<")==0)
		{
			container = "std::vector<";
		}
		else if(className.find("std::deque<")==0)
		{
			container = "std::deque<";
		}
		else if(className.find("std::queue<")==0)
		{
			container = "std::queue<";
		}
		else if(className.find("std::list<")==0)
		{
			container = "std::list<";
		}
		else if(className.find("std::set<")==0)
		{
			container = "std::set<";
		}
		else if(className.find("std::multiset<")==0)
		{
			container = "std::multiset<";
		}
		vector<int> elesizes;
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
		if(container.find("std::vector<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			vector<DummyList>* vdl = (vector<DummyList>*)tv;
			vector<DummyList>::iterator it = vdl->begin();
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
		else if(container.find("std::deque<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			deque<DummyList>* vdl = (deque<DummyList>*)tv;
			deque<DummyList>::iterator it = vdl->begin();
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
		else if(container.find("std::set<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			set<DummyList>* vdl = (set<DummyList>*)tv;
			set<DummyList>::iterator it = vdl->begin();
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
		else if(container.find("std::multiset<")==0 && elesizes.size()>0 && className.find("std::list<")==0)
		{
			int counter = 0;
			multiset<DummyList>* vdl = (multiset<DummyList>*)tv;
			multiset<DummyList>::iterator it = vdl->begin();
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
			queue<DummyList>* vdl = (deque<DummyList>*)tv;
			queue<DummyList>::iterator it = vdl->begin();
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
		if(className.find("std::vector<")==0)
		{
			tv = unserializevec(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::list<")==0)
		{
			tv = unserializelist(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::deque<")==0)
		{
			tv = unserializedq(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::set<")==0)
		{
			tv = unserializeset(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::multiset<")==0)
		{
			tv = unserializemultiset(intermediateObject, appName, size, base, className);
		}
		else if(className.find("std::queue<")==0)
		{
			tv = unserializeq(intermediateObject, appName, size, base, className);
		}
	}
	return tv;
}

void* SerializeBase::unserializeset(void* unserableObject, string appName, int &size, SerializeBase* base, string classN)
{
	string className = base->getConatinerElementClassName(unserableObject, classN);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new set<string>();
	else if(className=="int")
		t = new set<int>();
	else if(className=="short")
		t = new set<short>();
	else if(className=="long")
		t = new set<long>();
	else if(className=="long long")
		t = new set<long long>();
	else if(className=="long double")
		t = new set<long double>();
	else if(className=="unsigned int")
		t = new set<unsigned int>();
	else if(className=="unsigned short")
		t = new set<unsigned short>();
	else if(className=="unsigned long")
		t = new set<unsigned long>();
	else if(className=="unsigned long long")
		t = new set<unsigned long long>();
	else if(className=="float")
		t = new set<float>();
	else if(className=="double")
		t = new set<double>();
	else if(className=="bool")
		t = new set<bool>();
	else if(className=="char")
		t = new set<char>();
	else if(className=="unsigned char")
		t = new set<unsigned char>();
	else
	{
		return _unserContainer(unserableObject,className,appName,"Set",base);
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

void* SerializeBase::unserializelist(void* unserableObject, string appName, int &size, SerializeBase* base, string classN)
{
	string className = base->getConatinerElementClassName(unserableObject, classN);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new list<string>();
	else if(className=="int")
		t = new list<int>();
	else if(className=="short")
		t = new list<short>();
	else if(className=="long")
		t = new list<long>();
	else if(className=="long long")
		t = new list<long long>();
	else if(className=="long double")
		t = new list<long double>();
	else if(className=="unsigned int")
		t = new list<unsigned int>();
	else if(className=="unsigned short")
		t = new list<unsigned short>();
	else if(className=="unsigned long")
		t = new list<unsigned long>();
	else if(className=="unsigned long long")
		t = new list<unsigned long long>();
	else if(className=="float")
		t = new list<float>();
	else if(className=="double")
		t = new list<double>();
	else if(className=="bool")
		t = new list<bool>();
	else if(className=="char")
		t = new list<char>();
	else if(className=="unsigned char")
		t = new list<unsigned char>();
	else
	{
		return _unserContainer(unserableObject,className,appName,"Lis",base);
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

void* SerializeBase::unserializeq(void* unserableObject, string appName, int &size, SerializeBase* base, string classN)
{
	string className = base->getConatinerElementClassName(unserableObject, classN);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new std::queue<string>();
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
		return _unserContainer(unserableObject,className,appName,"Q",base);
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

void* SerializeBase::unserializevec(void* unserableObject, string appName, int &size, SerializeBase* base, string classN)
{
	string className = base->getConatinerElementClassName(unserableObject, classN);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new vector<string>();
	else if(className=="int")
		t = new vector<int>();
	else if(className=="short")
		t = new vector<short>();
	else if(className=="long")
		t = new vector<long>();
	else if(className=="long long")
		t = new vector<long long>();
	else if(className=="long double")
		t = new vector<long double>();
	else if(className=="unsigned int")
		t = new vector<unsigned int>();
	else if(className=="unsigned short")
		t = new vector<unsigned short>();
	else if(className=="unsigned long")
		t = new vector<unsigned long>();
	else if(className=="unsigned long long")
		t = new vector<unsigned long long>();
	else if(className=="float")
		t = new vector<float>();
	else if(className=="double")
		t = new vector<double>();
	else if(className=="bool")
		t = new vector<bool>();
	else if(className=="char")
		t = new vector<char>();
	else if(className=="unsigned char")
		t = new vector<unsigned char>();
	else
	{
		return _unserContainer(unserableObject,className,appName,"Vec",base);
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

void* SerializeBase::unserializedq(void* unserableObject, string appName, int &size, SerializeBase* base, string classN)
{
	string className = base->getConatinerElementClassName(unserableObject, classN);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new deque<string>();
	else if(className=="int")
		t = new deque<int>();
	else if(className=="short")
		t = new deque<short>();
	else if(className=="long")
		t = new deque<long>();
	else if(className=="long long")
		t = new deque<long long>();
	else if(className=="long double")
		t = new deque<long double>();
	else if(className=="unsigned int")
		t = new deque<unsigned int>();
	else if(className=="unsigned short")
		t = new deque<unsigned short>();
	else if(className=="unsigned long")
		t = new deque<unsigned long>();
	else if(className=="unsigned long long")
		t = new deque<unsigned long long>();
	else if(className=="float")
		t = new deque<float>();
	else if(className=="double")
		t = new deque<double>();
	else if(className=="bool")
		t = new deque<bool>();
	else if(className=="char")
		t = new deque<char>();
	else if(className=="unsigned char")
		t = new deque<unsigned char>();
	else
	{
		return _unserContainer(unserableObject,className,appName,"Dq",base);
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

void* SerializeBase::unserializemultiset(void* unserableObject, string appName, int &size, SerializeBase* base, string classN)
{
	string className = base->getConatinerElementClassName(unserableObject, classN);
	void* t = NULL;
	if(className=="std::string" || className=="string")
		t = new multiset<string>();
	else if(className=="int")
		t = new multiset<int>();
	else if(className=="short")
		t = new multiset<short>();
	else if(className=="long")
		t = new multiset<long>();
	else if(className=="long long")
		t = new multiset<long long>();
	else if(className=="long double")
		t = new multiset<long double>();
	else if(className=="unsigned int")
		t = new multiset<unsigned int>();
	else if(className=="unsigned short")
		t = new multiset<unsigned short>();
	else if(className=="unsigned long")
		t = new multiset<unsigned long>();
	else if(className=="unsigned long long")
		t = new multiset<unsigned long long>();
	else if(className=="float")
		t = new multiset<float>();
	else if(className=="double")
		t = new multiset<double>();
	else if(className=="bool")
		t = new multiset<bool>();
	else if(className=="char")
		t = new multiset<char>();
	else if(className=="unsigned char")
		t = new multiset<unsigned char>();
	else
	{
		return _unserContainer(unserableObject,className,appName,"MulSet",base);
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

void* SerializeBase::_unserContainer(void* unserableObject, string className, string appName, string type, SerializeBase* base)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = base->getSerializationMethodName(className,appName,false,type);
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (void*, SerializeBase*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(unserableObject, base);
	}
	return obj;
}

void* SerializeBase::_unser(void* unserableObject, string className, string appName, SerializeBase* base)
{
	StringUtil::replaceAll(className, "::", "_");
	void* obj = NULL;
	string libName = INTER_LIB_FILE;
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string methodname = base->getSerializationMethodName(className,appName,false);
	void *mkr = dlsym(dlib, methodname.c_str());
	typedef void* (*RfPtr) (void*, SerializeBase*);
	RfPtr f = (RfPtr)mkr;
	if(f!=NULL)
	{
		obj = f(unserableObject, base);
	}
	return obj;
}

string SerializeBase::getTemplateArg(string s, string& tem)
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

	string tfrst;
	if(tem.find("<")!=string::npos)
		tfrst = tem.substr(tem.find("<")+1, tem.length());
	else
		tfrst = tem.substr(0, tem.length());
	if(tem.find(",")!=string::npos)
		tfrst = tfrst.substr(0, tfrst.find(",")+1);
	StringUtil::trim(tfrst);

	tem = s.substr(tem.length());
	tem = tem.substr(tem.find(",")+1);
	return tfrst;
}
