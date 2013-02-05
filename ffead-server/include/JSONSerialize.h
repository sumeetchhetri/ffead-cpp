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
 * JSONSerialize.h
 *
 *  Created on: 27-Jan-2013
 *      Author: sumeetc
 */

#ifndef JSONSERIALIZE_H_
#define JSONSERIALIZE_H_
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
#include <algorithm>
#include "DateFormat.h"
#include "BinaryData.h"
#include "Constants.h"
#include "AMEFResources.h"
#include "JSONUtil.h"

class JSONSerialize {
	static string demangle(const char *mangled);
	static string getClassName(void* instance);
	static string _handleAllSerialization(string className,void *t);
	static void* _handleAllUnSerialization(string json,string className);
	static string _ser(void* t,string classN);
	static string _serContainers(void* t,string classN,string type);
	static string _ser(Object);
	static void* _unser(string json,string classN);
	static void* unserializeConatiner(string json, string className,string type);

	static void* _unserCont(string json,string className,string type);

	template <class T> static string serializevec(vector<T> t)
	{
		vector<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string json = "[";
		while(cnt++<(int)t.size())
		{
			json += serialize<T>(*st.begin());
			if(cnt!=(int)t.size())
				json += ",";
			st.erase(st.begin());
		}
		json += "]";
		return json;
	}

	template <class T> static string serializelist(list<T> t)
	{
		list<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string json = "[";
		while(cnt++<(int)t.size())
		{
			json += serialize<T>(*st.begin());
			if(cnt!=(int)t.size())
				json += ",";
			st.erase(st.begin());
		}
		json += "]";
		return json;
	}

	template <class T> static string serializeset(set<T> t)
	{
		set<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string json = "[";
		while(cnt++<(int)t.size())
		{
			json += serialize<T>(*st.begin());
			if(cnt!=(int)t.size())
				json += ",";
			st.erase(st.begin());
		}
		json += "]";
		return json;
	}

	template <class T> static string serializemultiset(multiset<T> t)
	{
		multiset<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string json = "[";
		while(cnt++<(int)t.size())
		{
			json += serialize<T>(*st.begin());
			if(cnt!=(int)t.size())
				json += ",";
			st.erase(st.begin());
		}
		json += "]";
		return json;
	}

	template <class T> static string serializeq(std::queue<T> t)
	{
		std::queue<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string json = "[";
		while(cnt++<(int)t.size())
		{
			json += serialize<T>(st.front());
			if(cnt!=(int)t.size())
				json += ",";
			st.pop();
		}
		json += "]";
		return json;
	}

	template <class T> static string serializedq(deque<T> t)
	{
		deque<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string json = "[";
		while(cnt++<(int)t.size())
		{
			json += serialize<T>(*st.begin());
			if(cnt!=(int)t.size())
				json += ",";
			st.erase(st.begin());
		}
		json += "]";
		return json;
	}

	template <class T> static void* unserContainer(vector<T> &t, string type)
	{
		if(type=="Lis")
		{
			list<T>* tt = new list<T>;
			std::copy(t.begin(), t.end(), std::back_inserter(*tt));
			return tt;
		}
		else if(type=="Set")
		{
			set<T>* tt = new set<T>;
			std::copy(t.begin(), t.end(), std::inserter(*tt, tt->begin()));
			return tt;
		}
		else if(type=="MulSet")
		{
			multiset<T>* tt = new multiset<T>;
			std::copy(t.begin(), t.end(), std::inserter(*tt, tt->begin()));
			return tt;
		}
		else if(type=="Dq")
		{
			deque<T>* tt = new deque<T>;
			std::copy(t.begin(), t.end(), std::inserter(*tt, tt->begin()));
			return tt;
		}
		else if(type=="Q")
		{
			std::queue<T>* qq = new std::queue<T>;
			for (int var = 0; var < (int)t.size(); ++var) {
				qq->push(t.at(var));
			}
			return qq;
		}
		else
			return &t;
	}

public:
	JSONSerialize(){}
	~JSONSerialize(){}

	template <class T> static string serialize(T t)
	{
		string json;
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
		string json;
		json = "[";
		int cnt = 0;
		while (cnt++<mp.size())
		{
			json += "{\"key\":";
			json += serialize<K>(mpt.begin()->first);
			json += ",\"value\":";
			json += serialize<V>(mpt.begin()->second);
			mpt.erase(mpt.begin());
			json += "}";
			if(cnt!=(int)mp.size())
				json += ",";
		}
		json += "]";
		return json;
	}
	template <class K,class V> static string serialize(multimap<K,V> mp)
	{
		multimap<K,V> mpt  = mp;
		string json;
		json = "[";
		int cnt = 0;
		while (cnt++<mp.size())
		{
			json += "{\"key\":";
			json += serialize<K>(mpt.begin()->first);
			json += ",\"value\":";
			json += serialize<V>(mpt.begin()->second);
			mpt.erase(mpt.begin());
			json += "}";
			if(cnt!=(int)mp.size())
				json += ",";
		}
		json += "]";
		return json;
	}

	template <class T> static T unserialize(string json)
	{
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return *(T*)_handleAllUnSerialization(json,className);
	}
	static void* unSerializeUnknown(string json,string className)
	{
		return _handleAllUnSerialization(json,className);
	}
};

#endif /* JSONSERIALIZE_H_ */
