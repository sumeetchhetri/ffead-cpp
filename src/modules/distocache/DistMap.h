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
 * DistMap.h
 *
 *  Created on: 05-Apr-2013
 *      Author: sumeetc
 */

#ifndef DISTMAP_H_
#define DISTMAP_H_
#include "PooledDistoCacheConnectionFactory.h"
#include "BinarySerialize.h"
#include "map"

template <class K, class V> class DistMap {
	std::string cacheKey;
	DistoCacheClientUtils* cl;
public:
	DistMap(const std::string& cacheKey)
	{
		this->cacheKey = cacheKey;
		cl = PooledDistoCacheConnectionFactory::getConnection();
		try {
			cl->allocate(cacheKey, "map");
		} catch(const std::runtime_error& err) {
			if(std::string(err.what())!="Entry already exists") {
				throw err;
			}
		}
	}
	void insert(const std::pair<K, V>& entry)
	{
		std::string serValue = BinarySerialize::serialize<V>(entry.second, -1);
		cl->addMapEntry(cacheKey, CastUtil::lexical_cast<std::string>(entry.first), serValue);
	}
	const V operator[](const K& key) const
	{
		std::string serValue = cl->getMapEntryValue(cacheKey, CastUtil::lexical_cast<std::string>(key));
		return BinarySerialize::unserialize<V>(serValue, -1);
	}
	V at(const K& key)
	{
		std::string serValue = cl->getMapEntryValue(cacheKey, CastUtil::lexical_cast<std::string>(key));
		return BinarySerialize::unserialize<V>(serValue, -1);
	}
	void erase(const K& key)
	{
		cl->removeMapEntry(cacheKey, CastUtil::lexical_cast<std::string>(key));
	}
	size_t size()
	{
		return cl->size(cacheKey);
	}
	bool isEmpty()
	{
		return cl->isEmpty(cacheKey);
	}
	void clear()
	{
		cl->clear(cacheKey);
	}
	void free()
	{
		cl->deallocate(cacheKey);
	}
	virtual ~DistMap()
	{
		PooledDistoCacheConnectionFactory::releaseConnection(cl);
	}

	class iterator {
			friend class DistMap;
			int position;
			std::string cacheKey;
			DistoCacheClientUtils* cl;
			iterator(std::string cacheKey, DistoCacheClientUtils* cl)
			{
				this->cacheKey = cacheKey;
				this->cl = cl;
				position = -1;
			}
		public:
			iterator()
			{
				position = -1;
				cl = NULL;
			}
			V get()
			{
				if(position>=0)
				{
					std::string serValue = cl->getMapEntryValueByPosition(cacheKey, position);
					return BinarySerialize::unserialize<V>(serValue, -1);
				}
				else
				{
					throw std::runtime_error("Position value is less than 0");
				}
			}
			void set(V v)
			{
				if(position>=0)
				{
					std::string serValue = BinarySerialize::serialize<V>(v, -1);
					cl->setMapEntryValueByPosition(cacheKey, position, serValue);
				}
				else
				{
					throw std::runtime_error("Position value is less than 0");
				}
			}
			iterator& operator++()
			{
				position++;
				return *this;
			}
			iterator& operator+=(int incValue)
			{
				position += incValue;
				return *this;
			}
			iterator operator++(int)
			{
				iterator temp;
				temp.position = position;
				++position;
				return temp;
			}
			iterator& operator--()
			{
				position--;
				return *this;
			}
			iterator& operator-=(int incValue)
			{
				position -= incValue;
				return *this;
			}
			iterator operator--(int)
			{
				iterator temp;
				temp.position = position;
				--position;
				return temp;
			}
			bool operator!=(const iterator& b)
			{
				return position!=b.position;
			}
		};

	iterator begin()
	{
		iterator it(cacheKey, cl);
		it.position = 0;
		return it;
	}
	iterator end()
	{
		iterator it(cacheKey, cl);
		it.position = size();
		return it;
	}
};

#endif /* DISTMAP_H_ */
