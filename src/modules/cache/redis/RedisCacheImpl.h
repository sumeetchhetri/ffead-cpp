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

class RedisCacheImpl: public CacheInterface, public ConnectionPooler {
	redisReply* execute(const char* format, ...);
	bool replyStatus(redisReply* reply);
	std::string replyValue(redisReply* reply);
	ConnectionProperties properties;
public:
	RedisCacheImpl(const ConnectionProperties& properties);
	~RedisCacheImpl();
	void init();

	bool set(const std::string& key, GenericObject& value, const int& expireSeconds);
	bool add(const std::string& key, GenericObject& value, const int& expireSeconds);
	bool replace(const std::string& key, GenericObject& value, const int& expireSeconds);

	std::string getValue(const std::string& key);

	bool remove(const std::string& key);
	long long increment(const std::string& key, const int& number= 1);
	long long decrement(const std::string& key, const int& number= 1);
	long double incrementFloat(const std::string& key, const double& number = 1.0);
	long double decrementFloat(const std::string& key, const double& number = 1.0);
	std::map<std::string, std::string> statistics();
	bool flushAll();

	void* executeCommand(const std::string& command, ...);

	void initEnv();
	void destroy();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
};

#endif /* REDISCACHEIMPL_H_ */
