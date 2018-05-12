/*
 * RedisCacheImpl.cpp
 *
 *  Created on: 07-May-2014
 *      Author: sumeetc
 */

#include "RedisCacheImpl.h"

RedisCacheImpl::RedisCacheImpl(ConnectionPooler* pool) {
	this->pool = pool;
	this->properties = pool->getProperties();
	this->defaultExpireSeconds = -1;
	if(properties.getProperty("expiryTime")!="") {
		try {
			this->defaultExpireSeconds = CastUtil::lexical_cast<int>(properties.getProperty("expiryTime"));
		} catch(...) {
		}
	}
	if(this->defaultExpireSeconds<=0) {
		this->defaultExpireSeconds = 120;
	}
}

RedisCacheImpl::~RedisCacheImpl() {
}

bool RedisCacheImpl::remove(const std::string& key) {
	redisReply* reply = execute("DEL %s", key.c_str());
	return replyStatus(reply);
}

long long RedisCacheImpl::increment(const std::string& key, const int& number) {
	redisReply* reply = execute("INCRBY %s %d", key.c_str(), number);
	std::string val = replyValue(reply);
	if(val=="") {
		throw "Command Failed";
	} else {
		return CastUtil::lexical_cast<long long>(val);
	}
}

long long RedisCacheImpl::decrement(const std::string& key, const int& number) {
	redisReply* reply = execute("DECRBY %s %d", key.c_str(), number);
	std::string val = replyValue(reply);
	if(val=="") {
		throw "Command Failed";
	} else {
		return CastUtil::lexical_cast<long long>(val);
	}
}

long double RedisCacheImpl::incrementFloat(const std::string& key, const double& number) {
	redisReply* reply = execute("INCRBYFLOAT %s %f", key.c_str(), number);
	std::string val = replyValue(reply);
	if(val=="") {
		throw "Command Failed";
	} else {
		return CastUtil::lexical_cast<long double>(val);
	}
}

long double RedisCacheImpl::decrementFloat(const std::string& key, const double& number) {
	return incrementFloat(key, -number);
}

std::map<std::string, std::string> RedisCacheImpl::statistics() {
	redisReply* reply = execute("INFO", 0);
	std::string sValue = replyValue(reply);
	std::map<std::string, std::string> stats;
	if(sValue=="")return stats;
	std::vector<std::string> data = StringUtil::splitAndReturn<std::vector<std::string> >(sValue, "\r\n");
	for (int var = 0; var < (int)data.size(); ++var) {
		if(data.at(var).find(":")!=std::string::npos) {
			std::string key = data.at(var).substr(0, data.at(var).find(":"));
			std::string value = data.at(var).substr(data.at(var).find(":")+1);
			stats[key] = value;
		}
	}
	return stats;
}

bool RedisCacheImpl::flushAll() {
	redisReply* reply = execute("FLUSHALL", 0);
	return replyStatus(reply);
}

bool RedisCacheImpl::set(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	redisReply* reply = execute("SET %s %s EX %d", key.c_str(), valueStr.c_str(), expireSeconds);
	return replyStatus(reply);
}

bool RedisCacheImpl::add(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	redisReply* reply = execute("SET %s %s EX %d NX", key.c_str(), valueStr.c_str(), expireSeconds);
	return replyStatus(reply);
}

bool RedisCacheImpl::replace(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	redisReply* reply = execute("SET %s %s EX %d XX", key.c_str(), valueStr.c_str(), expireSeconds);
	return replyStatus(reply);
}

std::string RedisCacheImpl::getValue(const std::string& key) {
	redisReply* reply = execute("GET %s", key.c_str());
	return replyValue(reply);
}

std::vector<std::string> RedisCacheImpl::getValues(const std::vector<std::string>& keys) {
	std::vector<std::string> rv;
	std::string cmd = "MGET ";
	for(int i=0;i<(int)keys.size();++i) {
		cmd += keys.at(i) + " ";
	}
	Connection* connection = pool->checkout();
	redisContext* c = (redisContext*) connection->getConn();
	redisReply* reply = (redisReply*) redisCommand(c, cmd.c_str());
	if (reply->type == REDIS_REPLY_ARRAY) {
		for (int i=0; i<(int)reply->elements; ++i) {
			std::string rval;
			for (int var = 0; var < (int)reply->element[i]->len; ++var) {
				rval.push_back(reply->element[i]->str[var]);
			}
			rv.push_back(rval);
		}
	}
	freeReplyObject(reply);
	pool->release(connection);
	return rv;
}

bool RedisCacheImpl::replyStatus(redisReply* reply) {
	bool status = false;
	if (reply->type == REDIS_REPLY_ERROR || reply->type == REDIS_REPLY_NIL) {
		status = false;
	} else {
		status = true;
	}
	freeReplyObject(reply);
	return status;
}

std::string RedisCacheImpl::replyValue(redisReply* reply) {
	if (reply->type == REDIS_REPLY_INTEGER) {
		return CastUtil::lexical_cast<std::string>(reply->integer);
	} else if (reply->type == REDIS_REPLY_STRING) {
		std::string value;
		for (int var = 0; var < reply->len; ++var) {
			value.push_back(reply->str[var]);
		}
		return value;
	}
	freeReplyObject(reply);
	return "";
}

redisReply* RedisCacheImpl::execute(const char* format, ...) {
	Connection* connection = pool->checkout();
	redisContext* c = (redisContext*) connection->getConn();
	va_list vl;
	va_start(vl, format);
	redisReply* reply = (redisReply*) redisvCommand(c, format, vl);
	va_end(vl);
	pool->release(connection);
	return reply;
}

void* RedisCacheImpl::executeCommand(const std::string& command, ...) {
	va_list vl;
	std::string c = command;
	va_start(vl, c);
	void* reply = execute(command.c_str());
	va_end(vl);
	return reply;
}

void RedisCacheImpl::init() {
}

RedisCacheConnectionPool::RedisCacheConnectionPool(const ConnectionProperties& props) {
	logger = LoggerFactory::getLogger("RedisCacheConnectionPool");
	createPool(props);
}

RedisCacheConnectionPool::~RedisCacheConnectionPool() {
	destroyPool();
}

void* RedisCacheConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	int connTimeoutUs = node.getConnectionTimeout()*1000;
	int seconds = node.getConnectionTimeout()/1000;
	int microseconds = connTimeoutUs - (seconds*1000000);

	struct timeval timeout = { seconds, microseconds }; // 1.5 seconds
	redisContext *c = redisConnectWithTimeout(node.getHost().c_str(), node.getPort(), timeout);
	if (c == NULL || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		} else {
			printf("Connection error: can't allocate redis context\n");
		}
		return NULL;
	}
	return c;
}
void RedisCacheConnectionPool::closeConnection(void* conn) {
	redisFree((redisContext*)conn);
}

void RedisCacheConnectionPool::initEnv() {
}

void RedisCacheConnectionPool::destroy() {
}
