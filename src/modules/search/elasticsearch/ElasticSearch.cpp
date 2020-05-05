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
 * ElasticSearch.cpp
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#include "ElasticSearch.h"

std::string ElasticSearch::COLL_URL = "/admin/collections";

ElasticSearch::ElasticSearch() {
}

ElasticSearch::~ElasticSearch() {
}

void ElasticSearch::createIndex(IndexQuery& iq) {
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;

	JSONElement el;
	el.setName("action");

	rq.setUrl(COLL_URL);



	hc->execute(&rq, &rs, p);
	if(rs.getStatusCode()!="200") {
		throw std::runtime_error("Unable to create index");
	}
}

void ElasticSearch::updateIndex(IndexQuery& iq) {
}

void ElasticSearch::removeIndex(IndexQuery& iq) {
}

void ElasticSearch::add(DocumentQuery& iq) {
}

void ElasticSearch::update(DocumentQuery& iq) {
}

void ElasticSearch::remove(DocumentQuery& iq) {
}

std::string ElasticSearch::query(SearchQuery& q) {
	return std::string();
}
