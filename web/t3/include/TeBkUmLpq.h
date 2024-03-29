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
 * TeBkUmLpq.h
 *
 *  Created on: 03-Feb-2020
 *      Author: sumeetc
 */

#ifndef WEB_t1_INCLUDE_TeBkUmLpq_H_
#define WEB_t1_INCLUDE_TeBkUmLpq_H_
#include "TemplateHandler.h"
#include "vector"
#include "list"
#ifndef OS_MINGW
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "DataSourceManager.h"
#include <stdlib.h>
#include <algorithm>
#include "CryptoHandler.h"
#include "CastUtil.h"
#include <stdlib.h>
#include "CacheManager.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "JSONSerialize.h"
#include "string"
#include "yuarel.h"
#include "Router.h"
#include "Reflector.h"
#include <unordered_map>

class TeBkUmLpqWorld {
	int id;
	int randomNumber;
	static TemplateJson tJ;
	static TemplateJson tJc;
public:
	TeBkUmLpqWorld(int id);
	TeBkUmLpqWorld(int id, int randomNumber);
	TeBkUmLpqWorld();
	virtual ~TeBkUmLpqWorld();
	int getId() const;
	void setId(int id);
	void set(int id, int randomNumber);
	int getRandomNumber() const;
	void setRandomNumber(int randomNumber);
	bool operator < (const TeBkUmLpqWorld& other) const;
	static void tmplJson(int id, int randomNumber, std::string* ou) {
		size_t il = ou->length();
		ou->append(tJ.t);
		std::string ids = std::to_string(id);
		ou->insert(il+tJ.tpos.at(0), ids);
		ou->insert(il+tJ.tpos.at(1)+ids.length(), std::to_string(randomNumber));
	}
	void tmplJson(std::string* ou) {
		size_t il = ou->length();
		ou->append(tJ.t);
		std::string ids = std::to_string(id);
		ou->insert(il+tJ.tpos.at(0), ids);
		ou->insert(il+tJ.tpos.at(1)+ids.length(), std::to_string(randomNumber));
	}
	static void tmplJson(std::vector<TeBkUmLpqWorld>& vec, std::string* ou) {
		for(auto el: vec) {
			size_t il = ou->length();
			ou->append(tJc.t);
			std::string ids = std::to_string(el.id);
			ou->insert(il+tJc.tpos.at(0), ids);
			ou->insert(il+tJc.tpos.at(1)+ids.length(), std::to_string(el.randomNumber));
		}
		ou->insert(0, "[");
		ou->at(ou->length()-1) = ']';
	}
#ifdef HAVE_RAPID_JSON
	void toJson(rapidjson::Writer<rapidjson::StringBuffer>& w) {
		w.StartObject();
		w.String("id", 2);
	    w.Int(id);
		w.String("randomNumber", 12);
	    w.Int(randomNumber);
		w.EndObject();
	}
#endif
#ifdef HAVE_RAPID_JSON
	static void toJson(std::vector<TeBkUmLpqWorld>& vec, rapidjson::Writer<rapidjson::StringBuffer>& w) {
		w.StartArray();
		for(auto el: vec) {
			el.toJson(w);
		}
		w.EndArray();
	}
#endif
};

struct UpdQrData {
	std::vector<TeBkUmLpqWorld>* wlist;
	std::stringstream ss;
	bool status;
	int queryCount;
};

class TeBkUmLpqFortune {
	int id;
public:
	std::string message_i;
	std::string_view message;
	bool allocd;
	TeBkUmLpqFortune(int id);
	TeBkUmLpqFortune(int id, std::string message);
	TeBkUmLpqFortune(int id, const uint8_t * buf, size_t len);
	virtual ~TeBkUmLpqFortune();
	int getId() const;
	void setId(int id);
	bool operator < (const TeBkUmLpqFortune& other) const;
    TeBkUmLpqFortune() {
    	allocd = false;
    	id = 0;
    }
};

class TeBkUmLpqRouterPicoV;

class TeBkUmLpqMessage {
	std::string message;
	static TemplateJson tJ;
public:
	TeBkUmLpqMessage();
	TeBkUmLpqMessage(std::string message);
	virtual ~TeBkUmLpqMessage();
	const std::string& getMessage() const;
	void setMessage(const std::string& message);
	void tmplJson(std::string* ou) {
		size_t il = ou->length();
		ou->append(tJ.t);
		ou->insert(il+tJ.tpos.at(0), message);
	}
#ifdef HAVE_RAPID_JSON
	void toJson(rapidjson::Writer<rapidjson::StringBuffer>& w) {
		w.StartObject();
		w.String("message", 7);
	    w.String(message.c_str(), static_cast<rapidjson::SizeType>(message.length()));
		w.EndObject();
	}
#endif
};

#pragma @IgnoreSer
#pragma @IgnoreRef
class ReqData {
public:
	std::string h;
#ifdef HAVE_RAPID_JSON
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> wr;
#endif
	void reset() {
		h.clear();
#ifdef HAVE_RAPID_JSON
		sb.Clear();
		wr.Reset(sb);
#endif
	}
};

class TeBkUmLpqRouter : public Router {
	static const std::string HELLO_WORLD;
	static const std::string WORLD;
	static const std::string WORLD_ONE_QUERY;
	static const std::string WORLD_ALL_QUERY;
	static const std::string FORTUNE_ALL_QUERY;
	static int g_seed;

	static TemplatePtr tmplFunc;

	static Ser m_ser;
	static Ser w_ser;
	static SerCont wcont_ser;

	void db(TeBkUmLpqWorld&);
	void queries(const char*, int, std::vector<TeBkUmLpqWorld>&);
	void queriesMulti(const char*, int, std::vector<TeBkUmLpqWorld>&);
	void updates(const char*, int, std::vector<TeBkUmLpqWorld>&);
	void updatesMulti(const char*, int, std::vector<TeBkUmLpqWorld>&);
	void cachedWorlds(const char*, int, std::vector<TeBkUmLpqWorld>&);
	void handleTemplate(HttpRequest* req, HttpResponse* res, Writer* sif);
	std::string& getUpdQuery(int count);
	std::string& getMultiQuery(int count);

	std::unordered_map<int, std::string> _qC;
	std::unordered_map<int, std::string> _mqC;
	LibpqDataSourceImpl* sqli;
	LibpqDataSourceImpl* getDb();

	friend class TeBkUmLpqRouterPicoV;
public:
	TeBkUmLpqRouter();
	virtual ~TeBkUmLpqRouter();
	void updateCache();
	bool route(HttpRequest* req, HttpResponse* res, Writer* sif);
};

class TeBkUmLpqRouterPicoV : public TeBkUmLpqRouter {
	void handleTemplate(HttpResponse* res);
public:
	TeBkUmLpqRouterPicoV();
	virtual ~TeBkUmLpqRouterPicoV();
	bool route(HttpRequest* req, HttpResponse* res, Writer* sif);
};

#endif /* WEB_t1_INCLUDE_TeBkUmLpq_H_ */
