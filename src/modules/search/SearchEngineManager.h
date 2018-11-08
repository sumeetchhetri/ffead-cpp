/*
 * SearchEngineManager.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef SEARCHENGINEMANAGER_H_
#define SEARCHENGINEMANAGER_H_
#include "string"
#include "map"
#include "CacheInterface.h"
#include "ConnectionPooler.h"
#include "Mutex.h"
#include "SearchEngineInterface.h"
#include "HttpClient.h"

class SearchEngineConnectionPool: public ConnectionPooler {
	Logger logger;
	void initEnv();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
	void destroy();
public:
	SearchEngineConnectionPool(const ConnectionProperties& props);
	virtual ~SearchEngineConnectionPool();
};

class SearchEngineManager {
	static std::map<std::string, SearchEngineManager*> engines;
	static std::string defEngineName;
	ConnectionProperties props;
	ConnectionPooler* pool;
	Reflector* reflector;
	static void initCache(const ConnectionProperties& props, const std::string& appName);
	static void destroy();
	SearchEngineManager(const ConnectionProperties& props);
	friend class ConfigurationHandler;
public:
	SearchEngineManager(const ConnectionProperties& props);
	virtual ~SearchEngineManager();
	static SearchEngineInterface* getImpl(std::string name = "");
};

#endif /* SEARCHENGINEMANAGER_H_ */
