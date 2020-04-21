/*
 * MemoryCacheImpl.cpp
 *
 *  Created on: 10-May-2014
 *      Author: sumeetc
 */

#include "MemoryCacheImpl.h"

MemoryCacheImpl::MemoryCacheImpl(ConnectionPooler* pool) {
	this->pool = pool;
	this->properties = pool->getProperties();
	this->defaultExpireSeconds = -1;
	if(properties.getProperty("expiryTime")!="") {
		try {
			this->defaultExpireSeconds = CastUtil::toInt(properties.getProperty("expiryTime"));
		} catch(const std::exception& e) {
		}
	}
}

MemoryCacheImpl::~MemoryCacheImpl() {
}

bool MemoryCacheImpl::remove(const std::string& key) {
	((MemoryCacheConnectionPool*)pool)->internalMap.erase(key);
	return true;
}

long long MemoryCacheImpl::increment(const std::string& key, const int& number) {
	long long val = -1;
	((MemoryCacheConnectionPool*)pool)->lock.lock();
	if(((MemoryCacheConnectionPool*)pool)->internalMap.find(key)!=((MemoryCacheConnectionPool*)pool)->internalMap.end()) {
		val = CastUtil::toLonglong(((MemoryCacheConnectionPool*)pool)->internalMap[key]) + number;
		((MemoryCacheConnectionPool*)pool)->internalMap[key] = CastUtil::fromNumber(val);
	}
	((MemoryCacheConnectionPool*)pool)->lock.unlock();
	return val;
}

long long MemoryCacheImpl::decrement(const std::string& key, const int& number) {
	long long val = -1;
	((MemoryCacheConnectionPool*)pool)->lock.lock();
	if(((MemoryCacheConnectionPool*)pool)->internalMap.find(key)!=((MemoryCacheConnectionPool*)pool)->internalMap.end()) {
		val = CastUtil::toLonglong(((MemoryCacheConnectionPool*)pool)->internalMap[key]) - number;
		((MemoryCacheConnectionPool*)pool)->internalMap[key] = CastUtil::fromNumber(val);
	}
	((MemoryCacheConnectionPool*)pool)->lock.unlock();
	return val;
}

long double MemoryCacheImpl::incrementFloat(const std::string& key, const double& number) {
	long double val = -1.0;
	((MemoryCacheConnectionPool*)pool)->lock.lock();
	if(((MemoryCacheConnectionPool*)pool)->internalMap.find(key)!=((MemoryCacheConnectionPool*)pool)->internalMap.end()) {
		val = CastUtil::toLongdouble(((MemoryCacheConnectionPool*)pool)->internalMap[key]) + number;
		((MemoryCacheConnectionPool*)pool)->internalMap[key] = CastUtil::fromNumber(val);
	}
	((MemoryCacheConnectionPool*)pool)->lock.unlock();
	return val;
}

long double MemoryCacheImpl::decrementFloat(const std::string& key, const double& number) {
	long double val = -1.0;
	((MemoryCacheConnectionPool*)pool)->lock.lock();
	if(((MemoryCacheConnectionPool*)pool)->internalMap.find(key)!=((MemoryCacheConnectionPool*)pool)->internalMap.end()) {
		val = CastUtil::toLongdouble(((MemoryCacheConnectionPool*)pool)->internalMap[key]) - number;
		((MemoryCacheConnectionPool*)pool)->internalMap[key] = CastUtil::fromNumber(val);
	}
	((MemoryCacheConnectionPool*)pool)->lock.unlock();
	return val;
}

std::map<std::string, std::string> MemoryCacheImpl::statistics() {
	return ((MemoryCacheConnectionPool*)pool)->internalMap;
}

bool MemoryCacheImpl::flushAll() {
	return true;
}

bool MemoryCacheImpl::setInternal(const std::string& key, const std::string& value, const int& expireSeconds, const int& setOrAddOrRep) {
	if(setOrAddOrRep==1)
	{
		((MemoryCacheConnectionPool*)pool)->lock.lock();
		((MemoryCacheConnectionPool*)pool)->internalMap[key] = value;
		((MemoryCacheConnectionPool*)pool)->lock.unlock();
		return true;
	}
	else if(setOrAddOrRep==2 && ((MemoryCacheConnectionPool*)pool)->internalMap.find(key)==((MemoryCacheConnectionPool*)pool)->internalMap.end())
	{
		((MemoryCacheConnectionPool*)pool)->lock.lock();
		((MemoryCacheConnectionPool*)pool)->internalMap[key] = value;
		((MemoryCacheConnectionPool*)pool)->lock.unlock();
		return true;
	}
	else if(setOrAddOrRep==3 && ((MemoryCacheConnectionPool*)pool)->internalMap.find(key)!=((MemoryCacheConnectionPool*)pool)->internalMap.end())
	{
		((MemoryCacheConnectionPool*)pool)->lock.lock();
		((MemoryCacheConnectionPool*)pool)->internalMap[key] = value;
		((MemoryCacheConnectionPool*)pool)->lock.unlock();
		return true;
	}
	return false;
}

std::string MemoryCacheImpl::getValue(const std::string& key) {
	((MemoryCacheConnectionPool*)pool)->lock.lock();
	std::string rval = ((MemoryCacheConnectionPool*)pool)->internalMap[key];
	((MemoryCacheConnectionPool*)pool)->lock.unlock();
	return rval;
}

std::vector<std::string> MemoryCacheImpl::getValues(const std::vector<std::string>& keys) {
	std::vector<std::string> rv;
	for(int i=0;i<(int)keys.size();++i) {
		((MemoryCacheConnectionPool*)pool)->lock.lock();
		rv.push_back(((MemoryCacheConnectionPool*)pool)->internalMap[keys.at(i)]);
		((MemoryCacheConnectionPool*)pool)->lock.unlock();
	}
	return rv;
}

void* MemoryCacheImpl::executeCommand(const std::string& command, ...) {
	throw std::runtime_error("Not Implemented");
}

bool MemoryCacheImpl::set(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 1);
	return status;
}

bool MemoryCacheImpl::add(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 2);
	return status;
}

bool MemoryCacheImpl::replace(const std::string& key, GenericObject& value, int expireSeconds) {
	std::string valueStr = value.getSerilaizedState();
	bool status = setInternal(key, valueStr, expireSeconds, 3);
	return status;
}

bool MemoryCacheImpl::addToQ(const std::string& qname, const std::string& value) {
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;
	p->qlock.lock();
	if(p->internalQMap.find(qname)==p->internalQMap.end()) {
		p->internalQMap[qname];
	}
	p->internalQMap[qname].push(value);
	p->qlock.unlock();
	return false;
}

std::string MemoryCacheImpl::getFromQ(const std::string& qname) {
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;
	p->qlock.lock();
	std::string value;
	if(p->internalQMap.find(qname)!=p->internalQMap.end() && !p->internalQMap[qname].empty()) {
		value = p->internalQMap[qname].front();
		p->internalQMap[qname].pop();
	}
	p->qlock.unlock();
	return value;
}

void MemoryCacheImpl::init() {
}

MemoryCacheConnectionPool::MemoryCacheConnectionPool(const ConnectionProperties& props) {
}

MemoryCacheConnectionPool::~MemoryCacheConnectionPool() {
}

void* MemoryCacheConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	return NULL;
}

void MemoryCacheConnectionPool::closeConnection(void* conn) {
}

void MemoryCacheConnectionPool::initEnv() {
}

void MemoryCacheConnectionPool::destroy() {
}
