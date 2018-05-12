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
 * PoolThread.cpp
 *
 *  Created on: Mar 23, 2010
 *      Author: sumeet
 */

#include "PoolThread.h"

void* PoolThread::run(void *arg)
{
	PoolThread* ths  = static_cast<PoolThread*>(arg);
	while (ths->runFlag)
	{
		ths->wpool->c_mutex.lock();
		while (ths->wpool->count<=0)
		ths->wpool->c_mutex.conditionalWait();
		ths->wpool->c_mutex.unlock();

		Task* task = NULL;
		while((task = ths->wpool->getTask())!=NULL)
		{
			try
			{
				if(!task->isFuture)
					task->run();
				else
				{
					FutureTask* ftask = dynamic_cast<FutureTask*>(task);
					if(ftask!=NULL)
					{
						ftask->result = ftask->call();
						ftask->taskComplete();
					}
					else
					{
						task->run();
					}
				}
				if(task->cleanUp)
				{
					delete task;
				}
			}
			catch(const std::exception& e)
			{
				ths->logger << e.what() << std::flush;
				if(task->isFuture)
				{
					FutureTask* ftask = dynamic_cast<FutureTask*>(task);
					if(ftask!=NULL)
					{
						ftask->taskComplete();
					}
				}
			}
			catch(...)
			{
				ths->logger << "Error Occurred while executing task" << std::flush;
				if(task->isFuture)
				{
					FutureTask* ftask = dynamic_cast<FutureTask*>(task);
					if(ftask!=NULL)
					{
						ftask->taskComplete();
					}
				}
			}
		}
	}
	Task* task = NULL;
	while((task = ths->wpool->getTask())!=NULL)
	{
		if(task->cleanUp)
		{
			delete task;
		}
		continue;
	}
	ths->complete = true;
	return NULL;
}

PoolThread::PoolThread(TaskPool* wpool) {
	logger = LoggerFactory::getLogger("PoolThread");
	this->task = NULL;
	this->idle = true;
	this->thrdStarted = false;
	this->complete = false;
	this->runFlag = true;
	this->prioritybased = wpool->prioritybased;
	this->wpool = wpool;
	mthread = new Thread(&run, this);
}

PoolThread::~PoolThread() {
	this->runFlag = false;
	while(!this->complete)
	{
		Thread::sSleep(1);
	}
	//delete mthread;
	logger << "Destroyed PoolThread\n" << std::flush;
}

void PoolThread::stop() {
	if(!thrdStarted)return;
	this->runFlag = false;
	this->wpool->c_mutex.lock();
	this->wpool->count++;
	this->wpool->c_mutex.conditionalNotifyAll();
	this->wpool->c_mutex.unlock();
}

void PoolThread::execute() {
	if(thrdStarted)return;
	m_mutex.lock();
	mthread->execute();
	thrdStarted = true;
	m_mutex.unlock();
}

bool PoolThread::isComplete() {
	return complete;
}
