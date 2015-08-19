/*
 * MemcachedImpl.h
 *
 *  Created on: 09-May-2014
 *      Author: sumeetc
 */

#ifndef MEMCACHEDIMPL_H_
#define MEMCACHEDIMPL_H_
#include <libmemcached/memcached.h>
#include <libmemcached/util.h>
#include "CacheInterface.h"
#include "ConnectionPooler.h"

class MemcachedImpl : public CacheInterface, public ConnectionPooler {
	memcached_return_t setInternal(const string& key, const string& value, const int& expireSeconds, const int& setOrAddOrRep);
	bool replyStatus(const memcached_return_t& reply);
	string replyValue(const memcached_return_t& reply);
	ConnectionProperties properties;
public:
	MemcachedImpl(const ConnectionProperties& properties);
	~MemcachedImpl();
	void init();

	bool set(const string& key, GenericObject& value, const int& expireSeconds);
	bool add(const string& key, GenericObject& value, const int& expireSeconds);
	bool replace(const string& key, GenericObject& value, const int& expireSeconds);

	string getValue(const string& key);

	bool remove(const string& key);
	long long increment(const string& key, const int& number= 1);
	long long decrement(const string& key, const int& number= 1);
	long double incrementFloat(const string& key, const double& number = 1.0);
	long double decrementFloat(const string& key, const double& number = 1.0);
	map<string, string> statistics();
	bool flushAll();

	void* executeCommand(const string& command, ...);

	void initEnv();
	void destroy();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
};

#endif /* MEMCACHEDIMPL_H_ */
