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

string CacheMap::MAP_ENTRY = "mapentry", CacheMap::COLL_ENTRY = "collentry", CacheMap::REMOVE = "remove",
	   CacheMap::SIZE = "size", CacheMap::CLEAR = "clear", CacheMap::IS_EMPTY = "isempty",
	   CacheMap::ALLOCATE = "allocate", CacheMap::DEALLOCATE = "deallocate", CacheMap::MAP = "map",
	   CacheMap::SET = "set", CacheMap::LIST = "list", CacheMap::VECTOR = "vector", CacheMap::QUEUE = "queue",
	   CacheMap::DEQUE = "deque", CacheMap::ADD = "add", CacheMap::GET = "get", CacheMap::GET_FRONT = "getfront",
	   CacheMap::GET_BACK = "getback", CacheMap::POP = "pop", CacheMap::POP_BACK = "popback",
	   CacheMap::POP_FRONT = "popfront", CacheMap::PUSH_FRONT = "pushfront", CacheMap::PUSH_BACK = "pushback",
	   CacheMap::INSERT = "insert", CacheMap::OBJ_ENTRY = "objentry", CacheMap::GETBYPOS = "getbypos",
	   CacheMap::POPGET = "popget", CacheMap::POP_FRONTGET = "popfrontget", CacheMap::POP_BACKGET = "popbackget";

string CacheMap::ERR_NOELEMENTS = "No Elements in container", CacheMap::ERR_NOKEYCACHEMAP = "Unable to find value for Key specified",
	   CacheMap::ERR_INVCONTAINER = "Invalid container specified", CacheMap::ERR_OPNOTSUPP = "Operation not supported on container",
	   CacheMap::ERR_INDGRTCONTSIZ = "Index greater than container size", CacheMap::ERR_NOTAMAPCONT = "Not a map container",
	   CacheMap::ERR_NOVALUEFOUND = "No value found for key", CacheMap::ERR_ALLOCENTEXISTS = "Entry already exists",
	   CacheMap::ERR_NOVALUESPEC = "No value specified", CacheMap::ERR_NEGATIVEPOS = "Position value is less than 0",
	   CacheMap::ERR_POSNOTNUM = "Position value is not a number", CacheMap::ERR_NEGATIVEREP = "Repetition value is less than 0",
	   CacheMap::ERR_REPNOTNUM = "Repetition value is not a number", CacheMap::SUCCESS = "SUCCESS";

CacheMap::CacheMap() {
}

void CacheMap::allocate(string cacheKey, string type) {
	if(checkExistance(cacheKey))
	{
		throw ERR_ALLOCENTEXISTS;
	}

	void *entry = NULL;
	if(type==MAP)
	{
		entry = new map<string, string>;
	}
	else if(type==SET)
	{
		entry = new set<string>;
	}
	else if(type==LIST)
	{
		entry = new list<string>;
	}
	else if(type==VECTOR)
	{
		entry = new vector<string>;
	}
	else if(type==QUEUE)
	{
		entry = new queue<string>;
	}
	else if(type==DEQUE)
	{
		entry = new deque<string>;
	}
	else
	{
		throw ERR_INVCONTAINER;
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

void CacheMap::deallocate(string cacheKey) {
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
		throw ERR_NOKEYCACHEMAP;
	}
}

void CacheMap::addObjectEntry(string key, string value) {
	instance->objMapMutex.lock();
	instance->objCacheMap[key] = value;
	instance->objMapMutex.unlock();
}

void CacheMap::removeObjectEntry(string key) {
	instance->objMapMutex.lock();
	instance->objCacheMap.erase(key);
	instance->objMapMutex.unlock();
}

string CacheMap::getObjectEntryValue(string key) {
	string value;
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
		throw ERR_NOVALUEFOUND;
	}
	return value;
}

void CacheMap::addMapEntry(string cacheKey, string key, string value) {
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		map<string, string>* valueMap = (map<string, string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		valueMap->insert(pair<string, string>(key, value));
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw ERR_NOTAMAPCONT;
	}
}

void CacheMap::removeMapEntry(string cacheKey, string key) {
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		map<string, string>* valueMap = (map<string, string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		valueMap->erase(key);
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw ERR_NOTAMAPCONT;
	}
}

void CacheMap::addCollectionEntry(string cacheKey, string value) {
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueVector = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			set<string>* valueVector = (set<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->insert(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			instance->mutex.lock();
			queue<string>* valueVector = (queue<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->push(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			deque<string>* valueVector = (deque<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
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

string CacheMap::getMapEntryValue(string cacheKey, string key) {
	string value;
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		map<string, string>* valueMap = (map<string, string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		if(valueMap->find(key)!=valueMap->end())
		{
			value = valueMap->find(key)->second;
		}
		else
		{
			throw ERR_NOVALUEFOUND;
		}
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	return value;
}

string CacheMap::getMapEntryValueByPosition(string cacheKey, int position) {
	string value;
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		map<string, string>* valueMap = (map<string, string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		if(position<(int)valueMap->size())
		{
			map<string, string>::iterator it = valueMap->begin();
			advance(it, position);
			value = it->second;
		}
		else
		{
			throw ERR_INDGRTCONTSIZ;
		}
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	return value;
}

void CacheMap::setMapEntryValueByPosition(string cacheKey, int position, string value) {
	if(checkExistance(cacheKey, MAP))
	{
		instance->mutex.lock();
		map<string, string>* valueMap = (map<string, string>*)instance->cacheMap[cacheKey];
		instance->mutex.unlock();

		instance->valueLocks[cacheKey]->lock();
		if(position<(int)valueMap->size())
		{
			map<string, string>::iterator it = valueMap->begin();
			advance(it, position);
			it->second = value;
		}
		else
		{
			throw ERR_INDGRTCONTSIZ;
		}
		instance->valueLocks[cacheKey]->unlock();
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
}

void CacheMap::setCollectionEntryAt(string cacheKey, int position, string value) {
	bool error = false;
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
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
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				list<string>::iterator it = valueList->begin();
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueDeque->size())
			{
				deque<string>::iterator it = valueDeque->begin();
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_INDGRTCONTSIZ;
	}
}

string CacheMap::getCollectionEntryAt(string cacheKey, int position) {
	string value;
	bool error = false;
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
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
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				list<string>::iterator it = valueList->begin();
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
			set<string>* valueSet = (set<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueSet->size())
			{
				set<string>::iterator it = valueSet->begin();
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueDeque->size())
			{
				deque<string>::iterator it = valueDeque->begin();
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_INDGRTCONTSIZ;
	}
	return value;
}

long CacheMap::size(string cacheKey) {
	long value = -1;
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueVector->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueList->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			set<string>* valueSet = (set<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueSet->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			instance->mutex.lock();
			queue<string>* valueQueue = (queue<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueQueue->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueDeque->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==MAP)
		{
			instance->mutex.lock();
			map<string, string>* valueMap = (map<string, string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueMap->size();
			instance->valueLocks[cacheKey]->unlock();
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	return value;
}


void CacheMap::removeCollectionEntryAt(string cacheKey, int position) {
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
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
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				list<string>::iterator it = valueList->begin();
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
			set<string>* valueSet = (set<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueSet->size())
			{
				set<string>::iterator it = valueSet->begin();
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_INDGRTCONTSIZ;
	}
}

bool CacheMap::isEmpty(string cacheKey) {
	bool value = false;
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueVector->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueList->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			set<string>* valueSet = (set<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueSet->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE)
		{
			instance->mutex.lock();
			queue<string>* valueQueue = (queue<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueQueue->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueDeque->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==MAP)
		{
			instance->mutex.lock();
			map<string, string>* valueMap = (map<string, string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			value = valueMap->empty();
			instance->valueLocks[cacheKey]->unlock();
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	return value;
}

void CacheMap::clear(string cacheKey) {
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueVector->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueList->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==SET)
		{
			instance->mutex.lock();
			set<string>* valueSet = (set<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueSet->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueDeque->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==MAP)
		{
			instance->mutex.lock();
			map<string, string>* valueMap = (map<string, string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueMap->clear();
			instance->valueLocks[cacheKey]->unlock();
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
}

void CacheMap::insert(string cacheKey, string value, int position) {
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
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
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				list<string>::iterator it = valueList->begin();
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
			set<string>* valueSet = (set<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueSet->size())
			{
				set<string>::iterator it = valueSet->begin();
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_INDGRTCONTSIZ;
	}
}

void CacheMap::insert(string cacheKey, string value, int position, int repeat) {
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
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
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			if(position<(int)valueList->size())
			{
				list<string>::iterator it = valueList->begin();
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_INDGRTCONTSIZ;
	}
}

void CacheMap::popValueQueue(string cacheKey) {
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==QUEUE)
		{
			instance->mutex.lock();
			queue<string>* valueQueue = (queue<string>*)instance->cacheMap[cacheKey];
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
				throw ERR_NOELEMENTS;
			}
		}
		else if(type==LIST || type==VECTOR || type==SET || type==DEQUE)
		{
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
}

string CacheMap::popGetValueQueue(string cacheKey) {
	string resp;
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==QUEUE)
		{
			instance->mutex.lock();
			queue<string>* valueQueue = (queue<string>*)instance->cacheMap[cacheKey];
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
				throw ERR_NOELEMENTS;
			}
		}
		else if(type==LIST || type==VECTOR || type==SET || type==DEQUE)
		{
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	return resp;
}

void CacheMap::pushBackValue(string cacheKey, string value) {
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueList->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueDeque->push_back(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE || type==VECTOR || type==SET)
		{
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
}

void CacheMap::pushFrontValue(string cacheKey, string value) {
	string type = checkExistanceAndGetType(cacheKey);
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueList->push_front(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==DEQUE)
		{
			instance->mutex.lock();
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
			instance->mutex.unlock();

			instance->valueLocks[cacheKey]->lock();
			valueDeque->push_front(value);
			instance->valueLocks[cacheKey]->unlock();
		}
		else if(type==QUEUE || type==VECTOR || type==SET)
		{
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
}

void CacheMap::popFrontValue(string cacheKey) {
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_NOELEMENTS;
	}
}

void CacheMap::popBackValue(string cacheKey) {
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_NOELEMENTS;
	}
}

string CacheMap::popGetFrontValue(string cacheKey) {
	string resp;
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_NOELEMENTS;
	}
	return resp;
}

string CacheMap::popGetBackValue(string cacheKey) {
	string resp;
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==LIST)
		{
			instance->mutex.lock();
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_NOELEMENTS;
	}
}

string CacheMap::getFrontValue(string cacheKey) {
	string value;
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
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
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
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
			queue<string>* valueQueue = (queue<string>*)instance->cacheMap[cacheKey];
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_NOELEMENTS;
	}
	return value;
}

string CacheMap::getBackValue(string cacheKey) {
	string value;
	string type = checkExistanceAndGetType(cacheKey);
	bool error = false;
	if(type!="")
	{
		if(type==VECTOR)
		{
			instance->mutex.lock();
			vector<string>* valueVector = (vector<string>*)instance->cacheMap[cacheKey];
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
			list<string>* valueList = (list<string>*)instance->cacheMap[cacheKey];
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
			queue<string>* valueQueue = (queue<string>*)instance->cacheMap[cacheKey];
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
			deque<string>* valueDeque = (deque<string>*)instance->cacheMap[cacheKey];
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
			throw ERR_OPNOTSUPP;
		}
		else
		{
			throw ERR_INVCONTAINER;
		}
	}
	else
	{
		throw ERR_NOKEYCACHEMAP;
	}
	if(error)
	{
		throw ERR_NOELEMENTS;
	}
	return value;
}

CacheMap::~CacheMap() {
}

bool CacheMap::checkObjectExistance(string key)
{
	bool flag = false;
	instance->objMapMutex.lock();
	flag = instance->objCacheMap.find(key)!=instance->objCacheMap.end();
	instance->objMapMutex.unlock();
	return flag;
}

bool CacheMap::checkExistance(string cacheKey, string type)
{
	bool flag = false;
	instance->mutex.lock();
	flag = instance->cacheMap.find(cacheKey)!=instance->cacheMap.end()
					&& (type=="" || (type!="" && instance->valueTypes.find(cacheKey)!=instance->valueTypes.end()
							&& instance->valueTypes[cacheKey]==type));
	instance->mutex.unlock();
	return flag;
}


string CacheMap::checkExistanceAndGetType(string cacheKey)
{
	string type;
	instance->mutex.lock();
	if(instance->cacheMap.find(cacheKey)!=instance->cacheMap.end())
	{
		type = instance->valueTypes[cacheKey];
	}
	instance->mutex.unlock();
	return type;
}
