/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * TeBkRestController.h
 *
 *  Created on: 11-Mar-2015
 *      Author: sumeetc
 */

#ifndef TEBKRESTCONTROLLER_H_
#define TEBKRESTCONTROLLER_H_
#include "TeBkWorld.h"
#include "vector"
#include "CastUtil.h"
#include "TeBkMessage.h"
#include "DataSourceManager.h"
#include <stdlib.h>
#include "CacheManager.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "JSONSerialize.h"


#pragma @RestController path="/"
class TeBkRestController {
	static const std::string HELLO_WORLD;
	static std::string WORLD;
public:
	#pragma @GET path="/json" statusCode="200" ocontentType="application/json"
	TeBkMessage json();

	#pragma @GET path="/db" statusCode="200" ocontentType="application/json"
	TeBkWorld db();

	#pragma @GET path="/queries" statusCode="200" ocontentType="application/json"
	std::vector<TeBkWorld> queries(
					#pragma @QueryParam name="queries"
					std::string queries);

	#pragma @GET path="/updates" statusCode="200" ocontentType="application/json"
	std::vector<TeBkWorld> updates(
					#pragma @QueryParam name="queries"
					std::string queries);

	#pragma @GET path="/plaintext" statusCode="200" ocontentType="text/plain"
	std::string plaintext();

	#pragma @GET path="/cached-worlds" statusCode="200" ocontentType="application/json"
	std::vector<TeBkWorld> cachedWorlds(
					#pragma @QueryParam name="count"
					std::string count);

	#pragma @GET path="/plaintext-um" statusCode="200" ocontentType="text/plain" unmapped="true"
	void plaintextUM(HttpRequest* req, HttpResponse* res);

	void updateCache();
	TeBkRestController();
	virtual ~TeBkRestController();
};

#endif /* TEBKRESTCONTROLLER_H_ */
