/*
 * Pmutex.h
 *
 *  Created on: 10-Aug-2012
 *      Author: sumeetc
 */

#ifndef PMUTEX_H_
#define PMUTEX_H_
#include <pthread.h>

class Pmutex {
	pthread_mutex_t mut;
public:
	Pmutex();
	virtual ~Pmutex();
	void lock();
	void unlock();
};

#endif /* PMUTEX_H_ */
