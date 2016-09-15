/*
	Copyright 2009-2012, Sumeet Chhetri 
  
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

typedef std::map<std::string,Constructor> ctorMap;
typedef std::map<std::string,Method> methMap;
typedef std::map<std::string,Field> fldMap;
typedef std::vector<Field> fldVec;
typedef std::vector<std::string> args;
class ClassInfo {
public:
	ClassInfo();
	virtual ~ClassInfo();
    //void *getInstance() const;
    //void setInstance(void *);
    std::string getClassName() const;
    void setClassName(const std::string&);
    std::string getBase() const;
    void setBase(const std::string&);
    void setNamespace(const std::string&);
    std::string getNamespace();
    /*string getPublic_fields() const;
    void setPublic_fields(const std::string&);
    std::string getPrivate_fields() const;
    void setPrivate_fields(const std::string&);
    std::string getProtected_fields() const;
    void setProtected_fields(const std::string&);
    std::string getPublic_meths() const;
    void setPublic_meths(const std::string&);
    std::string getPrivate_meths() const;
    void setPrivate_meths(const std::string&);
    std::string getProtected_meths() const;
    void setProtected_meths(const std::string&);
    std::vector<Method> getMethods() const;
    void setMethods(const std::vector<Method>&);
    std::vector<Field> getFields() const;
    void setFields(const std::vector<Field>&);
    void setMeths(const methMap&);*/
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
private:
	std::string className;
	std::string base;
	std::string namespc;
	int size;
	/*string public_fields;
	std::string private_fields;
	std::string protected_fields;
	std::string public_meths;
	std::string private_meths;
	std::string protected_meths;
	std::vector<Method> methods;
	std::vector<Field> fields;*/
	ctorMap ctors;
	methMap meths;
	fldMap fields;
	fldVec fldvec;
	std::string destRefName;
	std::map<std::string, std::string> operRefNames;
	std::string contRefName;
	static Constructor nullcons;
	static Method nullmeth;
};

#endif /* CLASSINFO_H_ */
