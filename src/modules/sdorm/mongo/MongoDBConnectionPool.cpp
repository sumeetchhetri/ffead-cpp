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
 * MongoDBConnectionPool.cpp
 *
 *  Created on: 02-Jun-2014
 *      Author: sumeetc
 */

#include "MongoDBConnectionPool.h"

bool MongoDBConnectionPool::inited = false;

MongoDBConnectionPool::MongoDBConnectionPool() {
	isReplicaSet = false;
	isSSL = false;
	isUnixDomainSocket = false;
	isSharded = false;
	uri = NULL;
}

MongoDBConnectionPool::MongoDBConnectionPool(const ConnectionProperties& props) {
	logger = LoggerFactory::getLogger("SQLConnectionPool");
	std::map<std::string, std::string> mp = props.getProperties();
	isReplicaSet = StringUtil::toLowerCopy(mp["isReplicaSet"])=="true";
	isSSL = StringUtil::toLowerCopy(mp["isSSL"])=="true";
	isUnixDomainSocket = StringUtil::toLowerCopy(mp["isUnixDomainSocket"])=="true";
	isSharded = StringUtil::toLowerCopy(mp["isSharded"])=="true";
	replicaSetName = mp["replicaName"];
	uri = NULL;
	createPool(props);
 }

void MongoDBConnectionPool::initEnv() {
	if(!inited) {
		mongoc_init();
		inited = true;
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

	int poolmin = 2, poolmax = 5;
	if((props.getPoolWriteSize()+props.getPoolReadSize())>0) {
		poolmax = props.getPoolWriteSize()+props.getPoolReadSize();
		if(poolmax<poolmin)
			poolmax += poolmin;
	}

	/*if(connectionString.at(connectionString.length()-1)=='?') {
		connectionString += "minPoolSize=" + CastUtil::fromNumber(poolmin);
	} else {
		connectionString += "&minPoolSize=" + CastUtil::fromNumber(poolmin);
	}*/
	if(connectionString.at(connectionString.length()-1)=='?') {
		connectionString += "maxPoolSize=" + CastUtil::fromNumber(poolmax);
	} else {
		connectionString += "&maxPoolSize=" + CastUtil::fromNumber(poolmax);
	}

	if(connTimeOut=="") {
		connTimeOut = "10000";
	}
	connectionString += "&connectTimeoutMS=" + connTimeOut;
	connectionString += "&serverSelectionTimeoutMS=" + connTimeOut;

	connectionString = "mongodb://" + connectionString;

	uri = mongoc_uri_new(connectionString.c_str());

	dbName = mongoc_uri_get_database(uri);

	mongoc_client_t* tclient = mongoc_client_new_from_uri (uri);
	if (!tclient) {
		throw std::runtime_error("Unable to create mongodb connection");
	}
	bson_t *command, reply;
	bool retval;
	bson_error_t error;
	command = BCON_NEW("ping", BCON_INT32 (1));
	retval = mongoc_client_command_simple(tclient, "admin", command, NULL, &reply, &error);
	if (!retval) {
		bson_destroy (command);
		mongoc_client_destroy(tclient);
		mongoc_uri_destroy (uri);
		throw std::runtime_error(error.message);
	}
	bson_destroy (command);
	mongoc_client_destroy(tclient);


	mongoc_client_pool_t *pool = mongoc_client_pool_new(uri);
	if(pool==NULL) {
		throw std::runtime_error("Unable to create mongodb connection pool");
	}
	mongoc_client_t *client = mongoc_client_pool_pop(pool);
	if(client==NULL) {
		throw std::runtime_error("Unable to create mongodb connection");
	}
	mongoc_client_pool_push(pool, client);
	setEnv(pool);
	props.setNewConnectionStrategy(true);
}

void* MongoDBConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	mongoc_client_pool_t *pool = (mongoc_client_pool_t*)getEnv();
	mongoc_client_t *client = mongoc_client_pool_pop(pool);
	//std::cout << client << std::endl;
	return client;
}

void MongoDBConnectionPool::closeConnection(void* conn) {
	mongoc_client_pool_t *pool = (mongoc_client_pool_t*)getEnv();
	mongoc_client_t *client = (mongoc_client_t*)conn;
	mongoc_client_pool_push(pool, client);
}

void MongoDBConnectionPool::destroy() {
	mongoc_client_pool_t *pool = (mongoc_client_pool_t*)getEnv();
	mongoc_client_pool_destroy(pool);
	mongoc_uri_destroy(uri);

	//TODO how to cleanup mongoc_cleanup();
}

MongoDBConnectionPool::~MongoDBConnectionPool() {
	destroyPool();
}

