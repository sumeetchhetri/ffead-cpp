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
 * StringUtil.h
 *
 *  Created on: Aug 5, 2012
 *      Author: Sumeet
 */

#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_
#include "string"
#include <algorithm>
#include <sstream>
#include <iterator>
#include "vector"
using namespace std;

class StringUtil {
	static void splitInternal(vector<string> &output, const string& input, const string& delimiter);
public:
	StringUtil();
	virtual ~StringUtil();
	static void eraseAll(string &str, const string& ths);
	static void capitalized(string &str);
	static string capitalizedCopy(const string &str);
	static void toUpper(string &str);
	static string toUpperCopy(const string& str);
	static void toLower(string& str);
	static string toLowerCopy(const string& str);
	static void replaceFirst(string &str, const string& ths, const string& with);
	static string replaceFirstCopy(const string &str, const string& ths, const string& with);
	static void replaceLast(string &str, const string& ths, const string& with);
	static string replaceLastCopy(const string &str, const string& ths, const string& with);
	static void replaceAll(string &str, const string& ths, const string& with);
	static string replaceAllCopy(const string &str, const string& ths, const string& with);
	static vector<string> split(const string& input, const string& delimiter);
	static void split(vector<string> &output, const string& input, const string& delimiter);
	static vector<string> split(const string& input, vector<string> delimiters);
	static void split(vector<string> &output, const string& input, vector<string> delimiters);
	static void trim(string& str);
	static string trimCopy(const string &str);
};

#endif /* STRINGUTIL_H_ */
