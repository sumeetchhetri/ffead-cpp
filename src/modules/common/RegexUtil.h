/*
	Copyright 2009-2012, Sumeet Chhetri

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
 * RegexUtil.h
 *
 *  Created on: 14-Aug-2012
 *      Author: sumeetc
 */

#ifndef REGEXUTIL_H_
#define REGEXUTIL_H_
#include "AppDefines.h"
#if !defined(OS_MINGW)
#include <sys/types.h>
#endif
#include <regex.h>
#include "vector"
#include "map"
#include "string"
#include "StringUtil.h"
#include <iostream>
#include <stdlib.h>
#include <libcuckoo/cuckoohash_map.hh>


class RegexUtil {
	static libcuckoo::cuckoohash_map<std::string, regex_t*> nlpatterns;
	static libcuckoo::cuckoohash_map<std::string, regex_t*> patterns;
	static bool cacheRegexes;
	friend class ConfigurationHandler;
	static bool isValidRegex(const std::string& pattern);
	static regex_t* getRegex(const std::string& pattern, const bool& matchNewLine);
public:
	static std::vector<std::string> findWithGroups(const std::string& text, const std::string& pattern, const int& groupCount, const bool& matchNewLine= false);
	static std::vector<std::string> findWithGroups(const std::string& text, const std::string& pattern, const bool& matchNewLine= false);
	static std::vector<std::string> search(const std::string& text, const std::string& pattern, const bool& matchNewLine= false);
	static void find(const std::string& text, const std::string& pattern, int &spos, int &epos, const bool& matchNewLine= false);
	static bool matches(const std::string& text, const std::string& pattern, const bool& matchNewLine= false);
	static int find(const std::string& text, const std::string& pattern, const bool& matchNewLine= false);
	static std::string replaceCopy(const std::string& text, const std::string& pattern, const std::string& with, const bool& matchNewLine= false);
	static bool replace(std::string& text, const std::string& pattern, const std::string& with, const bool& matchNewLine= false);
	static void flushCache();
};

#endif /* REGEXUTIL_H_ */
