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
 * Constructor.cpp
 *
 *  Created on: Mar 30, 2010
 *      Author: sumeet
 */

#include "Constructor.h"

Constructor::Constructor() {
	// TODO Auto-generated constructor stub

}

Constructor::~Constructor() {
	// TODO Auto-generated destructor stub
}

std::vector<std::string> Constructor::getArgumentTypes() const
{
	return argumentTypes;
}

void Constructor::setArgumentTypes(const std::vector<std::string>& argumentTypes)
{
	this->argumentTypes = argumentTypes;
}

int Constructor::getArgNum() const
{
	return this->argumentTypes.size();
}

std::string Constructor::getName() const
{
	return name;
}

void Constructor::setName(const std::string& name)
{
	this->name = name;
}

void Constructor::clear()
{
	name="";
	argumentTypes.clear();
}

const std::string& Constructor::getRefName() const {
	return refName;
}

void Constructor::setRefName(const std::string& refName) {
	this->refName = refName;
}
