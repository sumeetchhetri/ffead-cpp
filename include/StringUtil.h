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
public:
	StringUtil();
	virtual ~StringUtil();
	static void toUpper(string &str);
	static string toUpperCopy(string str);
	static void toLower(string &str);
	static string toLowerCopy(string str);
	static void replaceFirst(string &str, string ths, string with);
	static string replaceFirstCopy(string str, string ths, string with);
	static void replaceAll(string str, string ths, string with);
	static string replaceAllCopy(string str, string ths, string with);
	static vector<string> split(string input, string delimiter);
	static void split(vector<string> &output, string input, string delimiter);
	//static void split(vector<string> &output, string input, const string delimiter);
};

#endif /* STRINGUTIL_H_ */
