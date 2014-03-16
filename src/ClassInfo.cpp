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
 * ClassInfo.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: sumeet
 */

#include "ClassInfo.h"

ClassInfo::ClassInfo() {
	// TODO Auto-generated constructor stub

}

ClassInfo::~ClassInfo() {
	// TODO Auto-generated destructor stub
}
/*
void *ClassInfo::getInstance() const
{
	return instance;
}

void ClassInfo::setInstance(void *instance)
{
	this->instance = instance;
}
*/
string ClassInfo::getClassName() const
{
	return className;
}

void ClassInfo::setClassName(string className)
{
	this->className = className;
}

string ClassInfo::getBase() const
{
	return base;
}

void ClassInfo::setBase(string base)
{
	this->base = base;
}

Constructor ClassInfo::getConstructor(args argumentTypes)
{
	string key = getClassName();
	for (unsigned int var = 0; var < argumentTypes.size(); var++)
	{
		if(argumentTypes.at(var)!="")
			key += argumentTypes.at(var);
	}
	return ctors[key];
}

void ClassInfo::addConstructor(Constructor ctor)
{
	string key = getClassName();
	for (unsigned int var = 0; var < ctor.getArgumentTypes().size(); var++)
	{
		if(ctor.getArgumentTypes().at(var)!="")
			key += ctor.getArgumentTypes().at(var);
	}
	ctors[key] = ctor;
}

ctorMap ClassInfo::getConstructors()
{
	return this->ctors;
}

Method ClassInfo::getMethod(string methodName,args argumentTypes)
{
	string key = getClassName()+methodName;
	for (unsigned int var = 0; var < argumentTypes.size(); var++)
	{
		if(argumentTypes.at(var)!="")
		{
			string temp = argumentTypes.at(var);
			StringUtil::replaceFirst(temp," ","");
			StringUtil::replaceFirst(temp,"<","ts");
			StringUtil::replaceFirst(temp,">","te");
			StringUtil::replaceFirst(temp,"*","ptr");
			StringUtil::replaceFirst(temp,"&","adr");
			StringUtil::replaceFirst(temp,"std::","");
			StringUtil::replaceFirst(temp,"::","ns");
			key += temp;
		}
	}
	return meths[key];
}
void ClassInfo::addMethod(Method meth)
{
	string key = meth.getMethodName();
	/*for (unsigned int var = 0; var < meth.getArgumentTypes().size(); var++)
	{
		if(meth.getArgumentTypes().at(var)!="")
			key += meth.getArgumentTypes().at(var);
	}*/
	meths[key] = meth;
}

Field ClassInfo::getField(string fldName)
{
	string key = fldName;
	return fields[key];
}
void ClassInfo::addField(Field fld)
{
	string key = fld.getFieldName();
	fields[key] = fld;
	fldvec.push_back(fld);
}

fldMap ClassInfo::getFields()
{
	return fields;
}

fldVec ClassInfo::getFieldVec()
{
	return fldvec;
}

methMap ClassInfo::getMethods()
{
	return meths;
}

void ClassInfo::setNamespace(string namespc)
{
	this->namespc = namespc;
}

string ClassInfo::getNamespace()
{
	return namespc;
}

void ClassInfo::setSize(int size)
{
	this->size = size;
}

int ClassInfo::getSize()
{
	return this->size;
}

/*
string ClassInfo::getPublic_fields() const
{
	return public_fields;
}

void ClassInfo::setPublic_fields(string public_fields)
{
	this->public_fields = public_fields;
}

string ClassInfo::getPrivate_fields() const
{
	return private_fields;
}

void ClassInfo::setPrivate_fields(string private_fields)
{
	this->private_fields = private_fields;
}

string ClassInfo::getProtected_fields() const
{
	return protected_fields;
}

void ClassInfo::setProtected_fields(string protected_fields)
{
	this->protected_fields = protected_fields;
}

string ClassInfo::getPublic_meths() const
{
	return public_meths;
}

void ClassInfo::setPublic_meths(string public_meths)
{
	this->public_meths = public_meths;
}

string ClassInfo::getPrivate_meths() const
{
	return private_meths;
}

void ClassInfo::setPrivate_meths(string private_meths)
{
	this->private_meths = private_meths;
}

string ClassInfo::getProtected_meths() const
{
	return protected_meths;
}

void ClassInfo::setProtected_meths(string protected_meths)
{
	this->protected_meths = protected_meths;
}

vector<Method> ClassInfo::getMethods() const
{
	return methods;
}

void ClassInfo::setMethods(vector<Method> methods)
{
	this->methods = methods;
}

vector<Field> ClassInfo::getFields() const
{
	return fields;
}

void ClassInfo::setFields(vector<Field> fields)
{
	this->fields = fields;
}
void ClassInfo::setMeths(methMap meths)
{
	this->meths = meths;
}*/
