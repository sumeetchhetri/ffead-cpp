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
	string readValueOrThrowExp(AMEFObject* object, const vector<string>& ignoreErrors);
public:
	DistoCacheClientUtils(const string& host, const int& port, const bool& isSSL= false);
	virtual ~DistoCacheClientUtils();
	void allocate(const string&, const string&);
	void deallocate(const string&);

	void addObjectEntry(const string& key, const string& value);
	void removeObjectEntry(const string& key);
	string getObjectEntryValue(const string&);

	void addMapEntry(const string&, const string&, const string&);
	void removeMapEntry(const string&, const string&);
	string getMapEntryValue(const string&, const string&);
	string getMapEntryValueByPosition(const string&, const int&);
	void setMapEntryValueByPosition(const string&, const int&, const string&);

	void addCollectionEntry(const string&, const string&);
	void removeCollectionEntryAt(const string&, const int&);
	string getCollectionEntryAt(const string&, const int&);
	void setCollectionEntryAt(const string&, const int&, const string&);

	size_t size(const string&);
	bool isEmpty(const string&);
	void clear(const string&);

	void insert(const string&, const string&, const int&);
	void insert(const string&, const string&, const int&, const int&);
	//Queue operations
	void popValueQueue(const string&);
	string popGetValueQueue(const string&);
	//Deque/List operations
	void pushBackValue(const string&, const string&);
	void pushFrontValue(const string&, const string&);
	void popFrontValue(const string&);
	void popBackValue(const string&);
	string popGetFrontValue(const string&);
	string popGetBackValue(const string&);
	//Queue/Deque/List/Vector operations
	string getFrontValue(const string&);
	string getBackValue(const string&);
};

#endif /* DISTOCACHECLIENTUTILS_H_ */
