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
	static void trimWith(string& str, const string& c);
	static void trim(string& str);
	static string trimCopy(const string &str);
};

#endif /* STRINGUTIL_H_ */
