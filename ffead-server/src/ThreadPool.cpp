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
	m_mutex = NULL;
	poller = NULL;
	tpool = NULL;
	wpool = NULL;
	this->runFlag = false;
	joinComplete = false;
	logger = LoggerFactory::getLogger("ThreadPool");
}

ThreadPool::ThreadPool()
{
	initPointers();
}

void ThreadPool::init(int initThreads, int maxThreads,bool console)
{
	if(wpool!=NULL)return;
	this->console = console;
	this->lowp = -1;
	this->highp = -1;
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	joinComplete = false;
	prioritypooling = false;
	initPointers();
	initializeThreads();
	start();
}

ThreadPool::ThreadPool(int initThreads, int maxThreads, int lowp, int highp) {
	if (lowp > highp)
		throw "Low Priority should be less than Highest Priority";
	this->console = false;
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	this->lowp = lowp;
	this->highp = highp;
	this->runFlag = false;
	joinComplete = false;
	prioritypooling = true;
	initPointers();
	initializeThreads();
}
ThreadPool::ThreadPool(int initThreads, int maxThreads, int lowp, int highp,bool console) {
	this->console = console;
	if (lowp > highp)
		throw "Low Priority should be less than Highest Priority";
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	this->lowp = lowp;
	this->highp = highp;
	this->runFlag = false;
	joinComplete = false;
	prioritypooling = true;
	initPointers();
	initializeThreads();
}

ThreadPool::ThreadPool(int initThreads, int maxThreads) {
	this->lowp = -1;
	this->highp = -1;
	this->console = false;
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	this->runFlag = false;
	joinComplete = false;
	prioritypooling = false;
	initPointers();
	initializeThreads();
}

ThreadPool::ThreadPool(int initThreads, int maxThreads,bool console) {
	this->console = console;
	this->lowp = -1;
	this->highp = -1;
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	this->runFlag = false;
	joinComplete = false;
	prioritypooling = false;
	initPointers();
	initializeThreads();
}

void ThreadPool::initializeThreads()
{
	if(runFlag)return;
	wpool = new TaskPool;
	wpool->console = console;
	tpool = new vector<PoolThread*>;
	for (int i = 0; i < initThreads; i++) {
		PoolThread *thread = new PoolThread(console);
		thread->execute();
		tpool->push_back(thread);
	}
	runFlag = true;
	poller = new Thread(&ThreadPool::poll, this);
	wpool->start();
	pollerStarted = false;
	complete = false;
	m_mutex = new ConditionMutex ;
}

void ThreadPool::start()
{
	if(pollerStarted)return;
	poller->execute();
	pollerStarted = true;
}

void* ThreadPool::poll(void *arg) {
	ThreadPool* ths = (ThreadPool*)arg;
	ths->m_mutex->lock();
	bool fl = ths->runFlag;
	ths->m_mutex->unlock();
	while (fl) {
		//ths->m_mutex->wait();
		if (!ths->prioritypooling) {
			while(ths->wpool->tasksPending())
			{
				Task *task = ths->wpool->getTask();
				if(task!=NULL)
				{
					ths->submitInternal(task);
				}
				Thread::mSleep(1);
			}
		} else if (ths->prioritypooling) {
			while(ths->wpool->tasksPPending()) {
				Task *task = ths->wpool->getPTask();
				if(task!=NULL)
				{
					ths->submitInternal(task);
				}
				Thread::mSleep(1);
			}
		}
		Thread::mSleep(1);
		ths->m_mutex->lock();
		fl = ths->runFlag;
		ths->m_mutex->unlock();
	}
	ths->m_mutex->lock();
	ths->complete = true;
	ths->m_mutex->unlock();
	return NULL;
}

void ThreadPool::submitInternal(Task *task) {
	bool flag = true;
	while (flag) {
		for (unsigned int var = 0; var < tpool->size(); var++) {
			if (tpool->at(var)->isIdle()) {
				tpool->at(var)->checkout(task);
				flag = false;
				break;
			}
		}
		Thread::mSleep(1);
	}
}

void ThreadPool::joinAll() {
	while (!joinComplete) {
		if (!prioritypooling) {
			while (wpool->tasksPending()) {
				Thread::sSleep(1);
			}
		} else {
			while (wpool->tasksPPending()) {
				Thread::sSleep(1);
			}
		}
		int i = 0;
		for (unsigned int var = 0; var < tpool->size(); var++) {
			if (tpool->at(var)->isIdle()) {
				i++;
			}
		}
		if (i == initThreads) {
			joinComplete = true;
			break;
		} else {
			Thread::sSleep(1);
		}
	}
}

void ThreadPool::submit(Task* task, int priority) {
	submit(*task, priority);
}
void ThreadPool::submit(Task &task, int priority) {
	if(console)
	{
		logger << "Adding task to wpool\n" << flush;
	}
	task.tunit = -1;
	task.type = -1;
	task.priority = priority;
	task.console = console;
	if (!prioritypooling) {
		wpool->addTask(task);
	} else {
		wpool->addPTask(task);
	}
	//m_mutex->interrupt();
}
void ThreadPool::submit (Task* task) {
	submit(*task);
}
void ThreadPool::submit (Task &task) {
	if(console)
	{
		logger << "Adding task to wpool\n" << flush;
	}
	task.tunit = -1;
	task.type = -1;
	task.priority = -1;
	task.console = console;
	if (!prioritypooling) {
		wpool->addTask(task);
	} else {
		wpool->addPTask(task);
	}
	//m_mutex->interrupt();
}
void ThreadPool::schedule(Task* task, long long tunit, int type) {
	schedule(*task, tunit, type);
}
void ThreadPool::schedule(Task &task, long long tunit, int type) {
	if(console)
	{
		logger << "Added task to wpool\n" << flush;
	}
	task.tunit = tunit;
	task.type = type;
	task.priority = -1;
	task.console = console;
	if (!prioritypooling) {
		wpool->addTask(task);
	} else {
		wpool->addPTask(task);
	}
	//m_mutex->interrupt();
}

void ThreadPool::submit(FutureTask *task, int priority) {
	submit(*task, priority);
}
void ThreadPool::submit(FutureTask &task, int priority) {
	if(console)
	{
		logger << "Adding task to wpool\n" << flush;
	}
	task.tunit = -1;
	task.type = -1;
	task.priority = priority;
	task.console = console;
	if (!prioritypooling) {
		wpool->addTask(&task);
	} else {
		wpool->addPTask(&task);
	}
	task.isFuture = true;
	//m_mutex->interrupt();
}
void ThreadPool::submit(FutureTask *task) {
	submit(*task);
}
void ThreadPool::submit(FutureTask &task) {
	if(console)
	{
		logger << "Adding task to wpool\n" << flush;
	}
	task.tunit = -1;
	task.type = -1;
	task.priority = -1;
	task.console = console;
	if (!prioritypooling) {
		wpool->addTask(&task);
	} else {
		wpool->addPTask(&task);
	}
	task.isFuture = true;
	//m_mutex->interrupt();
}
void ThreadPool::schedule(FutureTask *task, long long tunit, int type) {
	schedule(*task, tunit, type);
}
void ThreadPool::schedule(FutureTask &task, long long tunit, int type) {
	if(console)
	{
		logger << "Added task to wpool\n" << flush;
	}
	task.tunit = tunit;
	task.type = type;
	task.priority = -1;
	task.console = console;
	if (!prioritypooling) {
		wpool->addTask(&task);
	} else {
		wpool->addPTask(&task);
	}
	task.isFuture = true;
	//m_mutex->interrupt();
}

ThreadPool::~ThreadPool() {
	while(!joinComplete) {
		joinAll();
		Thread::mSleep(1);
	}
	this->m_mutex->lock();
	this->runFlag = false;
	this->m_mutex->unlock();

	m_mutex->lock();
	bool fl = this->complete;
	m_mutex->unlock();
	while(!fl)
	{
		m_mutex->lock();
		fl = this->complete;
		m_mutex->unlock();
		Thread::sSleep(1);
	}
	delete poller;
	delete wpool;
	for (int i = 0; i <(int)tpool->size(); i++) {
		delete tpool->at(i);
	}
	delete m_mutex;
	if(console)
	{
		logger << "Destroyed PoolThread Pool\n" << flush;
	}
}
