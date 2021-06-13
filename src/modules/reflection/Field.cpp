/*
	Copyright 2009-2020, Sumeet Chhetri 
  
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
 * Field.cpp
 *
 *  Created on: Oct 10, 2009
 *      Author: sumeet
 */

#include "Field.h"

Field::Field() {
	f = NULL;
	isStatic = false;
	isConst = false;
}

GetFld Field::getIns() const {
	return f;
}

Field::~Field() {
}

const std::string& Field::getAccessSpecifier() const
{
	return accessSpecifier;
}

void Field::setAccessSpecifier(const std::string& accessSpecifier)
{
	this->accessSpecifier = accessSpecifier;
}

const std::string& Field::getFieldName() const
{
	return fieldName;
}

void Field::setFieldName(const std::string& fieldName)
{
	this->fieldName = fieldName;
}

const std::string& Field::getType() const
{
	return type;
}

void Field::setType(const std::string& type)
{
	this->type = type;
}

void Field::clear()
{
	accessSpecifier ="";
	fieldName="";
	type="";
}

const std::string& Field::getRefName() const {
	return refName;
}

void Field::setRefName(const std::string& refName) {
	this->refName = refName;
}

void Field::updateStaticConst(bool sta, bool cons) {
	this->isStatic = sta;
	this->isConst = cons;
}
