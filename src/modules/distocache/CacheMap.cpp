/*
	Copyright 2009-2013, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*//*
 * CacheMap.cpp
 *
 *  Created on: 27-Mar-2013
 *      Author: sumeetc
 */

#include "CacheMap.h"

CacheMap* CacheMap::instance = NULL;

std::string CacheMap::MAP_ENTRY = "mapentry", CacheMap::COLL_ENTRY = "collentry", CacheMap::REMOVE = "remove",
	   CacheMap::SIZE = "size", CacheMap::CLEAR = "clear", CacheMap::IS_EMPTY = "isempty",
	   CacheMap::ALLOCATE = "allocate", CacheMap::DEALLOCATE = "deallocate", CacheMap::MAP = "map",
	   CacheMap::SET = "set", CacheMap::LIST = "list", CacheMap::VECTOR = "vector", CacheMap::QUEUE = "queue",
	   CacheMap::DEQUE = "deque", CacheMap::ADD = "add", CacheMap::GET = "get", CacheMap::GET_FRONT = "getfront",
	   CacheMap::GET_BACK = "getback", CacheMap::POP = "pop", CacheMap::POP_BACK = "popback",
	   CacheMap::POP_FRONT = "popfront", CacheMap::PUSH_FRONT = "pushfront", CacheMap::PUSH_BACK = "pushback",
	   CacheMap::INSERT = "insert", CacheMap::OBJ_ENTRY = "objentry", CacheMap::GETBYPOS = "getbypos",
	   CacheMap::POPGET = "popget", CacheMap::POP_FRONTGET = "popfrontget", CacheMap::POP_BACKGET = "popbackget";

std::string CacheMap::ERR_NOELEMENTS = "No Elements in container", CacheMap::ERR_NOKEYCACHEMAP = "Unable to find value for Key specified",
	   CacheMap::ERR_INVCONTAINER = "Invalid container specified", CacheMap::ERR_OPNOTSUPP = "Operation not supported on container",
	   CacheMap::ERR_INDGRTCONTSIZ = "Index greater than container size", CacheMap::ERR_NOTAMAPCONT = "Not a map container",
	   CacheMap::ERR_NOVALUEFOUND = "No value found for key", CacheMap::ERR_ALLOCENTEXISTS = "Entry already exists",
	   CacheMap::ERR_NOVALUESPEC = "No value specified", CacheMap::ERR_NEGATIVEPOS = "Position value is less than 0",
	   CacheMap::ERR_POSNOTNUM = "Position value is not a number", CacheMap::ERR_NEGATIVEREP = "Repetition value is less than 0",
	   CacheMap::ERR_REPNOTNUM = "Repetition value is not a number", CacheMap::SUCCESS = "SUCCESS";

CacheMap::CacheMap() {
}

void CacheMap::allocate(const std::string& cacheKey, const std::string& type) {
	if(checkExistance(cacheKey))
	{
		throw std::runtime_error(ERR_ALLOCENTEXISTS);
	}

	void *entry = NULL;
	if(type==MAP)
	{
		entry = new std::map<std::string, std::string>;
	}
	else if(type==SET)
	{
		entry = new std::set<std::string>;
	}
	else if(type==LIST)
	{
		entry = new std::list<std::string>;
	}
	else if(type==VECTOR)
	{
		entry = new std::vector<std::string>;
	}
	else if(type==QUEUE)
	{
		entry = new std::queue<std::string>;
	}
	else if(type==DEQUE)
	{
		entry = new std::deque<std::string>;
	}
	else
	{
		throw std::runtime_error(ERR_INVCONTAINER);
	}
	if(entry!=NULL)
	{
		instance->mutex.lock();
		instance->cacheMap[cacheKey] = entry;
		instance->valueTypes[cacheKey] = type;
		instance->valueLocks[cacheKey] = new Mutex;
		instance->mutex.unlock();
	}
}

void CacheMap::deallocate(const std::string& cacheKey) {
	if(checkExistance(cacheKey))
	{
		instance->mutex.lock();
		delete instance->valueLocks[cacheKey];
		delete instance->cacheMap[cacheKey];
		instance->cacheMap.erase(cacheKey);
		instance->valueLocks.erase(cacheKey);
		instance->valueTypes.erase(cacheKey);
		instance->mutex.unlock();
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
}

void CacheMap::addObjectEntry(const std::string& key, const std::string& value) {
	instance->objMapMutex.lock();
	instance->objCacheMap[key] = value;
	instance->objMapMutex.unlock();
}

void CacheMap::removeObjectEntry(const std::string& key) {
	instance->objMapMutex.lock();
	instance->objCacheMap.erase(key);
	instance->objMapMutex.unlock();
}

std::string CacheMap::getObjectEntryValue(const std::string& key) {
	std::string value;
	bool error = false;
	instance->objMapMutex.lock();
	if(instance->objCacheMap.find(key)!=instance->objCacheMap.end())
	{
		value = instance->objCacheMap[key];
	}
	else
	{
		error = true;
	}
	instance->objMapMutex.unlock();
	if(error)
	{
		throw std::runtime_error(ERR_NOVALUEFOUND);
	}
	return value;
}

void CacheMap::addMapEntry(const std::string& cacheKey, const std::string& key, const std::string& value) {
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		std::map<std::string, std::string>* valueMap = (std::map<std::string, std::string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		valueMap->insert(std::pair<std::string, std::string>(key, value));
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw std::runtime_error(ERR_NOTAMAPCONT);
	}
}

void CacheMap::removeMapEntry(const std::string& cacheKey, const std::string& key) {
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		std::map<std::string, std::string>* valueMap = (std::map<std::string, std::string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		valueMap->erase(key);
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw std::runtime_error(ERR_NOTAMAPCONT);
	}
}

void CacheMap::addCollectionEntry(const std::string& cacheKey, const std::string& value) {
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueVector = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			std::set<std::string>* valueVector = (std::set<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->insert(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			instance->mutex.lock();
			std::queue<std::string>* valueVector = (std::queue<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->push(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueVector = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
}

void CacheMap::init() {
	if(instance==NULL) {
		instance = new CacheMap;
	}
}

void CacheMap::destroy() {
	if(instance!=NULL) {
		delete instance;
	}
}

std::string CacheMap::getMapEntryValue(const std::string& cacheKey, const std::string& key) {
	std::string value;
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		std::map<std::string, std::string>* valueMap = (std::map<std::string, std::string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		if(valueMap->find(key)!=valueMap->end())
		{
			value = valueMap->find(key)->second;
		}
		else
		{
			throw std::runtime_error(ERR_NOVALUEFOUND);
		}
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	return value;
}

std::string CacheMap::getMapEntryValueByPosition(const std::string& cacheKey, const int& position) {
	std::string value;
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		std::map<std::string, std::string>* valueMap = (std::map<std::string, std::string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		if(position<(int)valueMap->size())
		{
			std::map<std::string, std::string>::iterator it = valueMap->begin();
			advance(it, position);
			value = it->second;
		}
		else
		{
			throw std::runtime_error(ERR_INDGRTCONTSIZ);
		}
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	return value;
}

void CacheMap::setMapEntryValueByPosition(const std::string& cacheKey, const int& position, const std::string& value) {
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		std::map<std::string, std::string>* valueMap = (std::map<std::string, std::string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		if(position<(int)valueMap->size())
		{
			std::map<std::string, std::string>::iterator it = valueMap->begin();
			advance(it, position);
			it->second = value;
		}
		else
		{
			throw std::runtime_error(ERR_INDGRTCONTSIZ);
		}
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
}

void CacheMap::setCollectionEntryAt(const std::string& cacheKey, const int& position, const std::string& value) {
	bool error = false;
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueVector->size())
			{
				(*valueVector)[position] = value;
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				std::list<std::string>::iterator it = valueList->begin();
				advance(it, position);
				*it = value;
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueDeque->size())
			{
				std::deque<std::string>::iterator it = valueDeque->begin();
				advance(it, position);
				*it = value;
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_INDGRTCONTSIZ);
	}
}

std::string CacheMap::getCollectionEntryAt(const std::string& cacheKey, const int& position) {
	std::string value;
	bool error = false;
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueVector->size())
			{
				value = valueVector->at(position);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				std::list<std::string>::iterator it = valueList->begin();
				advance(it, position);
				value = *it;
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			std::set<std::string>* valueSet = (std::set<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueSet->size())
			{
				std::set<std::string>::iterator it = valueSet->begin();
				advance(it, position);
				value = *it;
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueDeque->size())
			{
				std::deque<std::string>::iterator it = valueDeque->begin();
				advance(it, position);
				value = *it;
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_INDGRTCONTSIZ);
	}
	return value;
}

long CacheMap::size(const std::string& cacheKey) {
	long value = -1;
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueVector->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueList->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			std::set<std::string>* valueSet = (std::set<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueSet->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			instance->mutex.lock();
			std::queue<std::string>* valueQueue = (std::queue<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueQueue->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueDeque->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==MAP)
		{
			instance->mutex.lock();
			std::map<std::string, std::string>* valueMap = (std::map<std::string, std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueMap->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	return value;
}


void CacheMap::removeCollectionEntryAt(const std::string& cacheKey, const int& position) {
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueVector->size())
			{
				valueVector->erase(valueVector->begin()+position);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				std::list<std::string>::iterator it = valueList->begin();
				advance(it, position);
				valueList->erase(it);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			std::set<std::string>* valueSet = (std::set<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueSet->size())
			{
				std::set<std::string>::iterator it = valueSet->begin();
				advance(it, position);
				valueSet->erase(it);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueDeque->size())
			{
				valueDeque->erase(valueDeque->begin()+position);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_INDGRTCONTSIZ);
	}
}

bool CacheMap::isEmpty(const std::string& cacheKey) {
	bool value = false;
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueVector->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueList->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			std::set<std::string>* valueSet = (std::set<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueSet->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			instance->mutex.lock();
			std::queue<std::string>* valueQueue = (std::queue<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueQueue->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueDeque->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==MAP)
		{
			instance->mutex.lock();
			std::map<std::string, std::string>* valueMap = (std::map<std::string, std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueMap->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	return value;
}

void CacheMap::clear(const std::string& cacheKey) {
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueList->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			std::set<std::string>* valueSet = (std::set<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueSet->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueDeque->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==MAP)
		{
			instance->mutex.lock();
			std::map<std::string, std::string>* valueMap = (std::map<std::string, std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueMap->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
}

void CacheMap::insert(const std::string& cacheKey, const std::string& value, const int& position) {
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueVector->size())
			{
				valueVector->insert(valueVector->begin()+position, value);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				std::list<std::string>::iterator it = valueList->begin();
				advance(it, position);
				valueList->insert(it, value);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			std::set<std::string>* valueSet = (std::set<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueSet->size())
			{
				std::set<std::string>::iterator it = valueSet->begin();
				advance(it, position);
				valueSet->insert(it, value);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueDeque->size())
			{
					valueDeque->insert(valueDeque->begin()+position, value);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_INDGRTCONTSIZ);
	}
}

void CacheMap::insert(const std::string& cacheKey, const std::string& value, const int& position, const int& repeat) {
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueVector->size())
			{
				valueVector->insert(valueVector->begin()+position, repeat, value);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				std::list<std::string>::iterator it = valueList->begin();
				advance(it, position);
				valueList->insert(it, repeat, value);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueDeque->size())
			{
					valueDeque->insert(valueDeque->begin()+position, repeat, value);
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_INDGRTCONTSIZ);
	}
}

void CacheMap::popValueQueue(const std::string& cacheKey) {
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==QUEUE)
		{
			instance->mutex.lock();
			std::queue<std::string>* valueQueue = (std::queue<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			bool error = false;
			instance->valueLocks[cacheKey]->lock();
			if(valueQueue->size()>0)
			{
				valueQueue->pop();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
			if(error)
			{
				throw std::runtime_error(ERR_NOELEMENTS);
			}
		}
		else if(type==LIST || type==VECTOR || type==SET || type==DEQUE)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
}

std::string CacheMap::popGetValueQueue(const std::string& cacheKey) {
	std::string resp;
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==QUEUE)
		{
			instance->mutex.lock();
			std::queue<std::string>* valueQueue = (std::queue<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			bool error = false;
			instance->valueLocks[cacheKey]->lock();
			if(valueQueue->size()>0)
			{
				resp = valueQueue->front();
				valueQueue->pop();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
			if(error)
			{
				throw std::runtime_error(ERR_NOELEMENTS);
			}
		}
		else if(type==LIST || type==VECTOR || type==SET || type==DEQUE)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	return resp;
}

void CacheMap::pushBackValue(const std::string& cacheKey, const std::string& value) {
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueList->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueDeque->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE || type==VECTOR || type==SET)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
}

void CacheMap::pushFrontValue(const std::string& cacheKey, const std::string& value) {
	std::string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueList->push_front(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueDeque->push_front(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE || type==VECTOR || type==SET)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
}

void CacheMap::popFrontValue(const std::string& cacheKey) {
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueList->size()>0)
			{
				valueList->pop_front();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueDeque->size()>0)
			{
				valueDeque->pop_front();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE || type==VECTOR || type==SET)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_NOELEMENTS);
	}
}

void CacheMap::popBackValue(const std::string& cacheKey) {
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueList->size()>0)
			{
				valueList->pop_back();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueDeque->size()>0)
			{
				valueDeque->pop_back();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE || type==VECTOR || type==SET)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_NOELEMENTS);
	}
}

std::string CacheMap::popGetFrontValue(const std::string& cacheKey) {
	std::string resp;
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueList->size()>0)
			{
				resp = valueList->front();
				valueList->pop_front();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueDeque->size()>0)
			{
				resp = valueDeque->front();
				valueDeque->pop_front();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE || type==VECTOR || type==SET)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_NOELEMENTS);
	}
	return resp;
}

std::string CacheMap::popGetBackValue(const std::string& cacheKey) {
	std::string resp;
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueList->size()>0)
			{
				resp = valueList->back();
				valueList->pop_back();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueDeque->size()>0)
			{
				resp = valueDeque->back();
				valueDeque->pop_back();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE || type==VECTOR || type==SET)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_NOELEMENTS);
	}
	return resp;
}

std::string CacheMap::getFrontValue(const std::string& cacheKey) {
	std::string value;
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueVector->size()>0)
			{
				value = valueVector->front();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueList->size()>0)
			{
				value = valueList->front();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			instance->mutex.lock();
			std::queue<std::string>* valueQueue = (std::queue<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueQueue->size()>0)
			{
				value = valueQueue->front();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueDeque->size()>0)
			{
				value = valueDeque->front();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_NOELEMENTS);
	}
	return value;
}

std::string CacheMap::getBackValue(const std::string& cacheKey) {
	std::string value;
	std::string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			std::vector<std::string>* valueVector = (std::vector<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueVector->size()>0)
			{
				value = valueVector->back();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			std::list<std::string>* valueList = (std::list<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueList->size()>0)
			{
				value = valueList->back();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			instance->mutex.lock();
			std::queue<std::string>* valueQueue = (std::queue<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueQueue->size()>0)
			{
				value = valueQueue->back();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			std::deque<std::string>* valueDeque = (std::deque<std::string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(valueDeque->size()>0)
			{
				value = valueDeque->back();
			}
			else
			{
				error = true;
			}
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			throw std::runtime_error(ERR_OPNOTSUPP);
		}
		else
		{
			throw std::runtime_error(ERR_INVCONTAINER);
		}
	}
	else
	{
		throw std::runtime_error(ERR_NOKEYCACHEMAP);
	}
	if(error)
	{
		throw std::runtime_error(ERR_NOELEMENTS);
	}
	return value;
}

CacheMap::~CacheMap() {
}

bool CacheMap::checkObjectExistance(const std::string& key)
{
	bool flag = false;
	instance->objMapMutex.lock();
	flag = instance->objCacheMap.find(key)!=instance->objCacheMap.end();
	instance->objMapMutex.unlock();
	return flag;
}

bool CacheMap::checkExistance(const std::string& cacheKey, const std::string& type)
{
	bool flag = false;
	instance->mutex.lock();
	flag = instance->cacheMap.find(cacheKey)!=instance->cacheMap.end()
					&& (type=="" || (type!="" && instance->valueTypes.find(cacheKey)!=instance->valueTypes.end()
							&& instance->valueTypes[cacheKey]==type));
	instance->mutex.unlock();
	return flag;
}


std::string CacheMap::checkExistanceAndGetType(const std::string& cacheKey)
{
	std::string type;
	instance->mutex.lock();
	if(instance->cacheMap.find(cacheKey)!=instance->cacheMap.end())
	{
		type = instance->valueTypes[cacheKey];
	}
	instance->mutex.unlock();
	return type;
}
