/*
 * TemplateGenerator.h
 *
 *  Created on: 15-Feb-2013
 *      Author: sumeetc
 */

#ifndef TEMPLATEGENERATOR_H_
#define TEMPLATEGENERATOR_H_
#include "string"
#include "vector"
#include "fstream"
#include "StringUtil.h"
#include "map"
using namespace std;


class TemplateGenerator {
public:
	TemplateGenerator();
	virtual ~TemplateGenerator();
	static string generateTempCd(string,string &,string &,string app);
	static string generateTempCdAll(map<string, string> fileNames);
};

#endif /* TEMPLATEGENERATOR_H_ */
