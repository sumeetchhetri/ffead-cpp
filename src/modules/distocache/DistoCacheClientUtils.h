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



class DistoCacheClientUtils {
	static std::string SUCCESS, ERR_NOELEMENTS, ERR_NOKEYCACHEMAP, ERR_INVCONTAINER,
					  ERR_OPNOTSUPP, ERR_INDGRTCONTSIZ, ERR_NOVALUEFOUND, ERR_NOTAMAPCONT,
					  ERR_ALLOCENTEXISTS, ERR_NOVALUESPEC, ERR_NEGATIVEPOS,
					  ERR_POSNOTNUM, ERR_NEGATIVEREP, ERR_REPNOTNUM;
	ClientInterface* client;
	Mutex lock;
	bool inUse;
	friend class BasicDistoCacheConnectionFactory;
	friend class PooledDistoCacheConnectionFactory;
	std::string readValueOrThrowExp(AMEFObject* object, const std::vector<std::string>& ignoreErrors);
public:
	DistoCacheClientUtils(const std::string& host, const int& port, const bool& isSSL= false);
	virtual ~DistoCacheClientUtils();
	void allocate(const std::string&, const std::string&);
	void deallocate(const std::string&);

	void addObjectEntry(const std::string& key, const std::string& value);
	void removeObjectEntry(const std::string& key);
	std::string getObjectEntryValue(const std::string&);

	void addMapEntry(const std::string&, const std::string&, const std::string&);
	void removeMapEntry(const std::string&, const std::string&);
	std::string getMapEntryValue(const std::string&, const std::string&);
	std::string getMapEntryValueByPosition(const std::string&, const int&);
	void setMapEntryValueByPosition(const std::string&, const int&, const std::string&);

	void addCollectionEntry(const std::string&, const std::string&);
	void removeCollectionEntryAt(const std::string&, const int&);
	std::string getCollectionEntryAt(const std::string&, const int&);
	void setCollectionEntryAt(const std::string&, const int&, const std::string&);

	size_t size(const std::string&);
	bool isEmpty(const std::string&);
	void clear(const std::string&);

	void insert(const std::string&, const std::string&, const int&);
	void insert(const std::string&, const std::string&, const int&, const int&);
	//Queue operations
	void popValueQueue(const std::string&);
	std::string popGetValueQueue(const std::string&);
	//Deque/List operations
	void pushBackValue(const std::string&, const std::string&);
	void pushFrontValue(const std::string&, const std::string&);
	void popFrontValue(const std::string&);
	void popBackValue(const std::string&);
	std::string popGetFrontValue(const std::string&);
	std::string popGetBackValue(const std::string&);
	//Queue/Deque/List/Vector operations
	std::string getFrontValue(const std::string&);
	std::string getBackValue(const std::string&);
};

#endif /* DISTOCACHECLIENTUTILS_H_ */
