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
 * SolrSearch.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef SOLRSEARCH_H_
#define SOLRSEARCH_H_
#include "SearchEngineInterface.h"
#include "MurmurHash3.h"
#include <tuple>
#include <chrono>
#ifdef ZK_LIB
#include <zk/client.hpp>
#include <zk/connection.hpp>
#endif
//http://localhost:8983/solr/admin/info/system

class SolrCollectionShard {
public:
    std::string leader;
    std::string state;
    unsigned long start;
    unsigned long end;
    std::vector<std::tuple<std::string, std::string, std::string>> nodes;
    std::string core;
};

class SolrCollection {
public:
    std::map<std::string, SolrCollectionShard> shards;
    std::string getUrlForUpdate(std::string id) {
        std::string url;
        uint32_t hash = 0;
        MurmurHash3_x86_32(id.c_str(), (int)id.length(), 0, &hash);
        for (auto const& n : shards) {
            if(n.second.state=="active") {
                if(hash>=n.second.start && hash<=n.second.end) {
                    return n.second.leader;
                }
            }
        }
        return url;
    }
    std::string getUrlForRead(std::string id) {
        std::string url;
        for (auto const& n : shards) {
            if(n.second.state=="active" && n.second.nodes.size()>0) {
            	return std::get<0>(n.second.nodes.at(0));
            }
        }
        return url;
    }
};

class SolrState {
public:
    std::map<std::string, bool> live_nodes;
    std::map<std::string, SolrCollection> collections;
};

class SolrSearchConnectionPool: public ConnectionPooler {
	void initEnv();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
	void destroy();
public:
	SolrSearchConnectionPool(const ConnectionProperties& props);
	virtual ~SolrSearchConnectionPool();
};

class SolrSearch: public SearchEngineInterface {
    static std::string COLL_C_PARAMS, COLL_M_PARAMS;
    bool solrcloud;
    SolrState solrState;
public:
    SolrSearch(ConnectionPooler* pool);
    virtual ~SolrSearch();

    void createIndex(IndexQuery& iq);
    void updateIndex(IndexQuery& iq);
    void removeIndex(IndexQuery& iq);
    void indexOp(IndexQuery& iq);

    void add(DocumentQuery& iq);
    void update(DocumentQuery& iq);
    void remove(DocumentQuery& iq);
    void docOp(DocumentQuery& iq);

    std::string query(SearchQuery& q);
};

#endif /* SOLRSEARCH_H_ */
