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
/*
 * StringUtil.h
 *
 *  Created on: Aug 5, 2012
 *      Author: Sumeet
 */

#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_
#include "Compatibility.h"
#include "string"
#include <algorithm>
#include <sstream>
#include <iterator>
#include "vector"
#include <stdio.h>
#include "cstring"
#include "string_view"

class StringUtil {
	static std::string whitespaces;
public:
	static void eraseAll(std::string &str, const std::string& ths);
	static void capitalized(std::string &str);
	static std::string capitalizedCopy(const std::string &str);
	static void camelCased(std::string& str, const std::string& delim);
	static std::string camelCasedCopy(const std::string& str, const std::string& delim);
	static void toUpper(std::string &str);
	static std::string toUpperCopy(const std::string& str);
	static void toLower(std::string& str);
	static std::string toLowerCopy(const std::string& str);
	static void replaceFirst(std::string &str, const std::string& ths, const std::string& with);
	static std::string replaceFirstCopy(const std::string &str, const std::string& ths, const std::string& with);
	static void replaceLast(std::string &str, const std::string& ths, const std::string& with);
	static std::string replaceLastCopy(const std::string &str, const std::string& ths, const std::string& with);
	static void replaceAll(std::string &str, const std::string& ths, const std::string& with);
	static std::string replaceAllCopy(const std::string &str, const std::string& ths, const std::string& with);
	static std::vector<std::string> split(const std::string& input, const std::vector<std::string>& delimiters);
	static void split(std::vector<std::string> &output, const std::string& input, const std::vector<std::string>& delimiters);
	template < class ContainerT >
	static void split(ContainerT& tokens, const std::string& str, const std::string& delimiters, const bool& trimEmpty= false)
	{
		typedef ContainerT Base;
		typedef typename Base::value_type Vt;
		typedef typename Vt::size_type St;

		std::string::size_type pos, lastPos = 0;

		while(true)
		{
			pos = str.find_first_of(delimiters, lastPos);
			if(pos == std::string::npos)
			{
				pos = str.length();

				if(pos != lastPos || !trimEmpty)
					tokens.push_back(Vt(str.data()+lastPos,
							(St)pos-lastPos ));

				break;
			}
			else
			{
				if(pos != lastPos || !trimEmpty)
					tokens.push_back(Vt(str.data()+lastPos,
							(St)pos-lastPos ));
			}

			lastPos = pos + 1;
		}
	}
	template < class ContainerT >
	static ContainerT splitAndReturn(const std::string& str, const std::string& delimiters, const bool& trimEmpty= false)
	{
		typedef ContainerT Base;
		typedef typename Base::value_type Vt;
		typedef typename Vt::size_type St;

		ContainerT tokens;
		std::string::size_type pos, lastPos = 0;

		while(true)
		{
			pos = str.find_first_of(delimiters, lastPos);
			if(pos == std::string::npos)
			{
				pos = str.length();

				if(pos != lastPos || !trimEmpty)
					tokens.push_back(Vt(str.data()+lastPos,
							(St)pos-lastPos ));

				break;
			}
			else
			{
				if(pos != lastPos || !trimEmpty)
					tokens.push_back(Vt(str.data()+lastPos,
							(St)pos-lastPos ));
			}

			lastPos = pos + 1;
		}
		return tokens;
	}
	static inline void to_nbo(double in, double *out) {
		uint64_t *i = (uint64_t *)&in;
		uint32_t *r = (uint32_t *)out;

		/* convert input to network byte order */
		r[0] = htonl((uint32_t)((*i) >> 32));
		r[1] = htonl((uint32_t)*i);
	}
	static int countOccurrences(const std::string& input, const std::string& delimiter);
	static void trim(std::string& str);
	static std::string trimCopy(const std::string &str);
	static std::string toHEX(const int&);
	static std::string toHEX(const unsigned int&);
	static std::string toHEX(const long&);
	static std::string toHEX(const unsigned long&);
	static std::string toHEX(const long long&);
	static std::string toHEX(const unsigned long long&);
	static std::string toOCTAL(const int&);
	static std::string toOCTAL(const unsigned int&);
	static std::string toOCTAL(const long&);
	static std::string toOCTAL(const unsigned long&);
	static std::string toOCTAL(const long long&);
	static std::string toOCTAL(const unsigned long long&);
	static long int fromHEX(std::string hexVal);
	static bool startsWith(const std::string& str, const std::string& prefix);
	static bool endsWith(const std::string& str, const std::string& suffix);
	static bool endsWith(const std::string_view& str, const std::string& suffix);
};

#endif /* STRINGUTIL_H_ */
