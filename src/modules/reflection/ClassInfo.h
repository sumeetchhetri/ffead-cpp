/*
	Copyright 2009-2020, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (const the& "License"); 
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
 * ClassInfo.h
 *
 *  Created on: Aug 21, 2009
 *      Author: sumeet
 */

#ifndef CLASSINFO_H_
#define CLASSINFO_H_
#include "string"
#include <typeinfo>
#include "Constructor.h"
#include "Method.h"
#include "Field.h"
#include "vector"
#include "map"
#include "StringUtil.h"
#include <tuple>

typedef std::map<std::string,Constructor> ctorMap;
typedef std::map<std::string,Method> methMap;
typedef std::map<std::string,Field> fldMap;
typedef std::vector<Field> fldVec;
typedef std::vector<std::string> args;

class ClassInfo;

class ClassInfo {
	NewInst f;
	void* si;
	std::string className;
	std::string base;
	std::string namespc;
	int size;
	ctorMap ctors;
	methMap meths;
	fldMap fields;
	fldVec fldvec;
	std::string destRefName;
	std::map<std::string, std::string> operRefNames;
	std::vector<std::tuple<int, std::string>> bases;
	std::map<std::string, ClassInfo*> baseCis;
	std::string contRefName;
	static Constructor nullcons;
	static Method nullmeth;
	friend class FFEADContext;
public:
	void* getSI();
	NewInst getIns();
	ClassInfo();
	virtual ~ClassInfo();
    std::string getClassName() const;
    void setClassName(const std::string&);
    std::string getBase() const;
    void setBase(const std::string&);
    void addBase(int scope, const std::string& clas);
    void addRuntimeBase(const std::string&, ClassInfo*);
    ClassInfo* getRuntimeBase(const std::string&);
    std::vector<std::tuple<int, std::string>> getBases();
    void setNamespace(const std::string&);
    std::string getNamespace();
    const Constructor& getConstructor(const args&) const;
	void addConstructor(const Constructor&);
	ctorMap getConstructors();
    const Method& getMethod(const std::string&, args) const;
    void addMethod(const Method&);
    methMap getMethods();
    Field getField(const std::string&);
    void addField(const Field&);
    fldMap getFields();
    fldVec getFieldVec();
    void setSize(const int&);
    int getSize();
	const std::string& getDestRefName() const;
	void setDestRefName(const std::string& destRefName);
	std::string getOperatorRefName(const std::string&);
	void addOperatorRefName(const std::string& oper, const std::string& rn);
	std::string getContRefName();
	void setContRefName(const std::string& rn);
	bool isAbstractClass();
};

#endif /* CLASSINFO_H_ */
