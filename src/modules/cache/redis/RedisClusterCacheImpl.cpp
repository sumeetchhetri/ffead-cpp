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
 * RedisClusterCacheImpl.cpp
 *
 *  Created on: 07-May-2014
 *      Author: sumeetc
 */

#include "RedisClusterCacheImpl.h"

#ifdef HAVE_REDIS_CLUSTERINC

RedisClusterCacheImpl::RedisClusterCacheImpl(ConnectionPooler* pool) {
	this->pool = pool;
	this->properties = pool->getProperties();
	this->defaultExpireSeconds = -1;
	if(properties.getProperty("expiryTime")!="") {
		try {
			this->defaultExpireSeconds = CastUtil::toInt(properties.getProperty("expiryTime"));
		} catch(const std::exception& e) {
		}
	}
	if(this->defaultExpireSeconds<=0) {
		this->defaultExpireSeconds = 3600;
	}
}

RedisClusterCacheImpl::~RedisClusterCacheImpl() {
}

bool RedisClusterCacheImpl::remove(const std::string& key) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	return r->del(key)>0;
}

long long RedisClusterCacheImpl::increment(const std::string& key, const int& number) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	return r->incrby(key, number);
}

long long RedisClusterCacheImpl::decrement(const std::string& key, const int& number) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	return r->decrby(key, number);
}

long double RedisClusterCacheImpl::incrementFloat(const std::string& key, const double& number) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	return r->incrbyfloat(key, number);
}

long double RedisClusterCacheImpl::decrementFloat(const std::string& key, const double& number) {
	return incrementFloat(key, -number);
}

std::map<std::string, std::string> RedisClusterCacheImpl::statistics() {
	std::map<std::string, std::string> stats;
	return stats;
}

bool RedisClusterCacheImpl::flushAll() {
	return false;
}

bool RedisClusterCacheImpl::set(const std::string& key, GenericObject& value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	std::string valueStr = value.getSerilaizedState();
	r->set(key, valueStr, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::ALWAYS);
	return true;
}

bool RedisClusterCacheImpl::add(const std::string& key, GenericObject& value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	std::string valueStr = value.getSerilaizedState();
	r->set(key, valueStr, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::NOT_EXIST);
	return true;
}

bool RedisClusterCacheImpl::replace(const std::string& key, GenericObject& value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	std::string valueStr = value.getSerilaizedState();
	r->set(key, valueStr, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::EXIST);
	return true;
}

bool RedisClusterCacheImpl::setRaw(const std::string& key, const char* value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	r->set(key, value, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::ALWAYS);
	return true;
}

bool RedisClusterCacheImpl::addRaw(const std::string& key, const char* value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	r->set(key, value, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::NOT_EXIST);
	return true;
}

bool RedisClusterCacheImpl::replaceRaw(const std::string& key, const char* value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	r->set(key, value, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::EXIST);
	return true;
}

std::string RedisClusterCacheImpl::getValue(const std::string& key) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	std::optional<std::string> os = r->get(key);
	return os.has_value()?os.value():"";
}

std::vector<std::string> RedisClusterCacheImpl::getValues(const std::vector<std::string>& keys) {
	std::vector<std::string> rv;
	mgetRaw(keys, rv);
	return rv;
}

bool RedisClusterCacheImpl::setRaw(const unsigned long long& key, const std::string_view& value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	r->set(std::to_string(key), value, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::ALWAYS);
	return true;
}
bool RedisClusterCacheImpl::addRaw(const unsigned long long& key, const std::string_view& value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	r->set(std::to_string(key), value, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::NOT_EXIST);
	return true;
}
bool RedisClusterCacheImpl::replaceRaw(const unsigned long long& key, const std::string_view& value, int expireSeconds) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	r->set(std::to_string(key), value, std::chrono::milliseconds((expireSeconds<=0?defaultExpireSeconds:expireSeconds)*1000), sw::redis::UpdateType::EXIST);
	return true;
}
std::string RedisClusterCacheImpl::getValue(const unsigned long long& key) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	std::optional<std::string> os = r->get(std::to_string(key));
	return os.has_value()?os.value():"";
}
void RedisClusterCacheImpl::getValues(const std::vector<unsigned long long>& keys, std::vector<std::string>& rvalues) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();

	std::vector<std::string> skeys;
	for(int i=0;i<(int)keys.size();++i) {
		skeys.push_back(std::to_string(keys.at(i)));
	}
	std::vector<std::optional<std::string>> values;
	r->mget(skeys.begin(), skeys.end(), std::back_inserter(values));

	for (const auto &val : values) {
	    if (val) {
	    	rvalues.push_back(val.value());
	    } else {
	    	rvalues.push_back("");
	    }
	}
}
bool RedisClusterCacheImpl::remove(const unsigned long long& key) {
	return remove(std::to_string(key));
}

void RedisClusterCacheImpl::mgetRaw(const std::vector<std::string>& skeys, std::vector<std::string>& rvalues) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();

	std::vector<std::optional<std::string>> values;
	r->mget(skeys.begin(), skeys.end(), std::back_inserter(values));

	for (const auto &val : values) {
	    if (val) {
	    	rvalues.push_back(val.value());
	    } else {
	    	rvalues.push_back("");
	    }
	}
}

void* RedisClusterCacheImpl::executeCommand(const std::string command, ...) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	r->command<std::optional<std::string>>(command, "");
	return NULL;
}

bool RedisClusterCacheImpl::addToQ(const std::string& qname, const std::string& value) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	return r->lpush(qname, value)>0;
}

std::string RedisClusterCacheImpl::getFromQ(const std::string& qname) {
	Connection* connection = pool->checkout();
	sw::redis::RedisCluster* r = (sw::redis::RedisCluster*)connection->getConn();
	std::optional<std::string> os = r->rpop(qname);
	return os.has_value()?os.value():"";
}

void RedisClusterCacheImpl::init() {
}

RedisClusterCacheConnectionPool::RedisClusterCacheConnectionPool(const ConnectionProperties& props) {
	r = NULL;
}

RedisClusterCacheConnectionPool::~RedisClusterCacheConnectionPool() {
	destroyPool();
}

void* RedisClusterCacheConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	return r;
}
void RedisClusterCacheConnectionPool::closeConnection(void* conn) {
}

void RedisClusterCacheConnectionPool::initEnv() {
	getProps().setNewConnectionStrategy(true);
	sw::redis::ConnectionOptions connection_options;
	connection_options.host = getProperties().getNodes().at(0).getHost();  // Required.
	connection_options.port = getProperties().getNodes().at(0).getPort(); // Optional. The default port is 6379.
	//connection_options.password = "auth";   // Optional. No password by default.
	//connection_options.db = 1;  // Optional. Use the 0th database by default.

	// Optional. Timeout before we successfully send request to or receive response from redis.
	// By default, the timeout is 0ms, i.e. never timeout and block until we send or receive successfuly.
	// NOTE: if any command is timed out, we throw a TimeoutError exception.
	//connection_options.socket_timeout = getProperties().getNodes().at(0).getConnectionTimeout();
	connection_options.connect_timeout = std::chrono::milliseconds((int)getProperties().getNodes().at(0).getConnectionTimeout());

	int numConns = getProperties().getPoolReadSize() + getProperties().getPoolWriteSize();
	sw::redis::ConnectionPoolOptions pool_options;
	pool_options.size = numConns;

	r = new sw::redis::RedisCluster(connection_options, pool_options);
}

void RedisClusterCacheConnectionPool::destroy() {
	delete r;
}
#endif
