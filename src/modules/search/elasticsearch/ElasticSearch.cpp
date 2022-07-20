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

#ifdef HAVE_ELASTIC

ElasticSearch::ElasticSearch(ConnectionPooler* pool) {
	this->pool = pool;
}

ElasticSearch::~ElasticSearch() {
}

void ElasticSearch::createIndex(IndexQuery& iq) {
}

void ElasticSearch::updateIndex(IndexQuery& iq) {
}

void ElasticSearch::removeIndex(IndexQuery& iq) {
}

void ElasticSearch::add(DocumentQuery& iq) {
	Connection* connection = pool->checkout();
	elasticlient::Client* ec = (elasticlient::Client*)connection->getConn();
	cpr::Response indexResponse = ec->index(iq.getIndexName(), "docType", "docId", iq.getData());
	pool->release(connection);
	if(indexResponse.status_code!=200) {
		throw std::runtime_error("Error during document op " + indexResponse.text);
	}
}

void ElasticSearch::update(DocumentQuery& iq) {
	add(iq);
}

void ElasticSearch::remove(DocumentQuery& iq) {
	Connection* connection = pool->checkout();
	elasticlient::Client* ec = (elasticlient::Client*)connection->getConn();
	cpr::Response indexResponse = ec->remove(iq.getIndexName(), "docType", "docId");
	pool->release(connection);
	if(indexResponse.status_code!=200) {
		throw std::runtime_error("Error during document op " + indexResponse.text);
	}
}

std::string ElasticSearch::query(SearchQuery& q) {
	Connection* connection = pool->checkout();
	elasticlient::Client* ec = (elasticlient::Client*)connection->getConn();
	cpr::Response resp;
	if(q.getId()!="") {
		resp = ec->get(q.getIndexName(), "docType", q.getId());
	} else {
		resp = ec->search(q.getIndexName(), "docType", q.getData());
	}
	pool->release(connection);
	if(resp.status_code!=200) {
		throw std::runtime_error("Error during document op " + resp.text);
	} else {
		return resp.text;
	}
}

void ElasticSearchConnectionPool::initEnv() {
}

void* ElasticSearchConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	return new elasticlient::Client({node.getBaseUrl()});
}

void ElasticSearchConnectionPool::closeConnection(void* conn) {
	delete ((elasticlient::Client*)conn);
}

void ElasticSearchConnectionPool::destroy() {
}

ElasticSearchConnectionPool::ElasticSearchConnectionPool(const ConnectionProperties& props) {
	createPool(props);
}

ElasticSearchConnectionPool::~ElasticSearchConnectionPool() {
	destroyPool();
}


#endif
