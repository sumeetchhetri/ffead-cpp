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
#ifndef REFLECTOR_H_
#define REFLECTOR_H_
#include "ClassInfo.h"
#include "string"
#include "Method.h"
#include "Field.h"
#include <stdio.h>
#include <sys/wait.h>
#include <stdexcept>
/*Fix for Windows Cygwin*///#include <execinfo.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <typeinfo>
#include "Constants.h"
#include<iostream>

class Reflector
{
	bool dlibinstantiated;
	void* dlib;
	vector<string> objectT;
	vector<void*> objects;
	void cleanUp();
public:
	Reflector();
	Reflector(void*);
	virtual ~Reflector();
	ClassInfo getClassInfo(string,string appName = "default");
	void* getMethodInstance(Method method,string appName = "default")
	{
		string methodname = appName + "invokeReflectionCIMethodFor"+method.getMethodName();
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (void*,vals);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			return mkr;
		}
		return NULL;
	}
	template <class T> T invokeMethod(void* instance,Method method,vals values,string appName = "default")
	{
		T *obj;
		string methodname = appName + "invokeReflectionCIMethodFor"+method.getMethodName();
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (void*,vals);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			if(method.getReturnType()!="void")
				obj = (T*)f(instance,values);
			else
				f(instance,values);
		}
		return *obj;
	}
	void destroy(void* instance,string classn,string appName = "default")
	{
		StringUtil::replaceAll(classn, "::", "_");
		string methodname = appName + "invokeReflectionCIDtorFor"+classn;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void (*RfPtr) (void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			f(instance);
		}
	}
	void* invokeMethodGVP(void* instance,Method method,vals values,string appName = "default")
	{
		void *obj = NULL;
		string methodname = appName + "invokeReflectionCIMethodFor"+method.getMethodName();
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (void*,vals);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			if(method.getReturnType()!="void")
				obj = f(instance,values);
			else
				f(instance,values);
		}
		return obj;
	}

	template <class T> T newInstance(Constructor ctor,vals values,string appName = "default")
	{
		T *obj;
		string methodname = appName + "invokeReflectionCICtorFor"+ctor.getName();
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (vals);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			obj = (T*)f(values);
		}
		objectT.push_back(ctor.getName());
		objects.push_back(obj);
		return *obj;
	}
	template <class T> T newInstance(Constructor ctor,string appName = "default")
	{
		vals values;
		return newInstance<T>(ctor,values,appName);
	}

	void* newInstanceGVP(Constructor ctor,vals values,string appName = "default")
	{
		void *obj = NULL;
		string methodname = appName + "invokeReflectionCICtorFor"+ctor.getName();
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (vals);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			obj = f(values);
		}
		objectT.push_back(ctor.getName());
		objects.push_back(obj);
		return obj;
	}
	void* newInstanceGVP(Constructor ctor,string appName = "default")
	{
		vals values;
		return newInstanceGVP(ctor,values,appName);
	}

	void* invokeMethodUnknownReturn(void* instance,Method method,vals values,string appName = "default")
	{
		void* obj = NULL;
		string methodname = appName + "invokeReflectionCIMethodFor"+method.getMethodName();
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (void*,vals);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			if(method.getReturnType()!="void")
				obj = f(instance,values);
			else
				f(instance,values);
		}
		return obj;
	}

	template <class T> T getField(void* instance,Field field,string appName = "default")
	{
		T t;
		string fldname = appName + "invokeReflectionCIFieldFor"+field.getFieldName();
		void *mkr = dlsym(dlib, fldname.c_str());
		typedef T (*RfPtr) (void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			t = f(instance);
		}
		return t;
	}
	void* execOperator(void* instance,string operato,vals values,string classn,string appName = "default")
	{
		StringUtil::replaceAll(classn, "::", "_");
		void *resul = NULL;
		if(operato=="<")
		{
			string opname = appName + "operator"+classn+"LT";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato==">")
		{
			string opname = appName + "operator"+classn+"GT";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="==")
		{
			string opname = appName + "operator"+classn+"EQ";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="!=")
		{
			string opname = appName + "operator"+classn+"NE";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="<=")
		{
			string opname = appName + "operator"+classn+"LE";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato==">=")
		{
			string opname = appName + "operator"+classn+"GE";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="!")
		{
			string opname = appName + "operator"+classn+"NT";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="<<")
		{

		}
		else if(operato==">>")
		{

		}
		else if(operato=="+")
		{
			string opname = appName + "operator"+classn+"AD";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="-")
		{
			string opname = appName + "operator"+classn+"SU";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="/")
		{
			string opname = appName + "operator"+classn+"DI";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="*")
		{
			string opname = appName + "operator"+classn+"MU";
			void *mkr = dlsym(dlib, opname.c_str());
			typedef void* (*RfPtr) (void*,vals);
			RfPtr f = (RfPtr)mkr;
			if(f!=NULL)
			{
				resul = f(instance,values);
			}
		}
		else if(operato=="&&")
		{

		}
		else if(operato=="&")
		{

		}
		else if(operato=="||")
		{

		}
		else if(operato=="|")
		{

		}
		else if(operato=="[]")
		{

		}
		else if(operato=="()")
		{

		}
		return resul;
	}

	void vectorPushBack(void* vec,void* instance,string classN,string appName = "default")
	{
		StringUtil::replaceAll(classN, "::", "_");
		string methodname = appName + "invokeAdToVecFor"+classN;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (void*,void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			f(vec,instance);
		}
	}
	void* getNewVector(string classN,string appName = "default")
	{
		StringUtil::replaceAll(classN, "::", "_");
		void *obj = NULL;
		string methodname = appName + "invokeGetNewVecFor"+classN;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) ();
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			obj = f();
		}
		return obj;
	}
	int getVectorSize(void* vec,string classN,string appName = "default")
	{
		StringUtil::replaceAll(classN, "::", "_");
		int obj = 0;
		string methodname = appName + "invokeGetVecSizeFor"+classN;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef int (*RfPtr) (void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			obj = f(vec);
		}
		return obj;
	}
	void* getVectorElement(void* vec,int pos,string classN,string appName = "default")
	{
		StringUtil::replaceAll(classN, "::", "_");
		void *obj = NULL;
		string methodname = appName + "invokeGetVecElementFor"+classN;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (void*,int);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			obj = f(vec,pos);
		}
		return obj;
	}
};
#endif
