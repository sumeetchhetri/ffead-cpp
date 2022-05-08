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
 * SearchEngineInterface.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef SEARCHENGINEINTERFACE_H_
#define SEARCHENGINEINTERFACE_H_
#include "GenericObject.h"
#include "ConnectionPooler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "JSONUtil.h"
#include "HttpClient.h"
#include "IndexQuery.h"
#include "DocumentQuery.h"
#include "SearchQuery.h"

class SearchEngineInterface {
protected:
	ConnectionPooler* pool;
	std::string appName;
	Reflector* reflector;
	friend class SearchEngineManager;
public:
	SearchEngineInterface();
	virtual ~SearchEngineInterface();

	virtual void createIndex(IndexQuery& iq)=0;
	virtual void updateIndex(IndexQuery& iq)=0;
	virtual void removeIndex(IndexQuery& iq)=0;
	virtual void indexOp(IndexQuery& iq)=0;

	virtual void add(DocumentQuery& iq)=0;
	virtual void update(DocumentQuery& iq)=0;
	virtual void remove(DocumentQuery& iq)=0;
	virtual void docOp(DocumentQuery& iq)=0;

	virtual std::string query(SearchQuery& q)=0;
#ifdef HAVE_RAPID_JSON
	std::string query(SearchQuery& q, rapidjson::Value& doc) {
		rapidjson::StringBuffer b;
		rapidjson::Writer<rapidjson::StringBuffer> w(b);
		doc.Accept(w);
		q.setData(std::string(b.GetString(), b.GetSize()));
		return query(q);
	}
#endif

	template<class T> T get(std::string indxName, std::string& id) {
		T t;
		std::string cn = CastUtil::getClassName(t);
		int serOpt = SerializeBase::identifySerOption(cn);
		SearchQuery q;
		q.setIndexName(indxName);
		q.setId(id);
		std::string resp = query(q);
		return JSONSerialize::unserialize<T>(resp, serOpt);
	}

	template<class T> std::vector<T> getAll(std::string indxName) {
		T t;
		std::string cn = CastUtil::getClassName(t);
		int serOpt = SerializeBase::identifySerOption(cn);
		SearchQuery q;
		q.setIndexName(indxName);
		std::string resp = query(q);
		return JSONSerialize::unserialize<std::vector<T>>(resp, serOpt);
	}

	template<class T> std::vector<T> getList(SearchQuery& q) {
		T t;
		std::string cn = CastUtil::getClassName(t);
		int serOpt = SerializeBase::identifySerOption(cn);
		std::string resp = query(q);
		return JSONSerialize::unserialize<std::vector<T>>(resp, serOpt);
	}

	template<class T> T get(SearchQuery& q) {
		T t;
		std::string cn = CastUtil::getClassName(t);
		int serOpt = SerializeBase::identifySerOption(cn);
		std::string resp = query(q);
		return JSONSerialize::unserialize<T>(resp, serOpt);
	}
};

#endif /* SEARCHENGINEINTERFACE_H_ */
