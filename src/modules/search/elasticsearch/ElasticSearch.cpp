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
