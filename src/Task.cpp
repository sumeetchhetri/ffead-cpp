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
 * Task.cpp
 *
 *  Created on: Mar 23, 2010
 *      Author: sumeet
 */

#include "Task.h"

Task::Task() :
	tunit(-1), type(-1) {
}
Task::Task(int priority) :
	tunit(-1), type(-1) {
	this->priority = priority;
}
Task::Task(int tunit, int type) :
	tunit(-1), type(-1) {
	this->tunit = tunit;
	this->type = type;
}
Task::~Task()
{}
