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
 * Timer.cpp
 *
 *  Created on: Jan 8, 2010
 *      Author: sumeet
 */

#include "Timer.h"

Timer::Timer() {
	logger = Logger::getLogger("Timer");
}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

void Timer::start()
{
	clock_gettime(CLOCK_REALTIME, &st);
	logger << "\n--------------------------start Timer-------------------------::" << ((st.tv_sec*1E9 + st.tv_nsec)/1000) << "\n"<< flush;
}

long long Timer::getCurrentTime()
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	return ((en.tv_sec * 1E9) + en.tv_nsec);
}

long long Timer::getTimestamp()
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	return en.tv_sec;
}

void Timer::end()
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	long long elap = (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec))/1E3;
	logger << "\n--------------------------end Timer-------------------------::" << ((en.tv_sec*1E9 + en.tv_nsec)/1000) << "\n"<< flush;
	logger << "\n" << elap << "\n"<< flush;
}
long Timer::elapsedMicroSeconds()
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	return (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec))/1E3;
}
long Timer::elapsedMilliSeconds()
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	return (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec))/1E6;
}
long Timer::elapsedNanoSeconds()
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	return (((en.tv_sec - st.tv_sec) * 1E9) + (en.tv_nsec - st.tv_nsec));
}
int Timer::elapsedSeconds()
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	return (en.tv_sec - st.tv_sec);
}
