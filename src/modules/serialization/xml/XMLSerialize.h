/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * XMLSerialize.h
 *
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#ifndef XMLSERIALIZE_H_
#define XMLSERIALIZE_H_
#include "SerializeBase.h"
#include "SimpleXmlParser.h"

class XMLSerialize : public SerializeBase {
	static XMLSerialize _i;
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
	friend class Serializer;
public:
	XMLSerialize();
	XMLSerialize(void*);
	~XMLSerialize();

	int getSerializerType() {return 2;}

	std::string serializePrimitive(int serOpt, const std::string& className, void* t, void* serobject);
	template <class T> static std::string serialize(T& t, int serOpt, const std::string& appName = "")
	{
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		return _handleAllSerialization(serOpt,className,&t,appName,&_i,NULL,NULL,NULL,NULL);
	}
	template <class T> static std::string serialize(void* t, int serOpt, const std::string& className, const std::string& appName = "")
	{
		if(serOpt==-1) serOpt = identifySerOption(className);
		return _handleAllSerialization(serOpt,className,t,appName, &_i,NULL,NULL,NULL,NULL);
	}
	template <class T> static std::string serializePointer(T* t, int serOpt, const std::string& appName = "")
	{
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		return _handleAllSerialization(serOpt,className,t,appName, &_i,NULL,NULL,NULL,NULL);
	}
	static std::string serializeUnknown(void* t, int serOpt, const std::string& className, void* serobject, const std::string& appName = "");

	template <class K,class V> static std::string serializeMap(const std::map<K,V>& mp, const std::string& appName = "")
	{
		std::map<K,V> mpt  = mp;
		K k;
		std::string kclassName = CastUtil::getClassName(k);
		V v;
		std::string serval;
		std::string vclassName = CastUtil::getClassName(v);
		kclassName = "map-"+kclassName+"-"+vclassName;
		serval = "<" + kclassName + ">";
		while (mpt.begin()!=mpt.end())
		{
			std::string key = serialize<K>(mpt.begin()->first,appName);
			std::string value = serialize<V>(mpt.begin()->second,appName);
			serval += "<entry><key>" + key + "</key>";
			serval += "<value>" + value + "</value></entry>";
			mpt.erase(mpt.begin());
		}
		serval = "</" + kclassName + ">";
		return serval;
	}
	template <class K,class V> static std::map<K,V> unSerializeToMap(const std::string& serStr, const std::string& appName = "")
	{
		std::map<K,V> mp;
		SimpleXmlParser parser("Parser");
		try
		{
			Document doc;
			parser.parse(serStr, doc);
			Element message = doc.getRootElement();
			if(message.getTagName()!="" && message.getTagName().find("map-")!=std::string::npos
					&& message.getChildElements().size()>0)
			{
				std::string maptype = message.getTagName();
				std::string keytype = maptype.substr(maptype.find("-")+1);
				if(keytype.find("-")==std::string::npos)
					return mp;
				std::string valtype = keytype.substr(keytype.find("-")+1);
				keytype = keytype.substr(0, keytype.find("-"));
				for (int var = 0; var < (int)message.getChildElements().size(); var++)
				{
					Element* entry = (Element*)&(message.getChildElements().at(var));
					if(entry->getTagName()=="entry")
					{
						Element* key = entry->getElementByName("key");
						Element* value = entry->getElementByName("value");
						if(key->getTagName()!="" && value->getTagName()!="" && key->getChildElements().size()>0
								&& value->getChildElements().size()>0) {
							K k = unserialize<K>(&(key->getChildElements().at(0)), keytype);
							V v = unserialize<K>(&(value->getChildElements().at(0)), valtype);
							mp[k] = v;
						} else if(key->getTagName()!="" && key->getChildElements().size()>0) {
							K k = unserialize<K>(&(key->getChildElements().at(0)), keytype);
							V v;
							mp[k] = v;
						}
					}
				}
			}
		} catch(const XmlParseException& str) {
			std::cout << str.getMessage() << std::endl;
		} catch(const std::exception& e) {
			std::cout << "XML Parse Error" << std::endl;
		}
		return mp;
	}

	template <class K,class V> static std::string serializeMultimap(std::multimap<K,V>& mp, const std::string& appName = "")
	{
		std::multimap<K,V> mpt  = mp;
		K k;
		std::string kclassName = CastUtil::getClassName(k);
		V v;
		std::string serval;
		std::string vclassName = CastUtil::getClassName(k);
		kclassName = "multimap-"+kclassName+"-"+vclassName;
		serval = "<" + kclassName + ">";
		while (mpt.begin()!=mpt.end())
		{
			std::string key = serialize<K>(mpt.begin()->first,appName);
			std::string value = serialize<V>(mpt.begin()->second,appName);
			serval += "<entry><key>" + key + "</key>";
			serval += "<value>" + value + "</value></entry>";
			mpt.erase(mpt.begin());
		}
		serval = "</" + kclassName + ">";
		return serval;
	}
	template <class K,class V> static std::map<K,V> unSerializeToMultiMap(const std::string& serStr, const std::string& appName = "")
	{
		std::multimap<K,V> mp;
		SimpleXmlParser parser("Parser");
		try
		{
			Document doc;
			parser.parse(serStr, doc);
			Element message = doc.getRootElement();
			if(message.getTagName()!="" && message.getTagName().find("multimap-")!=std::string::npos
					&& message.getChildElements().size()>0)
			{
				std::string maptype = message.getTagName();
				std::string keytype = maptype.substr(maptype.find("-")+1);
				if(keytype.find("-")==std::string::npos)
					return mp;
				std::string valtype = keytype.substr(keytype.find("-")+1);
				keytype = keytype.substr(0, keytype.find("-"));
				for (int var = 0; var < (int)message.getChildElements().size(); var++)
				{
					Element* entry = (Element*)&(message.getChildElements().at(var));
					if(entry->getTagName()=="entry")
					{
						Element* key = entry->getElementByName("key");
						Element* value = entry->getElementByName("value");
						if(key->getTagName()!="" && value->getTagName()!="" && key->getChildElements().size()>0
								&& value->getChildElements().size()>0) {
							K k = unserialize<K>(&(key->getChildElements().at(0)), keytype);
							V v = unserialize<K>(&(value->getChildElements().at(0)), valtype);
							mp[k] = v;
						} else if(key->getTagName()!="" && key->getChildElements().size()>0) {
							K k = unserialize<K>(&(key->getChildElements().at(0)), keytype);
							V v;
							mp[k] = v;
						}
					}
				}
			}
		} catch(const XmlParseException& str) {
			std::cout << str.getMessage() << std::endl;
		} catch(const std::exception& e) {
			std::cout << "XML Parse Error" << std::endl;
		}
		return mp;
	}
	template <class T> static T unserialize(const std::string& objXml, int serOpt, const std::string& appName = "")
	{
		XMLSerialize serialize;
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
	template <class T> static T unserialize(Element* element, int serOpt, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T t;
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		T* tp = (T*)_handleAllUnSerialization("",serOpt,className,appName,&serialize,false,element);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T unserialize(Element* element, int serOpt, const std::string& className, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T t;
		if(serOpt==-1) serOpt = identifySerOption(className);
		T* tp = (T*)_handleAllUnSerialization("",serOpt,className,appName,&serialize,false,element);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T* unserializeToPointer(std::string objXml, int serOpt, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T* t;
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		return (T*)_handleAllUnSerialization(objXml,serOpt,className,appName,&serialize,false,NULL);
	}
	template <class T> static T* unserializeToPointer(Element* element, int serOpt, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T* t;
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		return (T*)_handleAllUnSerialization("",serOpt,className,appName,&serialize,false,element);
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

#endif /* XMLSERIALIZE_H_ */
