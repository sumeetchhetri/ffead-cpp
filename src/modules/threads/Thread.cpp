/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * Thread.cpp
 *
 *  Created on: 10-Aug-2012
 *      Author: sumeetc
 */

#include "Thread.h"


void* Thread::_service(void* arg)
{
	ThreadFunctor* threadFunctor  = static_cast<ThreadFunctor*>(arg);
	void* ret = threadFunctor->f(threadFunctor->arg);
	Thread* _t = threadFunctor->_t;
	delete _t;
	pthread_exit(NULL);
	return ret;
}

Thread::Thread(const ThreadFunc& f, void* arg, const bool& isDetached /*= true*/) {
	#ifndef OS_MINGW
		this->pthread = 0;
	#endif
	this->isDetached = isDetached;
	this->threadFunctor = new ThreadFunctor();
	this->threadFunctor->_t = this;
	this->threadFunctor->f = f;
	this->threadFunctor->arg = arg;
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);
}

Thread::~Thread() {
	//pthread_join(pthread, NULL);
	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&cond);
	delete threadFunctor;
}

void Thread::join() {
	if(!isDetached && pthread_join(pthread, NULL)) {
		perror("pthread_join");
		throw std::runtime_error("Error in join for pthread");
	}
}

void Thread::nSleep(const long& nanos) {
/*#ifdef HAVE_CLOCK_NANOSLEEP
	struct timespec deadline;
	clock_gettime(CLOCK_MONOTONIC, &deadline);

	// Add the time you want to sleep
	deadline.tv_nsec += nanos;

	// Normalize the time to account for the second boundary
	if(deadline.tv_nsec >= 1000000000) {
		deadline.tv_nsec -= 1000000000;
		deadline.tv_sec++;
	}
	clock_nanosleep(CLOCK_REALTIME, 0, &deadline, NULL);
#else*/
	struct timespec deadline = {0};
	deadline.tv_sec  = 0;
	deadline.tv_nsec = nanos;
	nanosleep(&deadline , (struct timespec *)NULL);
//#endif
}

void Thread::uSleep(const long& micros) {
	long rem = 0;
	if(micros>1000000) {
		sleep(micros/1000000);
		rem = micros%1000000;
	}
	nSleep(rem*1000);
}

void Thread::mSleep(const long& milis) {
	long rem = 0;
	if(milis>1000) {
		sleep(milis/1000);
		rem = milis%1000;
	}
	nSleep(rem*1000*1000);
}

void Thread::sSleep(const long& seconds) {
	sleep(seconds);
}

void Thread::wait() {
	pthread_mutex_lock(&mut);
	pthread_cond_wait(&cond, &mut);
	pthread_mutex_unlock(&mut);
}

void Thread::execute(int cid) {
	if(pthread_create(&pthread, NULL, _service, this->threadFunctor)) {
		perror("pthread_create");
		throw std::runtime_error("Error Creating pthread");
	}
	if(isDetached) {
		pthread_detach(pthread);
	}
#if !defined(CYGWIN) && !defined(OS_MINGW)
	if(cid>=0) {
#ifdef OS_BSD
		cpuset_t cpuset;
#else
		cpu_set_t cpuset;
#endif
		CPU_ZERO(&cpuset);
		CPU_SET(cid, &cpuset);
		if (pthread_setaffinity_np(pthread, sizeof(cpuset), &cpuset) != 0) {
			//throw std::runtime_error("pthread_setaffinity_np");
		}
	}
#endif
}

void Thread::interrupt() {
	pthread_mutex_lock(&mut);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mut);
}
