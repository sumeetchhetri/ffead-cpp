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
 * Thread.cpp
 *
 *  Created on: Mar 23, 2010
 *      Author: sumeet
 */

#include "Thread.h"

void* Thread::run(void *arg)
{
	while (true)
	{
		Thread* ths = (Thread*)arg;
		Pthread* mthread = ths->mthread;
		//cout << "started wait" << endl;
		mthread->wait();
		//cout << "end wait" << endl;
		ths = (Thread*)arg;
		Task* task = ths->task;
		bool console = ths->console;
		if (task != NULL)
		{
			if (task->tunit != -1 && task->type != -1 && task->tunit > 0 && task->type > 0)
			{
				if(console)
				{
					//logger << "Task scheduled for sleep\n" << flush;
				}
				if (task->type == TimeUnit::NANOSECONDS)
					Pthread::nSleep(task->tunit);
				else if (task->type == TimeUnit::MICROSECONDS)
					Pthread::uSleep(task->tunit);
				else if (task->type == TimeUnit::MILLISECONDS)
					Pthread::mSleep(task->tunit);
				else if (task->type == TimeUnit::SECONDS)
					Pthread::sSleep(task->tunit);
				else if (task->type == TimeUnit::MINUTES)
					Pthread::nSleep(task->tunit * 60);
				else if (task->type == TimeUnit::HOURS)
					Pthread::nSleep(task->tunit * 60 * 60);
				else if(task->type==TimeUnit::DAYS)
					Pthread::nSleep(task->tunit * 24 * 60 * 60);
			}
			try
			{
				task->run();
			}
			catch(exception& e)
			{
				if(console)
				{
					//logger << e.what() << flush;
				}
			}
			task->~Task();
			ths->task = NULL;
		}
		ths->idle = true;
	}
	return NULL;
}

Thread::Thread() {
	//logger = //logger::get//logger("Thread");
	task = NULL;
	idle = true;
	mthread = new Pthread(&run, this);
}

Thread::~Thread() {
	delete mthread;
	delete task;
	if(console)
	{
		//logger << "Destroyed Thread\n" << flush;
	}
}

void Thread::execute() {
	mthread->execute();
}

