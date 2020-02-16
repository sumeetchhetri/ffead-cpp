/*
 * TeBkUm.h
 *
 *  Created on: 03-Feb-2020
 *      Author: sumeetc
 */

#ifndef WEB_TE_BENCHMARK_UM_INCLUDE_TeBkUm_H_
#define WEB_TE_BENCHMARK_UM_INCLUDE_TeBkUm_H_
#include "TemplateHandler.h"
#include "vector"
#include "DataSourceManager.h"
#include <stdlib.h>
#include <algorithm>
#include "CryptoHandler.h"
#include "vector"
#include "CastUtil.h"
#include <stdlib.h>
#include "CacheManager.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "JSONSerialize.h"
#include "string"
#include "TeBkUmWorld.h"
#include "yuarel.h"
#include "Router.h"

typedef std::string (*TeBkUmTemplatePtr) (Context*);

#pragma @Entity
#pragma @Table name="fortune"
class TeBkUmFortune {
	#pragma @Id dbf="id"
	int id;
	#pragma @Column dbf="message"
	std::string message;
public:
	TeBkUmFortune();
	virtual ~TeBkUmFortune();
	int getId() const;
	void setId(int id);
	const std::string& getMessage() const;
	void setMessage(const std::string& message);
	bool operator < (const TeBkUmFortune& other) const;
};

class TeBkUmMessage {
	std::string message;
public:
	virtual ~TeBkUmMessage();
	const std::string& getMessage() const;
	void setMessage(const std::string& message);
};

#pragma @RestController path="/"
class TeBkUmRestController {
	static const std::string HELLO_WORLD;
	static std::string WORLD;
public:
	#pragma @GET path="/json" statusCode="200" ocontentType="application/json"
	TeBkUmMessage json();

	#pragma @GET path="/db" statusCode="200" ocontentType="application/json"
	TeBkUmWorld db();

	#pragma @GET path="/queries" statusCode="200" ocontentType="application/json"
	std::vector<TeBkUmWorld> queries(
					#pragma @QueryParam name="queries"
					std::string queries);

	#pragma @GET path="/updates" statusCode="200" ocontentType="application/json"
	std::vector<TeBkUmWorld> updates(
					#pragma @QueryParam name="queries"
					std::string queries);

	#pragma @GET path="/plaintext" statusCode="200" ocontentType="text/plain"
	std::string plaintext();

	#pragma @GET path="/cached-worlds" statusCode="200" ocontentType="application/json"
	std::vector<TeBkUmWorld> cachedWorlds(
						#pragma @QueryParam name="count"
						std::string count);

	//#pragma @Template path="fortunes" file="fortunes.tpe"
	void getContext(HttpRequest* request, Context* context);
	void updateCache();
	TeBkUmRestController();
	virtual ~TeBkUmRestController();
};

#pragma @Router
class TebBkUmRouter : public Router {
	TeBkUmRestController rc;
public:
	void route(HttpRequest* req, HttpResponse* res, void* dlib, void* ddlib);
};

#endif /* WEB_TE_BENCHMARK_UM_INCLUDE_TeBkUm_H_ */
