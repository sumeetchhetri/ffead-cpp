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
 * Field.h
 *
 *  Created on: Oct 10, 2009
 *      Author: sumeet
 */

#ifndef FIELD_H_
#define FIELD_H_
#include "string"
#include "vector"

typedef void* (*GetFld) (void*);

class Field {
	GetFld f;
	std::string refName;
	std::string accessSpecifier;
	std::string fieldName;
	std::string type;
	std::string initVal;
	friend class FFEADContext;
public:
	Field();
	virtual ~Field();
	const std::string& getAccessSpecifier() const;
    void setAccessSpecifier(const std::string&);
    const std::string& getFieldName() const;
    void setFieldName(const std::string&);
    const std::string& getType() const;
    void setType(const std::string&);
    void clear();
	const std::string& getRefName() const;
	void setRefName(const std::string& refName);
	GetFld getIns() const;
};

#endif /* FIELD_H_ */
