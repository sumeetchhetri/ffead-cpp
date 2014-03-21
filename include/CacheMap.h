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
	static bool checkExistance(string cacheKey, string type="");
	static string checkExistanceAndGetType(string cacheKey);
	static bool checkObjectExistance(string key);
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
	static void allocate(string,string);
	static void deallocate(string);

	static void addObjectEntry(string key, string value);
	static void removeObjectEntry(string key);
	static string getObjectEntryValue(string);

	static void addMapEntry(string, string, string);
	static void removeMapEntry(string, string);
	static string getMapEntryValue(string, string);
	static string getMapEntryValueByPosition(string, int);
	static void setMapEntryValueByPosition(string, int, string);

	static void addCollectionEntry(string, string);
	static void removeCollectionEntryAt(string, int);
	static string getCollectionEntryAt(string, int);
	static void setCollectionEntryAt(string, int, string);

	static long size(string);
	static bool isEmpty(string);
	static void clear(string);

	static void insert(string, string, int);
	static void insert(string, string, int, int);
	//Queue operations
	static void popValueQueue(string);
	static string popGetValueQueue(string);
	//Deque/List operations
	static void pushBackValue(string, string);
	static void pushFrontValue(string, string);
	static void popFrontValue(string);
	static void popBackValue(string);
	static string popGetFrontValue(string);
	static string popGetBackValue(string);
	//Queue/Deque/List/Vector operations
	static string getFrontValue(string);
	static string getBackValue(string);
	virtual ~CacheMap();
};

#endif /* CACHEMAP_H_ */
