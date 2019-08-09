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
 * SerializeBase.h
 *
 *  Created on: 09-Jun-2013
 *      Author: sumeetc
 */

#ifndef SERIALIZEBASE_H_
#define SERIALIZEBASE_H_
#include "AppDefines.h"
#include "CastUtil.h"
#include <stdexcept>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include "string"
#include <sstream>
#include <typeinfo>
#include "queue"
#include "deque"
#include "list"
#include "map"
#include "set"
#include "stack"
#include "DateFormat.h"
#include "BinaryData.h"
#include "Constants.h"
#include "CommonUtils.h"

class Dummy{
public:
	bool operator <(const Dummy& du) const
	{
		return true;
	}
};

class DummyList
{
public:
	DummyList(){}
	struct _List_node_base
	{
#ifdef __clang__
		_List_node_base* _M_prev;
		_List_node_base* _M_next;
#else
		_List_node_base* _M_next;
		_List_node_base* _M_prev;
#endif
	};
	struct _List_impl
	{
		_List_node_base _M_node;
	};
	_List_impl _M_impl;
	friend class SerializeBase;
public:
	bool operator <(const DummyList& du) const
	{
		return true;
	}
};

class DummyDeque
{
	struct Dummy_Deque_iterator
	{
	  void* _M_cur;
	  void* _M_first;
	  void* _M_last;
	  void** _M_node;
	};
	struct _Deque_impl
	{
#ifdef __clang__
		void** _M_map;
		size_t _M_start;
		size_t _M_map_size;
#else
		void** _M_map;
		size_t _M_map_size;
		Dummy_Deque_iterator _M_start;
		Dummy_Deque_iterator _M_finish;
#endif
	};
	_Deque_impl _M_impl;
	friend class SerializeBase;
public:
	bool operator <(const DummyDeque& du) const
	{
		return true;
	}
};

class DummyQueue
{
	DummyDeque dq;
	friend class SerializeBase;
	friend class Serialize;
public:
	bool operator <(const DummyQueue& du) const
	{
		return true;
	}
};

class DummySet
{
	struct _Rb_tree_node_base
	{
	    typedef _Rb_tree_node_base* _Base_ptr;
#ifdef __clang__
	    _Base_ptr		_M_left;
	    _Base_ptr		_M_right;
	    _Base_ptr		_M_parent;
	    bool	_M_color;
#else
	    std::_Rb_tree_color	_M_color;
	    _Base_ptr		_M_parent;
	    _Base_ptr		_M_left;
	    _Base_ptr		_M_right;
#endif
	};
	struct less
	{
	  bool
	  operator()(const int& __x, const int& __y) const
	  { return __x < __y; }
	};
	struct _Rb_tree_impl
	{
	  less		_M_key_compare;
	  _Rb_tree_node_base 	_M_header;
	  size_t 		_M_node_count; // Keeps track of size of tree.
	};
	struct _Rep_type
	{
		_Rb_tree_impl _M_impl;
	};
	_Rep_type _M_t;
	friend class SerializeBase;
public:
	bool operator <(const DummySet& du) const
	{
		return true;
	}
};

class DummyVec
{
	struct _Vector_impl
	{
		void* _M_start;
		void* _M_finish;
		void* _M_end_of_storage;
	};
	_Vector_impl _M_impl;
	friend class SerializeBase;
public:
	bool operator <(const DummyVec& du) const
	{
		return true;
	}
};

class SerializeBase;

typedef std::string (*SerCont) (void*, SerializeBase*, const std::string&);
typedef std::string (*Ser) (void*, SerializeBase*);
typedef void* (*UnSerCont) (void*, SerializeBase*, const std::string&);
typedef void* (*UnSer) (void*, SerializeBase*);


class SerializeBase {
	static void* dlib;
	static cuckoohash_map<std::string, Ser> _serFMap;
	static cuckoohash_map<std::string, SerCont> _serCFMap;
	static cuckoohash_map<std::string, UnSer> _unserFMap;
	static cuckoohash_map<std::string, UnSerCont> _unserCFMap;
	static void init(void* dlib);
	friend class CHServer;
protected:
	static void processClassName(std::string& className);
	static void processClassAndContainerNames(std::string& className, std::string& container);
public:
	template <class T> static std::string serializeset(std::set<T>& t, int serOpt, const std::string& appName, SerializeBase* base)
	{
		T tr;
		int cnt = 0;
		int size = t.size();
		std::string className = CastUtil::getClassName(tr);
		void* object = base->getSerializableObject();
		base->startContainerSerialization(object, className, "std::set");
		for(auto& v: t)
		{
			base->addContainerSerializableElement(object, serializeUnknown((void*)&v, serOpt, className, appName, base));
			base->afterAddContainerSerializableElement(object, cnt++, size);
		}
		base->endContainerSerialization(object, className, "std::set");
		std::string ser = base->fromSerializableObjectToString(object);
		base->cleanSerializableObject(object);
		return ser;
	}
	template <class T> static std::string serializemultiset(std::multiset<T>& t, int serOpt, const std::string& appName, SerializeBase* base)
	{
		T tr;
		int cnt = 0;
		int size = t.size();
		std::string className = CastUtil::getClassName(tr);
		void* object = base->getSerializableObject();
		base->startContainerSerialization(object, className, "std::multiset");
		for(auto& v: t)
		{
			base->addContainerSerializableElement(object, serializeUnknown((void*)&v, serOpt, className, appName, base));
			base->afterAddContainerSerializableElement(object, cnt++, size);
		}
		base->endContainerSerialization(object, className, "std::multiset");
		std::string ser = base->fromSerializableObjectToString(object);
		base->cleanSerializableObject(object);
		return ser;
	}
	template <class T> static std::string serializevec(std::vector<T>& t, int serOpt, const std::string& appName, SerializeBase* base)
	{
		T tr;
		int cnt = 0;
		int size = t.size();
		std::string className = CastUtil::getClassName(tr);
		void* object = base->getSerializableObject();
		base->startContainerSerialization(object, className, "std::vector");
		for(auto& v: t)
		{
			base->addContainerSerializableElement(object, serializeUnknown((void*)&v, serOpt, className, appName, base));
			base->afterAddContainerSerializableElement(object, cnt++, size);
		}
		base->endContainerSerialization(object, className, "std::vector");
		std::string ser = base->fromSerializableObjectToString(object);
		base->cleanSerializableObject(object);
		return ser;
	}
	template <class T> static std::string serializedq(std::deque<T>& t, int serOpt, const std::string& appName, SerializeBase* base)
	{
		T tr;
		int cnt = 0;
		int size = t.size();
		std::string className = CastUtil::getClassName(tr);
		void* object = base->getSerializableObject();
		base->startContainerSerialization(object, className, "std::deque");
		for(auto& v: t)
		{
			base->addContainerSerializableElement(object, serializeUnknown((void*)&v, serOpt, className, appName, base));
			base->afterAddContainerSerializableElement(object, cnt++, size);
		}
		base->endContainerSerialization(object, className, "std::deque");
		std::string ser = base->fromSerializableObjectToString(object);
		base->cleanSerializableObject(object);
		return ser;
	}
	template <class T> static std::string serializelist(std::list<T>& t, int serOpt, const std::string& appName, SerializeBase* base)
	{
		T tr;
		int cnt = 0;
		int size = t.size();
		std::string className = CastUtil::getClassName(tr);
		void* object = base->getSerializableObject();
		base->startContainerSerialization(object, className, "std::list");
		for(auto& v: t)
		{
			base->addContainerSerializableElement(object, serializeUnknown((void*)&v, serOpt, className, appName, base));
			base->afterAddContainerSerializableElement(object, cnt++, size);
		}
		base->endContainerSerialization(object, className, "std::list");
		std::string ser = base->fromSerializableObjectToString(object);
		base->cleanSerializableObject(object);
		return ser;
	}
	template <class T> static std::string serializeq(std::queue<T>& t, int serOpt, const std::string& appName, SerializeBase* base)
	{
		T tr;
		DummyQueue* dptr = (DummyQueue*)&t;
		std::deque<T>* tt = (std::deque<T>*)&dptr->dq;
		int cnt = 0;
		int size = tt->size();
		std::string className = CastUtil::getClassName(tr);
		void* object = base->getSerializableObject();
		base->startContainerSerialization(object, className, "std::queue");
		for(auto& v: *tt)
		{
			base->addContainerSerializableElement(object, serializeUnknown((void*)&v, serOpt, className, appName, base));
			base->afterAddContainerSerializableElement(object, cnt++, size);
		}
		base->endContainerSerialization(object, className, "std::queue");
		std::string ser = base->fromSerializableObjectToString(object);
		base->cleanSerializableObject(object);
		return ser;
	}
	template <class T> static std::string serialize(T t, int serOpt, const std::string& app, SerializeBase* base)
	{
		std::string appName = CommonUtils::getAppName(app);
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		return base->serializeUnknownBase(&t, serOpt, className, appName);
	}
	template <class T> static std::string serializePointer(T* t, int serOpt, const std::string& app, SerializeBase* base)
	{
		std::string appName = CommonUtils::getAppName(app);
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		return base->serializeUnknownBase(t, serOpt, className, appName);
	}
	static std::string serializeUnknown(void* t, int serOpt, const std::string& className, const std::string& appName, SerializeBase* base);
	template <class T> static T unSerialize(void* unserObj, int serOpt, const std::string& app, SerializeBase* base)
	{
		std::string appName = CommonUtils::getAppName(app);
		T t;
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		void* retVP = base->unSerializeUnknownBase(unserObj, serOpt, className, appName);
		if(retVP!=NULL)
		{
			t = *(T*)retVP;
			delete ((T*)retVP);
		}
		return t;
	}
	template <class T> static T* unSerializeToPointer(void* unserObj, int serOpt, const std::string& app, SerializeBase* base)
	{
		std::string appName = CommonUtils::getAppName(app);
		T t;
		std::string className = CastUtil::getClassName(t);
		if(serOpt==-1) serOpt = identifySerOption(className);
		void* retVP = base->unSerializeUnknownBase(unserObj, serOpt, className, appName);
		return (T*)retVP;
	}
	static void* unSerializeUnknown(const std::string& objXml, int serOpt, const std::string& className, const std::string& appName, SerializeBase* base);
	static void* unSerializeUnknown(void* unserObj, int serOpt, const std::string& className, const std::string& appName, SerializeBase* base);
	template <class T> static T* unSerializeKnownToPointer(void* unserObj, int serOpt, const std::string& className, const std::string& app, SerializeBase* base)
	{
		std::string appName = CommonUtils::getAppName(app);
		if(serOpt==-1) serOpt = identifySerOption(className);
		void* retVP = base->unSerializeUnknownBase(unserObj, serOpt, className, appName);
		return (T*)retVP;
	}
	template <class T> static T unSerializeKnown(void* unserObj, int serOpt, const std::string& className, const std::string& app, SerializeBase* base)
	{
		T t;
		std::string appName = CommonUtils::getAppName(app);
		if(serOpt==-1) serOpt = identifySerOption(className);
		void* retVP = base->unSerializeUnknownBase(unserObj, serOpt, className, appName);
		if(retVP!=NULL)
		{
			t = *(T*)retVP;
			delete ((T*)retVP);
		}
		return t;
	}
	template <class T> static T* unSerializeKnownToPointer(const std::string& objXml, int serOpt, const std::string& className, const std::string& app, SerializeBase* base)
	{
		std::string appName = CommonUtils::getAppName(app);
		if(serOpt==-1) serOpt = identifySerOption(className);
		void* retVP = base->unSerializeUnknownBase(objXml, serOpt, className, appName);
		return (T*)retVP;
	}
	template <class T> static T unSerializeKnown(const std::string& objXml, int serOpt, const std::string& className, const std::string& app, SerializeBase* base)
	{
		T t;
		std::string appName = CommonUtils::getAppName(app);
		if(serOpt==-1) serOpt = identifySerOption(className);
		void* retVP = base->unSerializeUnknownBase(objXml, serOpt, className, appName);
		if(retVP!=NULL)
		{
			t = *(T*)retVP;
			delete ((T*)retVP);
		}
		return t;
	}

	static SerCont serContFunc(std::string& className, const std::string& app, const std::string& type);
	static Ser serFunc(std::string& className, const std::string& app);
	static UnSerCont unSerContFunc(std::string& className, const std::string& app, const std::string& type);
	static UnSer unSerFunc(std::string& className, const std::string& app);

	static std::string _serContainer(void* t, const std::string& className, const std::string& app, const std::string& type, SerializeBase* base);
	static std::string _ser(void* t, const std::string& className, const std::string& app, SerializeBase* base);
	static void* _unserContainer(void* unserableObject, const std::string& className, const std::string& app, const std::string& type, SerializeBase* base);
	static void* _unser(void* unserableObject, const std::string& className, const std::string& app, SerializeBase* base);
	static std::string getSerializationMethodName(const std::string& className, const std::string& app, const bool& which, const std::string& type= "");

	//Base Methods for serialization, implementation in Serilaize, XMLSerialize and JSONSerialize
	virtual void* getSerializableObject() = 0;
	virtual void cleanSerializableObject(void* _1) = 0;
	virtual void startContainerSerialization(void* _1, const std::string& className, const std::string& container) = 0;
	virtual void endContainerSerialization(void* _1, const std::string& className, const std::string& container) = 0;
	virtual void afterAddContainerSerializableElement(void* _1, const int& counter, const int& size) = 0;
	virtual void addContainerSerializableElement(void* _1, const std::string& tem) = 0;
	virtual void addContainerSerializableElementMulti(void* _1, const std::string& tem) = 0;
	virtual std::string fromSerializableObjectToString(void* _1) = 0;
	virtual std::string serializePrimitive(int serOpt, const std::string& className, void* t) = 0;
	virtual std::string elementToSerializedString(void* _1, const int& counter) = 0;
	virtual void startObjectSerialization(void* _1, const std::string& className) = 0;
	virtual void endObjectSerialization(void* _1, const std::string& className) = 0;
	virtual void afterAddObjectProperty(void* _1) = 0;
	virtual void addObjectPrimitiveProperty(void* _1, int serOpt, const std::string& propName, const std::string& className, void* t) = 0;
	virtual void addObjectProperty(void* _1, const std::string& propName, std::string className, const std::string& t) = 0;

	virtual bool isValidClassNamespace(void* _1, const std::string& classname, const std::string& namespc, const bool& iscontainer= false) = 0;
	virtual bool isValidObjectProperty(void* _1, const std::string& propname, const int& counter) = 0;
	virtual void* getObjectProperty(void* _1, const int& counter) = 0;
	virtual void* getObjectPrimitiveValue(void* _1, int serOpt, const std::string& className, const std::string& propName) = 0;

	//
	virtual std::string getUnserializableClassName(void* _1, const std::string& className) = 0;
	virtual void* getPrimitiveValue(void* _1, int serOpt, const std::string& className) = 0;
	virtual void* getContainerElement(void* _1, const int& counter, const int& counter1= -1) = 0;
	virtual void* getUnserializableObject(const std::string& _1) = 0;
	virtual void cleanUnserializableObject(void* _1) = 0;
	virtual void cleanValidUnserializableObject(void* _1) = 0;
	virtual void* getValidUnserializableObject(const std::string& _1) = 0;
	virtual int getContainerSize(void* _1) = 0;
	virtual std::string getConatinerElementClassName(void* _1, const std::string& className) = 0;
	virtual void addPrimitiveElementToContainer(void* _1, int serOpt, const int& counter, const std::string& className, void* cont, const std::string& container) = 0;

	virtual std::string serializeUnknownBase(void* t, int serOpt, const std::string& className, const std::string& app) = 0;
	virtual void* unSerializeUnknownBase(void* unserObj, int serOpt, const std::string& className, const std::string& app) = 0;
	virtual void* unSerializeUnknownBase(const std::string& serVal, int serOpt, const std::string& className, const std::string& app) = 0;

	static int identifySerOption(std::string className);
	static std::string _handleAllSerialization(int serOpt, std::string className, void *t, const std::string& app, SerializeBase* base, Ser f1, SerCont f2, SerCont f3);
	static void* _handleAllUnSerialization(const std::string& serVal, int serOpt, std::string className, const std::string& app, SerializeBase* base, const bool& isJsonSer, void* serObject);

	static std::string handleMultiLevelSerialization(void* t, std::string className, const std::string& app, SerializeBase* base);
	static void* handleMultiLevelUnSerialization(void* root, std::string className, const std::string& app, int& size, SerializeBase* base);

	static std::string getTypeName(const std::string& type);
	static bool isPrimitiveDataType(const std::string& type);
	static std::string getTemplateArg(const std::string& s, std::string& tem);
	static void addToNestedContainer(void* roott, const std::string& className, const std::string& app, int& lsiz, const std::string& container, void* cont, const int& var, SerializeBase* base);
	static void* getNestedContainer(std::string& className);

	static void* unserializevec(void* unserableObject, int serOpt, const std::string& app, int &size, SerializeBase* base, const std::string& classN);
	static void* unserializelist(void* unserableObject, int serOpt, const std::string& app, int &size, SerializeBase* base, const std::string& classN);
	static void* unserializeset(void* unserableObject, int serOpt, const std::string& app, int &size, SerializeBase* base, const std::string& classN);
	static void* unserializemultiset(void* unserableObject, int serOpt, const std::string& app, int &size, SerializeBase* base, const std::string& classN);
	static void* unserializeq(void* unserableObject, int serOpt, const std::string& app, int &size, SerializeBase* base, const std::string& classN);
	static void* unserializedq(void* unserableObject, int serOpt, const std::string& app, int &size, SerializeBase* base, const std::string& classN);

	virtual ~SerializeBase();

	template <typename T> static void* getNewNestedContainer(const std::string& container)
	{
		if(container=="std::vector" || container=="vector")
		{
			return new std::vector<T>;
		}
		else if(container=="std::deque" || container=="deque")
		{
			return new std::deque<T>;
		}
		else if(container=="std::list" || container=="list")
		{
			return new std::list<T>;
		}
		else if(container=="std::queue" || container=="queue")
		{
			return new std::queue<T>;
		}
		return NULL;
	}

	template <typename T> static void addValueToNestedContainer(const std::string& container, const T& t, void* cont)
	{
		if(container.find("std::vector")==0 || container.find("vector")==0)
		{
			((std::vector<T>*)cont)->push_back(t);
		}
		else if(container.find("std::deque")==0 || container.find("deque")==0)
		{
			((std::deque<T>*)cont)->push_back(t);
		}
		else if(container.find("std::list")==0 || container.find("list")==0)
		{
			((std::list<T>*)cont)->push_back(t);
		}
		else if(container.find("std::queue")==0 || container.find("queue")==0)
		{
			((std::queue<T>*)cont)->push(t);
		}
	}

	template <typename T> static int getNestedContainerSize(const std::string& container, void* cont)
	{
		if(container.find("std::vector")==0 || container.find("vector")==0)
		{
			return ((std::vector<T>*)cont)->size();
		}
		else if(container.find("std::deque")==0 || container.find("deque")==0)
		{
			return ((std::deque<T>*)cont)->size();
		}
		else if(container.find("std::list")==0 || container.find("list")==0)
		{
			return ((std::list<T>*)cont)->size();
		}
		else if(container.find("std::queue")==0 || container.find("queue")==0)
		{
			return ((std::queue<T>*)cont)->size();
		}
		return -1;
	}

	template <typename T> static T getValueFromNestedContainer(const std::string& container, void* cont, const int& pos)
	{
		T t;
		if(container.find("std::vector")==0 || container.find("vector")==0)
		{
			return ((std::vector<T>*)cont)->at(pos);
		}
		else if(container.find("std::deque")==0 || container.find("deque")==0)
		{
			return ((std::deque<T>*)cont)->at(pos);
		}
		else if(container.find("std::list")==0 || container.find("list")==0)
		{
			typedef typename std::list<T>::iterator iterator_type;
			iterator_type it;
			it = ((std::list<T>*)cont)->begin();
			for(int i=0;i<pos;++i, ++it){}
			t = *it;
			return t;
		}
		else if(container.find("std::queue")==0 || container.find("queue")==0)
		{
			std::vector<T> tv;
			int c = 0;
			std::queue<T>* tq = (std::queue<T>*)cont;
			while (!tq->empty()) {
				tv.push_back(tq->front());
				tq->pop();
			}
			typedef typename std::vector<T>::iterator iterator_type;
			iterator_type it;
			for(it=tv.begin();it!=tv.end();++it) {
				if(c++==pos) {
					t = *it;
				}
				tq->push(*it);
			}
		}
		return t;
	}

	template <typename T> static void* getNewNestedContainerSV(const std::string& container)
	{
		if(container=="std::set" || container=="set")
		{
			return new std::set<T>;
		}
		else if(container=="std::multiset" || container=="multiset")
		{
			return new std::multiset<T>;
		}
		return NULL;
	}

	template <typename T> static void addValueToNestedContainerSV(const std::string& container, const T& t, void* cont)
	{
		if(container.find("std::set")==0 || container.find("set")==0)
		{
			((std::set<T>*)cont)->insert(t);
		}
		else if(container.find("std::multiset")==0 || container.find("multiset")==0)
		{
			((std::multiset<T>*)cont)->insert(t);
		}
	}

	template <typename T> static int getNestedContainerSizeSV(const std::string& container, void* cont)
	{
		if(container.find("std::set")==0 || container.find("set")==0)
		{
			return ((std::set<T>*)cont)->size();
		}
		else if(container.find("std::multiset")==0 || container.find("multiset")==0)
		{
			return ((std::multiset<T>*)cont)->size();
		}
		return -1;
	}

	template <typename T> static T getValueFromNestedContainerSV(const std::string& container, void* cont, const int& pos)
	{
		if(container.find("std::set")==0 || container.find("set")==0)
		{
			typedef typename std::set<T>::iterator iterator_type;
			iterator_type it;
			it = ((std::set<T>*)cont)->begin();
			for(int i=0;i<pos;++i, ++it){}
			return *it;
		}
		else if(container.find("std::multiset")==0 || container.find("multiset")==0)
		{
			typedef typename std::multiset<T>::iterator iterator_type;
			iterator_type it;
			it = ((std::multiset<T>*)cont)->begin();
			for(int i=0;i<pos;++i, ++it){}
			return *it;
		}
		T t;
		return t;
	}
	static std::string trySerialize(void* t, int serOpt, std::string& className, const std::string& app = "");
};

#endif /* SERIALIZEBASE_H_ */
