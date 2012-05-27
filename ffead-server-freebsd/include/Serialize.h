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
#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_
#include "XmlParser.h"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
//#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>
#include "string"
#include <sstream>
#include <typeinfo>
#include "Object.h"
#include "queue"
#include "deque"
#include "list"
#include "map"
#include "set"
#include "DateFormat.h"
#include "BinaryData.h"

using namespace std;
typedef string (*SerPtr) (void*);
typedef void* (*UnSerPtr) (string);
class Serialize{
	string demangle(const char *mangled);
	string getClassName(void* instance);
	string _hanldeAllSerialization(string className,void *t);
	void* _hanldeAllUnSerialization(string objXml,string className);
	string _ser(void* t,string classN);
	string _servec(void* t,string classN);
	string _serlis(void* t,string classN);
	string _serset(void* t,string classN);
	string _sermultiset(void* t,string classN);
	string _serq(void* t,string classN);
	string _serdq(void* t,string classN);
	string _ser(Object);
	void* _unser(string objXml,string classN);

	string serializevec(vector<string> *t);
	string serializevec(vector<int> *t);
	string serializevec(vector<float> *t);
	string serializevec(vector<double> *t);

	string serializeset(set<string> *t);
	string serializeset(set<int> *t);
	string serializeset(set<float> *t);
	string serializeset(set<double> *t);

	string serializemultiset(multiset<string> *t);
	string serializemultiset(multiset<int> *t);
	string serializemultiset(multiset<float> *t);
	string serializemultiset(multiset<double> *t);

	string serializelist(list<string> *t);
	string serializelist(list<int> *t);
	string serializelist(list<float> *t);
	string serializelist(list<double> *t);

	string serializeq(queue<string> *t);
	string serializeq(queue<int> *t);
	string serializeq(queue<float> *t);
	string serializeq(queue<double> *t);

	string serializedq(deque<string> *t);
	string serializedq(deque<int> *t);
	string serializedq(deque<float> *t);
	string serializedq(deque<double> *t);

	void* _unserVec(string objXml,string className);
	void* _unserLis(string objXml,string className);
	void* _unserQ(string objXml,string className);
	void* _unserDq(string objXml,string className);
	void* _unserSet(string objXml,string className);
	void* _unserMulSet(string objXml,string className);
public:
	Serialize(){}
	~Serialize(){}
	string serialize(string t);
	string serialize(int t);
	string serialize(float t);
	string serialize(double t);
	string serialize(bool t);

	template <class K,class V> string serialize(map<K,V> mp)
	{
		map<K,V> mpt  = mp;
		string objXml;
		objXml = "<map>";
		while (mpt.begin()!=mpt.end())
		{
			objXml += "<pair>";
			objXml += serialize<K>(mpt.begin()->first);
			objXml += serialize<V>(mpt.begin()->second);
			mpt.erase(mpt.begin());
			objXml += "</pair>";
		}
		objXml += "</map>";
		return objXml;
	}
	template <class K,class V> string serialize(multimap<K,V> mp)
	{
		multimap<K,V> mpt  = mp;
		string objXml;
		objXml = "<multimap>";
		while (mpt.begin()!=mpt.end())
		{
			objXml += "<pair>";
			objXml += serialize<K>(mpt.begin()->first);
			objXml += serialize<V>(mpt.begin()->second);
			mpt.erase(mpt.begin());
			objXml += "</pair>";
		}
		objXml += "</multimap>";
		return objXml;
	}
	template <class T> string serialize(T t)
	{
		string objXml;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return _hanldeAllSerialization(className,&t);
	}
	string serializeObject(Object t)
	{
		return _hanldeAllSerialization(t.getTypeName(),t.getVoidPointer());
	}
	string serializeUnknown(void* t,string className)
	{
		return _hanldeAllSerialization(className,t);
	}

	template <class T> T unserialize(string objXml)
	{
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return *(T*)_hanldeAllUnSerialization(objXml,className);
	}
	void* unSerializeUnknown(string objXml,string className)
	{
		return _hanldeAllUnSerialization(objXml,className);
	}
};
#endif
