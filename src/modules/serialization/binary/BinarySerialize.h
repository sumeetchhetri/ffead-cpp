/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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

typedef std::string (*SerPtr) (void*);
typedef void* (*UnSerPtr) (const std::string&);

class BinarySerialize : public SerializeBase {
	static BinarySerialize _i;
	std::string serializePrimitive(int serOpt, const std::string& className, void* t, void* serobject);
	void* getSerializableObject();
	void cleanSerializableObject(void* _1);
	void startContainerSerialization(void* _1, const std::string& className, const std::string& container);
	void endContainerSerialization(void* _1, const std::string& className, const std::string& container);
	void afterAddContainerSerializableElement(void* _1, const int& counter, const int& size);
	void addContainerSerializableElement(void* _1, const std::string& tem);
	void addContainerSerializableElementMulti(void* _1, const std::string& tem);
	std::string fromSerializableObjectToString(void* _1);
	std::string elementToSerializedString(void* _1, const int& counter);
	std::string getConatinerElementClassName(void* _1, const std::string& className);
	void* getContainerElement(void* _1, const int& counter, const int& counter1= -1);
	void addPrimitiveElementToContainer(void* _1, int serOpt, const int& counter, const std::string& className, void* cont, const std::string& container);
	void* getUnserializableObject(const std::string& _1);
	void cleanUnserializableObject(void* _1);
	void cleanValidUnserializableObject(void* _1);
	void* getValidUnserializableObject(const std::string& _1);
	int getContainerSize(void* _1);
	std::string getUnserializableClassName(void* _1, const std::string& className);
	void* getPrimitiveValue(void* _1, int serOpt, const std::string& className);
public:
	BinarySerialize();
	BinarySerialize(void*);
	~BinarySerialize();

	int getSerializerType() {return 3;}

	template <class T> static std::string serialize(T& t, int serOpt, const std::string& appName = "")
	{
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		return _handleAllSerialization(className,&t,appName,&_i,NULL,NULL,NULL,NULL);
	}
	template <class T> static std::string serializePointer(T* t, int serOpt, const std::string& appName = "")
	{
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		return _handleAllSerialization(className,t,appName,&_i,NULL,NULL,NULL,NULL);
	}
	static std::string serializeUnknown(void* t, int serOpt, const std::string& className, void* serobject, const std::string& appName = "");

	template <class K,class V> static std::string serializeMap(std::map<K,V>& mp, const std::string& appName = "")
	{
		typedef typename std::map<K,V>::iterator kvmapiter;
		AMEFEncoder enc;
		AMEFObject object;
		K k;
		std::string kclassName = CastUtil::getClassName(k);
		V v;
		std::string vclassName = CastUtil::getClassName(v);
		kclassName = "map<"+kclassName+":"+vclassName+">";
		object.setName(kclassName);
		kvmapiter it;
		for (it=mp.begin();it!=mp.end();++it)
		{
			//string key = serialize<K>(it->first,appName);
			std::string value = serialize<V>(it->second,appName);
			object.addPacket(value, CastUtil::lexical_cast<std::string>(it->first));
		}
		return enc.encodeB(&object);
	}
	template <class K,class V> static std::map<K,V> unSerializeToMap(const std::string& serStr, const std::string& appName = "")
	{
		std::map<K,V> mp;
		AMEFDecoder dec;
		AMEFObject object;
		AMEFObject* root = dec.decodeB(serStr, true);
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				V val = unserialize<V>(root->getPackets().at(var)->getValueStr(), appName);
				K key = CastUtil::lexical_cast<K>(root->getPackets().at(var)->getNameStr());
				mp[key] = val;
			}
		}
		return mp;
	}

	template <class K,class V> static std::string serializeMultiMap(std::multimap<K,V>& mp, const std::string& appName = "")
	{
		typedef typename std::multimap<K,V>::iterator kvmapiter;
		AMEFEncoder enc;
		AMEFObject object;
		K k;
		std::string kclassName = CastUtil::getClassName(k);
		V v;
		std::string vclassName = CastUtil::getClassName(v);
		kclassName = "multimap<"+kclassName+":"+vclassName+">";
		object.setName(kclassName);
		kvmapiter it;
		for (it=mp.begin();it!=mp.end();++it)
		{
			//string key = serialize<K>(it->first,appName);
			std::string value = serialize<V>(it->second,appName);
			object.addPacket(value, CastUtil::lexical_cast<std::string>(it->first));
		}
		return enc.encodeB(&object);
	}
	template <class K,class V> static std::multimap<K,V> unSerializeToMultiMap(const std::string& serStr, const std::string& appName = "")
	{
		std::multimap<K,V> mp;
		AMEFDecoder dec;
		AMEFObject object;
		AMEFObject* root = dec.decodeB(serStr, true);
		if(root!=NULL)
		{
			for (int var = 0; var < (int)root->getPackets().size(); var++)
			{
				V val = unserialize<V>(root->getPackets().at(var)->getValueStr(), appName);
				K key = CastUtil::lexical_cast<K>(root->getPackets().at(var)->getNameStr());
				mp[key] = val;
			}
		}
		return mp;
	}


	template <class T> static T unserialize(const std::string& objXml, int serOpt, const std::string& appName = "")
	{
		BinarySerialize serialize;
		T t;
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		T* tp = (T*)_handleAllUnSerialization(objXml,serOpt,className,appName,&serialize,false,NULL);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T unserialize(AMEFObject* serObject, int serOpt, const std::string& appName = "")
	{
		BinarySerialize serialize;
		T t;
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		T* tp = (T*)_handleAllUnSerialization("",serOpt,className,appName,&serialize,false,serObject);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T* unserializeToPointer(const std::string& objXml, int serOpt, const std::string& appName = "")
	{
		BinarySerialize serialize;
		T* t;
		std::string className = CastUtil::getClassName(t);
		return (T*)_handleAllUnSerialization(objXml,serOpt,className,appName,&serialize,false,NULL);
	}
	template <class T> static T* unserializeToPointer(AMEFObject* serObject, int serOpt, const std::string& appName = "")
	{
		BinarySerialize serialize;
		T* t;
		std::string className = CastUtil::getClassName(t);
		return (T*)_handleAllUnSerialization("",serOpt,className,appName,&serialize,false,serObject);
	}

	bool isValidClassNamespace(void* _1, const std::string& className, const std::string& namespc, const bool& iscontainer= false);
	bool isValidObjectProperty(void* _1, const std::string& propname, const int& counter);
	void* getObjectProperty(void* _1, const int& counter);
	void startObjectSerialization(void* _1, const std::string& className);
	void endObjectSerialization(void* _1, const std::string& className);
	void afterAddObjectProperty(void* _1, const std::string& propName);
	void addObjectPrimitiveProperty(void* _1, int serOpt, const std::string& propName, const std::string& className, void* t);
	void addObjectProperty(void* _1, const std::string& propName, std::string className);
	void* getObjectPrimitiveValue(void* _1, int serOpt, const std::string& className, const std::string& propName);
	static void* unSerializeUnknown(const std::string& objXml, int serOpt, const std::string& className, const std::string& appName = "");
	std::string serializeUnknownBase(void* t, int serOpt, const std::string& className, const std::string& appName, void* serobject);
	void* unSerializeUnknownBase(void* unserObj, int serOpt, const std::string& className, const std::string& appName = "");
	void* unSerializeUnknownBase(const std::string& serVal, int serOpt, const std::string& className, const std::string& appName = "");
};

#endif /* BINARYSERIALIZE_H_ */
