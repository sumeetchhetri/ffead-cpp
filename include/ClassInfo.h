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
using namespace std;
typedef map<string,Constructor> ctorMap;
typedef map<string,Method> methMap;
typedef map<string,Field> fldMap;
typedef vector<Field> fldVec;
typedef vector<string> args;
class ClassInfo {
public:
	ClassInfo();
	virtual ~ClassInfo();
    //void *getInstance() const;
    //void setInstance(void *);
    string getClassName() const;
    void setClassName(string);
    string getBase() const;
    void setBase(string);
    void setNamespace(string);
    string getNamespace();
    /*string getPublic_fields() const;
    void setPublic_fields(string);
    string getPrivate_fields() const;
    void setPrivate_fields(string);
    string getProtected_fields() const;
    void setProtected_fields(string);
    string getPublic_meths() const;
    void setPublic_meths(string);
    string getPrivate_meths() const;
    void setPrivate_meths(string);
    string getProtected_meths() const;
    void setProtected_meths(string);
    vector<Method> getMethods() const;
    void setMethods(vector<Method>);
    vector<Field> getFields() const;
    void setFields(vector<Field>);
    void setMeths(methMap);*/
    Constructor getConstructor(args);
	void addConstructor(Constructor);
	ctorMap getConstructors();
    Method getMethod(string,args);
    void addMethod(Method);
    methMap getMethods();
    Field getField(string);
    void addField(Field);
    fldMap getFields();
    fldVec getFieldVec();
    void setSize(int);
    int getSize();
private:
	void* instance;
	string className;
	string base;
	string namespc;
	int size;
	/*string public_fields;
	string private_fields;
	string protected_fields;
	string public_meths;
	string private_meths;
	string protected_meths;
	vector<Method> methods;
	vector<Field> fields;*/
	ctorMap ctors;
	methMap meths;
	fldMap fields;
	fldVec fldvec;
};

#endif /* CLASSINFO_H_ */
