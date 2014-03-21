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
	XMLSerialize();
	XMLSerialize(void*);
	~XMLSerialize();

	template <class T> static string serialize(T t, string appName = "default")
	{
		XMLSerialize serialize;
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
		XMLSerialize serialize;
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

	/*template <class K,class V> static string serialize(map<K,V> mp, string appName = "default")
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
			string key = serialize<K>(mpt.begin()->first,appName);
			string value = serialize<V>(mpt.begin()->second,appName);
			mpt.erase(mpt.begin());
			object.addPacket(value, key);
		}
		return enc.encodeB(&object, false);
	}

	template <class K,class V> static string serialize(multimap<K,V> mp, string appName = "default")
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
			string key = serialize<K>(mpt.begin()->first,appName);
			string value = serialize<V>(mpt.begin()->second,appName);
			mpt.erase(mpt.begin());
			object.addPacket(value, key);
		}
		return enc.encodeB(&object, false);
	}*/
	template <class T> static T unserialize(string objXml, string appName = "default")
	{
		XMLSerialize serialize;
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
	template <class T> static T unserialize(Element* element, string appName = "default")
	{
		XMLSerialize serialize;
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		T* tp = (T*)_handleAllUnSerialization("",className,appName,&serialize,false,element);
		if(tp!=NULL)
		{
			t = *(T*)tp;
			delete ((T*)tp);
		}
		return t;
	}
	template <class T> static T* unserializeToPointer(string objXml, string appName = "default")
	{
		XMLSerialize serialize;
		T* t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		return (T*)_handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
	}
	template <class T> static T* unserializeToPointer(Element* element, string appName = "default")
	{
		XMLSerialize serialize;
		T* t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		if(className.find(",")!=string::npos)
		{
			className = className.substr(0, className.find(",")+1);
		}
		return (T*)_handleAllUnSerialization("",className,appName,&serialize,false,element);
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

#endif /* XMLSERIALIZE_H_ */
