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
	bool isZkUrl = StringUtil::toLowerCopy(pool->getProperties().getProperty("isZkUrl"))=="true";
	solrcloud = StringUtil::toLowerCopy(pool->getProperties().getProperty("solrcloud"))=="true";
	std::string colls = pool->getProperties().getProperty("collections");
	if(StringUtil::trimCopy(colls)=="") {
		throw std::runtime_error("No collections provided");
	}
	std::vector<std::string> colnames = StringUtil::splitAndReturn<std::vector<std::string> >(colls, ",");
	for(std::string cn: colnames) {
		solrState.collections[cn];
	}
	if(solrcloud) {
		if(isZkUrl) {
#ifdef ZK_LIB
			auto client = zk::client::connect(res).get();
			auto get_res = client.get("/").get();
			std::cout << get_res.stat() << std::endl;
			auto get_c_res = client.get_children("/live_nodes").get();
			for(std::string lv: get_c_res.children()) {
				solrState.live_nodes[lv] = true;
			}
			std::cout << get_c_res << std::endl;
			get_c_res = client.get_children("/collections").get();
			std::cout << get_c_res << std::endl;
			for(std::string ent: get_c_res.children()) {
				if(solrState.collections.find(ent)!=solrState.collections.end()) {
					get_res = client.get("/collections/"+ent+"/state.json").get();
					std::copy(get_res.data().begin(), get_res.data().end(), std::ostream_iterator<char>(std::cout, ""));
					//std::string s(get_res.data().c_str(), get_res.data().end());
					rapidjson::Document doc;
					doc.Parse(&get_res.data()[0], get_res.data().size());
					if(doc.HasMember(ent)) {
						rapidjson::GenericObject o = doc[ent].GetObject();
						if(o.HasMember("shards")) {
							for (auto& m : o["shards"].GetObject()) {
								SolrCollectionShard& scs = solrState.collections[ent].shards[m.name.GetString()];
								std::vector<std::string> ranges;
								StringUtil::split(ranges, m.value["range"].GetString(), "-");
								scs.start = std::stoul(ranges.at(0), 0, 16);
								scs.end = std::stoul(ranges.at(1), 0, 16);
								scs.state = m.value["state"].GetString();
								scs.state = m.value["state"].GetString();
								for (auto& r : m.value["replicas"].GetObject()) {
									if(r.value.HasMember("leader") && strncmp(r.value["leader"].GetString(), "true", 4)==0) {
										scs.leader = r.value["base_url"].GetString();
									}
									scs.nodes.push_back(std::make_tuple(r.value["base_url"].GetString(), r.value["node_name"].GetString(), r.value["core"].GetString()));
								}
							}
						}
					}
				}
			}
			client.close();
#endif
		} else {
			HttpClient hc(pool->getProperties().getNodes().at(0).getBaseUrl());
			HttpRequest rq;
			HttpResponse rs;
			propMap p;

			rq.setUrl("/admin/collections?action=CLUSTERSTATUS&wt=json");
			rq.setMethod("GET");
			hc.execute(&rq, &rs, p);

			if(rs.getStatusCode()!="200") {
				if(rs.getContent()!="") {
					throw std::runtime_error("Error during init op " + rs.getContent());
				}
				throw std::runtime_error("Error during init op ");
			} else {
#ifdef HAVE_RAPID_JSON
				rapidjson::Document doc;
				doc.Parse(&rs.getContent()[0], rs.getContent().size());
				if(doc["responseHeader"]["status"].GetInt()>0) {
					throw std::runtime_error("Error during index op " + std::to_string(doc["responseHeader"]["status"].GetInt()));
				}
				if(doc.HasMember("cluster")) {
					for (auto& m : doc["cluster"]["live_nodes"].GetArray()) {
						solrState.live_nodes[m.GetString()] = true;
					}
					for (auto& co : doc["cluster"]["collections"].GetObject()) {
						std::string ent = co.name.GetString();
						if(solrState.collections.find(ent)!=solrState.collections.end()) {
							rapidjson::GenericObject o = co.value.GetObject();
							if(o.HasMember("shards")) {
								for (auto& m : o["shards"].GetObject()) {
									SolrCollectionShard& scs = solrState.collections[ent].shards[m.name.GetString()];
									std::vector<std::string> ranges;
									StringUtil::split(ranges, m.value["range"].GetString(), "-");
									scs.start = std::stoul(ranges.at(0), 0, 16);
									scs.end = std::stoul(ranges.at(1), 0, 16);
									scs.state = m.value["state"].GetString();
									for (auto& r : m.value["replicas"].GetObject()) {
										if(r.value.HasMember("leader") && strncmp(r.value["leader"].GetString(), "true", 4)==0) {
											scs.leader = r.value["base_url"].GetString();
										}
										scs.nodes.push_back(std::make_tuple(r.value["base_url"].GetString(), r.value["node_name"].GetString(), r.value["core"].GetString()));
									}
								}
							}
						}
					}
				}
#endif
			}
		}
	}
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
    if(!solrcloud) {
        throw std::runtime_error("Cannot create collection in standalone mode using API");
    } else {
        uri.append("/admin/collections");
        uri.append("?wt=json&action=");
    }
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
    } else {
        JSONElement out;
        JSONUtil::getDocument(rs.getContent(), out);
        JSONElement* rh = out.getNodeP("responseHeader");
        int status = CastUtil::toInt(rh->getNodeP("status")->getValue());
        if(status>0) {
            throw std::runtime_error("Error during index op " + rh->getNodeP("status")->getValue());
        }
    }
}

void SolrSearch::createIndex(IndexQuery& iq) {
	iq.setOpType("CREATE");
    iq.getProperties().emplace("name", iq.getName());
    return indexOp(iq);
}

void SolrSearch::removeIndex(IndexQuery& iq) {
}

void SolrSearch::updateIndex(IndexQuery& iq) {
}

void SolrSearch::docOp(DocumentQuery& iq) {
	Connection* connection = pool->checkout();
    HttpClient* hc = (HttpClient*)connection->getConn();
    HttpRequest rq;
    HttpResponse rs;
    propMap p;
    //p["VERBOSE"] = "true";
    //p["NO_TRANSFER_ENCODING"] = "true";
    std::map<std::string, std::string>::iterator it;

    std::string uri;
    if(!solrcloud) {
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
    } else {
        std::string ct = rs.getHeader(HttpRequest::ContentType);
        if(ct.find("application/json")!=std::string::npos) {
#ifdef HAVE_RAPID_JSON
            rapidjson::Document doc;
            doc.Parse(rs.getContent().c_str(), rs.getContent().length());
            int status = doc["responseHeader"]["status"].GetInt();
            if(status>0) {
                std::string msg(doc["error"]["msg"].GetString(), doc["error"]["msg"].GetStringLength());
                throw std::runtime_error("Error during index op " + msg);
            }
#endif
        } else {
            throw std::runtime_error("Error during document op " + rs.getContent());
        }
    }
}

void SolrSearch::add(DocumentQuery& iq) {
    if(!solrcloud) {
        iq.getProperties().emplace("____urprefix", "/json/docs");
    } else {
        iq.getProperties().emplace("____urprefix", "/json");
    }
    if(iq.commit) {
        iq.getProperties().emplace("commit", "true");
    }
    docOp(iq);
}

void SolrSearch::update(DocumentQuery& iq) {
    if(!solrcloud) {
        iq.getProperties().emplace("____urprefix", "/json/docs");
    } else {
        iq.getProperties().emplace("____urprefix", "/json");
    }
    if(iq.commit) {
        iq.getProperties().emplace("commit", "true");
    }
    docOp(iq);
}

void SolrSearch::remove(DocumentQuery& iq) {
    if(iq.getId().length()>0) {
        iq.setData("{\"delete\":{\"query\":\"id:"+iq.getId()+"\"}}");
    } else {
        iq.setData("{\"delete\":{\"query\":\""+iq.getData()+"\"}}");
    }
    if(iq.commit) {
        iq.getProperties().emplace("commit", "true");
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
    if(!solrcloud) {
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
            throw std::runtime_error("Error during document op " + rs.getContent());
        }
        throw std::runtime_error("Error during document op ");
    } else {
        std::string ct = rs.getHeader(HttpRequest::ContentType);
        if(ct.find("application/json")!=std::string::npos) {
#ifdef HAVE_RAPID_JSON
            rapidjson::Document doc;
            doc.Parse(rs.getContent().c_str(), rs.getContent().length());
            int status = doc["responseHeader"]["status"].GetInt();
            if(status>0) {
                std::string msg(doc["error"]["msg"].GetString(), doc["error"]["msg"].GetStringLength());
                throw std::runtime_error("Error during index op " + msg);
            } else {
                std::cout << rs.getContent() << std::endl;
            }
#endif
        } else {
            throw std::runtime_error("Error during document op " + rs.getContent());
        }
    }

    return rs.getContent();
}

void SolrSearchConnectionPool::initEnv() {
	bool isZkUrl = false;
	bool inited = false;
	bool solrcloud = StringUtil::toLowerCopy(getProperties().getProperty("solrcloud"))=="true";
	std::string colls = getProperties().getProperty("collections");
	if(StringUtil::trimCopy(colls)=="") {
		throw std::runtime_error("No collections provided");
	}
	if(solrcloud) {
		for(const ConnectionNode& cn: getProperties().getNodes()) {
			std::string burl = cn.getBaseUrl();
			StringUtil::trim(burl);
			StringUtil::toLower(burl);
			if(burl.find("zk:")==0) {
				isZkUrl = true;
				break;
			}
		}
		if(isZkUrl && getProperties().getNodes().size()>1) {
			throw std::runtime_error("Please specify single node with zookeeper ensemble url");
		} else {
			for(const ConnectionNode& cn: getProperties().getNodes()) {
				std::string burl = cn.getBaseUrl();
				StringUtil::trim(burl);
				StringUtil::toLower(burl);
				if(burl.find("http://")!=0 || burl.find("https://")!=0) {
					throw std::runtime_error("Please specify a valid node with solr cloud http url");
				}
			}
		}
		getProps().addProperty("isZkUrl", isZkUrl?"true":"false");
	} else {
		for(const ConnectionNode& cn: getProperties().getNodes()) {
			std::string burl = cn.getBaseUrl();
			StringUtil::trim(burl);
			StringUtil::toLower(burl);
			if(burl.find("http://")!=0 || burl.find("https://")!=0) {
				throw std::runtime_error("Please specify a valid single node with solr http url");
			}
		}
	}

#ifndef HAVE_RAPID_JSON
		throw std::runtime_error("Cannot start solr client without rapidjson");
#endif

	if(solrcloud) {
		if(isZkUrl) {
#ifndef ZK_LIB
			throw std::runtime_error("Cannot start solr cloud client without zk lib as a zookeeper ensemble url is specified");
#endif
		}
	}
}

void* SolrSearchConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	return new HttpClient();
}

void SolrSearchConnectionPool::closeConnection(void* conn) {
	delete ((HttpClient*)conn);
}

void SolrSearchConnectionPool::destroy() {
}

SolrSearchConnectionPool::SolrSearchConnectionPool(const ConnectionProperties& props) {
	createPool(props);
}

SolrSearchConnectionPool::~SolrSearchConnectionPool() {
	destroyPool();
}

