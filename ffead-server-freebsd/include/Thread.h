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
 * Thread.h
 *
 *  Created on: Mar 23, 2010
 *      Author: sumeet
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <boost/thread.hpp>
#include "Task.h"
#include "TimeUnit.h"

class Thread {
	bool console;
	void run();
	Thread();
	virtual ~Thread();
	boost::thread *mthread;
	bool idle;
	Task *task;
	friend class ThreadPool;
};

#endif /* THREAD_H_ */
