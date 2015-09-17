/*
 * Connection.cpp
 *
 *  Created on: 08-May-2014
 *      Author: sumeetc
 */

#include "Connection.h"

string ConnectionProperties::BLANK = "";

void* Connection::getConn() {
	return _conn;
}

void Connection::setConn(void* conn) {
	_conn = conn;
}

bool Connection::isBusy() const {
	return busy;
}

void Connection::setBusy(const bool& busy) {
	this->busy = busy;
}

Connection::Connection() {
	this->_conn = NULL;
	this->busy = false;
	this->type = false;
}

Connection::~Connection() {
}

bool Connection::isType() const {
	return type;
}

const ConnectionNode& Connection::getNode() const {
	return node;
}

void Connection::setNode(const ConnectionNode& node) {
	this->node = node;
}

void Connection::setType(const bool& type) {
	this->type = type;
}

float ConnectionNode::getConnectionTimeout() const {
	return connectionTimeout;
}

void ConnectionNode::setConnectionTimeout(const float& connectionTimeout) {
	this->connectionTimeout = connectionTimeout;
}

const string& ConnectionNode::getHost() const {
	return host;
}

void ConnectionNode::setHost(const string& host) {
	this->host = host;
}

const string& ConnectionNode::getPassword() const {
	return password;
}

void ConnectionNode::setPassword(const string& password) {
	this->password = password;
}

int ConnectionNode::getPort() const {
	return port;
}

void ConnectionNode::setPort(const int& port) {
	this->port = port;
}

float ConnectionNode::getReadTimeout() const {
	return readTimeout;
}

void ConnectionNode::setReadTimeout(const float& readTimeout) {
	this->readTimeout = readTimeout;
}

const string& ConnectionNode::getUsername() const {
	return username;
}

void ConnectionNode::setUsername(const string& username) {
	this->username = username;
}

const vector<ConnectionNode>& ConnectionProperties::getNodes() const {
	return nodes;
}

void ConnectionProperties::setNodes(const vector<ConnectionNode>& nodes) {
	this->nodes = nodes;
}

void ConnectionProperties::addNode(const ConnectionNode& node) {
	this->nodes.push_back(node);
}

bool ConnectionProperties::isNewConnectionStrategy() const {
	return newConnectionStrategy;
}

void ConnectionProperties::setNewConnectionStrategy(
		const bool& newConnectionStrategy) {
	this->newConnectionStrategy = newConnectionStrategy;
}

int ConnectionProperties::getPoolReadSize() const {
	return poolReadSize;
}

void ConnectionProperties::setPoolReadSize(const int& poolReadSize) {
	this->poolReadSize = poolReadSize;
}

int ConnectionProperties::getPoolWriteSize() const {
	return poolWriteSize;
}

void ConnectionProperties::setPoolWriteSize(const int& poolWriteSize) {
	this->poolWriteSize = poolWriteSize;
}

const map<string, string>& ConnectionProperties::getProperties() const {
	return properties;
}

const string& ConnectionProperties::getProperty(const string& name) const {
	if(properties.find(name)!=properties.end()) {
		return properties.find(name)->second;
	}
	return BLANK;
}

void ConnectionProperties::setProperties(
		const map<string, string>& properties) {
	this->properties = properties;
}

void ConnectionProperties::addProperty(const string& name, const string& value) {
	this->properties[name] = value;
}

const string& ConnectionProperties::getType() const {
	return type;
}

void ConnectionProperties::setType(const string& type) {
	this->type = type;
}

ConnectionNode::ConnectionNode() {
	readTimeout = 10000;
	connectionTimeout = 10000;
	port = -1;
}

ConnectionNode::~ConnectionNode() {
}

ConnectionProperties::ConnectionProperties() {
	newConnectionStrategy = false;
	poolWriteSize = 1;
	poolReadSize = 0;
}

ConnectionProperties::~ConnectionProperties() {
}

const string& ConnectionNode::getDatabaseName() const {
	return databaseName;
}

void ConnectionNode::setDatabaseName(const string& databaseName) {
	this->databaseName = databaseName;
}

const string& ConnectionProperties::getName() const {
	return name;
}

void ConnectionProperties::setName(const string& name) {
	this->name = name;
}

const string& ConnectionNode::getDsn() const {
	return dsn;
}
