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
using namespace std;

class CacheMap {
	map<string, void*> cacheMap;
	Mutex objMapMutex;
	map<string, string> objCacheMap;
	map<string, string> valueTypes;
	map<string, Mutex*> valueLocks;
	Mutex mutex;
	static CacheMap* instance;
	CacheMap();
	static bool checkExistance(const string& cacheKey, const string& type="");
	static string checkExistanceAndGetType(const string& cacheKey);
	static bool checkObjectExistance(const string& key);
public:
	static string MAP_ENTRY, COLL_ENTRY, SIZE, CLEAR, IS_EMPTY,
				  ALLOCATE, DEALLOCATE, MAP, SET, LIST, VECTOR,
				  QUEUE, DEQUE, ADD, GET, GET_FRONT, GET_BACK,
				  POP, POP_BACK, POP_FRONT, PUSH_FRONT, PUSH_BACK,
				  INSERT, OBJ_ENTRY, REMOVE, GETBYPOS, POPGET, POP_FRONTGET, POP_BACKGET;
	static string SUCCESS, ERR_NOELEMENTS, ERR_NOKEYCACHEMAP, ERR_INVCONTAINER,
				  ERR_OPNOTSUPP, ERR_INDGRTCONTSIZ, ERR_NOVALUEFOUND, ERR_NOTAMAPCONT,
				  ERR_ALLOCENTEXISTS, ERR_NOVALUESPEC, ERR_NEGATIVEPOS,
				  ERR_POSNOTNUM, ERR_NEGATIVEREP, ERR_REPNOTNUM;

	static void init();
	static void destroy();
	static void allocate(const string&, const string&);
	static void deallocate(const string&);

	static void addObjectEntry(const string& key, const string& value);
	static void removeObjectEntry(const string& key);
	static string getObjectEntryValue(const string&);

	static void addMapEntry(const string&, const string&, const string&);
	static void removeMapEntry(const string&, const string&);
	static string getMapEntryValue(const string&, const string&);
	static string getMapEntryValueByPosition(const string&, const int&);
	static void setMapEntryValueByPosition(const string&, const int&, const string&);

	static void addCollectionEntry(const string&, const string&);
	static void removeCollectionEntryAt(const string&, const int&);
	static string getCollectionEntryAt(const string&, const int&);
	static void setCollectionEntryAt(const string&, const int&, const string&);

	static long size(const string&);
	static bool isEmpty(const string&);
	static void clear(const string&);

	static void insert(const string&, const string&, const int&);
	static void insert(const string&, const string&, const int&, const int&);
	//Queue operations
	static void popValueQueue(const string&);
	static string popGetValueQueue(const string&);
	//Deque/List operations
	static void pushBackValue(const string&, const string&);
	static void pushFrontValue(const string&, const string&);
	static void popFrontValue(const string&);
	static void popBackValue(const string&);
	static string popGetFrontValue(const string&);
	static string popGetBackValue(const string&);
	//Queue/Deque/List/Vector operations
	static string getFrontValue(const string&);
	static string getBackValue(const string&);
	virtual ~CacheMap();
};

#endif /* CACHEMAP_H_ */
