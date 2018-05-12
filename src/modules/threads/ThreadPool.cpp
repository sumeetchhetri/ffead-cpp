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
	pollerStarted = false;
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

void ThreadPool::init(const int& maxThreads)
{
	if(inited)return;
	initPointers();
	this->lowp = -1;
	this->highp = -1;
	this->maxThreads = maxThreads;
	joinComplete = false;
	prioritypooling = false;
	initializeThreads();
	start();
	inited = true;
}

ThreadPool::ThreadPool(const int& maxThreads, const int& lowp, const int& highp) {
	if (lowp > highp)
		throw "Low Priority should be less than Highest Priority";
	initPointers();
	this->maxThreads = maxThreads;
	this->lowp = lowp;
	this->highp = highp;
	this->runFlag = false;
	joinComplete = false;
	prioritypooling = true;
	initializeThreads();
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
	initializeThreads();
}

void ThreadPool::initializeThreads()
{
	if(runFlag)return;
	wpool.init(prioritypooling, allowScheduledTasks);
	for (int i = 0; i < maxThreads; i++) {
		PoolThread *thread = new PoolThread(&wpool);
		thread->execute();
		tpool.push_back(thread);
	}
	runFlag = true;
	wpool.start();
	pollerStarted = false;
}

void ThreadPool::start()
{
	if(pollerStarted)return;
	pollerStarted = true;
}

void ThreadPool::joinAll() {
	while (!joinComplete) {
		/*while (wpool->tasksPending()) {
			Thread::sSleep(1);
		}*/
		for (unsigned int var = 0; var < tpool.size(); var++) {
			tpool.at(var)->stop();
		}
		joinComplete = true;
		for (unsigned int var = 0; var < tpool.size(); var++) {
			joinComplete &= tpool.at(var)->isComplete();
		}
		Thread::sSleep(1);
	}
}

void ThreadPool::submit(Task* task, const int& priority) {
	if(this->prioritypooling) {
		submit(*task, priority);
	} else {
		submit(*task);
	}
}
void ThreadPool::submit(Task &task, const int& priority) {
	if(this->prioritypooling) {
		task.tunit = -1;
		task.type = -1;
		task.priority = priority;
		wpool.addPTask(task);
	} else {
		submit(task);
	}
}
void ThreadPool::submit (Task* task) {
	submit(*task);
}
void ThreadPool::submit (Task &task) {
	task.tunit = -1;
	task.type = -1;
	task.priority = -1;
	wpool.addTask(task);
}

void ThreadPool::schedule(Task* task, const long long& tunit, const int& type) {
	if(this->allowScheduledTasks) {
		schedule(*task, tunit, type);
	} else {
		submit(*task);
	}
}

void ThreadPool::schedule(Task &task, const long long& tunit, const int& type) {
	if(this->allowScheduledTasks) {
		task.tunit = tunit;
		task.type = type;
		task.priority = -1;
		wpool.addSTask(task);
	} else {
		submit(task);
	}
}

void ThreadPool::submit(FutureTask *task, const int& priority) {
	if(this->prioritypooling) {
		submit(*task, priority);
	} else {
		submit(*task);
	}
}

void ThreadPool::submit(FutureTask &task, const int& priority) {
	if(this->prioritypooling) {
		task.tunit = -1;
		task.type = -1;
		task.priority = priority;
		wpool.addPTask(&task);
		task.isFuture = true;
	} else {
		submit(task);
	}
}
void ThreadPool::submit(FutureTask *task) {
	submit(*task);
}

void ThreadPool::submit(FutureTask &task) {
	task.tunit = -1;
	task.type = -1;
	task.priority = -1;
	task.isFuture = true;
	wpool.addTask(&task);
}

void ThreadPool::schedule(FutureTask *task, const long long& tunit, const int& type) {
	if(this->allowScheduledTasks) {
		schedule(*task, tunit, type);
	}
}

void ThreadPool::schedule(FutureTask &task, const long long& tunit, const int& type) {
	if(this->allowScheduledTasks) {
		task.tunit = tunit;
		task.type = type;
		task.priority = -1;
		task.isFuture = true;
		wpool.addSTask(&task);
	}
}

ThreadPool::~ThreadPool() {
	joinAll();
	this->runFlag = false;
	wpool.stop();
	//delete wpool;
	for (int i = 0; i <(int)tpool.size(); i++) {
		delete tpool.at(i);
	}
	//delete tpool;
	//delete m_mutex;
	logger << "Destroyed PoolThread Pool\n" << std::flush;
}
