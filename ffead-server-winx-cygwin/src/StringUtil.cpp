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
	str[0] = toupper(str[0]);
}

string StringUtil::capitalizedCopy(const string& str)
{
	string strn(str);
	capitalized(strn);
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

vector<string> StringUtil::split(const string& input, vector<string> delimiters)
{
	vector<string> output;
	split(output, input, delimiters);
	return output;
}

void StringUtil::split(vector<string>& output, const string& input, vector<string> delimiters)
{
	output.push_back(input);
	for (int var = 0; var < (int)delimiters.size(); ++var) {
		vector<string> output1;
		for (int var1 = 0; var1 < (int)output.size(); ++var1) {
			split(output1, output.at(var1), delimiters.at(var));
		}
		output.swap(output1);
	}
}

void StringUtil::trimWith(string& str, const string& c = " ")
{
	if(str=="")return;
	size_t p2 = str.find_last_not_of(c);
	if (p2 == string::npos)
		return;
	size_t p1 = str.find_first_not_of(c);
	if (p1 == string::npos)
		p1 = 0;
	str = str.substr(p1, (p2-p1)+1);
}

void StringUtil::trim(string& str)
{
	trimWith(str, " \t");
	trimWith(str, "\t ");
	trimWith(str);
	trimWith(str, "\t");
	trimWith(str, "\n");
	trimWith(str, "\r\n");
}

string StringUtil::trimCopy(const string& str)
{
	string strn(str);
	trim(strn);
	return strn;
}
