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
 * DistoCacheClientUtils.h
 *
 *  Created on: 04-Apr-2013
 *      Author: sumeetc
 */

#ifndef DISTOCACHECLIENTUTILS_H_
#define DISTOCACHECLIENTUTILS_H_
#include "Client.h"
#include "SSLClient.h"
#include "AMEFEncoder.h"
#include "AMEFDecoder.h"
#include <exception>

using namespace std;

class DistoCacheClientUtils {
	static string SUCCESS, ERR_NOELEMENTS, ERR_NOKEYCACHEMAP, ERR_INVCONTAINER,
					  ERR_OPNOTSUPP, ERR_INDGRTCONTSIZ, ERR_NOVALUEFOUND, ERR_NOTAMAPCONT,
					  ERR_ALLOCENTEXISTS, ERR_NOVALUESPEC, ERR_NEGATIVEPOS,
					  ERR_POSNOTNUM, ERR_NEGATIVEREP, ERR_REPNOTNUM;
	ClientInterface* client;
	Mutex lock;
	bool inUse;
	friend class BasicDistoCacheConnectionFactory;
	friend class PooledDistoCacheConnectionFactory;
	string readValueOrThrowExp(AMEFObject* object, vector<string> ignoreErrors);
public:
	DistoCacheClientUtils(string host, int port, bool isSSL = false);
	virtual ~DistoCacheClientUtils();
	void allocate(string,string);
	void deallocate(string);

	void addObjectEntry(string key, string value);
	void removeObjectEntry(string key);
	string getObjectEntryValue(string);

	void addMapEntry(string, string, string);
	void removeMapEntry(string, string);
	string getMapEntryValue(string, string);
	string getMapEntryValueByPosition(string, int);
	void setMapEntryValueByPosition(string, int, string);

	void addCollectionEntry(string, string);
	void removeCollectionEntryAt(string, int);
	string getCollectionEntryAt(string, int);
	void setCollectionEntryAt(string, int, string);

	size_t size(string);
	bool isEmpty(string);
	void clear(string);

	void insert(string, string, int);
	void insert(string, string, int, int);
	//Queue operations
	void popValueQueue(string);
	string popGetValueQueue(string);
	//Deque/List operations
	void pushBackValue(string, string);
	void pushFrontValue(string, string);
	void popFrontValue(string);
	void popBackValue(string);
	string popGetFrontValue(string);
	string popGetBackValue(string);
	//Queue/Deque/List/Vector operations
	string getFrontValue(string);
	string getBackValue(string);
};

#endif /* DISTOCACHECLIENTUTILS_H_ */
