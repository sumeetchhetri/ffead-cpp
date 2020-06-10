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
 * SynchronizedQueue.h
 *
 *  Created on: 08-Mar-2015
 *      Author: sumeetc
 */

#ifndef SYNCHRONIZEDQUEUE_H_
#define SYNCHRONIZEDQUEUE_H_
#include "queue"
#include "Mutex.h"

template<typename T>
class SynchronizedQueue {
	Mutex _m;
	std::queue<T> _q;
public:
	void push(const T& t)
	{
		_m.lock();
		_q.push(t);
		_m.unlock();
	}
	bool pop(T& t)
	{
		bool fl = false;
		_m.lock();
		if(!_q.empty()) {
			t = _q.front();
			_q.pop();
			fl = true;
		}
		_m.unlock();
		return fl;
	}
};

#endif /* SYNCHRONIZEDQUEUE_H_ */
