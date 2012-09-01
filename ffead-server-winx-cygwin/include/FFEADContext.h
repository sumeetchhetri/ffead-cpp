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
 * FFEADContext.h
 *
 *  Created on: Oct 17, 2010
 *      Author: root
 */

#ifndef FFEADCONTEXT_H_
#define FFEADCONTEXT_H_
#include "XmlParser.h"
#include "Reflector.h"
#include "CastUtil.h"
#include "AfcUtil.h"

class Bean
{
	friend class FFEADContext;
	string name,inbuilt,value,clas,bean,intfType,injectAs;
	bool realbean;
	vector<string> injs,names,types;
};
typedef map<string,Bean> beanMap;
class FFEADContext {
	beanMap beans,injbns;
	map<string,void*> objects;
	bool cleared;
public:
	FFEADContext(string);
	virtual ~FFEADContext();
	void* getBean(string);
	void* getBean(Bean);
	void clear();
};

#endif /* FFEADCONTEXT_H_ */
