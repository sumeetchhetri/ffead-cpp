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
	TaskPool* pool  = static_cast<TaskPool*>(arg);
	std::vector<Task*>::iterator iter;
	while(pool->runFlag)
	{
		std::vector<Task*>::iterator it;
		std::vector<Timer>::iterator tit;
		int counter = 0;
		pool->s_mutex.lock();
		for (it=pool->scheduledtasks.begin(),tit=pool->scheduledTimers.begin();
				it!=pool->scheduledtasks.end(),tit!=pool->scheduledTimers.end();++counter) {
			Task* task = *it;
			Timer& timer = *tit;
			if(task!=NULL && task->isWaitOver(&timer))
			{
				pool->c_mutex.lock();
				pool->tasks.push(task);
				pool->c_mutex.unlock();

				it = pool->scheduledtasks.erase(it);
				tit = pool->scheduledTimers.erase(tit);
			}
			else
			{
				++it;
				++tit;
			}
			if(counter==100)  {
				break;
			}
		}
		pool->s_mutex.unlock();
		Thread::mSleep(1);
	}
	pool->complete = true;
	return NULL;
}

TaskPool::TaskPool() {}

void TaskPool::init(const bool& prioritybased, const bool& allowScheduledTasks) {
	runFlag = true;
	complete = false;
	count = 0;
	thrdStarted = false;
	this->prioritybased = prioritybased;
	this->allowScheduledTasks = allowScheduledTasks;
	if(allowScheduledTasks) {
		mthread = new Thread(&run, this);
	} else {
		mthread = NULL;
	}
}

void TaskPool::start() {
	if(thrdStarted)return;
	if(this->allowScheduledTasks) {
		mthread->execute();
	}
	thrdStarted = true;
}

void TaskPool::addTask(Task &task) {
	c_mutex.lock();
	tasks.push(&task);
	++count;
	c_mutex.conditionalNotifyOne();
	c_mutex.unlock();
}

void TaskPool::addPTask(Task &task) {
	c_mutex.lock();
	ptasks.push(&task);
	++count;
	c_mutex.conditionalNotifyOne();
	c_mutex.unlock();
}

void TaskPool::addSTask(Task &task) {
	if (task.type >= 0 && task.type <= 6 && task.tunit > 0)
	{
		s_mutex.lock();
		scheduledTimers.push_back(Timer());
		Timer* t = &(scheduledTimers.back());
		t->start();
		scheduledtasks.push_back(&task);
		s_mutex.unlock();
	}
}

void TaskPool::addTask(Task *task) {
	c_mutex.lock();
	tasks.push(task);
	++count;
	c_mutex.conditionalNotifyOne();
	c_mutex.unlock();
}

void TaskPool::addPTask(Task *task) {
	c_mutex.lock();
	ptasks.push(task);
	++count;
	c_mutex.conditionalNotifyOne();
	c_mutex.unlock();
}

void TaskPool::addSTask(Task *task) {
	if (task->type >= 0 && task->type <= 6 && task->tunit > 0)
	{
		s_mutex.lock();
		scheduledTimers.push_back(Timer());
		Timer* t = &(scheduledTimers.back());
		t->start();
		scheduledtasks.push_back(task);
		s_mutex.unlock();
	}
}

Task* TaskPool::getTask() {
	Task *task = NULL;
	c_mutex.lock();
	if(!prioritybased && !tasks.empty())
	{
		task = tasks.front();
		tasks.pop();
		--count;
	}
	else if(prioritybased && !ptasks.empty())
	{
		task = ptasks.top();
		ptasks.pop();
		--count;
	}
	c_mutex.unlock();
	return task;
}

bool TaskPool::tasksPending() {
	c_mutex.lock();
	bool tp = !prioritybased?!tasks.empty():!ptasks.empty();
	c_mutex.unlock();
	s_mutex.lock();
	tp |= !scheduledtasks.empty();
	s_mutex.unlock();
	return tp;
}

void TaskPool::stop() {
	c_mutex.lock();
	++count;
	c_mutex.conditionalNotifyAll();
	c_mutex.unlock();
	Thread::sSleep(1);
}

TaskPool::~TaskPool() {
	runFlag = false;
	if(allowScheduledTasks) {
		//delete mthread;
	}
}
