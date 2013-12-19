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
 * Mutex.cpp
 *
 *  Created on: 10-Aug-2012
 *      Author: sumeetc
 */

#include "Mutex.h"

Mutex::Mutex() {
	pthread_mutex_init(&mut, NULL);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&mut);
}

void Mutex::lock() {
	pthread_mutex_lock(&mut);
}

void Mutex::unlock() {
	pthread_mutex_unlock(&mut);
}

ConditionMutex::ConditionMutex() {
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);
}

ConditionMutex::~ConditionMutex() {
	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&cond);
}

void ConditionMutex::lock() {
	pthread_mutex_lock(&mut);
}

void ConditionMutex::unlock() {
	pthread_mutex_unlock(&mut);
}

void ConditionMutex::wait() {
	pthread_mutex_lock(&mut);
	pthread_cond_wait(&cond, &mut);
	pthread_mutex_unlock(&mut);
}

void ConditionMutex::interrupt() {
	pthread_mutex_lock(&mut);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mut);
}
