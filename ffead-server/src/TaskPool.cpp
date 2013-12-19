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
 * TaskPool.cpp
 *
 *  Created on: Mar 23, 2010
 *      Author: sumeet
 */

#include "TaskPool.h"

void* TaskPool::run(void *arg)
{
	TaskPool* pool = (TaskPool*)arg;
	vector<Task*>::iterator iter;
	pool->m_mutex->lock();
	bool fl = pool->runFlag;
	pool->m_mutex->unlock();
	while(fl)
	{
		pool->m_mutex->wait();
		pool->m_mutex->lock();
		int total = pool->scheduledtasks->size();
		pool->m_mutex->unlock();
		std::queue<int> tobeRemoved;
		for (int i=0; i<total;i++) {
			pool->m_mutex->lock();
			Task* task = pool->scheduledtasks->at(i);
			Timer* timer = pool->scheduledTimers->at(i);
			pool->m_mutex->unlock();
			if(task!=NULL)
			{
				if(task->isWaitOver(timer))
				{
					tobeRemoved.push(i);
					pool->m_mutex->lock();
					pool->tasks->push(task);
					pool->m_mutex->unlock();
				}
			}
		}
		int counter = 0;
		while (!tobeRemoved.empty()) {
			int index = tobeRemoved.front() - counter;
			counter++;
			tobeRemoved.pop();
			pool->m_mutex->lock();
			Timer* timer = pool->scheduledTimers->at(index);
			pool->scheduledtasks->erase(pool->scheduledtasks->begin()+index);
			pool->scheduledTimers->erase(pool->scheduledTimers->begin()+index);
			delete timer;
			pool->m_mutex->unlock();
		}
		Thread::mSleep(1);
		pool->m_mutex->lock();
		fl = pool->runFlag;
		pool->m_mutex->unlock();
	}
	pool->m_mutex->lock();
	pool->complete = true;
	pool->m_mutex->unlock();
	return NULL;
}

TaskPool::TaskPool() {
	m_mutex = new ConditionMutex();
	tasks = new std::queue<Task*>;
	ptasks = new list<Task*>;
	scheduledtasks = new vector<Task*>;
	scheduledTimers = new vector<Timer*>;
	runFlag = true;
	complete = false;
	mthread = new Thread(&run, this);
	thrdStarted = false;
}

void TaskPool::start() {
	if(thrdStarted)return;
	mthread->execute();
	thrdStarted = true;
}

void TaskPool::addTask(Task &task) {
	if (task.type >= 0 && task.type <= 6 && task.tunit > 0)
	{
		m_mutex->lock();
		Timer* t = new Timer;
		t->start();
		scheduledTimers->push_back(t);
		scheduledtasks->push_back(&task);
		m_mutex->unlock();
		m_mutex->interrupt();
	}
	else
	{
		m_mutex->lock();
		tasks->push(&task);
		m_mutex->unlock();
	}

}

void TaskPool::addTask(Task *task) {
	if (task->type >= 0 && task->type <= 6 && task->tunit > 0)
	{
		m_mutex->lock();
		Timer* t = new Timer;
		t->start();
		scheduledTimers->push_back(t);
		scheduledtasks->push_back(task);
		m_mutex->unlock();
		m_mutex->interrupt();
	}
	else
	{
		m_mutex->lock();
		tasks->push(task);
		m_mutex->unlock();
	}
}

void TaskPool::addPTask(Task &task) {
	if (task.type >= 0 && task.type <= 6 && task.tunit > 0)
	{
		m_mutex->lock();
		Timer* t = new Timer;
		t->start();
		scheduledTimers->push_back(t);
		scheduledtasks->push_back(&task);
		m_mutex->unlock();
		m_mutex->interrupt();
	}
	else
	{
		m_mutex->lock();
		ptasks->push_back(&task);
		m_mutex->unlock();
	}
}
void TaskPool::addPTask(Task *task) {
	if (task->type >= 0 && task->type <= 6 && task->tunit > 0)
	{
		m_mutex->lock();
		Timer* t = new Timer;
		t->start();
		scheduledTimers->push_back(t);
		scheduledtasks->push_back(task);
		m_mutex->unlock();
		m_mutex->interrupt();
	}
	else
	{
		m_mutex->lock();
		ptasks->push_back(task);
		m_mutex->unlock();
	}
}
Task* TaskPool::getTask() {
	Task *task = NULL;
	m_mutex->lock();
	if(!tasks->empty())
	{
		task = tasks->front();
		tasks->pop();
	}
	m_mutex->unlock();
	return task;
}
Task* TaskPool::getPTask() {
	m_mutex->lock();
	int currpri = 0;
	Task *task = NULL;
	list<Task*>::iterator iter, iter1;
	for (iter = ptasks->begin(); iter != ptasks->end(); ++iter) {
		if ((*iter)->priority > currpri) {
			task = *iter;
			iter1 = iter;
			currpri = task->priority;
		}
	}
	if(task!=NULL)ptasks->remove(task);
	m_mutex->unlock();
	return task;
}
bool TaskPool::tasksPending() {
	m_mutex->lock();
	bool tp = !tasks->empty();
	tp |= !scheduledtasks->empty();
	m_mutex->unlock();
	return tp;
}
bool TaskPool::tasksPPending() {
	m_mutex->lock();
	bool tp = !ptasks->empty();
	tp |= !scheduledtasks->empty();
	m_mutex->unlock();
	return tp;
}
TaskPool::~TaskPool() {
	m_mutex->lock();
	runFlag = false;
	bool fl = this->complete;
	m_mutex->unlock();
	while(!fl)
	{
		m_mutex->lock();
		fl = this->complete;
		m_mutex->unlock();
		Thread::sSleep(1);
	}
	delete mthread;
	delete tasks;
	delete ptasks;
	delete scheduledtasks;
	delete scheduledTimers;
	delete m_mutex;
}
