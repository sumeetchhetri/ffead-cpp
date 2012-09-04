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
 * RegexUtil.h
 *
 *  Created on: 14-Aug-2012
 *      Author: sumeetc
 */

#ifndef REGEXUTIL_H_
#define REGEXUTIL_H_
#include <regex.h>
#include <sys/types.h>
#include "vector"
#include "map"
#include "string"
#include "StringUtil.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

class RegexUtil {
	static map<string, regex_t> patterns;
public:
	RegexUtil();
	virtual ~RegexUtil();
	static vector<string> search(const string& text, const string& pattern);
	static string replace(const string& text, const string& pattern, const string& with);
};

#endif /* REGEXUTIL_H_ */
