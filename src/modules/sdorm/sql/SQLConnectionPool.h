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
 * SQLConnectionPool.h
 *
 *  Created on: 14-May-2014
 *      Author: sumeetc
 */

#ifndef SQLCONNECTIONPOOL_H_
#define SQLCONNECTIONPOOL_H_
#include "ConnectionPooler.h"
#ifdef HAVE_LIBODBC
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#endif
#include "LoggerFactory.h"

class SQLConnectionPool: public ConnectionPooler {
	Logger logger;
	void initEnv();
	void* newConnection(const bool& isWrite, const ConnectionNode& node);
	void closeConnection(void* conn);
	void destroy();
public:
	SQLConnectionPool(const ConnectionProperties& props);
	virtual ~SQLConnectionPool();
};

#endif /* SQLCONNECTIONPOOL_H_ */
