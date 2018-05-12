/*
 * MemcachedImpl.cpp
 *
 *  Created on: 09-May-2014
 *      Author: sumeetc
 */

#include "MemcachedImpl.h"

MemcachedImpl::MemcachedImpl(ConnectionPooler* pool) {
	this->pool = pool;
	this->properties = pool->getProperties();
	this->defaultExpireSeconds = -1;
	if(properties.getProperty("expiryTime")!="") {
		try {
			this->defaultExpireSeconds = CastUtil::lexical_cast<int>(properties.getProperty("expiryTime"));
		} catch(...) {
		}
	}
}

MemcachedImpl::~MemcachedImpl() {
}

bool MemcachedImpl::remove(const std::string& key) {
	Connection* connection = pool->checkout();
	memcached_return_t reply = memcached_delete((memcached_st*)connection->getConn(),
			key.c_str(), key.length(), (time_t)0);
	pool->release(connection);
	return replyStatus(reply);
}

long long MemcachedImpl::increment(const std::string& key, const int& number) {
	Connection* connection = pool->checkout();
	uint64_t value;
	memcached_increment((memcached_st*)connection->getConn(),
			key.c_str(), key.length(), (uint32_t)number, &value);
	pool->release(connection);
	return value;
}

long long MemcachedImpl::decrement(const std::string& key, const int& number) {
	Connection* connection = pool->checkout();
	uint64_t value;
	memcached_decrement((memcached_st*)connection->getConn(),
			key.c_str(), key.length(), (uint32_t)number, &value);
	pool->release(connection);
	return value;
}

long double MemcachedImpl::incrementFloat(const std::string& key, const double& number) {
	throw "Not Implemented";
}

long double MemcachedImpl::decrementFloat(const std::string& key, const double& number) {
	throw "Not Implemented";
}

std::map<std::string, std::string> MemcachedImpl::statistics() {
	Connection* connection = pool->checkout();
	memcached_return_t reply;
	memcached_stat_st* stat = memcached_stat((memcached_st*)connection->getConn(), NULL, &reply);
	char** keys = memcached_stat_get_keys((memcached_st*)connection->getConn(), stat, &reply);
	std::map<std::string, std::string> stats;
	if(replyStatus(reply)) {
		char **ptr;
		for (ptr= keys; *ptr; ptr++) {
			char* valuec = memcached_stat_get_value((memcached_st*)connection->getConn(), stat, *ptr, &reply);
			stats[std::string(*ptr)] = std::string(valuec);
		}
		free(keys);
	}
	memcached_stat_free(NULL, stat);
	pool->release(connection);
	return stats;
}

bool MemcachedImpl::flushAll() {
	Connection* connection = pool->checkout();
	memcached_return_t reply = memcached_flush_buffers((memcached_st*)connection->getConn());
	pool->release(connection);
	return replyStatus(reply);
}

memcached_return_t MemcachedImpl::setInternal(const std::string& key, const std::string& value, const int& expireSeconds, const int& setOrAddOrRep) {
	Connection* connection = pool->checkout();
	memcached_return_t reply;
	if(setOrAddOrRep==1)
	{
		reply = memcached_set((memcached_st*)connection->getConn(),
				key.c_str(), key.length(), value.c_str(), value.length(), (time_t)expireSeconds, (uint32_t)0);
	}
	else if(setOrAddOrRep==2)
	{
		reply = memcached_add((memcached_st*)connection->getConn(),
				key.c_str(), key.length(), value.c_str(), value.length(), (time_t)expireSeconds, (uint32_t)0);
	}
	else if(setOrAddOrRep==3)
	{
		reply = memcached_replace((memcached_st*)connection->getConn(),
				key.c_str(), key.length(), value.c_str(), value.length(), (time_t)expireSeconds, (uint32_t)0);
	}
	pool->release(connection);
	return reply;
}

bool MemcachedImpl::set(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	memcached_return_t reply = setInternal(key, valueStr, expireSeconds, 1);
	return replyStatus(reply);
}

bool MemcachedImpl::add(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	memcached_return_t reply = setInternal(key, valueStr, expireSeconds, 2);
	return replyStatus(reply);
}

bool MemcachedImpl::replace(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	memcached_return_t reply = setInternal(key, valueStr, expireSeconds, 3);
	return replyStatus(reply);
}

std::string MemcachedImpl::getValue(const std::string& key) {
	Connection* connection = pool->checkout();
	size_t siz;
	uint32_t fl = (uint32_t)0;
	memcached_return_t reply;
	char* value = memcached_get((memcached_st*)connection->getConn(), key.c_str(), key.length(),
			&siz, &fl, &reply);
	std::string rval;
	if(replyStatus(reply)) {
		for (int var = 0; var < (int)siz; ++var) {
			rval.push_back(value[var]);
		}
	}
	free(value);
	pool->release(connection);
	return rval;
}

std::vector<std::string> MemcachedImpl::getValues(const std::vector<std::string>& skeys) {
	std::vector<std::string> rv;
	Connection* connection = pool->checkout();
	uint32_t fl = (uint32_t)0;
	int num = (int)skeys.size();

	char *keys[num];
	size_t key_length[num];
	for(int i=0;i<(int)skeys.size();++i) {
		keys[i] = (char*)skeys.at(i).c_str();
		key_length[i] = skeys.at(i).length();
	}
	char* value;
	char return_key[MEMCACHED_MAX_KEY];
	size_t return_key_length;
	size_t return_value_length;
	memcached_return_t reply;
	reply = memcached_mget((memcached_st*)connection->getConn(), keys, key_length, num);
	while ((value = memcached_fetch((memcached_st*)connection->getConn(), return_key, &return_key_length, &return_value_length, &fl, &reply)))
	{
		std::string rval;
		for (int var = 0; var < (int)return_value_length; ++var) {
			rval.push_back(value[var]);
		}
		rv.push_back(rval);
		free(value);
	}
	pool->release(connection);
	return rv;
}

bool MemcachedImpl::replyStatus(const memcached_return_t& reply) {
	return memcached_success(reply);
}

void* MemcachedImpl::executeCommand(const std::string& command, ...) {
	throw "Not Implemented";
}

void MemcachedImpl::init() {
}

MemcachedConnectionPool::MemcachedConnectionPool(const ConnectionProperties& props) {
	logger = LoggerFactory::getLogger("MemcachedConnectionPool");
	createPool(props);
}

MemcachedConnectionPool::~MemcachedConnectionPool() {
	destroyPool();
}

void* MemcachedConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	memcached_pool_st* pool = (memcached_pool_st*)getEnv();
	memcached_return_t rc;
	memcached_st *memc = memcached_pool_pop(pool, true, &rc);
	return memc;
}

void MemcachedConnectionPool::closeConnection(void* conn) {
	memcached_pool_st* pool = (memcached_pool_st*)getEnv();
	memcached_pool_push(pool, (memcached_st*)conn);
}

void MemcachedConnectionPool::initEnv() {
	std::string configStr;
	int numConns = 0;
	for (int var = 0; var < (int)getProperties().getNodes().size(); ++var) {
		configStr.append("--SERVER=");
		configStr.append(getProperties().getNodes().at(var).getHost());
		//configStr.append(":");
		//configStr.append(CastUtil::lexical_cast<std::string>(nodes.at(var).getPort()));
		configStr.append(" --CONNECT-TIMEOUT=");
		configStr.append(CastUtil::lexical_cast<std::string>((long)getProperties().getNodes().at(var).getConnectionTimeout()));
		configStr.append(" ");
	}

	numConns = getProperties().getPoolReadSize() + getProperties().getPoolWriteSize();
	if(numConns<=-1) {
		numConns = 5;
	}
	configStr.append("--POOL-MIN=1 --POOL-MAX=");
	configStr.append(CastUtil::lexical_cast<std::string>(numConns));

	memcached_pool_st* pool = memcached_pool(configStr.c_str(), configStr.length());
	if(pool==NULL) {
		throw "Unable to create memcached connection pool";
	}
	getProps().setNewConnectionStrategy(true);
	setEnv(pool);
}

void MemcachedConnectionPool::destroy() {
	memcached_pool_st* pool = (memcached_pool_st*)getEnv();
	memcached_pool_destroy(pool);
}

