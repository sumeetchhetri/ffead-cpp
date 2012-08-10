/*
 * Pthread.cpp
 *
 *  Created on: 10-Aug-2012
 *      Author: sumeetc
 */

#include "Pthread.h"

Pthread::Pthread(PthreadFunc f, void* arg) {
	this->f = f;
	this->arg = arg;
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);
}

Pthread::~Pthread() {
	// TODO Auto-generated destructor stub
}

void Pthread::join() {
	if(pthread_join(pthread, NULL)) {
		throw "Error in join for pthread";
	}
}

void Pthread::nSleep(long nanos) {
	struct timespec req={0},rem={0};
	req.tv_sec = 0;
	req.tv_nsec = nanos;
	int ret = nanosleep(&req, &rem);
	if(ret==-1)
	{
		struct timespec temp_rem;
		ret = nanosleep(&req, &temp_rem);
	}
}

void Pthread::uSleep(long micros) {
	usleep(micros);
}

void Pthread::mSleep(long milis) {
	usleep(milis*1000);
}

void Pthread::sSleep(long seconds) {
	sleep(seconds);
}

void Pthread::wait() {
	pthread_mutex_lock(&mut);
	pthread_cond_wait(&cond, &mut);
	pthread_mutex_unlock(&mut);
}

void Pthread::execute() {
	if(pthread_create(&pthread, NULL, f, arg)) {
		throw "Error Creating pthread";
	}
}

void Pthread::interrupt() {
	pthread_mutex_lock(&mut);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mut);
}
