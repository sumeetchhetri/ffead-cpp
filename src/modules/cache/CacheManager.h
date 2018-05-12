/*
 * CacheManager.h
 *
 *  Created on: 12-Oct-2014
 *      Author: sumeetc
 */

#ifndef CACHEMANAGER_H_
#define CACHEMANAGER_H_
#include "MemoryCacheImpl.h"
#ifdef INC_MEMCACHED
#include"MemcachedImpl.h"
#endif
#ifdef INC_REDISCACHE
#include "RedisCacheImpl.h"
#endif

class CacheManager {
	static std::map<std::string, CacheManager*> caches;
	static std::string defDsnName;
	ConnectionProperties props;
	ConnectionPooler* pool;
	Reflector* reflector;
	static void initCache(const ConnectionProperties& props, const std::string& appName);
	static void destroy();
	CacheManager(const ConnectionProperties& props);
	friend class ConfigurationHandler;
public:
	CacheManager();
	virtual ~CacheManager();
	static CacheInterface* getImpl(std::string name = "");
};

#endif /* CACHEMANAGER_H_ */
