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
 * ReusableInstanceHolder.cpp
 *
 *  Created on: 03-Aug-2019
 *      Author: sumeetc
 */

#include "ReusableInstanceHolder.h"

ReusableInstanceHolder::ReusableInstanceHolder(Create crt, Init ini, Destroy des, int limit) {
	this->crt = crt;
	this->ini = ini;
	this->des = des;
	this->limit = limit;
}

void ReusableInstanceHolder::push(void *item) {
	if(limit<=(int)repo.size_approx()) {
		des(item);
	} else {
		repo.enqueue(item);
	}
}

void* ReusableInstanceHolder::pull(void* args) {
	void* item;
	if(!repo.try_dequeue(item)) {
		item = crt(args);
	} else {
		ini(item, args);
	}
	return item;
}

ReusableInstanceHolder::~ReusableInstanceHolder() {
	void* item;
	while(repo.try_dequeue(item)) {
		des(item);
	}
}

