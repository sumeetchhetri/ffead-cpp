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
#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_
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

using namespace std;
typedef string (*SerPtr) (void*);
typedef void* (*UnSerPtr) (string);

class Serialize{
	static string demangle(const char *mangled);
	static string getClassName(void* instance);
	static string _hanldeAllSerialization(string className,void *t);
	static void* _hanldeAllUnSerialization(string objXml,string className);
	static string _ser(void* t,string classN);
	static string _servec(void* t,string classN);
	static string _serlis(void* t,string classN);
	static string _serset(void* t,string classN);
	static string _sermultiset(void* t,string classN);
	static string _serq(void* t,string classN);
	static string _serdq(void* t,string classN);
	static string _ser(Object);
	static void* _unser(string objXml,string classN);
	static void* unserializevec(AMEFObject* root, string objXml);
	static void* unserializelist(AMEFObject* root, string objXml);
	static void* unserializeset(AMEFObject* root, string objXml);
	static void* unserializemultiset(AMEFObject* root, string objXml);
	static void* unserializeq(AMEFObject* root, string objXml);
	static void* unserializedq(AMEFObject* root, string objXml);

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
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "set<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		while(st.begin()!=t.end())
		{
			object.addPacket(serialize<T>(*st.begin()));
			st.erase(st.begin());
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializemultiset(multiset<T> t)
	{
		multiset<T> st = t;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "multiset<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		while(st.begin()!=t.end())
		{
			object.addPacket(serialize<T>(*st.begin()));
			st.erase(st.begin());
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializevec(vector<T> t)
	{
		vector<T> st = t;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "vector<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		while(st.begin()!=t.end())
		{
			object.addPacket(serialize<T>(*st.begin()));
			st.erase(st.begin());
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializedq(deque<T> t)
	{
		deque<T> st = t;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "deque<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		while(st.begin()!=t.end())
		{
			object.addPacket(serialize<T>(*st.begin()));
			st.erase(st.begin());
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializelist(list<T> t)
	{
		list<T> st = t;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "lists<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		while(st.begin()!=t.end())
		{
			object.addPacket(serialize<T>(*st.begin()));
			st.erase(st.begin());
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializeq(std::queue<T> t)
	{
		std::queue<T> tt = t;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "std::queue<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		if(!tt.empty())
		{
			for(int var=0;var<(int)tt.size();var++)
			{
				object.addPacket(serialize<T>(tt.front()));
				tt.pop();
			}
		}
		return enc.encodeB(&object, false);
	}
	/*template <class T> static string serializestack(stack<T> t)
	{
		stack<T> tt = t;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "stack<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		if(!tt.empty())
		{
			for(int var=0;var<(int)tt.size();var++)
			{
				object.addPacket(serialize<T>(tt.front()));
				tt.pop();
			}
		}
		return enc.encodeB(&object, false);
	}*/
public:
	Serialize(){}
	~Serialize(){}

	template <class T> static string serialize(T t)
	{
		string objXml;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return _hanldeAllSerialization(className,&t);
	}
	static string serializeObject(Object t)
	{
		return _hanldeAllSerialization(t.getTypeName(),t.getVoidPointer());
	}

	static string serializeUnknown(void* t,string className)
	{
		return _hanldeAllSerialization(className,t);
	}

	template <class K,class V> static string serialize(map<K,V> mp)
	{
		map<K,V> mpt  = mp;
		AMEFEncoder enc;
		AMEFObject object;
		K k;
		const char *mangled = typeid(k).name();
		string kclassName = demangle(mangled);
		V v;
		*mangled = typeid(v).name();
		string vclassName = demangle(mangled);
		kclassName = "map<"+kclassName+":"+vclassName+">";
		object.setName(kclassName);
		while (mpt.begin()!=mpt.end())
		{
			string key = serialize<K>(mpt.begin()->first);
			string value = serialize<V>(mpt.begin()->second);
			mpt.erase(mpt.begin());
			object.addPacket(value, key);
		}
		return enc.encodeB(&object, false);
	}

	template <class K,class V> static string serialize(multimap<K,V> mp)
	{
		multimap<K,V> mpt  = mp;
		AMEFEncoder enc;
		AMEFObject object;
		K k;
		const char *mangled = typeid(k).name();
		string kclassName = demangle(mangled);
		V v;
		*mangled = typeid(v).name();
		string vclassName = demangle(mangled);
		kclassName = "multimap<"+kclassName+":"+vclassName+">";
		object.setName(kclassName);
		while (mpt.begin()!=mpt.end())
		{
			string key = serialize<K>(mpt.begin()->first);
			string value = serialize<V>(mpt.begin()->second);
			mpt.erase(mpt.begin());
			object.addPacket(value, key);
		}
		return enc.encodeB(&object, false);
	}

	template <class T> T static unserialize(string objXml)
	{
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return *(T*)_hanldeAllUnSerialization(objXml,className);
	}

	static void* unSerializeUnknown(string objXml,string className)
	{
		return _hanldeAllUnSerialization(objXml,className);
	}
};
#endif
