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
	f = NULL;
	si = NULL;
}

ClassInfo::~ClassInfo() {
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

void ClassInfo::addBase(int scope, const std::string& clas) {
	bases.push_back(std::make_tuple(scope, clas));
}

void ClassInfo::addRuntimeBase(const std::string& clsnm, ClassInfo* ci) {
	baseCis[clsnm] = ci;
}

ClassInfo* ClassInfo::getRuntimeBase(const std::string& clsnm) {
	if(baseCis.find(clsnm)!=baseCis.end()) {
		return baseCis.find(clsnm)->second;
	}
	return NULL;
}

std::vector<std::tuple<int, std::string>> ClassInfo::getBases() {
	return bases;
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

bool ClassInfo::isAbstractClass() {
	for(auto it: meths) {
		if(it.second.isPureVirtual()) {
			return true;
		}
	}
	return false;
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
		if(baseCis.size()>0) {
			for(auto it: baseCis) {
				ClassInfo* t1 = it.second;
				const Method& m = t1->getMethod(methodName, argumentTypes);
				if(m.getMethodName()=="") {
					return nullmeth;
				}
				return m;
			}
		}
		return nullmeth;
	}
}
void ClassInfo::addMethod(const Method& meth)
{
	std::string key = meth.getMethodName();
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

void* ClassInfo::getSI() {
	return si;
}

NewInst ClassInfo::getIns() {
	return f;
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
