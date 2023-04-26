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
 * DistQueue.h
 *
 *  Created on: 04-Apr-2013
 *      Author: sumeetc
 */

#ifndef DISTQUEUE_H_
#define DISTQUEUE_H_
#include "PooledDistoCacheConnectionFactory.h"
#include "BinarySerialize.h"

template <class T> class DistQueue {
	std::string cacheKey;
	DistoCacheClientUtils* cl;
public:
	DistQueue(const std::string& cacheKey)
	{
		this->cacheKey = cacheKey;
		cl = PooledDistoCacheConnectionFactory::getConnection();
		try {
			cl->allocate(cacheKey, "queue");
		} catch(const std::runtime_error& err) {
			if(std::string(err.what())!="Entry already exists") {
				throw err;
			}
		}
	}
	void push(const T& t)
	{
		std::string serValue = BinarySerialize::serialize<T>(t, -1);
		cl->addCollectionEntry(cacheKey, serValue);
	}
	T front()
	{
		std::string serValue = cl->getFrontValue(cacheKey);
		return BinarySerialize::unserialize<T>(serValue, -1);
	}
	T back()
	{
		std::string serValue = cl->getBackValue(cacheKey);
		return BinarySerialize::unserialize<T>(serValue, -1);
	}
	void pop()
	{
		cl->popValueQueue(cacheKey);
	}
	std::string get_pop()
	{
		return cl->popGetValueQueue(cacheKey);
	}
	size_t size()
	{
		return cl->size(cacheKey);
	}
	bool isEmpty()
	{
		return cl->isEmpty(cacheKey);
	}
	void free()
	{
		cl->deallocate(cacheKey);
	}
	virtual ~DistQueue()
	{
		PooledDistoCacheConnectionFactory::releaseConnection(cl);
	}
};

#endif /* DISTQUEUE_H_ */
