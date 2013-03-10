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
using namespace std;

class CastUtil {
	static string demangle(const char *mangled)
	{
		int status;
		char *demangled;
		using namespace abi;
		demangled = __cxa_demangle(mangled, NULL, 0, &status);
		string s(demangled);
		delete demangled;
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
		T t;
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
		}
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
		else if(tn=="size_t")
		{
			long int d = 1;
			d = strtol(val, &endptr, 10);
			if(*endptr)
			{
				throw "Conversion exception - string to size_t";
			}
			t = (size_t)d;
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
};

#endif /* CASTUTIL_H_ */
