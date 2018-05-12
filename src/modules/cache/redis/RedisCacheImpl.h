/*
 * RedisCacheImpl.h
 *
 *  Created on: 07-May-2014
 *      Author: sumeetc
 */

#ifndef REDISCACHEIMPL_H_
#define REDISCACHEIMPL_H_
#include <hiredis/hiredis.h>
#include "CacheInterface.h"
#include "ConnectionPooler.h"

class RedisCacheConnectionPool: public ConnectionPooler {
	Logger logger;
	void initEnv();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
	void destroy();
public:
	RedisCacheConnectionPool(const ConnectionProperties& props);
	virtual ~RedisCacheConnectionPool();
};

class RedisCacheImpl: public CacheInterface {
	redisReply* execute(const char* format, ...);
	bool replyStatus(redisReply* reply);
	std::string replyValue(redisReply* reply);
	ConnectionProperties properties;
public:
	RedisCacheImpl(ConnectionPooler* pool);
	~RedisCacheImpl();
	void init();

	bool set(const std::string& key, GenericObject& value, int expireSeconds);
	bool add(const std::string& key, GenericObject& value, int expireSeconds);
	bool replace(const std::string& key, GenericObject& value, int expireSeconds);

	std::string getValue(const std::string& key);
	std::vector<std::string> getValues(const std::vector<std::string>& keys);

	bool remove(const std::string& key);
	long long increment(const std::string& key, const int& number= 1);
	long long decrement(const std::string& key, const int& number= 1);
	long double incrementFloat(const std::string& key, const double& number = 1.0);
	long double decrementFloat(const std::string& key, const double& number = 1.0);
	std::map<std::string, std::string> statistics();
	bool flushAll();

	void* executeCommand(const std::string& command, ...);
};

#endif /* REDISCACHEIMPL_H_ */
