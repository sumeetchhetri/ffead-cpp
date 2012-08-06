/*
 * CastUtil.h
 *
 *  Created on: Aug 5, 2012
 *      Author: Sumeet
 */

#ifndef CASTUTIL_H_
#define CASTUTIL_H_
#include "sstream"
using namespace std;

class CastUtil {
public:
	CastUtil();
	virtual ~CastUtil();
	template <typename T, typename R> static R cast(T val)
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

	template <typename T> static T lexical_cast(short val)
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
	template <typename T> static T lexical_cast(int val)
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
	template <typename T> static T lexical_cast(long val)
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
	template <typename T> static T lexical_cast(long long val)
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
	template <typename T> static T lexical_cast(double val)
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
	template <typename T> static T lexical_cast(float val)
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
	template <typename T> static T lexical_cast(bool val)
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
	template <typename T> static T lexical_cast(string val)
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
};

#endif /* CASTUTIL_H_ */
