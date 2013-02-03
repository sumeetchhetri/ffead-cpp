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
#ifndef XMLSERIALIZABLE_H_
#define XMLSERIALIZABLE_H_
#include "XmlParser.h"
#include "CastUtil.h"
#include <stdexcept>
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
#include "stack"
#include "DateFormat.h"
#include "BinaryData.h"
#include "Constants.h"
#include "AMEFResources.h"
#include "JSONUtil.h"

using namespace std;
typedef string (*SerPtr) (void*);
typedef void* (*UnSerPtr) (string);

class XMLSerialize {
	static string demangle(const char *mangled);
	static string getClassName(void* instance);
	static string _handleAllSerialization(string className,void *t);
	static void* _handleAllUnSerialization(string objXml,string className);
	static string _ser(void* t,string classN);
	static string _servec(void* t,string classN);
	static string _serlis(void* t,string classN);
	static string _serset(void* t,string classN);
	static string _sermultiset(void* t,string classN);
	static string _serq(void* t,string classN);
	static string _serdq(void* t,string classN);
	static string _ser(Object);
	static void* _unser(string objXml,string classN);
	static void* unserializevec(string objXml);
	static void* unserializelist(string objXml);
	static void* unserializeset(string objXml);
	static void* unserializemultiset(string objXml);
	static void* unserializeq(string objXml);
	static void* unserializedq(string objXml);

	static void* _unserVec(string objXml,string className);
	static void* _unserLis(string objXml,string className);
	static void* _unserQ(string objXml,string className);
	static void* _unserDq(string objXml,string className);
	static void* _unserSet(string objXml,string className);
	static void* _unserMulSet(string objXml,string className);

	template <class T> static string serializeset(set<T> t)
	{
		set<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string objXml = "<set-"+className+">";
		while(cnt++<t.size())
		{
			objXml += serialize<T>(*st.begin());
			st.erase(st.begin());
		}
		objXml += "</set-"+className+">";
		return objXml;
	}

	template <class T> static string serializemultiset(multiset<T> t)
	{
		multiset<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string objXml = "<multiset-"+className+">";
		while(cnt++<t.size())
		{
			objXml += serialize<T>(*st.begin());
			st.erase(st.begin());
		}
		objXml += "</multiset-"+className+">";
		return objXml;
	}

	template <class T> static string serializevec(vector<T> t)
	{
		vector<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string objXml = "<vector-"+className+">";
		while(cnt++<t.size())
		{
			objXml += serialize<T>(*st.begin());
			st.erase(st.begin());
		}
		objXml += "</vector-"+className+">";
		return objXml;
	}
	template <class T> static string serializedq(deque<T> t)
	{
		deque<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string objXml = "<deque-"+className+">";
		while(cnt++<t.size())
		{
			objXml += serialize<T>(*st.begin());
			st.erase(st.begin());
		}
		objXml += "</deque-"+className+">";
		return objXml;
	}
	template <class T> static string serializelist(list<T> t)
	{
		list<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		list<string>::iterator it;
		string objXml = "<list-"+className+">";
		while(cnt++<t.size())
		{
			objXml += serialize<T>(*st.begin());
			st.erase(st.begin());
		}
		objXml += "</list-"+className+">";
		return objXml;
	}
	template <class T> static string serializeq(std::queue<T> t)
	{
		std::queue<T> tt = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string objXml = "<queue-"+className+">";
		if(!tt.empty())
		{
			for(int var=0;var<(int)tt.size();var++)
			{
				objXml += serialize<T>(tt.front());
				tt.pop();
			}
		}
		objXml += "</queue-"+className+">";
		return objXml;
	}

public:
	XMLSerialize(){}
	~XMLSerialize(){}

	template <class T> static string serialize(T t)
	{
		string objXml;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return _handleAllSerialization(className,&t);
	}

	static string serializeObject(Object t)
	{
		return _handleAllSerialization(t.getTypeName(),t.getVoidPointer());
	}
	static string serializeUnknown(void* t,string className)
	{
		return _handleAllSerialization(className,t);
	}

	template <class K,class V> static string serialize(map<K,V> mp)
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
	template <class K,class V> static string serialize(multimap<K,V> mp)
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

	template <class T> static T unserialize(string objXml)
	{
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return *(T*)_handleAllUnSerialization(objXml,className);
	}
	static void* unSerializeUnknown(string objXml,string className)
	{
		return _handleAllUnSerialization(objXml,className);
	}
};
#endif
