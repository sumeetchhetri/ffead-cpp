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
 * CfusionEngine.h
 *
 *  Created on: Nov 12, 2009
 *      Author: sumeet
 */

#ifndef CFUSIONENGINE_H_
#define CFUSIONENGINE_H_
#include "string"
#include "fstream"
#include "iostream"
#include "XmlParser.h"
#include "AfcUtil.h"
using namespace std;
typedef vector<Element> ElementList;
class CfusionEngine {
public:
	CfusionEngine();
	virtual ~CfusionEngine();
	void execute(string);
};

#endif /* CFUSIONENGINE_H_ */
