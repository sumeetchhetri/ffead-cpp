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
#include "SerializeBase.h"
#include "AMEFResources.h"

using namespace std;
typedef string (*SerPtr) (void*);
typedef void* (*UnSerPtr) (string);


class Serialize : public SerializeBase {
	//static string _handleAllSerialization(string className,void *t, string appName);
	//static void* _handleAllUnSerialization(string objXml,string className, string appName);
	static string _ser(void* t,string classN, string appName);
	static string _servec(void* t,string classN, string appName);
	static string _serlis(void* t,string classN, string appName);
	static string _serset(void* t,string classN, string appName);
	static string _sermultiset(void* t,string classN, string appName);
	static string _serq(void* t,string classN, string appName);
	static string _serdq(void* t,string classN, string appName);
	static string _ser(Object, string appName);
	static void* _unser(string objXml,string classN, string appName);
	/*static void* unserializevec(AMEFObject* root, string appName, int &size);
	static void* unserializelist(AMEFObject* root, string appName, int &size);
	static void* unserializeset(AMEFObject* root, string appName, int &size);
	static void* unserializemultiset(AMEFObject* root, string appName, int &size);
	static void* unserializeq(AMEFObject* root, string appName, int &size);
	static void* unserializedq(AMEFObject* root, string appName, int &size);*/

	static void* _unserVec(string objXml,string className, string appName);
	static void* _unserLis(string objXml,string className, string appName);
	static void* _unserQ(string objXml,string className, string appName);
	static void* _unserDq(string objXml,string className, string appName);
	static void* _unserSet(string objXml,string className, string appName);
	static void* _unserMulSet(string objXml,string className, string appName);

	string serializePrimitive(string className, void* t)
	{
		string objXml;
		AMEFEncoder enc;
		AMEFObject object;

		if(className=="std::string" || className=="string")
		{
			string tem = *(string*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="char")
		{
			char tem = *(char*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="unsigned char")
		{
			unsigned char tem = *(unsigned char*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="int")
		{
			int tem = *(int*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="unsigned int")
		{
			unsigned int tem = *(unsigned int*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="short")
		{
			short tem = *(short*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="unsigned short")
		{
			unsigned short tem = *(unsigned short*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="long")
		{
			long tem = *(long*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="unsigned long")
		{
			unsigned long tem = *(unsigned long*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="long long")
		{
			long long tem = *(long long*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="unsigned long long")
		{
			unsigned long long tem = *(unsigned long long*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="float")
		{
			float tem = *(float*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="double")
		{
			double tem = *(double*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="long double")
		{
			long double tem = *(long double*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="bool")
		{
			bool tem = *(bool*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="Date")
		{
			string tem = *(string*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		else if(className=="BinaryData")
		{
			string tem = *(string*)t;
			object.addPacket(tem, className);
			objXml = enc.encodeB(&object, false);
		}
		return enc.encodeB(&object, false);
	}

	void* getSerializableObject()
	{
		return new AMEFObject;
	}
	void cleanSerializableObject(void* _1)
	{
		AMEFObject* object = (AMEFObject*)_1;
		object->~AMEFObject();
	}
	void startContainerSerialization(void* _1, string className, string container)
	{
		string contName = container + "<" + className + ">";
		AMEFEncoder enc;
		AMEFObject* object = (AMEFObject*)_1;
		object->setName(contName);
	}
	void endContainerSerialization(void* _1, string className, string){}
	void afterAddContainerSerializableElement(void* _1, int counter, int size){}
	void addContainerSerializableElement(void* _1, string tem)
	{
		AMEFEncoder enc;
		AMEFObject* object = (AMEFObject*)_1;
		object->addPacket(tem);
	}
	void addContainerSerializableElementMulti(void* _1, string tem)
	{
		//tem = tem.substr(4);
		AMEFEncoder enc;
		AMEFObject* object = (AMEFObject*)_1;
		object->addPacket(tem);
	}
	string fromSerializableObjectToString(void* _1)
	{
		AMEFEncoder enc;
		AMEFObject* object = (AMEFObject*)_1;
		return enc.encodeB(object, false);
	}

	string getConatinerElementClassName(void* _1, string className)
	{
		AMEFObject* root = (AMEFObject*)_1;
		string stlclassName = root->getNameStr();
		if(stlclassName.find(">")!=string::npos)
		{
			className = stlclassName.substr(stlclassName.find("<")+1);
			className = className.substr(0, className.find(">"));
		}
		else
		{
			className = stlclassName.substr(stlclassName.find("<")+1);
			if(className.find(",")!=string::npos)
			{
				className = className.substr(0, className.find_last_of(','));
			}
		}
		StringUtil::trim(className);
		return className;
	}
	void* getContainerElement(void* _1, int counter, int counter1)
	{
		AMEFDecoder dec;
		AMEFObject* root = (AMEFObject*)_1;
		AMEFObject* root2 = dec.decodeB(root->getPackets().at(counter)->getValue(), true, false);
		return root2;
	}
	void addPrimitiveElementToContainer(void* _1, int counter, string className, void* cont, string container)
	{
		AMEFDecoder dec;
		AMEFObject* root = (AMEFObject*)_1;
		AMEFObject* root2 = dec.decodeB(root->getPackets().at(counter)->getValue(), true, false);
		if(className=="std::string" || className=="string")
		{
			string retVal = root2->getPackets().at(0)->getValueStr();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="int")
		{
			int retVal = root2->getPackets().at(0)->getIntValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="short")
		{
			short retVal = root2->getPackets().at(0)->getShortValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="long")
		{
			long retVal = root2->getPackets().at(0)->getLongValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="long long")
		{
			long long retVal = root2->getPackets().at(0)->getLongLongValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="long double")
		{
			long double retVal = root2->getPackets().at(0)->getLongDoubleValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="unsigned int")
		{
			unsigned int retVal = root2->getPackets().at(0)->getUIntValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="unsigned short")
		{
			unsigned short retVal = root2->getPackets().at(0)->getUShortValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="unsigned long")
		{
			unsigned long retVal = root2->getPackets().at(0)->getULongValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="unsigned long long")
		{
			unsigned long long retVal = root2->getPackets().at(0)->getULongLongValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="float")
		{
			float retVal = root2->getPackets().at(0)->getFloatValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="double")
		{
			double retVal = root2->getPackets().at(0)->getDoubleValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="bool")
		{
			bool retVal = root2->getPackets().at(0)->getBoolValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="char")
		{
			char retVal = root2->getPackets().at(0)->getCharValue();
			addValueToNestedContainer(container, retVal, cont);
		}
		else if(className=="unsigned char")
		{
			unsigned char retVal = root2->getPackets().at(0)->getUCharValue();
			addValueToNestedContainer(container, retVal, cont);
		}
	}
	void* getUnserializableObject(string _1)
	{
		AMEFDecoder dec;
		AMEFObject* root = dec.decodeB(_1, true, false);
		return root;
	}
	void* getValidUnserializableObject(string _1){return NULL;}
	int getContainerSize(void* _1)
	{
		AMEFObject* root = (AMEFObject*)_1;
		return root->getPackets().size();
	}
	string getUnserializableClassName(void* _1, string className)
	{
		AMEFObject* root = (AMEFObject*)_1;
		return root->getNameStr();
	}
	void* getPrimitiveValue(void* _1, string className)
	{
		AMEFObject* root = (AMEFObject*)_1;
		root = root->getPackets().at(0);
		if((className=="signed" || className=="int" || className=="signed int") && className==root->getNameStr())
		{
			int *vt = new int;
			*vt = root->getIntValue();
			return vt;
		}
		else if((className=="unsigned" || className=="unsigned int") && className==root->getNameStr())
		{
			unsigned int *vt = new unsigned int;
			*vt = root->getUIntValue();
			return vt;
		}
		else if((className=="short" || className=="short int" || className=="signed short" || className=="signed short int") && className==root->getNameStr())
		{
			short *vt = new short;
			*vt = root->getShortValue();
			return vt;
		}
		else if((className=="unsigned short" || className=="unsigned short int") && className==root->getNameStr())
		{
			unsigned short *vt = new unsigned short;
			*vt = root->getUShortValue();
			return vt;
		}
		else if((className=="long int" || className=="signed long" || className=="signed long int" || className=="signed long long int") && className==root->getNameStr())
		{
			long *vt = new long;
			*vt = root->getLongLongValue();
			return vt;
		}
		else if((className=="unsigned long long" || className=="unsigned long long int") && className==root->getNameStr())
		{
			unsigned long long *vt = new unsigned long long;
			*vt = root->getULongLongValue();
			return vt;
		}
		else if((className=="char" || className=="signed char") && className==root->getNameStr())
		{
			char *vt = new char;
			*vt = root->getCharValue();
			return vt;
		}
		else if(className=="unsigned char" && className==root->getNameStr())
		{
			unsigned char *vt = new unsigned char;
			*vt = root->getUCharValue();
			return vt;
		}
		else if(className=="Date" && className==root->getNameStr())
		{
			DateFormat formt("yyyy-mm-dd hh:mi:ss");
			return formt.parse(root->getValueStr());
		}
		else if(className=="BinaryData" && className==root->getNameStr())
		{
			return BinaryData::unSerilaize(root->getValueStr());
		}
		else if(className=="float" && className==root->getNameStr())
		{
			float *vt = new float;
			*vt = root->getFloatValue();
			return vt;
		}
		else if(className=="double" && className==root->getNameStr())
		{
			double *vt = new double;
			*vt = root->getDoubleValue();
			return vt;
		}
		else if(className=="long double" && className==root->getNameStr())
		{
			long double *vt = new long double;
			*vt = root->getLongDoubleValue();
			return vt;
		}
		else if(className=="bool" && className==root->getNameStr())
		{
			bool *vt = new bool;
			*vt = root->getBoolValue();
			return vt;
		}
		else if((className=="std::string" || className=="string") && className==root->getNameStr())
		{
			string *vt = new string;
			*vt = root->getValueStr();
			return vt;
		}
		return NULL;
	}
	void cleanUnserializableObject(void* _1){}
	void cleanValidUnserializableObject(void* _1){}

	string getSerializationMethodName(string className, string appName, bool which, string type)
	{
		string methodname;
		if(which)
			methodname = appName + "binarySerialize" + className + type;
		else
			methodname = appName + "binaryUnSerialize" + className + type;
		return methodname;
	}
	/*template <class T> static string serializeset(set<T> t, string appName)
	{
		typedef typename set<T>::iterator iterator_type;
		iterator_type it;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "std::set<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		for(it = t.begin(); it!= t.end(); ++it)
		{
			object.addPacket(serialize<T>(*it, appName));
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializemultiset(multiset<T> t, string appName)
	{
		typedef typename multiset<T>::iterator iterator_type;
		iterator_type it;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "std::multiset<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		for(it = t.begin(); it!= t.end(); ++it)
		{
			object.addPacket(serialize<T>(*it, appName));
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializevec(vector<T> t, string appName)
	{
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "std::vector<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		for (int var = 0; var < (int)t.size(); ++var) {
			object.addPacket(serialize<T>(t.at(var),appName));
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializedq(deque<T> t, string appName)
	{
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "std::deque<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		for (int var = 0; var < (int)t.size(); ++var) {
			object.addPacket(serialize<T>(t.at(var),appName));
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializelist(list<T> t, string appName)
	{
		typedef typename list<T>::iterator iterator_type;
		iterator_type it;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "std::list<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		for(it = t.begin(); it!= t.end(); ++it)
		{
			object.addPacket(serialize<T>(*it, appName));
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializeq(std::queue<T> t, string appName)
	{
		DummyQueue* dptr = (DummyQueue*)&t;
		deque<T>* tt = (deque<T>*)&dptr->dq;
		T td;
		const char *mangled = typeid(td).name();
		string className = demangle(mangled);
		className = "std::queue<" + className + ">";
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		for (int var = 0; var < (int)tt->size(); ++var) {
			object.addPacket(serialize<T>(tt->at(var),appName));
		}
		return enc.encodeB(&object, false);
	}
	template <class T> static string serializestack(stack<T> t, string appName)
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
				object.addPacket(serialize<T>(tt.front(),appName));
				tt.pop();
			}
		}
		return enc.encodeB(&object, false);
	}*/
	//static void* handleMultiLevelUnSerialization(AMEFObject* root, string className, string appName, int& size);
	//static void addTypeContainerToNestedContainer(AMEFObject* roott, string container, string className, string appName, int lsiz, void* cont);
public:
	Serialize(){}
	~Serialize(){}

	template <class T> static string serialize(T t, string appName = "default")
	{
		Serialize serialize;
		string objXml;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return _handleAllSerialization(className,&t,appName, &serialize);
	}
	template <class T> static string serializePointer(T* t, string appName = "default")
	{
		Serialize serialize;
		string objXml;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return _handleAllSerialization(className,t,appName, &serialize);
	}
	static string serializeObject(Object t, string appName = "default")
	{
		Serialize serialize;
		return _handleAllSerialization(t.getTypeName(),t.getVoidPointer(),appName, &serialize);
	}
	string elementToSerializedString(void* _1, int counter){}
	static string serializeUnknown(void* t,string className, string appName = "default")
	{
		Serialize serialize;
		return _handleAllSerialization(className,t,appName, &serialize);
	}


	void* getObjectProperty(void* _1, int counter)
	{
		return NULL;
	}
	template <class K,class V> static string serialize(map<K,V> mp, string appName = "default")
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
	}

	template <class T> static T unserialize(string objXml, string appName = "default")
	{
		Serialize serialize;
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		className = className.substr(0, className.find(",")+1);
		return *(T*)_handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
	}
	template <class T> static T* unserializeToPointer(string objXml, string appName = "default")
	{
		Serialize serialize;
		T t;
		const char *mangled = typeid(t).name();
		string className = demangle(mangled);
		return (T*)_handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
	}

	static void* unSerializeUnknown(string objXml,string className, string appName = "default")
	{
		Serialize serialize;
		return _handleAllUnSerialization(objXml,className,appName,&serialize,false,NULL);
	}

	/*static string serializeConatiners(void* t, string className, string appName)
	{
		Serialize serialize;
		AMEFEncoder enc;
		AMEFObject object;
		object.setName(className);
		if(className.find("std::vector<")==0) {
			StringUtil::replaceFirst(className,"std::vector<","");
			if(className.find("<")==string::npos) {
				string serval = _handleAllSerialization("std::vector<"+className+",", t, appName, &serialize);
				return serval;
			} else {
				if(className.find("std::vector<bool")==0)
				{
					vector<vector<bool> >* ptr = (vector<vector<bool> >*)t;
					vector<vector<bool> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						vector<bool>* la = (vector<bool>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::vector<")==0)
				{
					vector<vector<Dummy> >* ptr = (vector<vector<Dummy> >*)t;
					vector<vector<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						vector<Dummy>* la = (vector<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::deque<")==0)
				{
					vector<deque<Dummy> >* ptr = (vector<deque<Dummy> >*)t;
					vector<deque<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						deque<Dummy>* la = (deque<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::list<")==0)
				{
					vector<list<Dummy> >* ptr = (vector<list<Dummy> >*)t;
					vector<list<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						list<Dummy>* la = (list<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::set<")==0)
				{
					vector<set<Dummy> >* ptr = (vector<set<Dummy> >*)t;
					vector<set<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						set<Dummy>* la = (set<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::multiset<")==0)
				{
					vector<multiset<Dummy> >* ptr = (vector<multiset<Dummy> >*)t;
					vector<multiset<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						multiset<Dummy>* la = (multiset<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
			}
		}
		else if(className.find("std::list<")==0) {
			StringUtil::replaceFirst(className,"std::list<","");
			if(className.find("<")==string::npos) {
				string serval = _handleAllSerialization("std::list<"+className+",", t, appName, &serialize);
				return serval;
			} else {
				list<Dummy>* ptr = (list<Dummy>*)t;
				list<Dummy>::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					list<Dummy>* la = (list<Dummy>*)&(*itls);
					string serval = serializeConatiners(la, className, appName);
					serval = serval.substr(4);
					object.addPacket(serval);
				}
			}
		}
		else if(className.find("std::deque<")==0) {
			StringUtil::replaceFirst(className,"std::deque<","");
			if(className.find("<")==string::npos) {
				string serval = _handleAllSerialization("std::deque<"+className+",", t, appName, &serialize);
				return serval;
			} else {
				if(className.find("std::vector<bool")==0)
				{
					deque<vector<bool> >* ptr = (deque<vector<bool> >*)t;
					deque<vector<bool> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						vector<bool>* la = (vector<bool>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::vector<")==0)
				{
					deque<vector<Dummy> >* ptr = (deque<vector<Dummy> >*)t;
					deque<vector<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						vector<Dummy>* la = (vector<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::deque<")==0)
				{
					deque<deque<Dummy> >* ptr = (deque<deque<Dummy> >*)t;
					deque<deque<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						deque<Dummy>* la = (deque<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::list<")==0)
				{
					deque<list<Dummy> >* ptr = (deque<list<Dummy> >*)t;
					deque<list<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						list<Dummy>* la = (list<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::set<")==0)
				{
					deque<set<Dummy> >* ptr = (deque<set<Dummy> >*)t;
					deque<set<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						set<Dummy>* la = (set<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
				else if(className.find("std::multiset<")==0)
				{
					deque<multiset<Dummy> >* ptr = (deque<multiset<Dummy> >*)t;
					deque<multiset<Dummy> >::iterator itls = ptr->begin();
					for(;itls!=ptr->end();++itls) {
						multiset<Dummy>* la = (multiset<Dummy>*)&(*itls);
						string serval = serializeConatiners(la, className, appName);
						serval = serval.substr(4);
						object.addPacket(serval);
					}
				}
			}
		}
		else if(className.find("std::queue<")==0) {
			StringUtil::replaceFirst(className,"std::queue<","");
			if(className.find("<")==string::npos) {
				string serval = _handleAllSerialization("std::queue<"+className+",", t, appName, &serialize);
				return serval;
			} else {
				DummyQueue* dptr = (DummyQueue*)t;
				deque<deque<Dummy> >* ptr = (deque<deque<Dummy> >*)&dptr->dq;
				deque<deque<Dummy> >::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					deque<deque<Dummy> >* la = (deque<deque<Dummy> >*)&(*itls);
					string serval = serializeConatiners(la, className, appName);
					serval = serval.substr(4);
					object.addPacket(serval);
				}
			}
		}
		else if(className.find("std::set<")==0) {
			StringUtil::replaceFirst(className,"std::set<","");
			if(className.find("<")==string::npos) {
				string serval = _handleAllSerialization("std::set<"+className+",", t, appName, &serialize);
				return serval;
			} else {
				set<Dummy>* ptr = (set<Dummy>*)t;
				set<Dummy>::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					set<Dummy>* la = (set<Dummy>*)&(*itls);
					string serval = serializeConatiners(la, className, appName);
					serval = serval.substr(4);
					object.addPacket(serval);
				}
			}
		}
		else if(className.find("std::multiset<")==0) {
			StringUtil::replaceFirst(className,"std::multiset<","");
			if(className.find("<")==string::npos) {
				string serval = _handleAllSerialization("std::multiset<"+className+",", t, appName, &serialize);
				return serval;
			} else {
				multiset<Dummy>* ptr = (multiset<Dummy>*)t;
				multiset<Dummy>::iterator itls = ptr->begin();
				for(;itls!=ptr->end();++itls) {
					multiset<Dummy>* la = (multiset<Dummy>*)&(*itls);
					string serval = serializeConatiners(la, className, appName);
					serval = serval.substr(4);
					object.addPacket(serval);
				}
			}
		}
		return enc.encodeB(&object, false);
	}*/

	bool isValidClassNamespace(void* _1, string classname, string namespc, bool iscontainer = false)
	{
		return true;
	}

	bool isValidObjectProperty(void* _1, string propname, int c)
	{
		return false;
	}

	void startObjectSerialization(void* _1, string className)
	{}

	void endObjectSerialization(void* _1, string className)
	{}

	void afterAddObjectProperty(void* _1){}
	void addObjectPrimitiveProperty(void* _1, string propName, string className, void* t)
	{}
	void addObjectProperty(void* _1, string propName, string className, string t)
	{}
	void* getObjectPrimitiveValue(void* _1, string className, string propName)
	{}
	string serializeUnknownBase(void* t,string className, string appName)
	{
		return _handleAllSerialization(className,t,appName, this);
	}
	void* unSerializeUnknownBase(void* unserObj,string className, string appName)
	{
		return _handleAllUnSerialization("",className,appName,this,false,unserObj);
	}
};
#endif
