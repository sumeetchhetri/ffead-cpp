/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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
 * HttpClient.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef SRC_MODULES_CLIENT_UTIL_HTTP_HTTPCLIENT_H_
#define SRC_MODULES_CLIENT_UTIL_HTTP_HTTPCLIENT_H_

#include <curl/curl.h>
#include <string>
#include <map>
#include <cstdlib>
#include "HttpResponse.h"
#include "HttpRequest.h"


class HttpClient {
	std::string baseUrl;
	std::string _bd;
	std::vector<std::string> _hd;
	CURL* _h;
	static size_t onContent(void *res, size_t len, size_t mb, void *data);
	static size_t onHeaders(void *res, size_t len, size_t mb, void *data);
public:
	static void init();
	static void cleanup();
	HttpClient(std::string baseUrl);
	virtual ~HttpClient();

	void execute(HttpRequest* request, HttpResponse* response, propMap& props);
};

#endif /* SRC_MODULES_CLIENT_UTIL_HTTP_HTTPCLIENT_H_ */
