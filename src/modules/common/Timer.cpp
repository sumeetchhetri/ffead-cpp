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
 * Timer.cpp
 *
 *  Created on: Jan 8, 2010
 *      Author: sumeet
 */

#include "Timer.h"

Timer::Timer(bool threadsafe) {
	if(threadsafe) {
		m = new Mutex;
	} else {
		m = new DummyMutex;
	}
	started = false;
}

Timer::~Timer() {
	delete m;
}

void Timer::start()
{
	m->lock();
	clock_gettime(CLOCK_MONOTONIC, &st);
	m->unlock();
	started = true;
}

long long Timer::getCurrentTime()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	return ((en.tv_sec * 1E9) + en.tv_nsec);
}

long long Timer::getTimestamp()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	return en.tv_sec;
}

void Timer::end()
{
	m->lock();
	clock_gettime(CLOCK_MONOTONIC, &en);
	m->unlock();
}
long long Timer::elapsedMicroSeconds()
{
	if(!started)return 0;
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	m->lock();
	long long v = (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec))/1E3;
	m->unlock();
	return v;
}
long long Timer::elapsedMilliSeconds()
{
	if(!started)return 0;
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	m->lock();
	long long v = (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec))/1E6;
	m->unlock();
	return v;
}
long long Timer::elapsedNanoSeconds()
{
	if(!started)return 0;
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	m->lock();
	long long v = (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec));
	m->unlock();
	return v;
}
long long Timer::elapsedSeconds()
{
	if(!started)return 0;
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	m->lock();
	long long v = (en.tv_sec - st.tv_sec);
	m->unlock();
	return v;
}

long long Timer::timerMicroSeconds() {
	if(!started)return 0;
	m->lock();
	long long v = (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec))/1E3;
	m->unlock();
	return v;
}

long long Timer::timerMilliSeconds() {
	if(!started)return 0;
	m->lock();
	long long v = (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec))/1E6;
	m->unlock();
	return v;
}

long long Timer::timerNanoSeconds() {
	if(!started)return 0;
	m->lock();
	long long v = (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec));
	m->unlock();
	return v;
}

long long Timer::timerSeconds() {
	if(!started)return 0;
	m->lock();
	long long v = (en.tv_sec - st.tv_sec);
	m->unlock();
	return v;
}
