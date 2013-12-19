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
/*
 * CastUtil.h
 *
 *  Created on: Aug 5, 2012
 *      Author: Sumeet
 */

#ifndef CASTUTIL_H_
#define CASTUTIL_H_
#include "sstream"
#include <stdlib.h>
#include "StringUtil.h"
#include <cxxabi.h>
#include "cstring"
#include <stdio.h>
#include <assert.h>
using namespace std;

class CastUtil {
	static string demangle(const char *mangled)
	{
		int status;
		char *demangled;
		using namespace abi;
		demangled = __cxa_demangle(mangled, NULL, 0, &status);
		string s(demangled);
		free(demangled);
		return s;
	}
	template <typename T> static string getClassName(T t)
	{
		const char *mangled = typeid(t).name();
		string tn = demangle(mangled);
		if(tn[tn.length()-1]=='*')
			tn = tn.substr(0,tn.length()-1);
		return tn;
	}
public:
	CastUtil();
	virtual ~CastUtil();
	template <typename T, typename R> static R cast(const T& val)
	{
		return lexical_cast<R>(val);
	}

	template <typename T> static T lexical_cast(const short& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%d", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%d", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - short to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const unsigned short& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%d", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%d", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - unsigned short to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const int& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%d", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%d", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - int to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const unsigned int& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%u", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%u", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - unsigned int to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const long& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%ld", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%ld", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - long to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const unsigned long& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%lu", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%lu", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - unsigned long to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const long long& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%lld", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%lld", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - long long to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const unsigned long long& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%llu", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%llu", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - unsigned long long to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const double& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%f", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%f", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - double to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const long double& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%Lf", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%Lf", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - double to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const float& val)
	{
		T t;
		if(getClassName(t)=="std::string")
		{
			const int n = snprintf(NULL, 0, "%f", val);
			char ty[n+1];
			memset (ty,0,n+1);
			int c = snprintf(ty, n+1, "%f", val);
			assert(ty[n] == '\0');
			assert(c == n);
			return string(ty);
		}
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - float to " + tn);
		}
	}
	template <typename T> static T lexical_cast(const bool& val)
	{
		/*T t;
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			string tn = getClassName(t);
			throw ("Conversion exception - bool to " + tn);
		}*/
		T t;
		string tn = getClassName(t);
		if(tn=="std::string")
			t = val?"true":"false";
		else
			t = val?1:0;
		return t;
	}
	template <typename T> static T lexical_cast(const string& val)
	{
		return lexical_cast<T>(val.c_str());
	}
	template <typename T> static T lexical_cast(const char* val)
	{
		T t;
		string tn = getClassName(t);
		char* endptr;
		if(tn=="double" || tn=="float")
		{
			double d = 0;
			d = strtod(val, &endptr);
			if(*endptr)
			{
				throw "Conversion exception - string to double";
			}
			t = d;
		}
		else if(tn=="long double")
		{
			long double d = 0;
			if(sscanf(val, "%Lf", &d)==1)
				t = d;
			else
			{
				throw "Conversion exception - string to long double";
			}
			t = d;
		}
		else if(tn=="int")
		{
			int d = 1;
			d = strtol(val, &endptr, 10);
			if(*endptr)
			{
				throw "Conversion exception - string to int";
			}
			t = d;
		}
		else if(tn=="short")
		{
			short d = 1;
			d = strtol(val, &endptr, 10);
			if(*endptr)
			{
				throw "Conversion exception - string to short";
			}
			t = d;
		}
		else if(tn=="long")
		{
			long d = 1;
			d = strtol(val, &endptr, 10);
			if(*endptr)
			{
				throw "Conversion exception - string to long";
			}
			t = d;
		}
		else if(tn=="unsigned short")
		{
			unsigned short d = 1;
			d = strtoul(val, &endptr, 10);
			if(*endptr)
			{
				throw "Conversion exception - string to unsigned short";
			}
			t = d;
		}
		else if(tn=="unsigned int")
		{
			unsigned int d = 1;
			d = strtoul(val, &endptr, 10);
			if(*endptr)
			{
				throw "Conversion exception - string to unsigned int";
			}
			t = d;
		}
		else if(tn=="unsigned long")
		{
			unsigned long d = 1;
			d = strtoul(val, &endptr, 10);
			if(*endptr)
			{
				throw "Conversion exception - string to unsigned long";
			}
			t = d;
		}
		else if(tn=="long long")
		{
			long long d = -1;
			if(sscanf(val, "%lld", &d)==1)
				t = d;
			else
			{
				throw "Conversion exception - string to long long";
			}
		}
		else if(tn=="unsigned long long")
		{
			unsigned long long d = -1;
			if(sscanf(val, "%llu", &d)==1)
				t = d;
			else
			{
				throw "Conversion exception - string to unsigned long long";
			}
		}
		else if(tn=="bool")
		{
			bool d = false;
			if(StringUtil::toLowerCopy(val)=="true" || StringUtil::toLowerCopy(val)=="1")
				d = true;
			else if(StringUtil::toLowerCopy(val)=="false" || StringUtil::toLowerCopy(val)=="0")
				d = false;
			else
			{
				throw "Conversion exception - string to bool";
			}
			t = d;
		}
		else if(tn=="std::string" || tn=="string")
		{
			if(strlen(val)==0)return t;
			stringstream ss;
			ss << val;
			ss >> t;
			if(ss)
			{
				return t;
			}
			else
			{
				throw "Conversion exception - string to string";
			}
		}
		else
		{
			throw "Generic Conversion exception";
		}
		return t;
	}
	template <typename T> static bool isPrimitiveDataType()
	{
		T t;
		const char *mangled = typeid(t).name();
		string type = demangle(mangled);
		if(type[type.length()-1]=='*')
			type = type.substr(0,type.length()-1);

		StringUtil::trim(type);
		if(type=="short" || type=="short int" || type=="signed short" || type=="signed short int"
				|| type=="unsigned short" || type=="unsigned short int"
				|| type=="signed" || type=="int" || type=="signed int"
				|| type=="unsigned" || type=="unsigned int" || type=="long"
				|| type=="long int" || type=="signed long" || type=="signed long int"
				|| type=="unsigned long" || type=="unsigned long int"
				|| type=="long long" || type=="long long int" || type=="signed long long"
				|| type=="signed long long int" || type=="unsigned long long"
				|| type=="unsigned long long int" || type=="long double" || type=="bool"
				|| type=="float" || type=="double" || type=="string" || type=="std::string"
				|| type=="char" || type=="signed char" || type=="unsigned char"
				|| type=="wchar_t" ||  type=="char16_t" ||type=="char32_t")
		{
			return true;
		}
		return false;
	}
	template <typename T> static string getTypeName()
	{
		T t;
		const char *mangled = typeid(t).name();
		string type = demangle(mangled);
		if(type[type.length()-1]=='*')
			type = type.substr(0,type.length()-1);

		StringUtil::trim(type);
		if(type=="short" || type=="short int" || type=="signed short" || type=="signed short int")
		{
			return "short";
		}
		else if(type=="unsigned short" || type=="unsigned short int")
		{
			return "unsigned short";
		}
		else if(type=="signed" || type=="int" || type=="signed int")
		{
			return "int";
		}
		else if(type=="unsigned" || type=="unsigned int")
		{
			return "unsigned int";
		}
		else if(type=="long" || type=="long int" || type=="signed long" || type=="signed long int")
		{
			return "long";
		}
		else if(type=="unsigned long" || type=="unsigned long int")
		{
			return "unsigned long";
		}
		else if(type=="long long" || type=="long long int" || type=="signed long long" || type=="signed long long int")
		{
			return "long long";
		}
		else if(type=="unsigned long long" || type=="unsigned long long int")
		{
			return "unsigned long long";
		}
		else if(type=="long double")
		{
			return "long double";
		}
		return type;
	}
};

#endif /* CASTUTIL_H_ */
