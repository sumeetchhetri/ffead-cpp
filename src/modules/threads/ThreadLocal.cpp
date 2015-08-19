/*
 * ThreadLocal.cpp
 *
 *  Created on: 24-Nov-2014
 *      Author: sumeetc
 */

#include "ThreadLocal.h"

void ThreadLocal::init() {
	_value = NULL;
	pthread_key_create(&_key, NULL);
}

ThreadLocal::ThreadLocal() {
	init();
}

void ThreadLocal::set(void* val) {
	_value = val;
	if (pthread_getspecific(_key) == NULL) {
		pthread_setspecific(_key, _value);
	}
}

ThreadLocal::ThreadLocal(void* t) {
	init();
	_value = t;
	if (pthread_getspecific(_key) == NULL) {
		pthread_setspecific(_key, _value);
	}
}

void* ThreadLocal::get() {
	return _value;
}

ThreadLocal::~ThreadLocal() {
	if(_value) {
		delete _value;
	}
	pthread_key_delete(_key);
	_value = NULL;
}

