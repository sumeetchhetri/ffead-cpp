/*
 * MemoryCacheImpl.h
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */

#ifndef MEMORYCACHEIMPL_H_
#define MEMORYCACHEIMPL_H_
#include "CacheInterface.h"
#include "ConnectionPooler.h"

class MemoryCacheImpl : public CacheInterface, public ConnectionPooler {
	std::map<std::string, std::string> internalMap;
	ConnectionProperties properties;
	bool setInternal(const std::string& key, const std::string& value, const int& expireSeconds, const int& setOrAddOrRep);
public:
	MemoryCacheImpl(const ConnectionProperties& properties);
	~MemoryCacheImpl();
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

#endif /* MEMORYCACHEIMPL_H_ */
