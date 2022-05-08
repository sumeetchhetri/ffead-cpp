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
 * SearchEngineManager.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef SEARCHENGINEMANAGER_H_
#define SEARCHENGINEMANAGER_H_
#include "string"
#include "map"
#include "CacheInterface.h"
#include "ConnectionPooler.h"
#include "Mutex.h"
#include "SearchEngineInterface.h"
#include "HttpClient.h"
#ifdef HAVE_SOLR
#include "SolrSearch.h"
#endif
#ifdef HAVE_ELASTIC
#include "ElasticSearch.h"
#endif

class SearchEngineConnectionPool: public ConnectionPooler {
	void initEnv();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
	void destroy();
public:
	SearchEngineConnectionPool(const ConnectionProperties& props);
	virtual ~SearchEngineConnectionPool();
};

class SearchEngineManager {
	static std::map<std::string, SearchEngineManager*> engines;
	static std::map<std::string, std::string> defEngineNames;
	static std::map<std::string, bool> appInitCompletionStatus;
	ConnectionProperties props;
	ConnectionPooler* pool;
	static void initSearch(const ConnectionProperties& props, const std::string& appName, GetClassBeanIns f);
	static void destroy();
	SearchEngineManager(const ConnectionProperties& props);
	friend class ConfigurationHandler;
public:
	virtual ~SearchEngineManager();
	static SearchEngineInterface* getImpl(std::string name = "");
	static void triggerAppInitCompletion(std::string appName = "");
	static bool isInitCompleted();
};

#endif /* SEARCHENGINEMANAGER_H_ */
