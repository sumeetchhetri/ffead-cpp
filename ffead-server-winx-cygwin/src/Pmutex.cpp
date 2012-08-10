/*
 * Pmutex.cpp
 *
 *  Created on: 10-Aug-2012
 *      Author: sumeetc
 */

#include "Pmutex.h"

Pmutex::Pmutex() {
	// TODO Auto-generated constructor stub

}

Pmutex::~Pmutex() {
	// TODO Auto-generated destructor stub
}

void Pmutex::lock() {
	pthread_mutex_lock(&mut);
}

void Pmutex::unlock() {
	pthread_mutex_unlock(&mut);
}


