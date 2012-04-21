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
 * Class.h
 *
 *  Created on: Nov 20, 2009
 *      Author: sumeet
 */

#ifndef CLASS_H_
#define CLASS_H_
#include "PropFileReader.h"
#include "Method.h"

typedef map<string,Method> Meth;
typedef map<string,Field> Fld;

class Class {
	string name;
	string access;
	string type;
	string package;
	string base;
	string interfaces;
	string lang;
	Meth methods;
	Fld fields;
	void addMethod(Method);
	void addField(Field);
public:
	Class();
	virtual ~Class();
	void generatee(string);
    string getName() const;
    void setName(string);
    string getAccess() const;
    void setAccess(string);
    string getType() const;
    void setType(string);
    string getPackage() const;
    void setPackage(string);
    string getBase() const;
    void setBase(string);
    string getInterfaces() const;
    void setInterfaces(string);
    string getLang() const;
    void setLang(string);
    //Method getMethod(string,string);
    Field getField(string);
};

#endif /* CLASS_H_ */
