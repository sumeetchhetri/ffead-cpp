/*
 * ThreadLocal.cpp
 *
 *  Created on: 24-Nov-2014
 *      Author: sumeetc
 */

#include "ThreadLocal.h"

void ThreadLocal::init() {
	pthread_key_create(&_key, NULL);
}

ThreadLocal::ThreadLocal() {
	init();
}

void ThreadLocal::set(void* val) {
	if (pthread_getspecific(_key) == NULL) {
		pthread_setspecific(_key, val);
	}
}

ThreadLocal::ThreadLocal(void* t) {
	init();
	if (pthread_getspecific(_key) == NULL) {
		pthread_setspecific(_key, t);
	}
}

void* ThreadLocal::get() {
	return pthread_getspecific(_key);
}

ThreadLocal::~ThreadLocal() {
	pthread_key_delete(_key);
}

