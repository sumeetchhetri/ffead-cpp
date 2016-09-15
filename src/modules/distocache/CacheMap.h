/*
	Copyright 2009-2013, Sumeet Chhetri

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
 * CacheMap.h
 *
 *  Created on: 27-Mar-2013
 *      Author: sumeetc
 */

#ifndef CACHEMAP_H_
#define CACHEMAP_H_
#include "map"
#include "vector"
#include "set"
#include "queue"
#include "deque"
#include "list"
#include "stack"
#include "string"
#include "Mutex.h"
#include <iterator>


class CacheMap {
	std::map<std::string, void*> cacheMap;
	Mutex objMapMutex;
	std::map<std::string, std::string> objCacheMap;
	std::map<std::string, std::string> valueTypes;
	std::map<std::string, Mutex*> valueLocks;
	Mutex mutex;
	static CacheMap* instance;
	CacheMap();
	static bool checkExistance(const std::string& cacheKey, const std::string& type="");
	static std::string checkExistanceAndGetType(const std::string& cacheKey);
	static bool checkObjectExistance(const std::string& key);
public:
	static std::string MAP_ENTRY, COLL_ENTRY, SIZE, CLEAR, IS_EMPTY,
				  ALLOCATE, DEALLOCATE, MAP, SET, LIST, VECTOR,
				  QUEUE, DEQUE, ADD, GET, GET_FRONT, GET_BACK,
				  POP, POP_BACK, POP_FRONT, PUSH_FRONT, PUSH_BACK,
				  INSERT, OBJ_ENTRY, REMOVE, GETBYPOS, POPGET, POP_FRONTGET, POP_BACKGET;
	static std::string SUCCESS, ERR_NOELEMENTS, ERR_NOKEYCACHEMAP, ERR_INVCONTAINER,
				  ERR_OPNOTSUPP, ERR_INDGRTCONTSIZ, ERR_NOVALUEFOUND, ERR_NOTAMAPCONT,
				  ERR_ALLOCENTEXISTS, ERR_NOVALUESPEC, ERR_NEGATIVEPOS,
				  ERR_POSNOTNUM, ERR_NEGATIVEREP, ERR_REPNOTNUM;

	static void init();
	static void destroy();
	static void allocate(const std::string&, const std::string&);
	static void deallocate(const std::string&);

	static void addObjectEntry(const std::string& key, const std::string& value);
	static void removeObjectEntry(const std::string& key);
	static std::string getObjectEntryValue(const std::string&);

	static void addMapEntry(const std::string&, const std::string&, const std::string&);
	static void removeMapEntry(const std::string&, const std::string&);
	static std::string getMapEntryValue(const std::string&, const std::string&);
	static std::string getMapEntryValueByPosition(const std::string&, const int&);
	static void setMapEntryValueByPosition(const std::string&, const int&, const std::string&);

	static void addCollectionEntry(const std::string&, const std::string&);
	static void removeCollectionEntryAt(const std::string&, const int&);
	static std::string getCollectionEntryAt(const std::string&, const int&);
	static void setCollectionEntryAt(const std::string&, const int&, const std::string&);

	static long size(const std::string&);
	static bool isEmpty(const std::string&);
	static void clear(const std::string&);

	static void insert(const std::string&, const std::string&, const int&);
	static void insert(const std::string&, const std::string&, const int&, const int&);
	//Queue operations
	static void popValueQueue(const std::string&);
	static std::string popGetValueQueue(const std::string&);
	//Deque/List operations
	static void pushBackValue(const std::string&, const std::string&);
	static void pushFrontValue(const std::string&, const std::string&);
	static void popFrontValue(const std::string&);
	static void popBackValue(const std::string&);
	static std::string popGetFrontValue(const std::string&);
	static std::string popGetBackValue(const std::string&);
	//Queue/Deque/List/Vector operations
	static std::string getFrontValue(const std::string&);
	static std::string getBackValue(const std::string&);
	virtual ~CacheMap();
};

#endif /* CACHEMAP_H_ */
