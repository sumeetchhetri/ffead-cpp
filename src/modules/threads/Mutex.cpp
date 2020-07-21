/*
	Copyright 2009-2013, Sumeet Chhetri

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
	pthread_mutex_destroy(&mut);
}

void Mutex::lock() {
	pthread_mutex_lock(&mut);
}

void Mutex::unlock() {
	pthread_mutex_unlock(&mut);
}

DummyMutex::DummyMutex() {
}

DummyMutex::~DummyMutex() {
}

void DummyMutex::lock() {
}

void DummyMutex::unlock() {
}

ConditionMutex::ConditionMutex() {
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);
}

ConditionMutex::~ConditionMutex() {
	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&cond);
}

void ConditionMutex::lock() {
	pthread_mutex_lock(&mut);
}

void ConditionMutex::unlock() {
	pthread_mutex_unlock(&mut);
}

void ConditionMutex::wait() {
	pthread_mutex_lock(&mut);
	pthread_cond_wait(&cond, &mut);
	pthread_mutex_unlock(&mut);
}

void ConditionMutex::interrupt() {
	pthread_mutex_lock(&mut);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mut);
}

void ConditionMutex::conditionalWait() {
	pthread_cond_wait(&cond, &mut);
}

void ConditionMutex::conditionalNotifyOne() {
	pthread_cond_signal(&cond);
}

void ConditionMutex::conditionalNotifyAll() {
	pthread_cond_broadcast(&cond);
}

ReadWriteMutex::ReadWriteMutex() {
	pthread_rwlock_init(&mut, NULL);
}

ReadWriteMutex::~ReadWriteMutex() {
	pthread_rwlock_destroy(&mut);
}

void ReadWriteMutex::rlock() {
	pthread_rwlock_rdlock(&mut);
}

void ReadWriteMutex::wlock() {
	pthread_rwlock_wrlock(&mut);
}

void ReadWriteMutex::unlock() {
	pthread_rwlock_unlock(&mut);
}

FileBasedLock::FileBasedLock(const std::string& lkFile) {
	this->lkFile = lkFile;
	fp = NULL;
	remove(lkFile.c_str());
}

FileBasedLock::~FileBasedLock() {
	remove(lkFile.c_str());
}

void FileBasedLock::lock() {
#if !defined(CYGWIN) && !defined(OS_MINGW)
	fl.l_type = F_WRLCK;
	fl.l_len = 0;
	fp = fopen(lkFile.c_str(), "w+");
	if(fp == NULL)
	{
		throw std::runtime_error("Unable to create lock file");
	}
	if( fcntl(fileno(fp), F_SETLK, &fl) == -1)
	{
		throw std::runtime_error("Unable to acquire lock on file");
	}
#endif
}

void FileBasedLock::unlock() {
#if !defined(CYGWIN) && !defined(OS_MINGW)
	if(fcntl(fileno(fp), F_UNLCK, &fl) < 0)
	{
		throw std::runtime_error("Unable to release lock on file");
	}
	fclose(fp);
#endif
}
