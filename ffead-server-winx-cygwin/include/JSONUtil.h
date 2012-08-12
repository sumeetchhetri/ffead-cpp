/*
 * JSONUtil.h
 *
 *  Created on: 06-Aug-2012
 *      Author: sumeetc
 */

#ifndef JSONUTIL_H_
#define JSONUTIL_H_
#include "string"
#include "JSONElement.h"
#include "StringUtil.h"
#include "CastUtil.h"
#include <stdio.h>
using namespace std;

class JSONUtil {
	static void array(string& json, JSONElement* element);
	static void object(string& json, JSONElement* element);
	static void arrayOrObject(string& json, JSONElement* element);
	static void readJSON(string& json,bool isarray,JSONElement *par);
	static void validateSetValue(JSONElement* element, string value);
public:
	JSONUtil();
	virtual ~JSONUtil();
	static JSONElement getDocument(const string& json);
	static string getDocumentStr(JSONElement doc);
};

#endif /* JSONUTIL_H_ */
