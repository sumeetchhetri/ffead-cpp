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
 * Constructor.h
 *
 *  Created on: Mar 30, 2010
 *      Author: sumeet
 */

#ifndef CONSTRUCTOR_H_
#define CONSTRUCTOR_H_
#include "string"
#include "vector"

typedef std::vector<void *> vals;
typedef void* (*NewInst) (vals,bool);

class Constructor {
	NewInst f;
	std::string refName;
	std::string name;
	std::vector<std::string> argumentTypes;
	friend class FFEADContext;
public:
	NewInst getIns() const;
	Constructor();
	virtual ~Constructor();
	std::vector<std::string> getArgumentTypes() const;
	void setArgumentTypes(const std::vector<std::string>&);
	int getArgNum() const;
    std::string getName() const;
    void setName(const std::string& name);
    void clear();
	const std::string& getRefName() const;
	void setRefName(const std::string& refName);
};

#endif /* CONSTRUCTOR_H_ */
