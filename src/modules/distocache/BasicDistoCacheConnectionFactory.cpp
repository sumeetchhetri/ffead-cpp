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
 * BasicDistoCacheConnectionFactory.cpp
 *
 *  Created on: 04-Apr-2013
 *      Author: sumeetc
 */

#include "BasicDistoCacheConnectionFactory.h"

BasicDistoCacheConnectionFactory* BasicDistoCacheConnectionFactory::instance = NULL;

BasicDistoCacheConnectionFactory::BasicDistoCacheConnectionFactory(string host, int port, bool isSSL) {
	this->host = host;
	this->port = port;
	this->isSSL = isSSL;
}

BasicDistoCacheConnectionFactory::~BasicDistoCacheConnectionFactory() {
}

void BasicDistoCacheConnectionFactory::init(string host, int port, bool isSSL) {
	if(instance==NULL)
	{
		instance = new BasicDistoCacheConnectionFactory(host, port, isSSL);
	}
}


void BasicDistoCacheConnectionFactory::destroy() {
	if(instance!=NULL)
	{
		delete instance;
	}
}

DistoCacheClientUtils* BasicDistoCacheConnectionFactory::getConnection() {
	if(instance!=NULL)
	{
		return new DistoCacheClientUtils(instance->host, instance->port, instance->isSSL);;
	}
	throw "BasicDistoCacheConnectionFactory not initialized";
}

void BasicDistoCacheConnectionFactory::releaseConnection(DistoCacheClientUtils* clientUtil) {
	delete clientUtil;
}
