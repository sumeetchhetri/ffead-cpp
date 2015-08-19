/*
 * MemcachedImpl.cpp
 *
 *  Created on: 09-May-2014
 *      Author: sumeetc
 */

#include "MemcachedImpl.h"

MemcachedImpl::MemcachedImpl(const ConnectionProperties& properties) {
	this->properties = properties;
}

MemcachedImpl::~MemcachedImpl() {
	destroyPool();
}

void MemcachedImpl::initEnv() {
	string configStr;
	int numConns = 0;
	for (int var = 0; var < (int)properties.getNodes().size(); ++var) {
		configStr.append("--SERVER=");
		configStr.append(properties.getNodes().at(var).getHost());
		//configStr.append(":");
		//configStr.append(CastUtil::lexical_cast<string>(nodes.at(var).getPort()));
		configStr.append(" ");
	}

	numConns = properties.getPoolReadSize() + properties.getPoolWriteSize();
	configStr.append("--POOL-MIN=1 --POOL-MAX=");
	configStr.append(CastUtil::lexical_cast<string>(numConns));

	memcached_pool_st* pool = memcached_pool(configStr.c_str(), configStr.length());
	properties.setNewConnectionStrategy(true);
	setEnv(pool);
}

void MemcachedImpl::destroy() {
	memcached_pool_st* pool = (memcached_pool_st*)getEnv();
	memcached_pool_destroy(pool);
}

bool MemcachedImpl::remove(const string& key) {
	Connection* connection = checkout();
	memcached_return_t reply = memcached_delete((memcached_st*)connection->getConn(),
			key.c_str(), key.length(), (time_t)0);
	release(connection);
	return replyStatus(reply);
}

long long MemcachedImpl::increment(const string& key, const int& number) {
	Connection* connection = checkout();
	uint64_t value;
	memcached_increment((memcached_st*)connection->getConn(),
			key.c_str(), key.length(), (uint32_t)number, &value);
	release(connection);
	return value;
}

long long MemcachedImpl::decrement(const string& key, const int& number) {
	Connection* connection = checkout();
	uint64_t value;
	memcached_decrement((memcached_st*)connection->getConn(),
			key.c_str(), key.length(), (uint32_t)number, &value);
	release(connection);
	return value;
}

long double MemcachedImpl::incrementFloat(const string& key, const double& number) {
	throw "Not Implemented";
}

long double MemcachedImpl::decrementFloat(const string& key, const double& number) {
	throw "Not Implemented";
}

map<string, string> MemcachedImpl::statistics() {
	Connection* connection = checkout();
	memcached_return_t reply;
	memcached_stat_st* stat = memcached_stat((memcached_st*)connection->getConn(), NULL, &reply);
	char** keys = memcached_stat_get_keys((memcached_st*)connection->getConn(), stat, &reply);
	map<string, string> stats;
	if(replyStatus(reply)) {
		char **ptr;
		for (ptr= keys; *ptr; ptr++) {
			char* valuec = memcached_stat_get_value((memcached_st*)connection->getConn(), stat, *ptr, &reply);
			stats[string(*ptr)] = string(valuec);
		}
		free(keys);
	}
	memcached_stat_free(NULL, stat);
	release(connection);
	return stats;
}

bool MemcachedImpl::flushAll() {
	Connection* connection = checkout();
	memcached_return_t reply = memcached_flush_buffers((memcached_st*)connection->getConn());
	release(connection);
	return replyStatus(reply);
}

void* MemcachedImpl::newConnection(const bool& isWrite, const ConnectionNode& node) {
	memcached_pool_st* pool = (memcached_pool_st*)getEnv();
	memcached_return_t rc;
	memcached_st *memc = memcached_pool_pop(pool, true, &rc);
	return memc;
}

memcached_return_t MemcachedImpl::setInternal(const string& key, const string& value, const int& expireSeconds, const int& setOrAddOrRep) {
	Connection* connection = checkout();
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
	release(connection);
	return reply;
}

bool MemcachedImpl::set(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	memcached_return_t reply = setInternal(key, valueStr, expireSeconds, 1);
	return replyStatus(reply);
}

bool MemcachedImpl::add(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	memcached_return_t reply = setInternal(key, valueStr, expireSeconds, 2);
	return replyStatus(reply);
}

bool MemcachedImpl::replace(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	memcached_return_t reply = setInternal(key, valueStr, expireSeconds, 3);
	return replyStatus(reply);
}

string MemcachedImpl::getValue(const string& key) {
	Connection* connection = checkout();
	size_t siz;
	uint32_t fl = (uint32_t)0;
	memcached_return_t reply;
	char* value = memcached_get((memcached_st*)connection->getConn(), key.c_str(), key.length(),
			&siz, &fl, &reply);
	string rval;
	if(replyStatus(reply)) {
		for (int var = 0; var < (int)siz; ++var) {
			rval.push_back(value[var]);
		}
	}
	free(value);
	release(connection);
	return rval;
}

bool MemcachedImpl::replyStatus(const memcached_return_t& reply) {
	return memcached_success(reply);
}

void* MemcachedImpl::executeCommand(const string& command, ...) {
	throw "Not Implemented";
}

void MemcachedImpl::init() {
	createPool(properties);
}

void MemcachedImpl::closeConnection(void* conn) {
	memcached_pool_st* pool = (memcached_pool_st*)getEnv();
	memcached_pool_push(pool, (memcached_st*)conn);
}
