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
 * RedisClusterCacheImpl.h
 *
 *  Created on: 07-May-2014
 *      Author: sumeetc
 */

#ifndef REDISCLUSTERCACHEIMPL_H_
#define REDISCLUSTERCACHEIMPL_H_
#include "CacheInterface.h"
#include "ConnectionPooler.h"
#include <chrono>
#include <optional>
#ifdef HAVE_REDIS_CLUSTERINC
#include <sw/redis++/redis++.h>

class RedisClusterCacheConnectionPool: public ConnectionPooler {
	void initEnv();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
	void destroy();
	sw::redis::RedisCluster* r;
public:
	RedisClusterCacheConnectionPool(const ConnectionProperties& props);
	virtual ~RedisClusterCacheConnectionPool();
};

class RedisClusterCacheImpl: public CacheInterface {
	ConnectionProperties properties;
public:
	RedisClusterCacheImpl(ConnectionPooler* pool);
	~RedisClusterCacheImpl();
	void init();

	bool setRaw(const unsigned long long& key, const std::string_view& value, int expireSeconds = -1);
	bool addRaw(const unsigned long long& key, const std::string_view& value, int expireSeconds = -1);
	bool replaceRaw(const unsigned long long& key, const std::string_view& value, int expireSeconds = -1);
	std::string getValue(const unsigned long long& key);
	void getValues(const std::vector<unsigned long long>& keys, std::vector<std::string>& values);
	bool remove(const unsigned long long& key);

	bool set(const std::string& key, GenericObject& value, int expireSeconds);
	bool add(const std::string& key, GenericObject& value, int expireSeconds);
	bool replace(const std::string& key, GenericObject& value, int expireSeconds);

	bool setRaw(const std::string& key, const char* value, int expireSeconds = -1);
	bool addRaw(const std::string& key, const char* value, int expireSeconds = -1);
	bool replaceRaw(const std::string& key, const char* value, int expireSeconds = -1);
	void mgetRaw(const std::vector<std::string>& keys, std::vector<std::string>& values);

	std::string getValue(const std::string& key);
	std::vector<std::string> getValues(const std::vector<std::string>& keys);

	bool remove(const std::string& key);
	long long increment(const std::string& key, const int& number= 1);
	long long decrement(const std::string& key, const int& number= 1);
	long double incrementFloat(const std::string& key, const double& number = 1.0);
	long double decrementFloat(const std::string& key, const double& number = 1.0);
	std::map<std::string, std::string> statistics();
	bool flushAll();

	void* executeCommand(const std::string command, ...);
	bool addToQ(const std::string& qname, const std::string& value);
	std::string getFromQ(const std::string& qname);
};
#endif
#endif /* REDISCLUSTERCACHEIMPL_H_ */
