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
#include "IndexQuery.h"
#include "DocumentQuery.h"
#include "SearchQuery.h"
#include "GenericObject.h"
#include "ConnectionPooler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "JSONUtil.h"
#include "HttpClient.h"

class SearchEngineInterface {
	ConnectionPooler* pool;
	std::string appName;
	Reflector* reflector;
public:
	SearchEngineInterface();
	virtual ~SearchEngineInterface();

	virtual void createIndex(IndexQuery& iq)=0;
	virtual void updateIndex(IndexQuery& iq)=0;
	virtual void removeIndex(IndexQuery& iq)=0;

	virtual void add(DocumentQuery& iq)=0;
	virtual void update(DocumentQuery& iq)=0;
	virtual void remove(DocumentQuery& iq)=0;

	virtual std::string query(SearchQuery& q)=0;

	template<class T> T get(GenericObject& id) {

	}

	template<class T> std::vector<T> getAll() {

	}

	template<class T> std::vector<T> getList(SearchQuery& q) {

	}

	template<class T> T get(SearchQuery& q) {

	}
};

#endif /* SEARCHENGINEINTERFACE_H_ */
