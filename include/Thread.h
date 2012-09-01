/*
 * Thread.h
 *
 *  Created on: 10-Aug-2012
 *      Author: sumeetc
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <pthread.h>
#include <time.h>
#include <unistd.h>
typedef void* (*ThreadFunc)(void*);

class Thread {
	ThreadFunc f;
	void* arg;
	pthread_t pthread;
	pthread_cond_t cond;
	pthread_mutex_t mut;
public:
	Thread(ThreadFunc f, void* arg);
	virtual ~Thread();
	void execute();
	void join();
	static void nSleep(long nanos);
	static void uSleep(long nanos);
	static void mSleep(long nanos);
	static void sSleep(long nanos);
	void wait();
	void interrupt();
};

#endif /* THREAD_H_ */
