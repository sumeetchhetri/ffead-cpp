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

Constructor ClassInfo::nullcons;
Method ClassInfo::nullmeth;

ClassInfo::ClassInfo() {
	size = 0;
}

ClassInfo::~ClassInfo() {
	// TODO Auto-generated destructor stub
}

std::string ClassInfo::getClassName() const
{
	return className;
}

void ClassInfo::setClassName(const std::string& className)
{
	this->className = className;
}

std::string ClassInfo::getBase() const
{
	return base;
}

void ClassInfo::setBase(const std::string& base)
{
	this->base = base;
}

const Constructor& ClassInfo::getConstructor(const args& argumentTypes) const
{
	std::string key = getClassName();
	for (unsigned int var = 0; var < argumentTypes.size(); var++)
	{
		if(argumentTypes.at(var)!="")
			key += argumentTypes.at(var);
	}
	if(ctors.find(key)!=ctors.end())
	{
		return ctors.find(key)->second;
	}
	return nullcons;
}

void ClassInfo::addConstructor(const Constructor& ctor)
{
	std::string key = getClassName();
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

const Method& ClassInfo::getMethod(const std::string& methodName, args argumentTypes) const
{
	std::string key1 = getClassName()+methodName;
	for (unsigned int var = 0; var < argumentTypes.size(); var++)
	{
		if(argumentTypes.at(var)!="")
		{
			std::string temp = argumentTypes.at(var);
			key1 += temp;
		}
	}
	StringUtil::replaceAll(key1,"std::","");
	StringUtil::replaceAll(key1, " ", "");
	StringUtil::replaceAll(key1, "*", "ptr");
	StringUtil::replaceAll(key1, "&", "");
	if(meths.find(key1)!=meths.end()) {
		return meths.find(key1)->second;
	} else {
		return nullmeth;
	}
}
void ClassInfo::addMethod(const Method& meth)
{
	std::string key = meth.getMethodName();
	/*for (unsigned int var = 0; var < meth.getArgumentTypes().size(); var++)
	{
		if(meth.getArgumentTypes().at(var)!="")
			key += meth.getArgumentTypes().at(var);
	}*/
	meths[key] = meth;
}

Field ClassInfo::getField(const std::string& fldName)
{
	std::string key = fldName;
	return fields[key];
}
void ClassInfo::addField(const Field& fld)
{
	std::string key = fld.getFieldName();
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

void ClassInfo::setNamespace(const std::string& namespc)
{
	this->namespc = namespc;
}

std::string ClassInfo::getNamespace()
{
	return namespc;
}

void ClassInfo::setSize(const int& size)
{
	this->size = size;
}

int ClassInfo::getSize()
{
	return this->size;
}

const std::string& ClassInfo::getDestRefName() const {
	return destRefName;
}

void ClassInfo::setDestRefName(const std::string& destRefName) {
	this->destRefName = destRefName;
}

std::string ClassInfo::getOperatorRefName(const std::string& oper) {
	if(operRefNames.find(oper)!=operRefNames.end()) {
		return operRefNames.find(oper)->second;
	}
	return "";
}

void ClassInfo::addOperatorRefName(const std::string& oper, const std::string& rn) {
	if(operRefNames.find(oper)==operRefNames.end()) {
		operRefNames[oper] = rn;
	}
}

std::string ClassInfo::getContRefName() {
	return contRefName;
}

/*
 * _fcrcnf_ = 0
 * _fcrcnsf_ = 1
 * _fcrcsf_ = 2
 * _fcrcssf_ = 3
 * _fcrcgpf_ = 4
 * _fcrcgpsf_ = 5
 * _fcrcgf_ = 6
 * _fcrcgsf_ = 7
 * _fcrcaf_ = 8
 * _fcrcasf_ = 9
 */

void ClassInfo::setContRefName(const std::string& rn) {
	contRefName = rn;
}
/*
std::string ClassInfo::getPublic_fields() const
{
	return public_fields;
}

void ClassInfo::setPublic_fields(const std::string& public_fields)
{
	this->public_fields = public_fields;
}

std::string ClassInfo::getPrivate_fields() const
{
	return private_fields;
}

void ClassInfo::setPrivate_fields(const std::string& private_fields)
{
	this->private_fields = private_fields;
}

std::string ClassInfo::getProtected_fields() const
{
	return protected_fields;
}

void ClassInfo::setProtected_fields(const std::string& protected_fields)
{
	this->protected_fields = protected_fields;
}

std::string ClassInfo::getPublic_meths() const
{
	return public_meths;
}

void ClassInfo::setPublic_meths(const std::string& public_meths)
{
	this->public_meths = public_meths;
}

std::string ClassInfo::getPrivate_meths() const
{
	return private_meths;
}

void ClassInfo::setPrivate_meths(const std::string& private_meths)
{
	this->private_meths = private_meths;
}

std::string ClassInfo::getProtected_meths() const
{
	return protected_meths;
}

void ClassInfo::setProtected_meths(const std::string& protected_meths)
{
	this->protected_meths = protected_meths;
}

std::vector<Method> ClassInfo::getMethods() const
{
	return methods;
}

void ClassInfo::setMethods(const std::vector<Method>& methods)
{
	this->methods = methods;
}

std::vector<Field> ClassInfo::getFields() const
{
	return fields;
}

void ClassInfo::setFields(const std::vector<Field>& fields)
{
	this->fields = fields;
}
void ClassInfo::setMeths(const methMap& meths)
{
	this->meths = meths;
}*/
