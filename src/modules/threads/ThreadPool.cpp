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
 * ThreadPool.cpp
 *
 *  Created on: Mar 23, 2010
 *      Author: sumeet
 */
#include "ThreadPool.h"

void ThreadPool::initPointers()
{
	inited = false;
	prioritypooling = false;
	maxThreads = 0;
	lowp = 0;
	highp = 0;
	this->runFlag = false;
	joinComplete = false;
	this->allowScheduledTasks = false;
	logger = LoggerFactory::getLogger("ThreadPool");
}

ThreadPool::ThreadPool()
{
	initPointers();
}

void ThreadPool::init(const int& maxThreads, const bool& setAffinity)
{
	if(inited)return;
	currentThread = 0;
	initPointers();
	this->lowp = -1;
	this->highp = -1;
	this->maxThreads = maxThreads;
	joinComplete = false;
	prioritypooling = false;
	initializeThreads(setAffinity);
	inited = true;
}

ThreadPool::ThreadPool(const int& maxThreads, const int& lowp, const int& highp) {
	if (lowp > highp)
		throw std::runtime_error("Low Priority should be less than Highest Priority");
	initPointers();
	this->maxThreads = maxThreads;
	this->lowp = lowp;
	this->highp = highp;
	this->runFlag = false;
	joinComplete = false;
	prioritypooling = true;
	initializeThreads(false);
}

ThreadPool::ThreadPool(const int& maxThreads, const bool& allowScheduledTasks /* = false*/) {
	initPointers();
	this->lowp = -1;
	this->highp = -1;
	this->maxThreads = maxThreads;
	this->runFlag = false;
	joinComplete = false;
	prioritypooling = false;
	this->allowScheduledTasks = allowScheduledTasks;
	initializeThreads(false);
}

void ThreadPool::initializeThreads(const bool& setAffinity)
{
	if(runFlag)return;
	if(prioritypooling || allowScheduledTasks) {
		wpool.init(prioritypooling, allowScheduledTasks);
	}
	for (int i = 0; i < maxThreads; i++) {
		PoolThread* thread = NULL;
		if(prioritypooling || allowScheduledTasks) {
			thread = new PoolThread(&wpool, i+1);
		} else {
			thread = new PoolThread(i+1);
		}
		thread->execute(setAffinity?i:-1);
		tpool.push_back(thread);
	}
	runFlag = true;
	if(prioritypooling || allowScheduledTasks) {
		wpool.start();
	}
}

void ThreadPool::joinAll() {
	while (!joinComplete) {
		/*while (wpool->tasksPending()) {
			Thread::sSleep(1);
		}*/
		for (int var = 0; var < maxThreads; var++) {
			tpool.at(var)->stop();
		}
		joinComplete = true;
		for (int var = 0; var < maxThreads; var++) {
			joinComplete &= tpool.at(var)->isComplete();
		}
		Thread::sSleep(1);
	}
}

void ThreadPool::submit(Task* task) {
	if(task->getTid()!=-1) {
		tpool.at(task->getTid())->addTask(task);
		return;
	}
	//https://stackoverflow.com/questions/33554255/c-thread-safe-increment-with-modulo-without-mutex-using-stdatomic
	int index = currentThread ++;
	int id = index % maxThreads;
	// If size could wrap, then re-write the modulo value.
	// oldValue keeps getting re-read.
	// modulo occurs when nothing else updates it.
	int oldValue = currentThread;
	int newValue = oldValue % maxThreads;
	while (!currentThread.compare_exchange_weak( oldValue, newValue, std::memory_order_relaxed ))
		newValue = oldValue % maxThreads;
	task->setTid(id);
	tpool.at(id)->addTask(task);
}
void ThreadPool::submit(Task* task, const int& priority) {
	if(this->prioritypooling) {
		task->tunit = -1;
		task->type = -1;
		task->priority = priority;
		wpool.addPTask(task);
	} else {
		submit(task);
	}
}
void ThreadPool::schedule(Task* task, const long long& tunit, const int& type) {
	if(this->allowScheduledTasks) {
		task->tunit = tunit;
		task->type = type;
		task->priority = -1;
		wpool.addSTask(task);
	} else {
		submit(task);
	}
}

void ThreadPool::submit(FutureTask* task) {
	/*if(task->getTid()!=-1) {
		tpool.at(task->getTid())->addTask(task);
		return;
	}*/
	//https://stackoverflow.com/questions/33554255/c-thread-safe-increment-with-modulo-without-mutex-using-stdatomic
	int index = currentThread ++;
	int id = index % maxThreads;
	// If size could wrap, then re-write the modulo value.
	// oldValue keeps getting re-read.
	// modulo occurs when nothing else updates it.
	int oldValue = currentThread;
	int newValue = oldValue % maxThreads;
	while (!currentThread.compare_exchange_weak( oldValue, newValue, std::memory_order_relaxed ))
		newValue = oldValue % maxThreads;
	//task->setTid(id);
	tpool.at(id)->addTask(task);
}
void ThreadPool::submit(FutureTask* task, const int& priority) {
	if(this->prioritypooling) {
		task->tunit = -1;
		task->type = -1;
		task->priority = priority;
		wpool.addPTask(task);
		task->isFuture = true;
	} else {
		submit(task);
	}
}
void ThreadPool::schedule(FutureTask* task, const long long& tunit, const int& type) {
	if(this->allowScheduledTasks) {
		task->tunit = tunit;
		task->type = type;
		task->priority = -1;
		task->isFuture = true;
		wpool.addSTask(task);
	} else {
		submit(task);
	}
}

ThreadPool::~ThreadPool() {
	joinAll();
	this->runFlag = false;
	wpool.stop();
	for (int i = 0; i <maxThreads; i++) {
		delete tpool.at(i);
	}
	logger << "Destroyed ThreadPool\n" << std::flush;
}
