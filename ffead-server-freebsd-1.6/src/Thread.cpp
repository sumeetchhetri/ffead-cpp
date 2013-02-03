/*
	Copyright 2009-2012, Sumeet Chhetri

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

Thread::Thread(ThreadFunc f, void* arg) {
	this->f = f;
	this->arg = arg;
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);
}

Thread::~Thread() {
	// TODO Auto-generated destructor stub
}

void Thread::join() {
	if(pthread_join(pthread, NULL)) {
		throw "Error in join for pthread";
	}
}

void Thread::nSleep(long nanos) {
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

void Thread::uSleep(long micros) {
	usleep(micros);
}

void Thread::mSleep(long milis) {
	usleep(milis*1000);
}

void Thread::sSleep(long seconds) {
	sleep(seconds);
}

void Thread::wait() {
	pthread_mutex_lock(&mut);
	pthread_cond_wait(&cond, &mut);
	pthread_mutex_unlock(&mut);
}

void Thread::execute() {
	if(pthread_create(&pthread, NULL, f, arg)) {
		throw "Error Creating pthread";
	}
}

void Thread::interrupt() {
	pthread_mutex_lock(&mut);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mut);
}
