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
 * Connection.h
 *
 *  Created on: 08-May-2014
 *      Author: sumeetc
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_
#include "string"
#include "map"
#include "vector"


class ConnectionNode {
public:
	float getConnectionTimeout() const;
	void setConnectionTimeout(const float& connectionTimeout);
	const std::string& getHost() const;
	void setHost(const std::string& host);
	const std::string& getPassword() const;
	void setPassword(const std::string& password);
	int getPort() const;
	void setPort(const int& port);
	float getReadTimeout() const;
	void setReadTimeout(const float& readTimeout);
	const std::string& getUsername() const;
	void setUsername(const std::string& username);
	ConnectionNode();
	virtual ~ConnectionNode();
	const std::string& getDatabaseName() const;
	void setDatabaseName(const std::string& databaseName);
	const std::string& getDsn() const;
	void setBaseUrl(const std::string& url);
	const std::string& getBaseUrl() const;

private:
	std::string url;
	std::string dsn;
	std::string host;
	std::string username;
	std::string password;
	std::string databaseName;
	int port;
	float readTimeout;
	float connectionTimeout;
	friend class ConnectionPooler;
	friend class ConfigurationHandler;
};

class Connection
{
	bool busy;
	bool type;
	void* _conn;
	bool outOfPool;
	ConnectionNode node;
public:
	Connection();
	virtual ~Connection();
	void* getConn();
	void setConn(void* conn);
	bool isBusy() const;
	void setBusy(const bool& busy);
	bool isType() const;
	void setType(const bool& type);
	bool isOutOfPool() const;
	void setOutOfPool(const bool& outOfPool);
	const ConnectionNode& getNode() const;
	void setNode(const ConnectionNode& node);
};

class ConnectionProperties {
	static std::string BLANK;
public:
	bool isNewConnectionStrategy() const;
	void setNewConnectionStrategy(const bool& newConnectionStrategy);
	const std::vector<ConnectionNode>& getNodes() const;
	void setNodes(const std::vector<ConnectionNode>& nodes);
	void addNode(const ConnectionNode& node);
	int getPoolReadSize() const;
	void setPoolReadSize(const int& poolReadSize);
	int getPoolWriteSize() const;
	void setPoolWriteSize(const int& poolWriteSize);
	const std::map<std::string, std::string>& getProperties() const;
	const std::string& getProperty(const std::string& name) const;
	void setProperties(const std::map<std::string, std::string>& properties);
	void addProperty(const std::string& name, const std::string& value);
	const std::string& getType() const;
	void setType(const std::string& type);
	ConnectionProperties();
	virtual ~ConnectionProperties();
	const std::string& getName() const;
	void setName(const std::string& name);

private:
	std::string name;
	std::string type;
	int poolReadSize;
	int poolWriteSize;
	bool newConnectionStrategy;
	std::vector<ConnectionNode> nodes;
	std::map<std::string, std::string> properties;
	friend class ConfigurationHandler;
};

#endif /* CONNECTION_H_ */
