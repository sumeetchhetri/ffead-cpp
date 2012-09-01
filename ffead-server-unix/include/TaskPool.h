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
 * TaskPool.h
 *
 *  Created on: Mar 23, 2010
 *      Author: sumeet
 */

#ifndef TASKPOOL_H_
#define TASKPOOL_H_
#include "vector"
#include "queue"
#include "list"
#include "Task.h"
#include "Mutex.h"

class TaskPool {
	std::queue<Task*> *tasks;
	list<Task*> *ptasks;
	Mutex *m_mutex;
	bool console;
	friend class ThreadPool;
public:
	TaskPool();
	void addTask(Task &task);
	void addTask(Task *task);
	void addPTask(Task &task);
	void addPTask(Task *task);
	Task* getTask();
	Task* getPTask();
	bool tasksPending();
	bool tasksPPending();
	~TaskPool();
};


#endif /* TASKPOOL_H_ */
