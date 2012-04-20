/*
 * HttpResponseParser.h
 *
 *  Created on: Nov 27, 2010
 *      Author: sumeet
 */

#ifndef HTTPRESPONSEPARSER_H_
#define HTTPRESPONSEPARSER_H_
#include "map"
#include "vector"
#include "sstream"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "stdio.h"
#include "fstream"
#include "iostream"

using namespace std;
class HttpResponseParser {
	map<string,string> headers;
	static string *headernames;
	string content;
public:
	HttpResponseParser();
	string getHeaderValue(string head)
	{
		return headers[head];
	}
	string getContent() const
	{
		return this->content;
	}
	HttpResponseParser(string,string);
	HttpResponseParser(string vecstr);
	virtual ~HttpResponseParser();
};

#endif /* HTTPRESPONSEPARSER_H_ */
