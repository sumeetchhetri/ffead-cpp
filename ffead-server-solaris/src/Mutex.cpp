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
	// TODO Auto-generated destructor stub
}

void Mutex::lock() {
	pthread_mutex_lock(&mut);
}

void Mutex::unlock() {
	pthread_mutex_unlock(&mut);
}


