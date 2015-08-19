/*
 * MemoryCacheImpl.cpp
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */

#include "MemoryCacheImpl.h"

MemoryCacheImpl::MemoryCacheImpl(const ConnectionProperties& properties) {
	this->properties = properties;
}

MemoryCacheImpl::~MemoryCacheImpl() {
	destroyPool();
}

void MemoryCacheImpl::init() {
}

void MemoryCacheImpl::initEnv() {
}

void MemoryCacheImpl::destroy() {
}

bool MemoryCacheImpl::remove(const string& key) {
	internalMap.erase(key);
	return true;
}

long long MemoryCacheImpl::increment(const string& key, const int& number) {
	if(internalMap.find(key)!=internalMap.end()) {
		long long val = CastUtil::lexical_cast<long long>(internalMap[key]) + number;
		internalMap[key] = CastUtil::lexical_cast<string>(val);
		return val;
	}
	return -1;
}

long long MemoryCacheImpl::decrement(const string& key, const int& number) {
	if(internalMap.find(key)!=internalMap.end()) {
		long long val = CastUtil::lexical_cast<long long>(internalMap[key]) - number;
		internalMap[key] = CastUtil::lexical_cast<string>(val);
		return val;
	}
	return -1;
}

long double MemoryCacheImpl::incrementFloat(const string& key, const double& number) {
	if(internalMap.find(key)!=internalMap.end()) {
		long double val = CastUtil::lexical_cast<long double>(internalMap[key]) + number;
		internalMap[key] = CastUtil::lexical_cast<string>(val);
		return val;
	}
	return -1.0;
}

long double MemoryCacheImpl::decrementFloat(const string& key, const double& number) {
	if(internalMap.find(key)!=internalMap.end()) {
		long double val = CastUtil::lexical_cast<long double>(internalMap[key]) - number;
		internalMap[key] = CastUtil::lexical_cast<string>(val);
		return val;
	}
	return -1.0;
}

map<string, string> MemoryCacheImpl::statistics() {
	return internalMap;
}

bool MemoryCacheImpl::flushAll() {
	return true;
}

void* MemoryCacheImpl::newConnection(const bool& isWrite, const ConnectionNode& node) {
	return NULL;
}

bool MemoryCacheImpl::setInternal(const string& key, const string& value, const int& expireSeconds, const int& setOrAddOrRep) {
	if(setOrAddOrRep==1)
	{
		internalMap[key] = value;
		return true;
	}
	else if(setOrAddOrRep==2 && internalMap.find(key)==internalMap.end())
	{
		internalMap[key] = value;
		return true;
	}
	else if(setOrAddOrRep==3 && internalMap.find(key)!=internalMap.end())
	{
		internalMap[key] = value;
		return true;
	}
	return false;
}

string MemoryCacheImpl::getValue(const string& key) {
	string rval = internalMap[key];
	return rval;
}

void* MemoryCacheImpl::executeCommand(const string& command, ...) {
	throw "Not Implemented";
}

void MemoryCacheImpl::closeConnection(void* conn) {
}

bool MemoryCacheImpl::set(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 1);
	return status;
}

bool MemoryCacheImpl::add(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 2);
	return status;
}

bool MemoryCacheImpl::replace(const string& key, GenericObject& value, const int& expireSeconds) {
	string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 3);
	return status;
}
