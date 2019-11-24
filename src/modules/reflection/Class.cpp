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
 * Class.cpp
 *
 *  Created on: Nov 20, 2009
 *      Author: sumeet
 */

#include "Class.h"

Class::Class() {
	

}

Class::~Class() {
	
}

std::string Class::getName() const
{
	return name;
}

void Class::setName(const std::string& name)
{
	this->name = name;
}

std::string Class::getAccess() const
{
	return access;
}

void Class::setAccess(const std::string& access)
{
	this->access = access;
}

std::string Class::getType() const
{
	return type;
}

void Class::setType(const std::string& type)
{
	this->type = type;
}

std::string Class::getPackage() const
{
	return package;
}

void Class::setPackage(const std::string& package)
{
	this->package = package;
}

std::string Class::getBase() const
{
	return base;
}

void Class::setBase(const std::string& base)
{
	this->base = base;
}

std::string Class::getInterfaces() const
{
	return interfaces;
}

void Class::setInterfaces(const std::string& interfaces)
{
	this->interfaces = interfaces;
}

std::string Class::getLang() const
{
	return lang;
}

void Class::setLang(const std::string& lang)
{
	this->lang = lang;
}

/*Method Class::getMethod(const std::string& name, const args& args)
{
	for (int var = 0; var < args.size(); ++var)
	{
		name += (args.at(var));
	}
	return this->methods[name];
}*/

Field Class::getField(const std::string& name)
{
	return this->fields[name];
}

void Class::addMethod(const Method& meth)
{
	std::string key = meth.getMethodName();
	for (int var = 0; var < (int)meth.getArgumentTypes().size(); ++var)
	{
		key += (meth.getArgumentTypes().at(var));
	}
	this->methods[key] = meth;
}
/*void Class::addField(const Field& fld)
{
	this->field[fld.getFieldName()] = fld;
}*/
