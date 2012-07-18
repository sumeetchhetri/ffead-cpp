/*
	Copyright 2010, Sumeet Chhetri 
  
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

ThreadPool::ThreadPool()
{
	logger = Logger::getLogger("ThreadPool");
}
void ThreadPool::init(int initThreads, int maxThreads,bool console)
{
	this->console = console;
	this->lowp = -1;
	this->highp = -1;
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	wpool = new TaskPool;
	wpool->console = console;
	tpool = new vector<Thread*> ;
	for (int i = 0; i < initThreads; i++) {
		Thread *thread = new Thread();
		thread->console = console;
		tpool->push_back(thread);
	}
	poller = new boost::thread(boost::bind(&ThreadPool::poll, this));
}

ThreadPool::ThreadPool(int initThreads, int maxThreads, int lowp, int highp) {
	if (lowp > highp)
		throw "Low Priority should be less than Highest Priority";
	logger = Logger::getLogger("ThreadPool");
	this->console = false;
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	this->lowp = lowp;
	this->highp = highp;
	tpool = new vector<Thread*> ;
	wpool = new TaskPool;
	wpool->console = console;
	for (int i = 0; i < initThreads; i++) {
		Thread *thread = new Thread();
		thread->console = console;
		tpool->push_back(thread);
	}
	poller = new boost::thread(boost::bind(&ThreadPool::poll, this));
	prioritypooling = true;
}
ThreadPool::ThreadPool(int initThreads, int maxThreads, int lowp, int highp,bool console) {
	this->console = console;
	if (lowp > highp)
		throw "Low Priority should be less than Highest Priority";
	logger = Logger::getLogger("ThreadPool");
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	this->lowp = lowp;
	this->highp = highp;
	tpool = new vector<Thread*> ;
	wpool = new TaskPool;
	wpool->console = console;
	for (int i = 0; i < initThreads; i++) {
		Thread *thread = new Thread();
		thread->console = console;
		tpool->push_back(thread);
	}
	poller = new boost::thread(boost::bind(&ThreadPool::poll, this));
	prioritypooling = true;
}

ThreadPool::ThreadPool(int initThreads, int maxThreads) {
	logger = Logger::getLogger("ThreadPool");
	this->lowp = -1;
	this->highp = -1;
	this->console = false;
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	wpool = new TaskPool;
	wpool->console = console;
	tpool = new vector<Thread*> ;
	for (int i = 0; i < initThreads; i++) {
		Thread *thread = new Thread();
		thread->console = console;
		tpool->push_back(thread);
	}
	poller = new boost::thread(boost::bind(&ThreadPool::poll, this));
}

ThreadPool::ThreadPool(int initThreads, int maxThreads,bool console) {
	logger = Logger::getLogger("ThreadPool");
	this->console = console;
	this->lowp = -1;
	this->highp = -1;
	this->initThreads = initThreads;
	this->maxThreads = maxThreads;
	wpool = new TaskPool;
	wpool->console = console;
	tpool = new vector<Thread*> ;
	for (int i = 0; i < initThreads; i++) {
		Thread *thread = new Thread();
		thread->console = console;
		tpool->push_back(thread);
	}
	poller = new boost::thread(boost::bind(&ThreadPool::poll, this));
}

void ThreadPool::poll() {
	while (true) {
		if (!prioritypooling) {
			if (wpool->tasksPending()) {
				Task *task = wpool->getTask();
				Thread *idleThread = getIdleThread();
				idleThread->task = task;
				idleThread->idle = false;
				idleThread->mthread->interrupt();
			} else {
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			}
		} else {
			if (wpool->tasksPPending()) {
				Task *task = wpool->getPTask();
				Thread *idleThread = getIdleThread();
				idleThread->task = task;
				idleThread->idle = false;
				idleThread->mthread->interrupt();
			} else {
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			}
		}
	}
}
Thread* ThreadPool::getIdleThread() {
	while (true) {
		for (unsigned int var = 0; var < tpool->size(); var++) {
			if (tpool->at(var)->idle) {
				return tpool->at(var);
			}
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
}
void ThreadPool::joinAll() {
	while (true) {
		if (!prioritypooling) {
			while (wpool->tasksPending()) {
				boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
			}
		} else {
			while (wpool->tasksPPending()) {
				boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
			}
		}
		int i = 0;
		for (unsigned int var = 0; var < tpool->size(); var++) {
			if (tpool->at(var)->idle) {
				i++;
			}
		}
		if (i == initThreads) {
			break;
		} else {
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
	}
}
void ThreadPool::execute(Task &task, int priority) {
	if(console)
	{
		logger << "Added task to wpool\n" << flush;
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
}
void ThreadPool::execute(Task &task) {
	if(console)
	{
		logger << "Added task to wpool\n" << flush;
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
}
void ThreadPool::schedule(Task &task, int tunit, int type) {
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
}

ThreadPool::~ThreadPool() {
	delete poller;
	delete tpool;
	delete wpool;
	if(console)
	{
		logger << "Destroyed Thread Pool\n" << flush;
	}
}
