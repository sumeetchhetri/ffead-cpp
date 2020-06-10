/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * SynchronizedMap.h
 *
 *  Created on: 09-Mar-2015
 *      Author: sumeetc
 */

#ifndef SYNCHRONIZEDMAP_H_
#define SYNCHRONIZEDMAP_H_
#include "map"
#include "Mutex.h"



template<typename K, typename V>
class SynchronizedMap {
	Mutex _l;
	std::map<K, V> _m;
public:
	void put(const K& k, const V& v)
	{
		_l.lock();
		_m[k] = v;
		_l.unlock();
	}
	void get(const K& k, V& v)
	{
		_l.lock();
		if(_m.find(k)!=_m.end())
		{
			v = _m[k];
		}
		_l.unlock();
	}
	bool find(const K& k)
	{
		bool fl = false;
		_l.lock();
		if(_m.find(k)!=_m.end())
		{
			fl = true;
		}
		_l.unlock();
		return fl;
	}
	bool find(const K& k, V& v)
	{
		bool fl = false;
		_l.lock();
		if(_m.find(k)!=_m.end())
		{
			v = _m[k];
			fl = true;
		}
		_l.unlock();
		return fl;
	}
	void erase(const K& k)
	{
		_l.lock();
		_m.erase(k);
		_l.unlock();
	}
};

#endif /* SYNCHRONIZEDMAP_H_ */
