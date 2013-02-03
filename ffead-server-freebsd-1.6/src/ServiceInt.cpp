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
 * ServiceInt.cpp
 *
 *  Created on: Mar 27, 2010
 *      Author: sumeet
 */

#include "ServiceInt.h"

void ServiceInt::setComponent(Component *_comp)
{
	this->_comp = _comp;
}

void ServiceInt::startTransaction()
{
	this->_comp->startTransaction();
}
void ServiceInt::endTransaction()
{
	this->_comp->endTransaction();
}
void ServiceInt::commit()
{
	this->_comp->commit();
}
void ServiceInt::rollback()
{
	this->_comp->rollback();
}
void ServiceInt::execute()
{
	this->_comp->execute();
}
Cibernate* ServiceInt::getCibernate()
{
	return this->_comp->getCibernate();
}
