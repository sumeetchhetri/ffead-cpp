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
 * Class.h
 *
 *  Created on: Nov 20, 2009
 *      Author: sumeet
 */

#ifndef CLASS_H_
#define CLASS_H_
#include "PropFileReader.h"
#include "Method.h"

typedef std::map<std::string,Method> Meth;
typedef std::map<std::string,Field> Fld;

class Class {
	std::string name;
	std::string access;
	std::string type;
	std::string package;
	std::string base;
	std::string interfaces;
	std::string lang;
	Meth methods;
	Fld fields;
	void addMethod(const Method&);
	void addField(const Field&);
public:
	Class();
	virtual ~Class();
	void generatee(const std::string&);
    std::string getName() const;
    void setName(const std::string&);
    std::string getAccess() const;
    void setAccess(const std::string&);
    std::string getType() const;
    void setType(const std::string&);
    std::string getPackage() const;
    void setPackage(const std::string&);
    std::string getBase() const;
    void setBase(const std::string&);
    std::string getInterfaces() const;
    void setInterfaces(const std::string&);
    std::string getLang() const;
    void setLang(const std::string&);
    //Method getMethod(const std::string&, const std::string&);
    Field getField(const std::string&);
};

#endif /* CLASS_H_ */
