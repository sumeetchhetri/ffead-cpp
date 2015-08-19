/*
 * RedisCacheImpl.cpp
 *
 *  Created on: 07-May-2014
 *      Author: sumeetc
 */

#include "RedisCacheImpl.h"

RedisCacheImpl::RedisCacheImpl(const ConnectionProperties& properties) {
	this->properties = properties;
}

RedisCacheImpl::~RedisCacheImpl() {
	destroyPool();
}

void RedisCacheImpl::initEnv() {
}

void RedisCacheImpl::destroy() {
}

bool RedisCacheImpl::remove(const string& key) {
	redisReply *reply = execute("DEL %s", key.c_str());
	return replyStatus(reply);
}

long long RedisCacheImpl::increment(const string& key, const int& number) {
	redisReply *reply = execute("INCRBY %s %d", key.c_str(), number);
	string val = replyValue(reply);
	if(val=="") {
		throw "Command Failed";
	} else {
		return CastUtil::lexical_cast<long long>(val);
	}
}

long long RedisCacheImpl::decrement(const string& key, const int& number) {
	redisReply *reply = execute("DECRBY %s %d", key.c_str(), number);
	string val = replyValue(reply);
	if(val=="") {
		throw "Command Failed";
	} else {
		return CastUtil::lexical_cast<long long>(val);
	}
}

long double RedisCacheImpl::incrementFloat(const string& key, const double& number) {
	redisReply *reply = execute("INCRBYFLOAT %s %f", key.c_str(), number);
	string val = replyValue(reply);
	if(val=="") {
		throw "Command Failed";
	} else {
		return CastUtil::lexical_cast<long double>(val);
	}
}

long double RedisCacheImpl::decrementFloat(const string& key, const double& number) {
	return incrementFloat(key, -number);
}

map<string, string> RedisCacheImpl::statistics() {
	redisReply *reply = execute("INFO", 0);
	string sValue = replyValue(reply);
	map<string, string> stats;
	if(sValue=="")return stats;
	vector<string> data = StringUtil::splitAndReturn<vector<string> >(sValue, "\r\n");
	for (int var = 0; var < (int)data.size(); ++var) {
		if(data.at(var).find(":")!=string::npos) {
			string key = data.at(var).substr(0, data.at(var).find(":"));
			string value = data.at(var).substr(data.at(var).find(":")+1);
			stats[key] = value;
		}
	}
	return stats;
}

bool RedisCacheImpl::flushAll() {
	redisReply *reply = execute("FLUSHALL", 0);
	return replyStatus(reply);
}

void* RedisCacheImpl::newConnection(const bool& isWrite, const ConnectionNode& node) {
	int connTimeout = node.getConnectionTimeout()*1000000;
	int seconds = connTimeout/1000000;
	int microseconds = connTimeout - (seconds*1000000);

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

bool RedisCacheImpl::set(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	redisReply *reply = execute("SET %s %s EX %d", key.c_str(), valueStr.c_str(), expireSeconds);
	return replyStatus(reply);
}

bool RedisCacheImpl::add(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	redisReply *reply = execute("SET %s %s EX %d NX", key.c_str(), valueStr.c_str(), expireSeconds);
	return replyStatus(reply);
}

bool RedisCacheImpl::replace(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	redisReply *reply = execute("SET %s %s EX %d XX", key.c_str(), valueStr.c_str(), expireSeconds);
	return replyStatus(reply);
}

string RedisCacheImpl::getValue(const string& key) {
	redisReply *reply = execute("GET %s", key.c_str());
	return replyValue(reply);
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

string RedisCacheImpl::replyValue(redisReply* reply) {
	if (reply->type == REDIS_REPLY_INTEGER) {
		return CastUtil::lexical_cast<string>(reply->integer);
	} else if (reply->type == REDIS_REPLY_STRING) {
		string value;
		for (int var = 0; var < reply->len; ++var) {
			value.push_back(reply->str[var]);
		}
		return value;
	}
	freeReplyObject(reply);
	return "";
}

redisReply* RedisCacheImpl::execute(const char* format, ...) {
	Connection* connection = checkout();
	redisContext* c = (redisContext*) connection->getConn();
	va_list vl;
	va_start(vl, format);
	redisReply *reply = (redisReply*) redisvCommand(c, format, vl);
	va_end(vl);
	release(connection);
	return reply;
}

void* RedisCacheImpl::executeCommand(const string& command, ...) {
	va_list vl;
	string c = command;
	va_start(vl, c);
	void* reply = execute(command.c_str());
	va_end(vl);
	return reply;
}

void RedisCacheImpl::init() {
	createPool(properties);
}

void RedisCacheImpl::closeConnection(void* conn) {
	redisFree((redisContext*)conn);
}
