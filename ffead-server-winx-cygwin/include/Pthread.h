/*
 * Pthread.h
 *
 *  Created on: 10-Aug-2012
 *      Author: sumeetc
 */

#ifndef PTHREAD_H_
#define PTHREAD_H_
#include <pthread.h>
#include <time.h>
#include <unistd.h>
typedef void* (*PthreadFunc)(void*);

class Pthread {
	PthreadFunc f;
	void* arg;
	pthread_t pthread;
	pthread_cond_t cond;
	pthread_mutex_t mut;
public:
	Pthread(PthreadFunc f, void* arg);
	virtual ~Pthread();
	void execute();
	void join();
	static void nSleep(long nanos);
	static void uSleep(long nanos);
	static void mSleep(long nanos);
	static void sSleep(long nanos);
	void wait();
	void interrupt();
};

#endif /* PTHREAD_H_ */
