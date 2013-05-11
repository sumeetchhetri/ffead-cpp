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
	static string _handleAllSerialization(string className,void *t, string appName);
	static void* _handleAllUnSerialization(string json,string className, string appName);
	static string _ser(void* t,string classN, string appName);
	static string _serContainers(void* t,string classN,string type, string appName);
	static string _ser(Object, string appName);
	static void* _unser(string json,string classN, string appName);
	static void* unserializeConatiner(string json, string className,string type, string appName);

	static void* _unserCont(string json,string className,string type, string appName);

	template <class T> static string serializevec(vector<T> t, string appName)
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

	template <class T> static string serializelist(list<T> t, string appName)
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

	template <class T> static string serializeset(set<T> t, string appName)
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

	template <class T> static string serializemultiset(multiset<T> t, string appName)
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

	template <class T> static string serializeq(std::queue<T> t, string appName)
	{
		std::queue<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string json = "[";
		while(cnt++<(int)t.size())
		{
			json += serialize<T>(st.front(),appName);
			if(cnt!=(int)t.size())
				json += ",";
			st.pop();
		}
		json += "]";
		return json;
	}

	template <class T> static string serializedq(deque<T> t, string appName)
	{
		deque<T> st = t;
		T td;
		int cnt = 0;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		string json = "[";
		while(cnt++<(int)t.size())
		{
			json += serialize<T>(*st.begin(),appName);
			if(cnt!=(int)t.size())
				json += ",";
			st.erase(st.begin());
		}
		json += "]";
		return json;
	}

	template <class T> static void* unserContainer(vector<T> &t, string type, string appName)
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

	template <class T> static string serialize(T t, string appName = "default")
	{
		string json;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return _handleAllSerialization(className,&t,appName);
	}

	static string serializeObject(Object t, string appName = "default")
	{
		return _handleAllSerialization(t.getTypeName(),t.getVoidPointer(),appName);
	}
	static string serializeUnknown(void* t,string className, string appName = "default")
	{
		return _handleAllSerialization(className,t,appName);
	}

	template <class K,class V> static string serialize(map<K,V> mp, string appName = "default")
	{
		map<K,V> mpt  = mp;
		string json;
		json = "[";
		int cnt = 0;
		while (cnt++<mp.size())
		{
			json += "{\"key\":";
			json += serialize<K>(mpt.begin()->first,appName);
			json += ",\"value\":";
			json += serialize<V>(mpt.begin()->second,appName);
			mpt.erase(mpt.begin());
			json += "}";
			if(cnt!=(int)mp.size())
				json += ",";
		}
		json += "]";
		return json;
	}
	template <class K,class V> static string serialize(multimap<K,V> mp, string appName = "default")
	{
		multimap<K,V> mpt  = mp;
		string json;
		json = "[";
		int cnt = 0;
		while (cnt++<mp.size())
		{
			json += "{\"key\":";
			json += serialize<K>(mpt.begin()->first,appName);
			json += ",\"value\":";
			json += serialize<V>(mpt.begin()->second,appName);
			mpt.erase(mpt.begin());
			json += "}";
			if(cnt!=(int)mp.size())
				json += ",";
		}
		json += "]";
		return json;
	}

	template <class T> static T unserialize(string json, string appName = "default")
	{
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return *(T*)_handleAllUnSerialization(json,className,appName);
	}
	static void* unSerializeUnknown(string json,string className, string appName = "default")
	{
		return _handleAllUnSerialization(json,className,appName);
	}
};

#endif /* JSONSERIALIZE_H_ */
