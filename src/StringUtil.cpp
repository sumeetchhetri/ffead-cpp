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

StringUtil::StringUtil() {
	// TODO Auto-generated constructor stub

}

StringUtil::~StringUtil() {
	// TODO Auto-generated destructor stub
}

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
	vector<string> vec = StringUtil::split(str, delim);
	str = "";
	for (int var = 0; var < (int)vec.size(); ++var) {
		str += StringUtil::capitalizedCopy(vec.at(var));
		if(var!=(int)vec.size()-1)
		{
			str += delim;
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

vector<string> StringUtil::split(const string& input, const string& delimiter)
{
	vector<string> output;
	split(output, input, delimiter);
	return output;

}

void StringUtil::splitInternal(vector<string> &output, const string& input, const string& delimiter)
{
	size_t start = 0;

	string temp = input;
	start = temp.find(delimiter);
	while(start!=string::npos)
	{
		if(start!=0)
		{
			output.push_back(temp.substr(0, start));
		}
		else
		{
			output.push_back("");
		}
		if(temp.length()>start+delimiter.length())
		{
			temp = temp.substr(start+delimiter.length());
			start = temp.find(delimiter);
		}
		else
		{
			temp = temp.substr(start);
			break;
		}
	}
	replaceFirst(temp, delimiter, "");
	output.push_back(temp);
}

void StringUtil::split(vector<string> &output, const string& input, const string& delimiter)
{
	output.clear();
	splitInternal(output, input, delimiter);
}

int StringUtil::countOccurrences(const string& input, const string& delimiter)
{
	vector<string> output;
	splitInternal(output, input, delimiter);
	return output.size()==0?0:output.size()-1;
}

vector<string> StringUtil::split(const string& input, vector<string> delimiters)
{
	vector<string> output;
	split(output, input, delimiters);
	return output;
}

void StringUtil::split(vector<string>& output, const string& input, vector<string> delimiters)
{
	output.clear();
	output.push_back(input);
	for (int var = 0; var < (int)delimiters.size(); ++var) {
		vector<string> output1;
		for (int var1 = 0; var1 < (int)output.size(); ++var1) {
			splitInternal(output1, output.at(var1), delimiters.at(var));
		}
		output.swap(output1);
	}
}

void StringUtil::trim(string& str)
{
	string c = " ";
	size_t p2 = str.find_last_not_of(c);
	if(p2 != string::npos)
	{
		while(str[p2]==' ' || str[p2]=='\t' || str[p2]=='\n' || str[p2]=='\r')
		{
			str = str.substr(0, p2);
			p2 = str.find_last_not_of(str[p2]);
		}
	}
	if (p2 == string::npos)
		p2 = str.length();
	size_t p1 = str.find_first_not_of(c);
	if(p1 != string::npos)
	{
		while(str[p1]==' ' || str[p1]=='\t' || str[p1]=='\n' || str[p1]=='\r')
		{
			str = str.substr(p1);
			p1 = str.find_first_not_of(str[p1]);
		}
	}
	if (p1 == string::npos)
		p1 = 0;
	str = str.substr(p1, (p2-p1)+1);
}

string StringUtil::trimCopy(const string& str)
{
	string strn(str);
	trim(strn);
	return strn;
}

string StringUtil::toHEX(int number)
{
	return toHEX((unsigned int)number);
}

string StringUtil::toHEX(unsigned int number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%x", number);
	string hexstr(hexVal);
	return hexstr;
}

string StringUtil::toHEX(long number)
{
	return toHEX((unsigned long)number);
}

string StringUtil::toHEX(unsigned long number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%lx", number);
	string hexstr(hexVal);
	return hexstr;
}


string StringUtil::toHEX(long long number)
{
	return toHEX((unsigned long long)number);
}

string StringUtil::toHEX(unsigned long long number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%llx", number);
	string hexstr(hexVal);
	return hexstr;
}

string StringUtil::toOCTAL(int number)
{
	return toOCTAL((unsigned int)number);
}

string StringUtil::toOCTAL(unsigned int number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%o", number);
	string hexstr(hexVal);
	return hexstr;
}

string StringUtil::toOCTAL(long number)
{
	return toOCTAL((unsigned long)number);
}

string StringUtil::toOCTAL(unsigned long number)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%lo", number);
	string hexstr(hexVal);
	return hexstr;
}


string StringUtil::toOCTAL(long long number)
{
	return toOCTAL((unsigned long long)number);
}

string StringUtil::toOCTAL(unsigned long long number)
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
	li = strtol(hexVal.c_str(), NULL, 10);
	return li;
}
