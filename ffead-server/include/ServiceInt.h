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
 * ServiceInt.h
 *
 *  Created on: Mar 25, 2010
 *      Author: sumeet
 */

#ifndef SERVICEINT_H_
#define SERVICEINT_H_
#include "Component.h"
#include "CibernateConnPools.h"
class ServiceInt
{
	Component *_comp;
public:
	void startTransaction();
	void endTransaction();
	void commit();
	void rollback();
	void execute();
	Cibernate* getCibernate();
	void setComponent(Component *);
};

#endif /* SERVICEINT_H_ */
