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

ClassInfo::ClassInfo() {
	size = 0;
}

ClassInfo::~ClassInfo() {
	// TODO Auto-generated destructor stub
}

string ClassInfo::getClassName() const
{
	return className;
}

void ClassInfo::setClassName(const string& className)
{
	this->className = className;
}

string ClassInfo::getBase() const
{
	return base;
}

void ClassInfo::setBase(const string& base)
{
	this->base = base;
}

const Constructor& ClassInfo::getConstructor(const args& argumentTypes) const
{
	string key = getClassName();
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

const Method& ClassInfo::getMethod(const string& methodName, args argumentTypes) const
{
	string key1 = getClassName()+methodName;
	string key2 = getClassName()+methodName;
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
			key1 += temp;

			temp = argumentTypes.at(var);
			StringUtil::replaceFirst(temp," ","");
			StringUtil::replaceFirst(temp,"<","ts");
			StringUtil::replaceFirst(temp,">","te");
			StringUtil::replaceFirst(temp,"*","ptr");
			StringUtil::replaceFirst(temp,"&","");
			StringUtil::replaceFirst(temp,"std::","");
			StringUtil::replaceFirst(temp,"::","ns");
			key2 += temp;
		}
	}
	if(meths.find(key1)!=meths.end()) {
		return meths.find(key1)->second;
	} else if(meths.find(key2)!=meths.end()) {
		return meths.find(key2)->second;
	} else {
		return Method();
	}
}
void ClassInfo::addMethod(const Method& meth)
{
	string key = meth.getMethodName();
	/*for (unsigned int var = 0; var < meth.getArgumentTypes().size(); var++)
	{
		if(meth.getArgumentTypes().at(var)!="")
			key += meth.getArgumentTypes().at(var);
	}*/
	meths[key] = meth;
}

Field ClassInfo::getField(const string& fldName)
{
	string key = fldName;
	return fields[key];
}
void ClassInfo::addField(const Field& fld)
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

void ClassInfo::setNamespace(const string& namespc)
{
	this->namespc = namespc;
}

string ClassInfo::getNamespace()
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

const string& ClassInfo::getDestRefName() const {
	return destRefName;
}

void ClassInfo::setDestRefName(const string& destRefName) {
	this->destRefName = destRefName;
}

string ClassInfo::getOperatorRefName(const string& oper) {
	if(operRefNames.find(oper)!=operRefNames.end()) {
		return operRefNames.find(oper)->second;
	}
	return "";
}

void ClassInfo::addOperatorRefName(const string& oper, const string& rn) {
	if(operRefNames.find(oper)==operRefNames.end()) {
		operRefNames[oper] = rn;
	}
}

string ClassInfo::getContRefName() {
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

void ClassInfo::setContRefName(const string& rn) {
	contRefName = rn;
}
/*
string ClassInfo::getPublic_fields() const
{
	return public_fields;
}

void ClassInfo::setPublic_fields(const string& public_fields)
{
	this->public_fields = public_fields;
}

string ClassInfo::getPrivate_fields() const
{
	return private_fields;
}

void ClassInfo::setPrivate_fields(const string& private_fields)
{
	this->private_fields = private_fields;
}

string ClassInfo::getProtected_fields() const
{
	return protected_fields;
}

void ClassInfo::setProtected_fields(const string& protected_fields)
{
	this->protected_fields = protected_fields;
}

string ClassInfo::getPublic_meths() const
{
	return public_meths;
}

void ClassInfo::setPublic_meths(const string& public_meths)
{
	this->public_meths = public_meths;
}

string ClassInfo::getPrivate_meths() const
{
	return private_meths;
}

void ClassInfo::setPrivate_meths(const string& private_meths)
{
	this->private_meths = private_meths;
}

string ClassInfo::getProtected_meths() const
{
	return protected_meths;
}

void ClassInfo::setProtected_meths(const string& protected_meths)
{
	this->protected_meths = protected_meths;
}

vector<Method> ClassInfo::getMethods() const
{
	return methods;
}

void ClassInfo::setMethods(const vector<Method>& methods)
{
	this->methods = methods;
}

vector<Field> ClassInfo::getFields() const
{
	return fields;
}

void ClassInfo::setFields(const vector<Field>& fields)
{
	this->fields = fields;
}
void ClassInfo::setMeths(const methMap& meths)
{
	this->meths = meths;
}*/
