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
 * MongoDBRawDataSourceImpl.h
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */
//NOT THREAD SAFE
#ifndef MongoDBRawDataSourceIMPL_H_
#define MongoDBRawDataSourceIMPL_H_
#include "Compatibility.h"
#include <stdio.h>
#include <stdlib.h>
#define MONGO_HAVE_STDINT
#include "mongoc.h"
#include "DataSourceInterface.h"
#include "MongoDBConnectionPool.h"
#include "LoggerFactory.h"

struct MgRawRes {
	long long l;
	std::string s;
	long double ld;
	double d;
	float f;
	bool b;
	Date dt;
	std::string n;
	int len;
	int type;
};

typedef void (*MgRawResFunc) (void* ctx, int, std::vector<MgRawRes>&);
typedef void (*MgRawBlkFunc) (bson_t*, bson_t*);

enum MongoDBRawOper {
	INSERT, SAVE, UPDATE, UPDATE_MULTI, REMOVE
};

class MongoDBRawConnectionPool : public MongoDBConnectionPool {
	void initEnv();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
	void destroy();
	friend class MongoDBRawDataSourceImpl;
public:
	MongoDBRawConnectionPool(const ConnectionProperties& props);
	virtual ~MongoDBRawConnectionPool();
};

class MongoDBRawDataSourceImpl : public DataSourceType {
	Logger logger;
	MongoDBRawConnectionPool* pool;
	mongoc_client_t* conn;
	mongoc_collection_t* coll;
	mongoc_bulk_operation_t* bulk;
public:
	DSType getType();
	MongoDBRawDataSourceImpl(ConnectionPooler* pool);
	virtual ~MongoDBRawDataSourceImpl();

	bool init();

	bool begin(const std::string&);
	bool end();

	bool startBulk(const std::string&);
	void addBulk(bson_t* q, bson_t* d);
	bool endBulk();

	void executeQuery(const bson_t* query, void* ctx, MgRawResFunc cb, int start = 0, int count = 0);
	void executeQuery(const std::string &query, void* ctx, MgRawResFunc cb, int start = 0, int count = 0);
	bool executeUpdateQuery(const std::string &query, const std::string &data, MongoDBRawOper oper);
	bool executeUpdateQuery(const bson_t* query, const bson_t* data, MongoDBRawOper oper);
	bool executeUpdateBulkQuery(int len, MgRawBlkFunc cb);
};

#endif /* MongoDBRawDataSourceIMPL_H_ */
