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
 * SolrSearch.cpp
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#include "SolrSearch.h"

std::string SolrSearch::COLL_C_PARAMS = ",router.name,numShards,shards,replicationFactor,nrtReplicas,tlogReplicas,pullReplicas,"
		"maxShardsPerNode,createNodeSet,createNodeSet.shuffle,collection.configName,router.field,autoAddReplicas,async,rule,snitch,policy,waitForFinalState,";
std::string SolrSearch::COLL_M_PARAMS = ",maxShardsPerNode,replicationFactor,autoAddReplicas,collection.configName,rule,snitch,policy,";

SolrSearch::SolrSearch(ConnectionPooler* pool) {
	this->pool = pool;
	v2 = StringUtil::toLowerCopy(pool->getProperties().getProperty("v2"))=="true";
}

SolrSearch::~SolrSearch() {
}

void SolrSearch::indexOp(IndexQuery& iq) {
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;
	std::map<std::string, std::string>::iterator it;

	std::string uri;
	uri.append("/admin/collections");
	uri.append("?wt=json&action=");
	uri.append(iq.getOpType());
	for(it=iq.properties.begin();it!=iq.properties.end();++it) {
		//if(COLL_C_PARAMS.find(","+it->first+",")!=std::string::npos) {
			if(it->first.find("____")==0) continue;
			uri.append("&" + it->first + "=" + it->second);
		//}
	}

	rq.setUrl(uri);
	rq.setMethod("GET");

	hc->execute(&rq, &rs, p);
	pool->release(connection);
	if(rs.getStatusCode()!="200") {
		if(rs.getContent()!="") {
			throw std::runtime_error("Error during index op " + rs.getContent());
		}
		throw std::runtime_error("Error during index op ");
	}
}

void SolrSearch::createIndex(IndexQuery& iq) {
	iq.getProperties().emplace("action", "CREATE");
	iq.getProperties().emplace("name", iq.getName());
	return indexOp(iq);
}

void SolrSearch::updateIndex(IndexQuery& iq) {
	iq.getProperties().emplace("action", "MODIFYCOLLECTION");
	iq.getProperties().emplace("name", iq.getName());
	return indexOp(iq);
}

void SolrSearch::removeIndex(IndexQuery& iq) {
	iq.getProperties().emplace("action", "DELETE");
	iq.getProperties().emplace("name", iq.getName());
	return indexOp(iq);
}

void SolrSearch::docOp(DocumentQuery& iq) {
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;
	std::map<std::string, std::string>::iterator it;

	std::string uri;
	if(!v2) {
		uri.append("/" + iq.getIndexName() + "/update" + iq.getProperties()["____urprefix"] + "?");
	} else {
		uri.append("/api/c/" + iq.getIndexName() + "/update" + iq.getProperties()["____urprefix"] + "?");
	}

	for(it=iq.properties.begin();it!=iq.properties.end();++it) {
		//if(COLL_C_PARAMS.find(","+it->first+",")!=std::string::npos) {
			if(it->first.find("____")==0) continue;
			uri.append("&" + it->first + "=" + it->second);
		//}
	}

	rq.setUrl(uri);
	rq.setMethod("POST");
	rq.setContent(std::move(iq.getData()));
	rq.addHeader(HttpRequest::ContentType, "application/json");

	hc->execute(&rq, &rs, p);
	pool->release(connection);
	if(rs.getStatusCode()!="200") {
		if(rs.getContent()!="") {
			throw std::runtime_error("Error during document op " + rs.getContent());
		}
		throw std::runtime_error("Error during document op ");
	}
}

void SolrSearch::add(DocumentQuery& iq) {
	if(!v2) {
		iq.getProperties().emplace("____urprefix", "/json/docs");
	} else {
		iq.getProperties().emplace("____urprefix", "/json");
	}
	docOp(iq);
}

void SolrSearch::update(DocumentQuery& iq) {
	if(!v2) {
		iq.getProperties().emplace("____urprefix", "/json/docs");
	} else {
		iq.getProperties().emplace("____urprefix", "/json");
	}
	docOp(iq);
}

void SolrSearch::remove(DocumentQuery& iq) {
	if(iq.getId().length()>0) {
		iq.setData("{\"delete\":{\"query\":\"id:"+iq.getId()+"\"}}");
	} else {
		iq.setData("{\"delete\":{\"query\":\""+iq.getData()+"\"}}");
	}
	docOp(iq);
}

std::string SolrSearch::query(SearchQuery& iq) {
	Connection* connection = pool->checkout();
	HttpClient* hc = (HttpClient*)connection->getConn();
	HttpRequest rq;
	HttpResponse rs;
	propMap p;
	std::map<std::string, std::string>::iterator it;

	std::string uri;
	if(!v2) {
		uri.append("/" + iq.getIndexName() + "/query?");
	} else {
		uri.append("/api/c/" + iq.getIndexName() + "/query?");
	}

	for(it=iq.properties.begin();it!=iq.properties.end();++it) {
		//if(COLL_C_PARAMS.find(","+it->first+",")!=std::string::npos) {
			if(it->first.find("____")==0) continue;
			uri.append("&" + it->first + "=" + it->second);
		//}
	}

	rq.setUrl(uri);
	rq.setMethod("POST");
	if(iq.getData().length()>0) {
		rq.setContent(std::move(iq.getData()));
	}
	rq.addHeader(HttpRequest::ContentType, "application/json");

	hc->execute(&rq, &rs, p);
	pool->release(connection);
	if(rs.getStatusCode()!="200") {
		if(rs.getContent()!="") {
			throw std::runtime_error("Error during search op " + rs.getContent());
		}
		throw std::runtime_error("Error during search op ");
	}

	return rs.getContent();
}
