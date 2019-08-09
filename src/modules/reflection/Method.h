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
 * Method.h
 *
 *  Created on: Oct 10, 2009
 *      Author: sumeet
 */

#ifndef METHOD_H_
#define METHOD_H_
#include "string"
#include "vector"

typedef std::vector<std::string> args;
typedef std::vector<void *> vals;
typedef void* (*GetMeth) (void*,vals,bool);

class Method {
	GetMeth f;
	std::string refName;
	std::string accessSpecifier;
	std::string methodName;
	std::string returnType;
	args argumentTypes;
	bool isstat;
	friend class FFEADContext;
public:
	Method();
	virtual ~Method();
	const std::string& getAccessSpecifier() const;
    void setAccessSpecifier(const std::string&);
    const std::string& getMethodName() const;
    void setMethodName(const std::string&);
    const std::string& getReturnType() const;
    void setReturnType(const std::string&);
    args getArgumentTypes() const;
    void setArgumentTypes(const args&);
    int getArgNum() const;
    void clear();
    bool isStatic();
    void setIsStatic(const bool& isStatic);
	const std::string& getRefName() const;
	void setRefName(const std::string& refName);
	GetMeth getIns() const;
};

#endif /* METHOD_H_ */
