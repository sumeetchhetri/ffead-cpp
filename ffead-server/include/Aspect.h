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
 * Aspect.h
 *
 *  Created on: Oct 17, 2009
 *      Author: sumeet
 */

#ifndef ASPECT_H_
#define ASPECT_H_
#include "string"
#include "vector"

using namespace std;
typedef vector<string> args;
class Aspect {
	string strict;
	string className;
	string when;
	string code;
	string methodName;
	string returnType;
	string arguTypes;
	args argumentTypes;
public:
	Aspect();
	virtual ~Aspect();
    string getMethodName() const;
    void setMethodName(string);
    string getReturnType() const;
    void setReturnType(string);
    args getArgumentTypes() const;
    void setArgumentTypes(args);
    int getArgNum() const;
    void clear();
    string getStrict() const;
    void setStrict(string);
    string getClassName() const;
    void setClassName(string);
    string getWhen() const;
    void setWhen(string);
    string getCode() const;
    void setCode(string );
    string getArguTypes() const;
    void setArguTypes(string);
};

#endif /* ASPECT_H_ */
