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
	transform(strn.begin(), strn.end(), strn.begin(), ::toupper);
	return strn;
}

void StringUtil::toLower(string& str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
}

string StringUtil::toLowerCopy(const string& str)
{
	string strn = str;
	transform(strn.begin(), strn.end(), strn.begin(), ::tolower);
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
	size_t start_pos = strn.find(ths);
	if(start_pos != std::string::npos)
	{
		strn.replace(start_pos, ths.length(), with);
	}
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
	if(ths.empty())
		return strn;
	size_t start_pos = 0;
	while((start_pos = strn.find(ths, start_pos)) != std::string::npos) {
		strn.replace(start_pos, ths.length(), with);
		start_pos += with.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
	return strn;
}

vector<string> split(const string& input, const string& delimiter)
{
	vector<string> output;
	size_t start = 0;
	size_t end = 0;

	while (start != string::npos && end != string::npos)
	{
		start = input.find_first_not_of(delimiter, end);
		if (start != string::npos)
		{
			end = input.find_first_of(delimiter, start);
			if (end != string::npos)
			{
				output.push_back(input.substr(start, end - start));
			}
			else
			{
				output.push_back(input.substr(start));
			}
		}
	}
	return output;

}

void StringUtil::split(vector<string> &output, const string& input, const string& delimiter)
{
	size_t start = 0;
	size_t end = 0;

	while (start != string::npos && end != string::npos)
	{
		start = input.find_first_not_of(delimiter, end);
		if (start != string::npos)
		{
			end = input.find_first_of(delimiter, start);
			if (end != string::npos)
			{
				output.push_back(input.substr(start, end - start));
			}
			else
			{
				output.push_back(input.substr(start));
			}
		}
	}
}
