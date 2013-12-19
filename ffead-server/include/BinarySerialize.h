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
 * BinarySerialize.h
 *
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#ifndef BINARYSERIALIZE_H_
#define BINARYSERIALIZE_H_
#include "SerializeBase.h"
#include "AMEFResources.h"

typedef string (*SerPtr) (void*);
typedef void* (*UnSerPtr) (string);

class BinarySerialize : public SerializeBase {

	string serializePrimitive(string className, void* t);
	void* getSerializableObject();
	void cleanSerializableObject(void* _1);
	void startContainerSerialization(void* _1, string className, string container);
	void endContainerSerialization(void* _1, string className, string container);
	void afterAddContainerSerializableElement(void* _1, int counter, int size);
	void addContainerSerializableElement(void* _1, string tem);
	void addContainerSerializableElementMulti(void* _1, string tem);
	string fromSerializableObjectToString(void* _1);
	string elementToSerializedString(void* _1, int counter);
	string getConatinerElementClassName(void* _1, string className);
	void* getContainerElement(void* _1, int counter, int counter1 = -1);
	void addPrimitiveElementToContainer(void* _1, int counter, string className, void* cont, string container);
	void* getUnserializableObject(string _1);
	void cleanUnserializableObject(void* _1);
	void cleanValidUnserializableObject(void* _1);
	void* getValidUnserializableObject(string _1);
	int getContainerSize(void* _1);
	string getUnserializableClassName(void* _1, string className);
	void* getPrimitiveValue(void* _1, string className);
	string getSerializationMethodName(string className, string appName, bool which, string type);
public:
	BinarySerialize();
	BinarySerialize(void*);
	~BinarySerialize();

	template <class T> static string serialize(T t, string appName = "default")
	{
		BinarySerialize serialize;
		string objXml;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		return _handleAllSerialization(className,&t,appName, &serialize);
	}
	template <class T> static string serializePointer(T* t, string appName = "default")
	{
		BinarySerialize serialize;
		string objXml;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		return _handleAllSerialization(className,t,appName, &serialize);
	}
	static string serializeUnknown(void* t,string className, string appName = "default");

	template <class K,class V> static string serialize(map<K,V> mp, string appName = "default")
	{
		typedef typename map<K,V>::iterator kvmapiter;
		AMEFEncoder enc;
		AMEFObject object;
		K k;
		const char *mangled = typeid(k).name();
		string kclassName = demangle(mangled);
		V v;
		mangled = typeid(v).name();
		string vclassName = demangle(mangled);
		kclassName = "map<"+kclassName+":"+vclassName+">";
		object.setName(kclassName);
		kvmapiter it;
		for (it=mp.begin();it!=mp.end();++it)
		{
			//string key = serialize<K>(it->first,appName);
			string value = serialize<V>(it->second,appName);
			object.addPacket(value, CastUtil::lexical_cast<string>(it->first));
		}
		return enc.encodeB(&object, false);
	}
	template <class K,class V> static map<K,V> unSerializeToMap(string serStr, string appName = "default")
	{
		map<K,V> mp;
		AMEFDecoder dec;
		AMEFObject object;
		AMEFObject* root = dec.decodeB(serStr, true, false);
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				V val = unserialize<V>(root->getPackets().at(var)->getValueStr());
				K key = CastUtil::lexical_cast<K>(root->getPackets().at(var)->getNameStr());
				mp[key] = val;
			}
		}
		return mp;
	}

	template <class K,class V> static string serialize(multimap<K,V> mp, string appName = "default")
	{
		typedef typename multimap<K,V>::iterator kvmapiter;
		AMEFEncoder enc;
		AMEFObject object;
		K k;
		const char *mangled = typeid(k).name();
		string kclassName = demangle(mangled);
		V v;
		mangled = typeid(v).name();
		string vclassName = demangle(mangled);
		kclassName = "multimap<"+kclassName+":"+vclassName+">";
		object.setName(kclassName);
		kvmapiter it;
		for (it=mp.begin();it!=mp.end();++it)
		{
			//string key = serialize<K>(it->first,appName);
			string value = serialize<V>(it->second,appName);
			object.addPacket(value, CastUtil::lexical_cast<string>(it->first));
		}
		return enc.encodeB(&object, false);
	}
	template <class K,class V> static multimap<K,V> unSerializeToMultimap(string serStr, string appName = "default")
	{
		multimap<K,V> mp;
		AMEFDecoder dec;
		AMEFObject object;
		AMEFObject* root = dec.decodeB(serStr, true, false);
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				V val = unserialize<V>(root->getPackets().at(var)->getValueStr());
				K key = CastUtil::lexical_cast<K>(root->getPackets().at(var)->getNameStr());
				mp[key] = val;
			}
		}
		return mp;
	}


	template <class T> static T unserialize(string objXml, string appName = "default")
	{
		BinarySerialize serialize;
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		T* tp = (T*)_handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T unserialize(AMEFObject* serObject, string appName = "default")
	{
		BinarySerialize serialize;
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		T* tp = (T*)_handleAllUnSerialization("",className,appName,&serialize,false,serObject);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T* unserializeToPointer(string objXml, string appName = "default")
	{
		BinarySerialize serialize;
		T* t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		return (T*)_handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
	}
	template <class T> static T* unserializeToPointer(AMEFObject* serObject, string appName = "default")
	{
		BinarySerialize serialize;
		T* t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		return (T*)_handleAllUnSerialization("",className,appName,&serialize,false,serObject);
	}

	bool isValidClassNamespace(void* _1, string className, string namespc, bool iscontainer = false);
	bool isValidObjectProperty(void* _1, string propname, int counter);
	void* getObjectProperty(void* _1, int counter);
	void startObjectSerialization(void* _1, string className);
	void endObjectSerialization(void* _1, string className);
	void afterAddObjectProperty(void* _1);
	void addObjectPrimitiveProperty(void* _1, string propName, string className, void* t);
	void addObjectProperty(void* _1, string propName, string className, string t);
	void* getObjectPrimitiveValue(void* _1, string className, string propName);
	static void* unSerializeUnknown(string objXml,string className, string appName = "default");
	string serializeUnknownBase(void* t,string className, string appName);
	void* unSerializeUnknownBase(void* unserObj,string className, string appName);
};

#endif /* BINARYSERIALIZE_H_ */
