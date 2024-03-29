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
#ifndef REFLECTOR_H_
#define REFLECTOR_H_
#include "AppDefines.h"
#include "ClassInfo.h"
#include "string"
#include "Method.h"
#include "Field.h"
#include <stdio.h>
#if !defined(OS_MINGW)
#include <sys/wait.h>
#endif
#include <dlfcn.h>
#include <stdexcept>
/*Fix for Windows Cygwin*///#include <execinfo.h>

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <typeinfo>
#include "Constants.h"
#include <iostream>
#include "queue"
#include "deque"
#include "list"
#include "map"
#include "set"
#include "CommonUtils.h"
#include "Date.h"
#include "BinaryData.h"

typedef void* (*ExecOp) (void*,vals,bool);
typedef void* (*GetNewCont) (void*,void*,int,std::string,int);
typedef void (*DestCont) (void*,void*,int,std::string,int);
typedef int* (*GetContSize) (void*,void*,int,std::string,int);
typedef void (*AddToCont) (void*,void*,int,std::string,int);
typedef void* (*GetContEleAt) (void*,void*,int,std::string,int);

class TemplateUtil {
	static void* ddlib;
public:
	static void init(void* ddlib);
	static TemplatePtr getTemplateFunc(const std::string& appName, const std::string& tpe);
};

class Reflector
{
	static ClassInfo nullclass;
	static ThreadLocal _ciMap;
	void* dlib;
	friend class FFEADContext;
public:
	static void cleanUp();
	Reflector();
	Reflector(void*);
	virtual ~Reflector();
	ClassInfo* getClassInfo(const std::string&, const std::string& app= "");
	ClassInfo* getClassInfo(const std::string&, std::string_view);
	GetMeth getMethodInstance(const Method& method)
	{
		return method.getIns()!=NULL?method.getIns():(GetMeth)dlsym(dlib, method.getRefName().c_str());
	}
	template <class T> T invokeMethod(void* instance, const Method& method, const vals& values, const bool& cleanVals = false)
	{
		T obj;
		GetMeth f = getMethodInstance(method);
		if(f!=NULL)
		{
			if(method.getReturnType()!="void")
			{
				void* rt = f(instance,values,cleanVals);
				obj = *(T*)rt;
				delete (T*)rt;
			}
			else
				f(instance,values,cleanVals);
		}
		return obj;
	}
	template <class T> void invokeMethod(T* t, void* instance, const Method& method, const vals& values, const bool& cleanVals = false)
	{
		GetMeth f = getMethodInstance(method);
		if(f!=NULL)
		{
			if(method.getReturnType()!="void")
			{
				void* rt = f(instance,values,cleanVals);
				*t = *(T*)rt;
				delete (T*)rt;
			}
			else
				f(instance,values,cleanVals);
		}
	}
	void destroy(void* instance, std::string cs, const std::string& app= "");
	void destroy(void* instance, std::string cs, std::string_view);
	void destroy(int serOpt, void* instance, std::string cs, const std::string& app= "");
	void destroy(int serOpt, void* instance, std::string cs, std::string_view);
	void* invokeMethodGVP(void* instance, const Method& method, const vals& values, const bool& cleanVals = false)
	{
		void *obj = NULL;
		GetMeth f = getMethodInstance(method);
		if(f!=NULL)
		{
			if(method.getReturnType()!="void")
				obj = f(instance,values,cleanVals);
			else
				f(instance,values,cleanVals);
		}
		return obj;
	}

	template <class T> T newInstance(const Constructor& ctor, const vals& values, const bool& cleanVals = false)
	{
		T obj;
		NewInst f = ctor.getIns()!=NULL?ctor.getIns():(NewInst)dlsym(dlib, ctor.getRefName().c_str());
		if(f!=NULL)
		{
			T* objt = (T*)f(values,cleanVals);
			obj = *objt;
			delete objt;
		}
		return *obj;
	}
	template <class T> T newInstance(const Constructor& ctor, const bool& cleanVals = false)
	{
		vals values;
		return newInstance<T>(ctor,values,cleanVals);
	}

	void* newInstanceGVP(const Constructor& ctor, const vals& values, const bool& cleanVals = false)
	{
		void *obj = NULL;
		NewInst f = ctor.getIns()!=NULL?ctor.getIns():(NewInst)dlsym(dlib, ctor.getRefName().c_str());
		if(f!=NULL)
		{
			obj = f(values,cleanVals);
		}
		return obj;
	}
	void* newInstanceGVP(const Constructor& ctor, const bool& cleanVals = false)
	{
		vals values;
		return newInstanceGVP(ctor,values,cleanVals);
	}

	void* invokeMethodUnknownReturn(void* instance, const Method& method, const vals& values, const bool& cleanVals = false)
	{
		void* obj = NULL;
		GetMeth f = getMethodInstance(method);
		if(f!=NULL)
		{
			if(method.getReturnType()!="void")
				obj = f(instance,values,cleanVals);
			else
				f(instance,values,cleanVals);
		}
		return obj;
	}

	template <class T> T getField(void* instance, const Field& field)
	{
		T t;
		GetFld f = field.getIns()!=NULL?field.getIns():(GetFld)dlsym(dlib, field.getRefName().c_str());
		if(f!=NULL)
		{
			T* fld = (T*)f(instance);
			t = *fld;
			delete fld;
		}
		return t;
	}

	void* execOperator(void* instance, const std::string& operato, const vals& values, const std::string& cs, const bool& cleanVals = false, const std::string& app= "")
	{
		ClassInfo* ci = getClassInfo(cs, app);
		std::string oprfn = ci->getOperatorRefName(operato);
		void *resul = NULL;
		void *mkr = dlsym(dlib, oprfn.c_str());
		ExecOp f = (ExecOp)mkr;
		if(f!=NULL)
		{
			resul = f(instance,values,cleanVals);
		}
		return resul;
	}
	void* getNewContainer(const std::string& cs, const std::string& contType, const std::string& app= "")
	{
		ClassInfo* ci = getClassInfo(cs, app);
		void *obj = NULL;
		std::string methodname = ci->getContRefName();
		int t = 1;
		if(contType=="std::set" || contType=="std::multiset") {
			t = 6;
			methodname += "sv";
		}
		void *mkr = dlsym(dlib, methodname.c_str());
		GetNewCont f = (GetNewCont)mkr;
		if(f!=NULL)
		{
			obj = f(NULL,NULL,-1,contType,t);
		}
		return obj;
	}
	void destroyContainer(void* vec, const std::string& cs, const std::string& contType, const std::string& app= "")
	{
		ClassInfo* ci = getClassInfo(cs, app);
		std::string methodname = ci->getContRefName();
		int t = -1;
		if(contType=="std::set" || contType=="std::multiset") {
			t = 0;
			methodname += "sv";
		}
		void *mkr = dlsym(dlib, methodname.c_str());
		DestCont f = (DestCont)mkr;
		if(f!=NULL)
		{
			f(vec,NULL,-1,contType,t);
		}
	}
	int getContainerSize(void* vec, const std::string& cs, const std::string& contType, const std::string& app= "")
	{
		ClassInfo* ci = getClassInfo(cs, app);
		int size = -1;
		std::string methodname = ci->getContRefName();
		int t = 2;
		if(contType=="std::set" || contType=="std::multiset") {
			t = 7;
			methodname += "sv";
		}
		void *mkr = dlsym(dlib, methodname.c_str());
		GetContSize f = (GetContSize)mkr;
		if(f!=NULL)
		{
			int* tt = f(vec,NULL,-1,contType,t);
			size = *tt;
			delete tt;
		}
		return size;
	}
	void addToContainer(void* vec, void* instance, const std::string& cs, const std::string& contType, const std::string& app= "")
	{
		ClassInfo* ci = getClassInfo(cs, app);
		std::string methodname = ci->getContRefName();
		int t = 3;
		if(contType=="std::set" || contType=="std::multiset") {
			t = 8;
			methodname += "sv";
		}
		void *mkr = dlsym(dlib, methodname.c_str());
		AddToCont f = (AddToCont)mkr;
		if(f!=NULL)
		{
			f(vec,instance,-1,contType,t);
		}
	}
	void* getContainerElementValueAt(void* vec, const int& pos, const std::string& cs, const std::string& contType, const std::string& app= "")
	{
		ClassInfo* ci = getClassInfo(cs, app);
		void *obj = NULL;
		std::string methodname = ci->getContRefName();
		int t = 4;
		if(contType=="std::set" || contType=="std::multiset") {
			t = 9;
			methodname += "sv";
		}
		void *mkr = dlsym(dlib, methodname.c_str());
		GetContEleAt f = (GetContEleAt)mkr;
		if(f!=NULL)
		{
			obj = f(vec,NULL,pos,contType,t);
		}
		return obj;
	}
	void* getContainerElementAt(void* vec, const int& pos, const std::string& cs, const std::string& contType, const std::string& app= "")
	{
		ClassInfo* ci = getClassInfo(cs, app);
		void *obj = NULL;
		std::string methodname = ci->getContRefName();
		int t = 5;
		if(contType=="std::set" || contType=="std::multiset") {
			t = 10;
			methodname += "sv";
		}
		void *mkr = dlsym(dlib, methodname.c_str());
		GetContEleAt f = (GetContEleAt)mkr;
		if(f!=NULL)
		{
			obj = f(vec,NULL,pos,contType,t);
		}
		return obj;
	}
	template <typename T> static void* getNewNestedContainer(const std::string& container)
	{
		if(container=="std::vector")
		{
			return new std::vector<T>;
		}
		else if(container=="std::deque")
		{
			return new std::deque<T>;
		}
		else if(container=="std::list")
		{
			return new std::list<T>;
		}
		else if(container=="std::queue")
		{
			return new std::queue<T>;
		}
		return NULL;
	}

	template <typename T> static void* destroyNestedContainer(const std::string& container, void* instance)
	{
		if(container=="std::vector")
		{
			delete (std::vector<T>*)instance;
		}
		else if(container=="std::deque")
		{
			delete (std::deque<T>*)instance;
		}
		else if(container=="std::list")
		{
			delete (std::list<T>*)instance;
		}
		else if(container=="std::queue")
		{
			delete (std::queue<T>*)instance;
		}
		return NULL;
	}

	template <typename T> static void* destroyNestedContainerSV(const std::string& container, void* instance)
	{
		if(container=="std::set")
		{
			delete (std::set<T>*)instance;
		}
		else if(container=="std::multiset")
		{
			delete (std::multiset<T>*)instance;
		}
		return NULL;
	}

	template <typename T> static void* getNewNestedContainerSV(const std::string& container)
	{
		if(container=="std::set")
		{
			return new std::set<T>;
		}
		else if(container=="std::multiset")
		{
			return new std::multiset<T>;
		}
		return NULL;
	}

	template <typename T> static void addValueToNestedContainer(const std::string& container, const T& t, void* cont)
	{
		if(container.find("std::vector")==0)
		{
			((std::vector<T>*)cont)->push_back(t);
		}
		else if(container.find("std::deque")==0)
		{
			((std::deque<T>*)cont)->push_back(t);
		}
		else if(container.find("std::list")==0)
		{
			((std::list<T>*)cont)->push_back(t);
		}
		else if(container.find("std::queue")==0)
		{
			((std::queue<T>*)cont)->push(t);
		}
	}

	template <typename T> static void addValueToNestedContainerSV(const std::string& container, const T& t, void* cont)
	{
		if(container.find("std::set")==0)
		{
			((std::set<T>*)cont)->insert(t);
		}
		else if(container.find("std::multiset")==0)
		{
			((std::multiset<T>*)cont)->insert(t);
		}
	}

	template <typename T> static int getNestedContainerSize(const std::string& container, void* cont)
	{
		if(container.find("std::vector")==0)
		{
			return ((std::vector<T>*)cont)->size();
		}
		else if(container.find("std::deque")==0)
		{
			return ((std::deque<T>*)cont)->size();
		}
		else if(container.find("std::list")==0)
		{
			return ((std::list<T>*)cont)->size();
		}
		else if(container.find("std::queue")==0)
		{
			return ((std::queue<T>*)cont)->size();
		}
		return -1;
	}

	template <typename T> static int getNestedContainerSizeSV(const std::string& container, void* cont)
	{
		if(container.find("std::set")==0)
		{
			return ((std::set<T>*)cont)->size();
		}
		else if(container.find("std::multiset")==0)
		{
			return ((std::multiset<T>*)cont)->size();
		}
		return -1;
	}

	template <typename T> static T getValueFromNestedContainer(const std::string& container, void* cont, const int& pos)
	{
		T t;
		if(container.find("std::vector")==0)
		{
			return ((std::vector<T>*)cont)->at(pos);
		}
		else if(container.find("std::deque")==0)
		{
			return ((std::deque<T>*)cont)->at(pos);
		}
		else if(container.find("std::list")==0)
		{
			typedef typename std::list<T>::iterator iterator_type;
			iterator_type it;
			it = ((std::list<T>*)cont)->begin();
			for(int i=0;i<pos;++i, ++it){}
			t = *it;
		}
		else if(container.find("std::queue")==0)
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

	template <typename T> static void* getPValueFromNestedContainer(const std::string& container, void* cont, const int& pos)
	{
		void *t = NULL;
		if(container.find("std::vector")==0)
		{
			return (void*)&((std::vector<T>*)cont)->at(pos);
		}
		else if(container.find("std::deque")==0)
		{
			return (void*)&((std::deque<T>*)cont)->at(pos);
		}
		else if(container.find("std::list")==0)
		{
			typedef typename std::list<T>::iterator iterator_type;
			iterator_type it;
			it = ((std::list<T>*)cont)->begin();
			for(int i=0;i<pos;++i, ++it){}
			return (void*)&(*it);
		}
		else if(container.find("std::queue")==0)
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
				tq->push(*it);
				if(c++==pos) {
					t = &(tq->back());
				}
			}
		}
		return NULL;
	}

	template <typename T> static T getValueFromNestedContainerSV(const std::string& container, void* cont, const int& pos)
	{
		if(container.find("std::set")==0)
		{
			typedef typename std::set<T>::iterator iterator_type;
			iterator_type it;
			it = ((std::set<T>*)cont)->begin();
			for(int i=0;i<pos;++i, ++it){}
			return *it;
		}
		else if(container.find("std::multiset")==0)
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

	template <typename T> static void* getPValueFromNestedContainerSV(const std::string& container, void* cont, const int& pos)
	{
		if(container.find("std::set")==0)
		{
			typedef typename std::set<T>::iterator iterator_type;
			iterator_type it;
			it = ((std::set<T>*)cont)->begin();
			for(int i=0;i<pos;++i, ++it){}
			return (void*)&(*it);
		}
		else if(container.find("std::multiset")==0)
		{
			typedef typename std::multiset<T>::iterator iterator_type;
			iterator_type it;
			it = ((std::multiset<T>*)cont)->begin();
			for(int i=0;i<pos;++i, ++it){}
			return (void*)&(*it);
		}
		return NULL;
	}
};
#endif
