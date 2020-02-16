/*
	Copyright 2009-2012, Sumeet Chhetri

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
 * CORSHandler.h
 *
 *  Created on: 16-Apr-2013
 *      Author: sumeetc
 */

#ifndef CORSHANDLER_H_
#define CORSHANDLER_H_
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HTTPResponseStatus.h"


class CorsConfig {
	void init();
	std::string allwdOrigins;
	std::vector<std::string> allwdOriginsv;
	std::string allwdMethods;
	std::vector<std::string> allwdMethodsv;
	std::string allwdHeaders;
	std::vector<std::string> allwdHeadersv;
	std::string exposedHeaders;
	std::vector<std::string> exposedHeadersv;
	bool allwdCredentials;
	long maxAge;
	friend class CORSHandler;
	friend class ConfigurationHandler;
public:
	CorsConfig();
	CorsConfig(const std::string& allwdOrigins, const std::string& allwdMethods, const std::string& allwdHeaders,
			const std::string& exposedHeaders, const bool& allwdCredentials, const long& maxAge);
	virtual ~CorsConfig();
	bool isOriginAllowed(const std::string& reqOrgLst);
	bool isMethodAllowed(const std::string& method);
	bool isMethodAllowed(const std::string_view& method);
	bool isHeaderAllowed(const strVec& reqHdrLst, std::string& erheadr);
};

class CORSHandler {
public:
	enum {
		PREFLIGHT, CORS, OTHER
	};
	virtual ~CORSHandler();
	static bool handle(CorsConfig& corsConfig, HttpRequest *req, HttpResponse *res);
};

#endif /* CORSHANDLER_H_ */
