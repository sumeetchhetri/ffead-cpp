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
	PoolThread* ths = (PoolThread*)arg;
	ths->m_mutex->lock();
	bool console = ths->console;
	bool fl = ths->runFlag;
	ths->m_mutex->unlock();
	while (fl)
	{
		ths->mthread->wait();
		Task* task = ths->getTask();
		if (task)
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
			catch(const exception& e)
			{
				if(console)
				{
					ths->logger << e.what() << flush;
				}
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
				if(console)
				{
					ths->logger << "Error Occurred while executing task" << flush;
				}
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
		ths->release();
		ths->m_mutex->lock();
		fl = ths->runFlag;
		ths->m_mutex->unlock();
	}
	ths->m_mutex->lock();
	ths->complete = true;
	ths->m_mutex->unlock();
	return NULL;
}

PoolThread::PoolThread(bool console) {
	logger = LoggerFactory::getLogger("PoolThread");
	this->task = NULL;
	this->idle = true;
	this->console = console;
	this->complete = false;
	this->runFlag = true;
	this->thrdStarted = false;
	m_mutex = new Mutex;
	mthread = new Thread(&run, this);
}

PoolThread::PoolThread() {
	logger = LoggerFactory::getLogger("PoolThread");
	this->task = NULL;
	this->idle = true;
	this->console = false;
	this->thrdStarted = false;
	m_mutex = new Mutex;
	mthread = new Thread(&run, this);
}

PoolThread::~PoolThread() {
	this->runFlag = false;
	m_mutex->lock();
	bool fl = this->complete;
	m_mutex->unlock();
	while(!fl)
	{
		m_mutex->lock();
		fl = this->complete;
		m_mutex->unlock();
		mthread->interrupt();
		Thread::mSleep(1);
	}
	delete mthread;
	delete m_mutex;
	if(console)
	{
		logger << "Destroyed PoolThread\n" << flush;
	}
}

void PoolThread::execute() {
	if(thrdStarted)return;
	m_mutex->lock();
	mthread->execute();
	thrdStarted = true;
	m_mutex->unlock();
}

void PoolThread::checkout(Task *task)
{
	m_mutex->lock();
	this->idle = false;
	this->task = task;
	if(thrdStarted)
		this->mthread->interrupt();
	m_mutex->unlock();
}

void PoolThread::release()
{
	m_mutex->lock();
	this->task = NULL;
	this->idle = true;
	m_mutex->unlock();
}

bool PoolThread::isIdle()
{
	m_mutex->lock();
	bool isIdle = idle;
	m_mutex->unlock();
	return isIdle;
}

Task* PoolThread::getTask()
{
	this->m_mutex->lock();
	Task* task = this->task;
	this->m_mutex->unlock();
	return task;
}
