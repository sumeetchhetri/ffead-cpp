/*
 * SolrSearch.cpp
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#include "SolrSearch.h"

std::string SolrSearch::COLL_C_PARAMS = ",router.name,numShards,shards,replicationFactor,nrtReplicas,tlogReplicas,pullReplicas,"
		"maxShardsPerNode,createNodeSet,createNodeSet.shuffle,collection.configName,router.field,autoAddReplicas,async,rule,snitch,policy,waitForFinalState,";
std::string SolrSearch::COLL_M_PARAMS = ",maxShardsPerNode,replicationFactor,autoAddReplicas,collection.configName,rule,snitch,policy,";

SolrSearch::SolrSearch() {
	isCloud = StringUtil::toLowerCopy(pool->getProperties().getProperty("isCloud"))=="true";
}

SolrSearch::~SolrSearch() {
}

void SolrSearch::createIndex(IndexQuery& iq) {
	if(!isCloud) {
		return;
	}
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;
	std::map<std::string, std::string>::iterator it;

	std::string uri;
	uri.append("/admin/collections");
	uri.append("?action=CREATE");
	uri.append("&name=" + iq.getName());
	for(it=iq.properties.begin();it!=iq.properties.end();++it) {
		if(COLL_C_PARAMS.find(","+it->first+",")!=std::string::npos) {
			uri.append("&" + it->first + "=" + it->second);
		}
	}

	rq.setUrl(uri);
	rq.setMethod("GET");

	hc->execute(&rq, &rs, p);
	if(rs.getStatusCode()!="200") {
		if(rs.getContent()!="") {
			throw std::runtime_error("Unable to create index " + rs.getContent());
		}
		throw std::runtime_error("Unable to create index");
	}
}

void SolrSearch::updateIndex(IndexQuery& iq) {
	if(!isCloud) {
		return;
	}
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;
	std::map<std::string, std::string>::iterator it;

	std::string uri;
	uri.append("/admin/collections/" + iq.getName() + "/update/json?split=/");
	uri.append("?action=MODIFYCOLLECTION");
	uri.append("&collection=" + iq.getName());
	for(it=iq.properties.begin();it!=iq.properties.end();++it) {
		if(COLL_M_PARAMS.find(","+it->first+",")!=std::string::npos) {
			uri.append("&" + it->first + "=" + it->second);
		}
	}

	rq.setUrl(uri);
	rq.setMethod("GET");

	hc->execute(&rq, &rs, p);
	if(rs.getStatusCode()!="200") {
		if(rs.getContent()!="") {
			throw std::runtime_error("Unable to update index " + rs.getContent());
		}
		throw std::runtime_error("Unable to update index");
	}
}

void SolrSearch::removeIndex(IndexQuery& iq) {
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;
	std::map<std::string, std::string>::iterator it;

	std::string uri;
	if(!isCloud) {
		uri.append("/admin/cores");
		uri.append("?action=UNLOAD");
		uri.append("&core=" + iq.getName());
		if(iq.properties.find("deleteInstanceDir")!=iq.properties.end()) {
			uri.append("&deleteInstanceDir=" + iq.properties["deleteInstanceDir"]);
		}
	} else {
		uri.append("/admin/collections");
		uri.append("?action=DELETE");
		uri.append("&name=" + iq.getName());
		if(iq.properties.find("async")!=iq.properties.end()) {
			uri.append("&async=" + iq.properties["async"]);
		}
	}

	rq.setUrl(uri);
	rq.setMethod("GET");

	hc->execute(&rq, &rs, p);
	if(rs.getStatusCode()!="200") {
		if(rs.getContent()!="") {
			throw std::runtime_error("Unable to delete index " + rs.getContent());
		}
		throw std::runtime_error("Unable to delete index");
	}
}

void SolrSearch::add(DocumentQuery& iq) {
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;
	std::map<std::string, std::string>::iterator it;

	std::string uri;
	if(!isCloud) {
		uri.append("/api/cores/" + iq.getIndexName() + "/update/json?split=/");
	} else {
		uri.append("/api/collections/" + iq.getIndexName() + "/update/json?split=/");
	}

	if(iq.properties.find("commit")!=iq.properties.end()) {
		uri.append("&commit=" + iq.properties["commit"]);
	}

	rq.setUrl(uri);
	rq.setMethod("POST");
	rq.setContent(iq.getData());
	rq.addHeader(HttpRequest::ContentType, "application/json");

	hc->execute(&rq, &rs, p);
	if(rs.getStatusCode()!="200") {
		if(rs.getContent()!="") {
			throw std::runtime_error("Unable to add document " + rs.getContent());
		}
		throw std::runtime_error("Unable to add document");
	}
}

void SolrSearch::update(DocumentQuery& iq) {
	add(iq);
}

void SolrSearch::remove(DocumentQuery& iq) {
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;
	std::map<std::string, std::string>::iterator it;

	std::string uri;
	if(!isCloud) {
		uri.append("/api/cores/" + iq.getIndexName() + "/update/json?");
	} else {
		uri.append("/api/collections/" + iq.getIndexName() + "/update/json?");
	}

	if(iq.properties.find("commit")!=iq.properties.end()) {
		uri.append("&commit=" + iq.properties["commit"]);
	}

	rq.setUrl(uri);
	rq.setMethod("POST");
	rq.setContent("{\"delete\": {\"id\":\""+iq.getId()+"\"}}");
	rq.addHeader(HttpRequest::ContentType, "application/json");

	hc->execute(&rq, &rs, p);
	if(rs.getStatusCode()!="200") {
		if(rs.getContent()!="") {
			throw std::runtime_error("Unable to add document " + rs.getContent());
		}
		throw std::runtime_error("Unable to add document");
	}
}

std::string SolrSearch::query(SearchQuery& q) {
	return std::string();
}
