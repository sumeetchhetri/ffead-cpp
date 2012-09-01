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
