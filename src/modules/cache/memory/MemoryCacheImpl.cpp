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

bool MemoryCacheImpl::remove(const std::string& key) {
	internalMap.erase(key);
	return true;
}

long long MemoryCacheImpl::increment(const std::string& key, const int& number) {
	if(internalMap.find(key)!=internalMap.end()) {
		long long val = CastUtil::lexical_cast<long long>(internalMap[key]) + number;
		internalMap[key] = CastUtil::lexical_cast<std::string>(val);
		return val;
	}
	return -1;
}

long long MemoryCacheImpl::decrement(const std::string& key, const int& number) {
	if(internalMap.find(key)!=internalMap.end()) {
		long long val = CastUtil::lexical_cast<long long>(internalMap[key]) - number;
		internalMap[key] = CastUtil::lexical_cast<std::string>(val);
		return val;
	}
	return -1;
}

long double MemoryCacheImpl::incrementFloat(const std::string& key, const double& number) {
	if(internalMap.find(key)!=internalMap.end()) {
		long double val = CastUtil::lexical_cast<long double>(internalMap[key]) + number;
		internalMap[key] = CastUtil::lexical_cast<std::string>(val);
		return val;
	}
	return -1.0;
}

long double MemoryCacheImpl::decrementFloat(const std::string& key, const double& number) {
	if(internalMap.find(key)!=internalMap.end()) {
		long double val = CastUtil::lexical_cast<long double>(internalMap[key]) - number;
		internalMap[key] = CastUtil::lexical_cast<std::string>(val);
		return val;
	}
	return -1.0;
}

std::map<std::string, std::string> MemoryCacheImpl::statistics() {
	return internalMap;
}

bool MemoryCacheImpl::flushAll() {
	return true;
}

void* MemoryCacheImpl::newConnection(const bool& isWrite, const ConnectionNode& node) {
	return NULL;
}

bool MemoryCacheImpl::setInternal(const std::string& key, const std::string& value, const int& expireSeconds, const int& setOrAddOrRep) {
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

std::string MemoryCacheImpl::getValue(const std::string& key) {
	std::string rval = internalMap[key];
	return rval;
}

void* MemoryCacheImpl::executeCommand(const std::string& command, ...) {
	throw "Not Implemented";
}

void MemoryCacheImpl::closeConnection(void* conn) {
}

bool MemoryCacheImpl::set(const std::string& key, GenericObject& value, const int& expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 1);
	return status;
}

bool MemoryCacheImpl::add(const std::string& key, GenericObject& value, const int& expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 2);
	return status;
}

bool MemoryCacheImpl::replace(const std::string& key, GenericObject& value, const int& expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 3);
	return status;
}
