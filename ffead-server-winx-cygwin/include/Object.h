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
/*
 * Object.h
 *
 *  Created on: Dec 27, 2009
 *      Author: sumeet
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include <stdexcept>
#include "iostream"
/*Fix for Windows Cygwin*///#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>
#include "string"
#include <sstream>
#include <typeinfo>
#include "Logger.h"
using namespace std;

class Object {
private:
	Logger logger;
	static string demangle(const char *mangled)
	{
		int status;
		char *demangled;
		using namespace abi;
		demangled = __cxa_demangle(mangled, NULL, 0, &status);
		//printf("\n---------Demanged --%s\n\n", demangled);
		stringstream ss;
		ss << demangled;
		string s;
		ss >> s;
		return s;

	}
	string typeName;
	void* pointer;
	string serailizedXML;
public:
	Object()
	{
		logger = Logger::getLogger("Object");
	}
	template <typename T> static string getClassName(T t)
	{
		const char *mangled = typeid(t).name();
		string tn = demangle(mangled);
		if(tn[tn.length()-1]=='*')
			tn = tn.substr(0,tn.length()-1);
		return tn;
	}
	template <typename T> void operator<<(T &t)
	{
		if(getClassName(t)=="Object")
		{
			Object *to = (Object *)&t;
			this->typeName = to->typeName;
			this->pointer = to->pointer;
		}
		else
		{
			this->typeName = getClassName(t);
			this->pointer = &t;
		}
		logger << " " << this->pointer << " " << flush;
		logger << this->typeName << "\n" << flush;
	}
	template <typename T> void operator<<(T *t)
	{
		if(getClassName(t)=="Object")
		{
			Object *to = (Object *)t;
			this->typeName = to->typeName;
			this->pointer = to->pointer;
		}
		else
		{
			this->typeName = getClassName(t);
			this->pointer = t;
		}
		logger << " " << this->pointer << " " << flush;
		logger << this->typeName << "\n" << flush;
	}
	~Object() {
		// TODO Auto-generated destructor stub
	}
	bool isInstanceOf(string className)
	{
		if(this->typeName==className)
			return true;
		else
			return false;
	}
	template <typename T> bool isSimilarObject(T t)
	{
		string cn = getClassName(&t);
		//logger << cn << flush;
		if(isInstanceOf(cn))
			return true;
		else
			return false;
	}
	template <typename T> bool isSimilarObject(T *t)
	{
		string cn = getClassName(t);
		//logger << cn << flush;
		if(isInstanceOf(cn))
			return true;
		else
			return false;
	}
	string getTypeName()
	{
		return this->typeName;
	}
	void setTypeName(string type)
	{
		this->typeName = type;
	}
	template <typename T> T* getPointer()
	{
		T t;
		string cn = getClassName(&t);
		//logger << cn << flush;
		if(isInstanceOf(cn))
			return (T*)this->pointer;
		else
			return NULL;
	}
	template <typename T> T getValue()
	{
		T t;
		string cn = getClassName(&t);
		if(isInstanceOf(cn))
			return *(T*)this->pointer;
		else
			return t;
	}
	void* getVoidPointer()
	{
		return this->pointer;
	}
	template <class T>
	static bool instanceOf(T instance,string className)
	{
		int status;
		const char *mangled = typeid(instance).name();
		using namespace abi;
		mangled = __cxa_demangle(mangled, NULL, 0, &status);
		stringstream ss;
		ss << mangled;
		string cls;
		ss >> cls;
		if(cls==className || cls==(className+"*")) return true;
		else return false;
	}
};

#endif /* OBJECT_H_ */
