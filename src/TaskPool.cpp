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

TaskPool::TaskPool() {
	m_mutex = new Mutex();
	tasks = new std::queue<Task*> ;
	ptasks = new list<Task*> ;
}
void TaskPool::addTask(Task &task) {
	m_mutex->lock();
	tasks->push(&task);
	m_mutex->unlock();
}
void TaskPool::addTask(Task *task) {
	m_mutex->lock();
	tasks->push(task);
	m_mutex->unlock();
}
void TaskPool::addPTask(Task &task) {
	m_mutex->lock();
	ptasks->push_back(&task);
	m_mutex->unlock();
}
void TaskPool::addPTask(Task *task) {
	m_mutex->lock();
	ptasks->push_back(task);
	m_mutex->unlock();
}
Task* TaskPool::getTask() {
	m_mutex->lock();
	Task *task = tasks->front();
	tasks->pop();
	m_mutex->unlock();
	return task;
}
Task* TaskPool::getPTask() {
	m_mutex->lock();
	int currpri = 0;
	Task *task;
	list<Task*>::iterator iter;
	for (iter = ptasks->begin(); iter != ptasks->end(); ++iter) {
		if ((*iter)->priority > currpri) {
			task = *iter;
			currpri = task->priority;
		}
	}
	ptasks->remove(task);
	m_mutex->unlock();
	return task;
}
bool TaskPool::tasksPending() {
	m_mutex->lock();
	bool tp = tasks->empty();
	m_mutex->unlock();
	return !tp;
}
bool TaskPool::tasksPPending() {
	m_mutex->lock();
	int tp = ptasks->size();
	m_mutex->unlock();
	return (tp > 0);
}
TaskPool::~TaskPool() {
	delete tasks;
	delete ptasks;
	delete m_mutex;
}
