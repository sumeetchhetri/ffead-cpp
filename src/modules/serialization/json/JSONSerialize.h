/*
	Copyright 2010, Sumeet Chhetri

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
 * JSONSerialize.h
 *
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#ifndef JSONSERIALIZE_H_
#define JSONSERIALIZE_H_
#include "SerializeBase.h"
#include "JSONUtil.h"

class JSONSerialize : public SerializeBase {

	std::string serializePrimitive(const std::string& className, void* t);
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
	void addPrimitiveElementToContainer(void* _1, const int& counter, const std::string& className, void* cont, const std::string& container);
	void* getUnserializableObject(const std::string& _1);
	void cleanUnserializableObject(void* _1);
	void cleanValidUnserializableObject(void* _1);
	void* getValidUnserializableObject(const std::string& _1);
	int getContainerSize(void* _1);
	std::string getUnserializableClassName(void* _1, const std::string& className);
	void* getPrimitiveValue(void* _1, const std::string& className);
public:
	JSONSerialize();
	JSONSerialize(void*);
	~JSONSerialize();

	template <class T> static std::string serialize(T& t, const std::string& appName = "")
	{
		JSONSerialize serialize;
		std::string className = CastUtil::getClassName(t);
		return _handleAllSerialization(className,&t,appName,&serialize);
	}
	template <class T> static std::string serializePointer(T* t, const std::string& appName = "")
	{
		JSONSerialize serialize;
		std::string className = CastUtil::getClassName(t);
		return _handleAllSerialization(className,t,appName,&serialize);
	}
	static std::string serializeUnknown(void* t, const std::string& className, const std::string& appName = "");

	/*template <class K,class V> static std::string serialize(const std::map<K,V>& mp, const std::string& appName = "")
	{
		std::map<K,V> mpt  = mp;
		AMEFEncoder enc;
		AMEFObject object;
		K k;
		std::string kclassName = CastUtil::getClassName(k);
		V v;
		std::string serval;
		std::string vclassName = CastUtil::getClassName(v);
		kclassName = "map<"+kclassName+":"+vclassName+">";
		object.setName(kclassName);
		while (mpt.begin()!=mpt.end())
		{
			std::string key = serialize<K>(mpt.begin()->first,appName);
			std::string value = serialize<V>(mpt.begin()->second,appName);
			mpt.erase(mpt.begin());
			object.addPacket(value, key);
		}
		return enc.encodeB(&object, false);
	}

	template <class K,class V> static std::string serialize(const std::multimap<K,V>& mp, const std::string& appName = "")
	{
		std::multimap<K,V> mpt  = mp;
		AMEFEncoder enc;
		AMEFObject object;
		K k;
		std::string kclassName = CastUtil::getClassName(k);
		V v;
		std::string serval;
		std::string vclassName = CastUtil::getClassName(v);
		kclassName = "multimap<"+kclassName+":"+vclassName+">";
		object.setName(kclassName);
		while (mpt.begin()!=mpt.end())
		{
			std::string key = serialize<K>(mpt.begin()->first,appName);
			std::string value = serialize<V>(mpt.begin()->second,appName);
			mpt.erase(mpt.begin());
			object.addPacket(value, key);
		}
		return enc.encodeB(&object, false);
	}*/
	template <class T> static T unserialize(const std::string& objXml, const std::string& appName = "")
	{
		JSONSerialize serialize;
		T t;
		std::string className = CastUtil::getClassName(t);
		T* tp = (T*)_handleAllUnSerialization(objXml,className,appName,&serialize,true,NULL);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T unserialize(JSONElement* element, const std::string& appName = "")
	{
		JSONSerialize serialize;
		T t;
		std::string className = CastUtil::getClassName(t);
		T* tp = (T*)_handleAllUnSerialization("",className,appName,&serialize,true,element);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T* unserializeToPointer(const std::string& objXml, const std::string& appName = "")
	{
		JSONSerialize serialize;
		T* t;
		std::string className = CastUtil::getClassName(t);
		return (T*)_handleAllUnSerialization(objXml,className,appName,&serialize,true,NULL);
	}
	template <class T> static T* unserializeToPointer(JSONElement* element, const std::string& appName = "")
	{
		JSONSerialize serialize;
		T* t;
		std::string className = CastUtil::getClassName(t);
		return (T*)_handleAllUnSerialization("",className,appName,&serialize,true,element);
	}

	bool isValidClassNamespace(void* _1, const std::string& className, const std::string& namespc, const bool& iscontainer= false);
	bool isValidObjectProperty(void* _1, const std::string& propname, const int& counter);
	void* getObjectProperty(void* _1, const int& counter);
	void startObjectSerialization(void* _1, const std::string& className);
	void endObjectSerialization(void* _1, const std::string& className);
	void afterAddObjectProperty(void* _1);
	void addObjectPrimitiveProperty(void* _1, const std::string& propName, const std::string& className, void* t);
	void addObjectProperty(void* _1, const std::string& propName, std::string className, const std::string& t);
	void* getObjectPrimitiveValue(void* _1, const std::string& className, const std::string& propName);
	static void* unSerializeUnknown(const std::string& objXml, const std::string& className, const std::string& appName = "");
	std::string serializeUnknownBase(void* t, const std::string& className, const std::string& appName = "");
	void* unSerializeUnknownBase(void* unserObj, const std::string& className, const std::string& appName = "");
	void* unSerializeUnknownBase(const std::string& serVal, const std::string& className, const std::string& appName = "");
};

#endif /* JSONSERIALIZE_H_ */
