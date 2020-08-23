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
 * MongoDBConnectionPool.h
 *
 *  Created on: 02-Jun-2014
 *      Author: sumeetc
 */

#ifndef MONGODBCONNECTIONPOOL_H_
#define MONGODBCONNECTIONPOOL_H_
#define MONGO_HAVE_STDINT
#include "mongoc.h"
#include "ConnectionPooler.h"
#include "LoggerFactory.h"

class MongoDBConnectionPool: public ConnectionPooler {
	Logger logger;
	void initEnv();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
	void destroy();
	mongoc_uri_t* uri;
	bool isReplicaSet;
	bool isSharded;
	bool isUnixDomainSocket;
	bool isSSL;
	std::string replicaSetName;
	std::string dbName;
	static bool inited;
	MongoDBConnectionPool();
	friend class MongoDBRawConnectionPool;
	friend class MongoDBRawDataSourceImpl;
public:
	MongoDBConnectionPool(const ConnectionProperties& props);
	virtual ~MongoDBConnectionPool();
};

#endif /* MONGODBCONNECTIONPOOL_H_ */
