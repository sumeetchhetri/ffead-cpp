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
	Logger logger = LoggerFactory::getLogger("PoolThread");
	PoolThread* ths  = static_cast<PoolThread*>(arg);
	Timer t;
	t.start();

	while (ths->runFlag)
	{
		Task* task;
		ths->c_mutex.lock();
		while (ths->condVar<=0)
		ths->c_mutex.conditionalWait();
		ths->c_mutex.unlock();
		bool f = ths->tasks.try_dequeue(task);
		if(f && task==NULL)break;

		ths->condVar--;
		try
		{
			ths->taskCount++;
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
				if(task->hdlr!=NULL) {
					task->hdlr->push(task);
				} else {
					delete task;
				}
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
			if(task->cleanUp)
			{
				if(task->hdlr!=NULL) {
					task->hdlr->push(task);
				} else {
					delete task;
				}
			}
		}

		if(t.elapsedSeconds()>=10) {
			std::string a = (ths->name+": Total Tasks handled = "+CastUtil::lexical_cast<std::string>(ths->taskCount)+"\n");
			logger.info(a);
			t.start();
		}
	}
	Task* task = NULL;
	while(ths->tasks.try_dequeue(task))
	{
		if(task->cleanUp)
		{
			if(task->hdlr!=NULL) {
				task->hdlr->push(task);
			} else {
				delete task;
			}
		}
	}
	ths->complete = true;
	return NULL;
}

void* PoolThread::runWithTaskPool(void *arg)
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
					if(task->hdlr!=NULL) {
						task->hdlr->push(task);
					} else {
						delete task;
					}
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
				if(task->cleanUp)
				{
					if(task->hdlr!=NULL) {
						task->hdlr->push(task);
					} else {
						delete task;
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
			if(task->hdlr!=NULL) {
				task->hdlr->push(task);
			} else {
				delete task;
			}
		}
	}
	ths->complete = true;
	return NULL;
}

PoolThread::PoolThread(int num) {
	logger = LoggerFactory::getLogger("PoolThread");
	this->idle = true;
	this->thrdStarted = false;
	this->complete = false;
	this->runFlag = true;
	wpool = NULL;
	mthread = new Thread(&run, this);
	this->condVar = 0;
	this->taskCount = 0;
	this->name = "Thread-" + CastUtil::lexical_cast<std::string>(num);
}

PoolThread::PoolThread(TaskPool* wpool, int num) {
	logger = LoggerFactory::getLogger("PoolThread");
	this->idle = true;
	this->thrdStarted = false;
	this->complete = false;
	this->runFlag = true;
	this->wpool = wpool;
	mthread = new Thread(&runWithTaskPool, this);
	this->condVar = 0;
	this->taskCount = 0;
	this->name = "Thread-" + CastUtil::lexical_cast<std::string>(num);
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
	c_mutex.lock();
	tasks.enqueue(NULL);
	this->condVar++;
	c_mutex.conditionalNotifyOne();
	c_mutex.unlock();
}

void PoolThread::execute(int cid) {
	if(thrdStarted)return;
	mthread->execute(cid);
	thrdStarted = true;
}

bool PoolThread::isComplete() {
	return complete;
}

void PoolThread::addTask(Task* task) {
	if(!runFlag)return;
	c_mutex.lock();
	tasks.enqueue(task);
	this->condVar++;
	c_mutex.conditionalNotifyOne();
	c_mutex.unlock();
}
