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
 * ConnectionPooler.cpp
 *
 *  Created on: 08-May-2014
 *      Author: sumeetc
 */

#include "ConnectionPooler.h"

ConnectionPooler::ConnectionPooler() {
	this->initialized = false;
	this->_env = NULL;
}

ConnectionPooler::~ConnectionPooler() {
}

Connection* ConnectionPooler::checkoutInternal() {
	int counter = 0;
	Connection* conn = NULL;
	do {
		if(!connections.try_dequeue(conn)) {
			counter++;
			if(conn==NULL) {
				usleep(1*1000);
			}
		}
	} while(counter<5 && conn==NULL);
	if(conn==NULL) {
		std::vector<ConnectionNode> nodes = properties.getNodes();
		void* c = this->newConnection(true, nodes.at(0));
		Connection* _c = new Connection();
		_c->setConn(c);
		_c->setNode(nodes.at(0));
		_c->setBusy(false);
		_c->setType(true);
		return _c;
	}
	return conn;
}

Connection* ConnectionPooler::checkout() {
	if(getProperties().isNewConnectionStrategy()) {
		std::vector<ConnectionNode> nodes = properties.getNodes();
		void* c = this->newConnection(true, nodes.at(0));
		if(c!=NULL) {
			Connection* connection = new Connection;
			connection->setConn(c);
			connection->setBusy(false);
			connection->setType(true);
			connection->setNode(nodes.at(0));
			return connection;
		}
		return NULL;
	} else {
		return checkoutInternal();
	}
}

void ConnectionPooler::release(Connection* _c) {
	if(getProperties().isNewConnectionStrategy()) {
		closeConnection(_c->getConn());
		delete _c;
		return;
	} else {
		this->connections.enqueue(_c);
	}
}

void ConnectionPooler::destroyPool() {
	if(!getProperties().isNewConnectionStrategy())
	{
		Connection* _c;
		while(connections.try_dequeue(_c))
		{
			closeConnection(_c);
			delete _c;
		}
	}
	destroy();
}

bool ConnectionPooler::isInitialized() const {
	return initialized;
}

void ConnectionPooler::createPool(const ConnectionProperties& properties) {
	this->properties = properties;

	initEnv();

	std::vector<ConnectionNode> nodes = properties.getNodes();

	if(getProperties().isNewConnectionStrategy())return;

	int nodeIndex = 0;
	for(int i=0;i<properties.getPoolWriteSize();i++)
	{
		if(nodeIndex>=(int)nodes.size())
			nodeIndex = 0;
		ConnectionNode node = nodes.at(nodeIndex);
		void* conn = this->newConnection(true, node);
		if(conn!=NULL) {
			Connection* _c = new Connection();
			_c->setConn(conn);
			_c->setNode(node);
			_c->setBusy(false);
			_c->setType(true);
			this->connections.enqueue(_c);
			this->initialized = true;
		} else if((int)nodes.size()==1) {
			break;
		}
	}
}

ConnectionProperties& ConnectionPooler::getProps() {
	return properties;
}

const ConnectionProperties& ConnectionPooler::getProperties() const {
	return properties;
}

void* ConnectionPooler::getEnv() {
	return _env;
}

void ConnectionPooler::setEnv(void* env) {
	_env = env;
}
