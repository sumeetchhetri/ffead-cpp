/*
 * Mutex.h
 *
 *  Created on: 10-Aug-2012
 *      Author: sumeetc
 */

#ifndef MUTEX_H_
#define MUTEX_H_
#include <pthread.h>

class Mutex {
	pthread_mutex_t mut;
public:
	Mutex();
	virtual ~Mutex();
	void lock();
	void unlock();
};

#endif /* MUTEX_H_ */
