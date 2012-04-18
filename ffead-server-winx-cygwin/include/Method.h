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
 * Method.h
 *
 *  Created on: Oct 10, 2009
 *      Author: sumeet
 */

#ifndef METHOD_H_
#define METHOD_H_
#include "string"
#include "vector"
#include "Field.h"
using namespace std;

typedef vector<string> args;
typedef vector<void *> vals;
class Method {
	string accessSpecifier;
	string methodName;
	string returnType;
	args argumentTypes;
	void addField(Field);
	/*args methCalls;
	args methCalls;*/
public:
	Method();
	virtual ~Method();
    string getAccessSpecifier() const;
    void setAccessSpecifier(string);
    string getMethodName() const;
    void setMethodName(string);
    string getReturnType() const;
    void setReturnType(string);
    args getArgumentTypes() const;
    void setArgumentTypes(args);
    int getArgNum() const;
    void clear();
    //string getKey();
};

#endif /* METHOD_H_ */
