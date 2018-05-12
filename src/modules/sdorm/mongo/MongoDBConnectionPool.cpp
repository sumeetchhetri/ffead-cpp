/*
 * MongoDBConnectionPool.cpp
 *
 *  Created on: 02-Jun-2014
 *      Author: sumeetc
 */

#include "MongoDBConnectionPool.h"

MongoDBConnectionPool::MongoDBConnectionPool(const ConnectionProperties& props) {
	logger = LoggerFactory::getLogger("SQLConnectionPool");
	std::map<std::string, std::string> mp = props.getProperties();
	isReplicaSet = StringUtil::toLowerCopy(mp["isReplicaSet"])=="true";
	isSSL = StringUtil::toLowerCopy(mp["isSSL"])=="true";
	isUnixDomainSocket = StringUtil::toLowerCopy(mp["isUnixDomainSocket"])=="true";
	isSharded = StringUtil::toLowerCopy(mp["isSharded"])=="true";
	replicaSetName = mp["replicaName"];
	createPool(props);
 }

void MongoDBConnectionPool::initEnv() {
	mongoc_init();

	ConnectionProperties props = getProperties();
	std::string connectionString = "";

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
			connectionString += props.getNodes().at(var).getHost() + ":" + CastUtil::lexical_cast<std::string>(port);
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

	if(connectionString.at(connectionString.length()-1)=='?') {
		connectionString += "minPoolSize=" + CastUtil::lexical_cast<std::string>(poolmin);
	} else {
		connectionString += "&minPoolSize=" + CastUtil::lexical_cast<std::string>(poolmin);
	}
	connectionString += "&maxPoolSize=" + CastUtil::lexical_cast<std::string>(poolmax);

	connectionString = "mongodb://" + connectionString;

	//std::cout << connectionString << std::endl;

	uri = mongoc_uri_new(connectionString.c_str());
	mongoc_client_pool_t *pool = mongoc_client_pool_new(uri);
	if(pool==NULL) {
		throw "Unable to create memcached connection pool";
	}
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

	mongoc_cleanup();
}

MongoDBConnectionPool::~MongoDBConnectionPool() {
	destroyPool();
}

