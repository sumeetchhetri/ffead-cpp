/*
	Copyright 2010, Sumeet Chhetri

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
