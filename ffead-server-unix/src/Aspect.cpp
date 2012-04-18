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
 * Aspect.cpp
 *
 *  Created on: Oct 17, 2009
 *      Author: sumeet
 */

#include "Aspect.h"

Aspect::Aspect() {
	// TODO Auto-generated constructor stub

}

Aspect::~Aspect() {
	// TODO Auto-generated destructor stub
}

string Aspect::getMethodName() const
{
	return methodName;
}

void Aspect::setMethodName(string methodName)
{
	this->methodName = methodName;
}

string Aspect::getReturnType() const
{
	return returnType;
}

void Aspect::setReturnType(string returnType)
{
	this->returnType = returnType;
}

args Aspect::getArgumentTypes() const
{
	return argumentTypes;
}

void Aspect::setArgumentTypes(args argumentTypes)
{
	this->argumentTypes = argumentTypes;
}

void Aspect::clear()
{
	strict="";
    className="";
	when="";
	code="";
	methodName="";
	returnType="";
	argumentTypes.clear();
}
string Aspect::getStrict() const
{
	return strict;
}

void Aspect::setStrict(string strict)
{
	this->strict = strict;
}

string Aspect::getClassName() const
{
	return className;
}

void Aspect::setClassName(string className)
{
	this->className = className;
}

string Aspect::getWhen() const
{
	return when;
}

void Aspect::setWhen(string when)
{
	this->when = when;
}

string Aspect::getCode() const
{
	return code;
}

void Aspect::setCode(string code)
{
	this->code = code;
}

string Aspect::getArguTypes() const
{
	return arguTypes;
}

void Aspect::setArguTypes(string arguTypes)
{
	this->arguTypes = arguTypes;
}
