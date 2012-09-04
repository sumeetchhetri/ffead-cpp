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
using namespace std;

class CastUtil {
	static string demangle(const char *mangled)
	{
		int status;
		char *demangled;
		using namespace abi;
		demangled = __cxa_demangle(mangled, NULL, 0, &status);
		stringstream ss;
		ss << demangled;
		string s;
		ss >> s;
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
		R t;
		stringstream ss;
		ss << val;
		ss >> t;
		if(ss)
		{
			return t;
		}
		else
		{
			throw "Conversion exception";
		}
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
			throw "Conversion exception";
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
			throw "Conversion exception";
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
			throw "Conversion exception";
		}
	}
	template <typename T> static T lexical_cast(const size_t& val)
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
			throw "Conversion exception";
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
			throw "Conversion exception";
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
			throw "Conversion exception";
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
			throw "Conversion exception";
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
			throw "Conversion exception";
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
			throw "Conversion exception";
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
				throw "Conversion exception";
			t = d;
		}
		else if(tn=="int" || tn=="short" || tn=="long" || tn=="size_t")
		{
			long int d = 1;
			d = strtol(val, &endptr, 10);
			if(*endptr)
				throw "Conversion exception";
			t = d;
		}
		else if(tn=="long long")
		{
			#if _GLIBCXX_USE_C99
				long long d = 1L;
				d = strtoll(val, &endptr, 10);
				if(*endptr)
					throw "Conversion exception";
				t = d;
			#endif
		}
		else if(tn=="bool")
		{
			bool d = false;
			if(StringUtil::toLowerCopy(val)=="true")
				d = true;
			else if(StringUtil::toLowerCopy(val)=="false")
				d = false;
			else
				throw "Conversion exception";
			t = d;
		}
		else if(tn=="std::string")
		{
			stringstream ss;
			ss << val;
			ss >> t;
			if(ss)
			{
				return t;
			}
			else
			{
				throw "Conversion exception";
			}
		}
		else
		{
			throw "Conversion exception";
		}
		return t;
	}
};

#endif /* CASTUTIL_H_ */
