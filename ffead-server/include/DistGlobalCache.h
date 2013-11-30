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
 * DistGlobalCache.h
 *
 *  Created on: 07-Apr-2013
 *      Author: sumeetc
 */

#ifndef DISTGLOBALCACHE_H_
#define DISTGLOBALCACHE_H_
#include "PooledDistoCacheConnectionFactory.h"
#include "BinarySerialize.h"

class DistGlobalCache {
	DistoCacheClientUtils* cl;
public:
	DistGlobalCache()
	{
		cl = PooledDistoCacheConnectionFactory::getConnection();
	}
	template <class T> void add(string key, T value)
	{
		string serValue = BinarySerialize::serialize<T>(value);
		cl->addObjectEntry(key, serValue);
	}
	template <class K, class V> void addMap(string key, map<K,V> value)
	{
		string serValue = BinarySerialize::serialize<K,V>(value);
		cl->addObjectEntry(key, serValue);
	}
	template <class T> T get(string key)
	{
		string serValue = cl->getObjectEntryValue(key);
		return BinarySerialize::unserialize<T>(serValue);
	}
	template <class K, class V> map<K, V> getMap(string key)
	{
		string serValue = cl->getObjectEntryValue(key);
		return BinarySerialize::unSerializeToMap<K, V>(serValue);
	}
	void erase(string key)
	{
		cl->removeObjectEntry(key);
	}
	~DistGlobalCache()
	{
		PooledDistoCacheConnectionFactory::releaseConnection(cl);
	}
};

#endif /* DISTGLOBALCACHE_H_ */
