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
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;
	p->lock.lock();
	auto it = p->internalMap.find(key);
	bool exists = it!=p->internalMap.end();
	if(exists) {
		p->lrul.erase(it->second);
		p->internalMap.erase(it);
	}
	p->lock.unlock();
	return true;
}

long long MemoryCacheImpl::increment(const std::string& key, const int& number) {
	long long val = -1;
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;

	p->lock.lock();
	auto it = p->internalMap.find(key);
	bool exists = it!=p->internalMap.end();
	if(exists) {
		p->lrul.splice(p->lrul.begin(), p->lrul, it->second);
		std::string rval = it->second->second;

		p->lrul.erase(it->second);
		p->internalMap.erase(it);

		val = CastUtil::toLongdouble(rval) + number;
		p->lrul.push_front(make_pair(key, CastUtil::fromNumber(val)));
		p->internalMap.insert(make_pair(key, p->lrul.begin()));
		clean();
	}
	p->lock.unlock();
	return val;
}

long long MemoryCacheImpl::decrement(const std::string& key, const int& number) {
	long long val = -1;
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;

	p->lock.lock();
	auto it = p->internalMap.find(key);
	bool exists = it!=p->internalMap.end();
	if(exists) {
		p->lrul.splice(p->lrul.begin(), p->lrul, it->second);
		std::string rval = it->second->second;

		p->lrul.erase(it->second);
		p->internalMap.erase(it);

		val = CastUtil::toLongdouble(rval) - number;
		p->lrul.push_front(make_pair(key, CastUtil::fromNumber(val)));
		p->internalMap.insert(make_pair(key, p->lrul.begin()));
		clean();
	}
	p->lock.unlock();
	return val;
}

long double MemoryCacheImpl::incrementFloat(const std::string& key, const double& number) {
	long double val = -1.0;
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;

	p->lock.lock();
	auto it = p->internalMap.find(key);
	bool exists = it!=p->internalMap.end();
	if(exists) {
		p->lrul.splice(p->lrul.begin(), p->lrul, it->second);
		std::string rval = it->second->second;

		p->lrul.erase(it->second);
		p->internalMap.erase(it);

		val = CastUtil::toLongdouble(rval) + number;
		p->lrul.push_front(make_pair(key, CastUtil::fromNumber(val)));
		p->internalMap.insert(make_pair(key, p->lrul.begin()));
		clean();
	}
	p->lock.unlock();
	return val;
}

long double MemoryCacheImpl::decrementFloat(const std::string& key, const double& number) {
	long double val = -1.0;
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;

	p->lock.lock();
	auto it = p->internalMap.find(key);
	bool exists = it!=p->internalMap.end();
	if(exists) {
		p->lrul.splice(p->lrul.begin(), p->lrul, it->second);
		std::string rval = it->second->second;

		p->lrul.erase(it->second);
		p->internalMap.erase(it);

		val = CastUtil::toLongdouble(rval) - number;
		p->lrul.push_front(make_pair(key, CastUtil::fromNumber(val)));
		p->internalMap.insert(make_pair(key, p->lrul.begin()));
		clean();
	}
	p->lock.unlock();
	return val;
}

std::map<std::string, std::string> MemoryCacheImpl::statistics() {
	std::map<std::string, std::string> m;
	return m;
}

bool MemoryCacheImpl::flushAll() {
	return true;
}

void MemoryCacheImpl::clean() {
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;
	while((int)p->internalMap.size()>p->size) {
		auto last_it = p->lrul.end(); last_it --;
		p->internalMap.erase(last_it->first);
		p->lrul.pop_back();
	}
}

bool MemoryCacheImpl::setInternal(const std::string& key, const std::string& value, const int& expireSeconds, const int& setOrAddOrRep) {
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;

	if(setOrAddOrRep<1 && setOrAddOrRep>3) return false;

	p->lock.lock();
	auto it = p->internalMap.find(key);
	bool exists = it!=p->internalMap.end();
	if(exists) {
		p->lrul.erase(it->second);
		p->internalMap.erase(it);
	}

	bool flag = false;
	if(setOrAddOrRep==1 || (setOrAddOrRep==2 && !exists) || (setOrAddOrRep==3 && exists))
	{
		p->lrul.push_front(make_pair(key, value));
		p->internalMap.insert(make_pair(key, p->lrul.begin()));
		clean();
		flag = true;
	}

	p->lock.unlock();
	return flag;
}

std::string MemoryCacheImpl::getValue(const std::string& key) {
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;
	std::string rval;
	p->lock.lock();
	if(p->internalMap.count(key)>0) {
		auto it = p->internalMap.find(key);
		p->lrul.splice(p->lrul.begin(), p->lrul, it->second);
		rval = it->second->second;
	}
	p->lock.unlock();
	return rval;
}

std::vector<std::string> MemoryCacheImpl::getValues(const std::vector<std::string>& keys) {
	std::vector<std::string> rv;
	mgetRaw(keys, rv);
	return rv;
}

void MemoryCacheImpl::mgetRaw(const std::vector<std::string>& keys, std::vector<std::string>& values) {
	MemoryCacheConnectionPool* p = (MemoryCacheConnectionPool*)pool;
	std::string rval;
	p->lock.lock();
	for(int i=0;i<(int)keys.size();++i) {
		const std::string& key = keys.at(i);
		if(p->internalMap.count(key)>0) {
			auto it = p->internalMap.find(key);
			p->lrul.splice(p->lrul.begin(), p->lrul, it->second);
			values.push_back(it->second->second);
		}
	}
	p->lock.unlock();
}

void* MemoryCacheImpl::executeCommand(const std::string command, ...) {
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

bool MemoryCacheImpl::setRaw(const std::string& key, const char* value, int expireSeconds) {
	bool status = setInternal(key, std::string(value), expireSeconds, 1);
	return status;
}

bool MemoryCacheImpl::addRaw(const std::string& key, const char* value, int expireSeconds) {
	bool status = setInternal(key, std::string(value), expireSeconds, 2);
	return status;
}

bool MemoryCacheImpl::replaceRaw(const std::string& key, const char* value, int expireSeconds) {
	bool status = setInternal(key, std::string(value), expireSeconds, 3);
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
	size = 10000;
	if(props.getProperty("size")!="") {
		try {
			size = CastUtil::toInt(props.getProperty("size"));
		} catch(const std::exception& e) {
		}
	}
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
