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
 * AOPEngine.h
 *
 *  Created on: Oct 17, 2009
 *      Author: sumeet
 */

#ifndef AOPENGINE_H_
#define AOPENGINE_H_
#include "Aspect.h"
#include "fstream"
#include "sstream"
#include "iostream"
#include "StringUtil.h"
typedef vector<Aspect> asp;
class AOPEngine {
	asp aspects;
public:
	AOPEngine();
	virtual ~AOPEngine();
	void execute(string);
};

#endif /* AOPENGINE_H_ */
