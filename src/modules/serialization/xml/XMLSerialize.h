/*
 * XMLSerialize.h
 *
 *  Created on: 12-Jun-2013
 *      Author: sumeetc
 */

#ifndef XMLSERIALIZE_H_
#define XMLSERIALIZE_H_
#include "SerializeBase.h"
#include "XmlParser.h"

class XMLSerialize : public SerializeBase {

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
	XMLSerialize();
	XMLSerialize(void*);
	~XMLSerialize();

	template <class T> static std::string serialize(T& t, const std::string& appName = "")
	{
		XMLSerialize serialize;
		std::string className = CastUtil::getClassName(t);
		return _handleAllSerialization(className,&t,appName, &serialize);
	}
	template <class T> static std::string serialize(void* t, const std::string& className, const std::string& appName = "")
	{
		XMLSerialize serialize;
		return _handleAllSerialization(className,t,appName, &serialize);
	}
	template <class T> static std::string serializePointer(T* t, const std::string& appName = "")
	{
		XMLSerialize serialize;
		std::string className = CastUtil::getClassName(t);
		return _handleAllSerialization(className,t,appName, &serialize);
	}
	static std::string serializeUnknown(void* t, const std::string& className, const std::string& appName = "");

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
		XmlParser parser("Parser");
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
					Element* entry = &(message.getChildElements().at(var));
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
		} catch(...) {
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
		XmlParser parser("Parser");
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
					Element* entry = &(message.getChildElements().at(var));
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
		} catch(...) {
			std::cout << "XML Parse Error" << std::endl;
		}
		return mp;
	}
	template <class T> static T unserialize(const std::string& objXml, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T t;
		std::string className = CastUtil::getClassName(t);
		T* tp = (T*)_handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T unserialize(Element* element, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T t;
		std::string className = CastUtil::getClassName(t);
		T* tp = (T*)_handleAllUnSerialization("",className,appName,&serialize,false,element);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T unserialize(Element* element, const std::string& className, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T t;
		T* tp = (T*)_handleAllUnSerialization("",className,appName,&serialize,false,element);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T* unserializeToPointer(std::string objXml, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T* t;
		std::string className = CastUtil::getClassName(t);
		return (T*)_handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
	}
	template <class T> static T* unserializeToPointer(Element* element, const std::string& appName = "")
	{
		XMLSerialize serialize;
		T* t;
		std::string className = CastUtil::getClassName(t);
		return (T*)_handleAllUnSerialization("",className,appName,&serialize,false,element);
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

#endif /* XMLSERIALIZE_H_ */
