/*
	Copyright 2009-2013, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
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
 * PooledDistoCacheConnectionFactory.cpp
 *
 *  Created on: 04-Apr-2013
 *      Author: sumeetc
 */

#include "PooledDistoCacheConnectionFactory.h"

PooledDistoCacheConnectionFactory* PooledDistoCacheConnectionFactory::instance = NULL;

PooledDistoCacheConnectionFactory::PooledDistoCacheConnectionFactory(string host, int port, int poolSize, bool isSSL) {
	this->host = host;
	this->port = port;
	this->isSSL = isSSL;
	this->poolSize = poolSize;
}

PooledDistoCacheConnectionFactory::~PooledDistoCacheConnectionFactory() {
}

void PooledDistoCacheConnectionFactory::init(string host, int port, int poolSize, bool isSSL) {
	if(instance==NULL)
	{
		instance = new PooledDistoCacheConnectionFactory(host, port, poolSize, isSSL);
		instance->createPool();
	}
}

void PooledDistoCacheConnectionFactory::createPool() {
	if(instance!=NULL)
	{
		for (int var = 0; var < instance->poolSize; ++var) {
			DistoCacheClientUtils* clientUtil = new DistoCacheClientUtils(instance->host, instance->port, instance->isSSL);
			instance->pool.push_back(clientUtil);
		}
	}
}

void PooledDistoCacheConnectionFactory::destroy() {
	if(instance!=NULL)
	{
		for (int var = 0; var < instance->poolSize; ++var) {
			DistoCacheClientUtils* clientUtil = instance->pool.at(var);
			delete clientUtil;
		}
		delete instance;
	}
}

DistoCacheClientUtils* PooledDistoCacheConnectionFactory::getConnection() {
	if(instance!=NULL)
	{
		for (int var = 0; var < instance->poolSize; ++var) {
			if(!instance->pool.at(var)->inUse)
				return instance->pool.at(var);
		}
		//No issues with synchronization as the DistoCacheClientUtils class is thread-safe
		return instance->pool.at(0);
	}
	throw "PooledDistoCacheConnectionFactory not initialized";
}

void PooledDistoCacheConnectionFactory::releaseConnection(DistoCacheClientUtils* clientUtil) {
	clientUtil->lock.lock();
	clientUtil->inUse = false;
	clientUtil->lock.unlock();
}
