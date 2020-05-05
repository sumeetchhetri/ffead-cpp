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
 * DataSourceManager.h
 *
 *  Created on: 14-May-2014
 *      Author: sumeetc
 */

#ifndef DATASOURCEMANAGER_H_
#define DATASOURCEMANAGER_H_

#include "DataSourceInterface.h"
#ifdef INC_SDORM_SQL
#include "SQLConnectionPool.h"
#include "SQLDataSourceImpl.h"
#endif
#ifdef INC_SDORM_MONGO
#include "MongoDBConnectionPool.h"
#include "MongoDBDataSourceImpl.h"
#endif

class DataSourceManager;

class DataSourceManager {
	Logger logger;
	static std::map<std::string, DataSourceManager*> dsns;
	static std::map<std::string, std::string> defDsnNames;
	ConnectionProperties props;
	Mapping mapping;
	ConnectionPooler* pool;
	static void initDSN(const ConnectionProperties& props, const Mapping& mapping);
	DataSourceManager(const ConnectionProperties& props, const Mapping& mapping);
	static void destroy();
	friend class ConfigurationHandler;
public:
	virtual ~DataSourceManager();
	static DataSourceInterface* getImpl(std::string name = "", std::string appName = "");
};

#endif /* DATASOURCEMANAGER_H_ */
