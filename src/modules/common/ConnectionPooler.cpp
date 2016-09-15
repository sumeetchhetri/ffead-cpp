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
	// TODO Auto-generated destructor stub
}

Connection* ConnectionPooler::checkoutInternal(connVec& conns, const bool& isWrite) {
	int counter = 0;
	Connection* conn = NULL;
	do {
		mutex.lock();
		for(unsigned int i=0;i<conns.size();i++)
		{
			if(!conns.at(i)->isBusy())
			{
				conn =  conns.at(i);
				conn->setBusy(true);
				break;
			}
		}
		mutex.unlock();
		counter++;
		if(conn==NULL) {
			usleep(1*1000);
		}
	} while(counter<5 && conn==NULL);
	if(conn==NULL) {
		std::vector<ConnectionNode> nodes = properties.getNodes();
		void* c = this->newConnection(true, nodes.at(0));
		conn = new Connection;
		conn->setConn(c);
		conn->setBusy(true);
		conn->setType(isWrite);
		mutex.lock();
		conns.push_back(conn);
		mutex.unlock();
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
		return checkoutInternal(this->writeConnections, true);
	}
}

Connection* ConnectionPooler::checkout(const bool& isWrite) {
	return checkoutInternal(this->writeConnections, isWrite);
}

void ConnectionPooler::release(Connection* conn) {
	if(getProperties().isNewConnectionStrategy()) {
		closeConnection(conn->getConn());
		delete conn;
		return;
	}
	mutex.lock();
	conn->setBusy(false);
	mutex.unlock();
}

void ConnectionPooler::destroyPool() {
	if(!getProperties().isNewConnectionStrategy())
	{
		for(int i=0;i<properties.getPoolWriteSize();i++)
		{
			Connection* connection = this->writeConnections.at(i);
			closeConnection(connection->getConn());
			delete connection;
		}
		for(int i=0;i<properties.getPoolReadSize();i++)
		{
			Connection* connection = this->readConnections.at(i);
			closeConnection(connection->getConn());
			delete connection;
		}
	}
	destroy();
}

const connVec& ConnectionPooler::getWriteConnections() const {
	return writeConnections;
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
			Connection* connection = new Connection;
			connection->setConn(conn);
			connection->setNode(node);
			connection->setBusy(false);
			connection->setType(true);
			this->writeConnections.push_back(connection);
			this->initialized = true;
		} else if((int)nodes.size()==1) {
			break;
		}
	}
	nodeIndex = 0;
	for(int i=0;i<properties.getPoolReadSize();i++)
	{
		if(nodeIndex>=(int)nodes.size())
			nodeIndex = 0;
		ConnectionNode node = nodes.at(nodeIndex);
		void* conn = this->newConnection(false, node);
		if(conn!=NULL) {
			Connection* connection = new Connection;
			connection->setConn(conn);
			connection->setNode(node);
			connection->setBusy(false);
			connection->setType(false);
			this->readConnections.push_back(connection);
			this->initialized = true;
		} else if((int)nodes.size()==1) {
			break;
		}
	}
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
