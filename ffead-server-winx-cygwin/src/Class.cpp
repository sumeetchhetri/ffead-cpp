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
 * Class.cpp
 *
 *  Created on: Nov 20, 2009
 *      Author: sumeet
 */

#include "Class.h"

Class::Class() {
	// TODO Auto-generated constructor stub

}

Class::~Class() {
	// TODO Auto-generated destructor stub
}

string Class::getName() const
{
	return name;
}

void Class::setName(string name)
{
	this->name = name;
}

string Class::getAccess() const
{
	return access;
}

void Class::setAccess(string access)
{
	this->access = access;
}

string Class::getType() const
{
	return type;
}

void Class::setType(string type)
{
	this->type = type;
}

string Class::getPackage() const
{
	return package;
}

void Class::setPackage(string package)
{
	this->package = package;
}

string Class::getBase() const
{
	return base;
}

void Class::setBase(string base)
{
	this->base = base;
}

string Class::getInterfaces() const
{
	return interfaces;
}

void Class::setInterfaces(string interfaces)
{
	this->interfaces = interfaces;
}

string Class::getLang() const
{
	return lang;
}

void Class::setLang(string lang)
{
	this->lang = lang;
}

/*Method Class::getMethod(string name,args args)
{
	for (int var = 0; var < args.size(); ++var)
	{
		name += (args.at(var));
	}
	return this->methods[name];
}*/

Field Class::getField(string name)
{
	return this->fields[name];
}

void Class::addMethod(Method meth)
{
	string key = meth.getMethodName();
	for (int var = 0; var < meth.getArgumentTypes().size(); ++var)
	{
		key += (meth.getArgumentTypes().at(var));
	}
	this->methods[key] = meth;
}
/*void Class::addField(Field fld)
{
	this->field[fld.getFieldName()] = fld;
}*/
