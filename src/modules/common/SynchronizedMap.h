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
