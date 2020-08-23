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
 * ConnectionPooler.h
 *
 *  Created on: 08-May-2014
 *      Author: sumeetc
 */

#ifndef CONNECTIONPOOLER_H_
#define CONNECTIONPOOLER_H_
#include "Compatibility.h"
#include <time.h>
#include "Connection.h"
#include "Mutex.h"
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "concurrentqueue.h"

class ConnectionPooler {
	moodycamel::ConcurrentQueue<Connection*> connections;
	bool initialized;
	void* _env;
	ConnectionProperties properties;
	Connection* checkoutInternal();
protected:
	void setProperties(const ConnectionProperties& properties);
	void destroyPool();
	void createPool(const ConnectionProperties& properties);
	virtual void initEnv()=0;
	virtual void* newConnection(const bool& isWrite, const ConnectionNode& node)=0;
	virtual void closeConnection(void* conn)=0;
	virtual void destroy()=0;
	void* getEnv();
	void setEnv(void* env);
	ConnectionProperties& getProps();
public:
	ConnectionPooler();
	virtual ~ConnectionPooler();
	Connection* checkout();
	void release(Connection* conn);
	bool isInitialized() const;
	const ConnectionProperties& getProperties() const;
};

#endif /* CONNECTIONPOOLER_H_ */
