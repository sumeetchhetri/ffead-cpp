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
 * ReusableInstanceHolder.h
 *
 *  Created on: 03-Aug-2019
 *      Author: sumeetc
 */

#ifndef SRC_MODULES_COMMON_REUSABLEINSTANCEHOLDER_H_
#define SRC_MODULES_COMMON_REUSABLEINSTANCEHOLDER_H_

#include "concurrentqueue.h"

typedef void* (*Create)(void* args);
typedef void (*Init)(void*, void*);
typedef void (*Destroy)(void*);

class ReusableInstanceHolder {
	Create crt;
	Init ini;
	Destroy des;
	int limit;
	moodycamel::ConcurrentQueue<void*> repo;
public:
	void push(void* in);
	void* pull(void* args);
	ReusableInstanceHolder(Create crt, Init ini, Destroy des, int limit);
	virtual ~ReusableInstanceHolder();
};

#endif /* SRC_MODULES_COMMON_REUSABLEINSTANCEHOLDER_H_ */
