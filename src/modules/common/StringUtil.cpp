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
 * StringUtil.cpp
 *
 *  Created on: Aug 5, 2012
 *      Author: Sumeet
 */

#include "StringUtil.h"

string StringUtil::whitespaces = " \t\f\v\n\r";

void StringUtil::toUpper(string &str)
{
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}

string StringUtil::toUpperCopy(const string& str)
{
	string strn = str;
	toUpper(strn);
	return strn;
}

void StringUtil::toLower(string& str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
}

string StringUtil::toLowerCopy(const string& str)
{
	string strn = str;
	toLower(strn);
	return strn;
}

void StringUtil::replaceFirst(string &str, const string& ths, const string& with)
{
	size_t start_pos = str.find(ths);
	if(start_pos != std::string::npos)
	{
		str.replace(start_pos, ths.length(), with);
	}
}

string StringUtil::replaceFirstCopy(const string &str, const string& ths, const string& with)
{
	string strn = str;
	replaceFirst(strn, ths, with);
	return strn;
}

void StringUtil::replaceAll(string &str, const string& ths, const string& with)
{
	if(ths.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(ths, start_pos)) != std::string::npos) {
		str.replace(start_pos, ths.length(), with);
		start_pos += with.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

string StringUtil::replaceAllCopy(const string &str, const string& ths, const string& with)
{
	string strn = str;
	replaceAll(strn, ths, with);
	return strn;
}

void StringUtil::eraseAll(string& str, const string& ths)
{
	if(ths.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(ths)) != std::string::npos) {
		str.erase(start_pos, ths.length());
	}
}

void StringUtil::capitalized(string& str)
{
	if(str.length()>0)
		str[0] = toupper(str[0]);
}

string StringUtil::capitalizedCopy(const string& str)
{
	string strn(str);
	capitalized(strn);
	return strn;
}

void StringUtil::camelCased(string& str, const string& delim)
{
	vector<string> vec = StringUtil::splitAndReturn<vector<string> >(str, delim);
	str.clear();
	for (int var = 0; var < (int)vec.size(); ++var) {
		str.append(StringUtil::capitalizedCopy(vec.at(var)));
		if(var!=(int)vec.size()-1)
		{
			str.append(delim);
		}
	}
}

string StringUtil::camelCasedCopy(const string& str, const string& delim)
{
	string strn(str);
	camelCased(strn, delim);
	return strn;
}

void StringUtil::replaceLast(string& str, const string& ths, const string& with)
{
	size_t start_pos = str.find_last_of(ths);
	if(start_pos != std::string::npos)
	{
		str.replace(start_pos, ths.length(), with);
	}
}

string StringUtil::replaceLastCopy(const string& str, const string& ths, const string& with)
{
	string strn = str;
	replaceLast(strn, ths, with);
	return strn;
}

int StringUtil::countOccurrences(const string& input, const string& delimiter)
{
	int counter = 0;
	size_t index = 0;
	while((index = input.find(delimiter, index))!=string::npos) {
		++counter;
		++index;
	}
	return counter;
}

vector<string> StringUtil::split(const string& input, const vector<string>& delimiters)
{
	vector<string> output;
	split(output, input, delimiters);
	return output;
}

void StringUtil::split(vector<string>& output, const string& input, const vector<string>& delimiters)
{
	output.clear();
	output.push_back(input);
	for (int var = 0; var < (int)delimiters.size(); ++var) {
		vector<string> output1;
		for (int var1 = 0; var1 < (int)output.size(); ++var1) {
			split(output1, output.at(var1), delimiters.at(var));
		}
		output.swap(output1);
	}
}

void StringUtil::trim(string& str)
{
	size_t end = str.find_last_not_of(whitespaces);
	size_t start = str.find_first_not_of(whitespaces);
	if(start!=string::npos && end!=string::npos)
		str = str.substr(start, end-start+1);
	else if (end!=string::npos)
		str.erase(end+1);
	else if(start!=string::npos)
		str = str.substr(start);
	else
		str.clear();
}

string StringUtil::trimCopy(const string& str)
{
	string strn(str);
	trim(strn);
	return strn;
}

string StringUtil::toHEX(const int& number)
{
	return toHEX((unsigned int)number);
}

string StringUtil::toHEX(const unsigned int& number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%x", number);
	string hexstr(hexVal);
	return hexstr;
}

string StringUtil::toHEX(const long& number)
{
	return toHEX((unsigned long)number);
}

string StringUtil::toHEX(const unsigned long& number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%lx", number);
	string hexstr(hexVal);
	return hexstr;
}


string StringUtil::toHEX(const long long& number)
{
	return toHEX((unsigned long long)number);
}

string StringUtil::toHEX(const unsigned long long& number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%llx", number);
	string hexstr(hexVal);
	return hexstr;
}

string StringUtil::toOCTAL(const int& number)
{
	return toOCTAL((unsigned int)number);
}

string StringUtil::toOCTAL(const unsigned int& number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%o", number);
	string hexstr(hexVal);
	return hexstr;
}

string StringUtil::toOCTAL(const long& number)
{
	return toOCTAL((unsigned long)number);
}

string StringUtil::toOCTAL(const unsigned long& number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%lo", number);
	string hexstr(hexVal);
	return hexstr;
}


string StringUtil::toOCTAL(const long long& number)
{
	return toOCTAL((unsigned long long)number);
}

string StringUtil::toOCTAL(const unsigned long long& number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%llo", number);
	string hexstr(hexVal);
	return hexstr;
}

long int StringUtil::fromHEX(string hexVal)
{
	if(hexVal.find("0x")==string::npos)
	{
		hexVal = "0x" + hexVal;
	}
	long int li;
	li = strtol(hexVal.c_str(), NULL, 16);
	return li;
}

bool StringUtil::startsWith(const string& str, const string& prefix)
{
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

bool StringUtil::endsWith(const string& str, const string& suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
