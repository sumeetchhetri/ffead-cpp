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
#include "Object.h"
using namespace std;

class CastUtil {
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
	template <typename T> static T lexical_cast(const char* val)
	{
		T t;
		string tn = Object::getClassName(t);
		char* endptr;
		if(tn=="double")
		{
			double d = 0;
			d = strtod(val, &endptr);
			if(*endptr)
				throw "Conversion exception";
			t = d;
		}
		else if(tn=="int" || tn=="short" || tn=="long")
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
		else
		{
			throw "Conversion exception";
		}
		return t;
	}
};

#endif /* CASTUTIL_H_ */
