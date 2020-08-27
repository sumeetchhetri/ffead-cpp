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
#include "MongoDBRawDataSourceImpl.h"

DSType MongoDBRawDataSourceImpl::getType() {
	return SD_RAW_MONGO;
}

MongoDBRawConnectionPool::MongoDBRawConnectionPool(const ConnectionProperties& props) {
	setProperties(props);
}
MongoDBRawConnectionPool::~MongoDBRawConnectionPool() {
}

void MongoDBRawConnectionPool::initEnv() {
	if(!MongoDBConnectionPool::inited) {
		mongoc_init();
		MongoDBConnectionPool::inited = true;
	}

	ConnectionProperties props = getProperties();
	std::string connectionString = "";
	std::string connTimeOut = "";

	if(isUnixDomainSocket)
	{
		if(props.getNodes().at(0).getUsername()!="" && props.getNodes().at(0).getPassword()!="") {
			connectionString += (props.getNodes().at(0).getUsername() + ":" +
					props.getNodes().at(0).getPassword() + "@");
		}
		connectionString += "/tmp/mongodb.sock";
		if(props.getNodes().at(0).getDatabaseName()!="") {
			connectionString += "/" + props.getNodes().at(0).getDatabaseName();
		}

		if(props.getNodes().at(0).getConnectionTimeout()>0) {
			connTimeOut = CastUtil::fromNumber((long)getProperties().getNodes().at(0).getConnectionTimeout());
		}
	}
	else
	{
		std::string databaseName = "";
		//std::cout << props.getNodes().size() << std::endl;
		for (int var = 0; var < (int)props.getNodes().size(); ++var) {
			if(databaseName=="") {
				databaseName = props.getNodes().at(var).getDatabaseName();
			}
			int port = props.getNodes().at(var).getPort();
			if(props.getNodes().at(var).getHost()=="") {
				continue;
			}
			if(port==-1) {
				port = 27017;
			}

			//std::cout << props.getNodes().at(var).getHost() << std::endl;
			if(props.getNodes().at(var).getUsername()!="" && props.getNodes().at(var).getPassword()!="") {
				connectionString += (props.getNodes().at(var).getUsername() + ":" +
						props.getNodes().at(var).getPassword() + "@");
			}
			connectionString += props.getNodes().at(var).getHost() + ":" + CastUtil::fromNumber(port);

			if(connTimeOut=="" && props.getNodes().at(var).getConnectionTimeout()>0) {
				connTimeOut = CastUtil::fromNumber((long)getProperties().getNodes().at(var).getConnectionTimeout());
			}

			if(var!=(int)props.getNodes().size()-1) {
				connectionString += ",";
			}
		}

		if(connectionString.at(connectionString.length()-1)==',') {
			connectionString = connectionString.substr(0, connectionString.length()-1);
		}

		connectionString += "/";

		if(databaseName!="") {
			connectionString += databaseName;
		}

		connectionString += "?";

		if(isReplicaSet && replicaSetName!="") {
			connectionString += "replicaSet="+replicaSetName;
			if(isSSL) {
				connectionString += "&ssl=true";
			}
		} else if(isSSL) {
			connectionString += "ssl=true";
		}
	}

	if(connectionString.at(connectionString.length()-1)=='?') {
		connectionString += "maxPoolSize=1";
	} else {
		connectionString += "&maxPoolSize=1";
	}

	if(connTimeOut=="") {
		connTimeOut = "10000";
	}
	connectionString += "&connectTimeoutMS=" + connTimeOut;
	connectionString += "&serverSelectionTimeoutMS=" + connTimeOut;

	connectionString = "mongodb://" + connectionString;

	uri = mongoc_uri_new(connectionString.c_str());

	dbName = mongoc_uri_get_database(uri);
}

void* MongoDBRawConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	return NULL;
}

void MongoDBRawConnectionPool::closeConnection(void* conn) {
}

void MongoDBRawConnectionPool::destroy() {
}

MongoDBRawDataSourceImpl::MongoDBRawDataSourceImpl(ConnectionPooler* pool) {
	this->conn = NULL;
	this->pool = (MongoDBRawConnectionPool*)pool;
	this->coll = NULL;
	this->bulk = NULL;
}

MongoDBRawDataSourceImpl::~MongoDBRawDataSourceImpl() {
	if(conn!=NULL) {
		mongoc_client_destroy(conn);
	}
}

bool MongoDBRawDataSourceImpl::init() {
	conn = mongoc_client_new_from_uri (pool->uri);
	if (!conn) {
		throw std::runtime_error("Unable to create mongodb connection");
	}
	return true;
}

bool MongoDBRawDataSourceImpl::begin(const std::string& collName) {
	if(coll!=NULL) {
		throw std::runtime_error("Already in a session, session still open");
	}
	coll = mongoc_client_get_collection (conn, pool->dbName.c_str(), collName.c_str());
	return coll!=NULL;
}

bool MongoDBRawDataSourceImpl::end() {
	if(coll!=NULL) {
		mongoc_collection_destroy(coll);
		coll = NULL;
		return true;
	}
	return false;
}

void MongoDBRawDataSourceImpl::executeQuery(const std::string &query, void* ctx, MgRawResFunc cb, int start, int count) {
	bson_error_t error;
	bson_t* qb = bson_new_from_json((const uint8_t*)query.c_str(), query.length(), &error);
	executeQuery(qb, ctx, cb, start, count);
}

void MongoDBRawDataSourceImpl::executeQuery(const bson_t* query, void* ctx, MgRawResFunc cb, int start, int count) {
	if(query==NULL) {
		query = BCON_NEW ("$query", "{", "}");
	}
	mongoc_cursor_t* cursor = mongoc_collection_find(coll, MONGOC_QUERY_NONE, start, count, 0, query, NULL, NULL);
	const bson_t *doc;
	bson_iter_t i;
	std::vector<MgRawRes> row;
	int rowNum = 0;
	while (mongoc_cursor_next(cursor, &doc))
	{
		row.clear();
		bson_iter_init(&i , doc);
		while (bson_iter_next(&i)) {
		    bson_type_t t = bson_iter_type(&i);
		    const char* key = bson_iter_key(&i);
		    //uint32_t klen = bson_iter_key_len(&i);
		    MgRawRes res;
		    res.type = t;
		    res.n = std::string(key);
		    switch (t) {
		    	case BSON_TYPE_INT32:
				{
					res.l = bson_iter_int32(&i);
					break;
				}
				case BSON_TYPE_INT64:
				{
					res.l = bson_iter_int64(&i);
					break;
				}
				case BSON_TYPE_DOUBLE:
				{
					res.d = bson_iter_double(&i);
					break;
				}
				case BSON_TYPE_BOOL:
				{
					res.b = bson_iter_bool(&i);
					break;
				}
				case BSON_TYPE_UTF8:
				{
					uint32_t len;
					res.s = std::string(bson_iter_utf8(&i, &len));
					break;
				}
				case BSON_TYPE_NULL: break;
				case BSON_TYPE_OID:
				{
					char oidhex[25];
					bson_oid_to_string(bson_iter_oid(&i), oidhex);
					res.s = std::string(oidhex);
					break;
				}
				case BSON_TYPE_TIMESTAMP:
				{
					uint32_t inc;
					uint32_t ts;
					bson_iter_timestamp(&i, &ts, &inc);
					res.dt = Date::getDateFromSeconds(ts);
					break;
				}
				case BSON_TYPE_DATE_TIME:
				{
					int64_t td;
					td = bson_iter_date_time(&i);
					res.dt = Date::getDateFromSeconds(td/1000);
					break;
				}
				default:
					break;
		    }
		    row.push_back(res);
		}
		cb(ctx, rowNum++, row);
	}
}

bool MongoDBRawDataSourceImpl::executeUpdateQuery(const std::string &query, const std::string &data, MongoDBRawOper oper) {
	bson_error_t error;
	bson_t* qb = query.length()>0?bson_new_from_json((const uint8_t*)query.c_str(), query.length(), &error):bson_new();
	bson_t* db = bson_new_from_json((const uint8_t*)query.c_str(), query.length(), &error);
	return executeUpdateQuery(qb, db, oper);
}

bool MongoDBRawDataSourceImpl::executeUpdateQuery(const bson_t* query, const bson_t* data, MongoDBRawOper oper) {
	bson_error_t er;
	switch(oper) {
		case INSERT:
			return mongoc_collection_insert(coll, MONGOC_INSERT_NONE, data, NULL, &er);
		case UPDATE:
			return mongoc_collection_update(coll, MONGOC_UPDATE_NONE, query, data, NULL, &er);
		case SAVE:
			return mongoc_collection_save(coll, data, NULL, &er);
		case UPDATE_MULTI:
			return mongoc_collection_update(coll, MONGOC_UPDATE_MULTI_UPDATE, query, data, NULL, &er);
		case REMOVE:
			return mongoc_collection_remove(coll, MONGOC_REMOVE_NONE, query, NULL, &er);
	}
	return false;
}

bool MongoDBRawDataSourceImpl::startBulk(const std::string& collName) {
	if(bulk!=NULL) {
		throw std::runtime_error("Already in a bulk session, bulk session still open");
	}
	begin(collName);
	bulk = mongoc_collection_create_bulk_operation (coll, true, NULL);
	return bulk != NULL;
}

void MongoDBRawDataSourceImpl::addBulk(bson_t* q, bson_t* d) {
	mongoc_bulk_operation_replace_one(bulk, q, d, false);
}

bool MongoDBRawDataSourceImpl::endBulk() {
	bson_t reply;
	bson_error_t error;
	bool fl = mongoc_bulk_operation_execute (bulk, &reply, &error);
	mongoc_bulk_operation_destroy (bulk);
	return fl;
}

bool MongoDBRawDataSourceImpl::executeUpdateBulkQuery(int len, MgRawBlkFunc cb) {
	if(len>0) {
		mongoc_bulk_operation_t* bulk = mongoc_collection_create_bulk_operation (coll, true, NULL);
		for(int i=0;i<len;i++) {
			bson_t q;
			bson_t d;
			cb(&q, &d);
			mongoc_bulk_operation_replace_one(bulk, &q, &d, false);
		}
		bson_t reply;
		bson_error_t error;
		bool fl = mongoc_bulk_operation_execute (bulk, &reply, &error);
		mongoc_bulk_operation_destroy (bulk);
		return fl;
	}
	return false;
}
