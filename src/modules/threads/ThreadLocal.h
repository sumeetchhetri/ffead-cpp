/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * ThreadLocal.h
 *
 *  Created on: 24-Nov-2014
 *      Author: sumeetc
 */

#ifndef THREADLOCAL_H_
#define THREADLOCAL_H_
#include "pthread.h"

class ThreadLocal {
	pthread_key_t _key;
	void init();
	friend class CommonUtils;
public:
	ThreadLocal();
	template<typename T> ThreadLocal(const T& t);
	template<typename T> ThreadLocal(T* t);
	ThreadLocal(void* t);
	void set(void*);
	void reset(void*);
	void* get();
	template<typename T> T get();
	template<typename T> T* getPointer();
	virtual ~ThreadLocal();
};

template<typename T>
inline ThreadLocal::ThreadLocal(const T& t) {
	init();
	if (pthread_getspecific(_key) == NULL) {
		pthread_setspecific(_key, &t);
	}
}

template<typename T>
inline ThreadLocal::ThreadLocal(T* t) {
	init();
	if (pthread_getspecific(_key) == NULL) {
		pthread_setspecific(_key, t);
	}
}

template<typename T>
inline T ThreadLocal::get() {
	void* tem = this->get();
	if(tem)
	{
		T* t = (T*)tem;
		return *t;
	}
	T t;
	return t;
}

template<typename T>
inline T* ThreadLocal::getPointer() {
	return (T*)this->get();
}

#endif /* THREADLOCAL_H_ */
