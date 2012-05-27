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
#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <typeinfo>

class Reflector
{
	vector<string> objectT;
	vector<void*> objects;
	string demangle(const char *mangled)
	{
		int status;	char *demangled;
		using namespace abi;
		demangled = __cxa_demangle(mangled, NULL, 0, &status);
		printf("\n---------Demanged --%s\n\n", demangled);
		stringstream ss;
		ss << demangled;
		string s;
		ss >> s;
		return s;
	}
	string getClassName(void* instance)
	{
		const char *mangled = typeid(instance).name();
		return demangle(mangled);
	}
	void cleanUp();
public:
	Reflector();
	virtual ~Reflector();
	ClassInfo getClassInfo(string);
	template <class T> T invokeMethod(void* instance,Method method,vals values)
	{
		T *obj;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeReflectionCIMethodFor"+method.getMethodName();
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
	void destroy(void* instance,string classn)
	{
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeReflectionCIDtorFor"+classn;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void (*RfPtr) (void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			f(instance);
		}
	}
	void* invokeMethodGVP(void* instance,Method method,vals values)
	{
		void *obj = NULL;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeReflectionCIMethodFor"+method.getMethodName();
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

	template <class T> T newInstance(Constructor ctor,vals values)
	{
		T *obj;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeReflectionCICtorFor"+ctor.getName();
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
	template <class T> T newInstance(Constructor ctor)
	{
		vals values;
		return newInstance<T>(ctor,values);
	}

	void* newInstanceGVP(Constructor ctor,vals values)
	{
		void *obj = NULL;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeReflectionCICtorFor"+ctor.getName();
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
	void* newInstanceGVP(Constructor ctor)
	{
		vals values;
		return newInstanceGVP(ctor,values);
	}

	void* invokeMethodUnknownReturn(void* instance,Method method,vals values)
	{
		void* obj = NULL;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeReflectionCIMethodFor"+method.getMethodName();
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

	template <class T> T getField(void* instance,Field field)
	{
		T t;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string fldname = "invokeReflectionCIFieldFor"+field.getFieldName();
		void *mkr = dlsym(dlib, fldname.c_str());
		typedef T (*RfPtr) (void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			t = f(instance);
		}
		return t;
	}
	void* execOperator(void* instance,string operato,vals values,string classn)
	{
		void *resul = NULL;
		if(operato=="<")
		{
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"LT";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"GT";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"EQ";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"NE";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"LE";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"GE";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"NT";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"AD";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"SU";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"DI";
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
			string libName = "libinter.so";
			void *dlib = dlopen(libName.c_str(), RTLD_NOW);
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string opname = "operator"+classn+"MU";
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

	void vectorPushBack(void* vec,void* instance,string classN)
	{
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeAdToVecFor"+classN;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) (void*,void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			f(vec,instance);
		}
	}
	void* getNewVector(string classN)
	{
		void *obj = NULL;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeGetNewVecFor"+classN;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef void* (*RfPtr) ();
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			obj = f();
		}
		return obj;
	}
	int getVectorSize(void* vec,string classN)
	{
		int obj = 0;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeGetVecSizeFor"+classN;
		void *mkr = dlsym(dlib, methodname.c_str());
		typedef int (*RfPtr) (void*);
		RfPtr f = (RfPtr)mkr;
		if(f!=NULL)
		{
			obj = f(vec);
		}
		return obj;
	}
	void* getVectorElement(void* vec,int pos,string classN)
	{
		void *obj = NULL;
		string libName = "libinter.so";
		void *dlib = dlopen(libName.c_str(), RTLD_NOW);
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		string methodname = "invokeGetVecElementFor"+classN;
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
