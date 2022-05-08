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
#ifdef HAVE_REDIS_CLUSTERINC
#include "RedisClusterCacheImpl.h"
#endif
#if HAVE_REDISINC
#include "RedisCacheImpl.h"
#endif
#include "GenericObject.h"

class CacheManager {
	static bool isSinglEVH;
	static std::map<std::string, CacheInterface*> sevhCchImpls;
	Logger logger;
	static std::map<std::string, CacheManager*> caches;
	static std::map<std::string, std::string> defDsnNames;
	static std::map<std::string, bool> appInitCompletionStatus;
	ConnectionProperties props;
	ConnectionPooler* pool;
	static void initCache(const ConnectionProperties& props, const std::string& appName, GetClassBeanIns f);
	static void destroy();
	CacheManager(const ConnectionProperties& props);
	friend class ConfigurationHandler;
public:
	static void init(bool isSinglEVH);
	CacheManager();
	virtual ~CacheManager();
	static CacheInterface* getImpl(std::string name = "", std::string appName = "");
	static void cleanImpl(CacheInterface*);
	static void triggerAppInitCompletion(std::string appName = "");
	static bool isInitCompleted();
};

#endif /* CACHEMANAGER_H_ */
