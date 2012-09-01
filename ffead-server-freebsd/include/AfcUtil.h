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
 * AfcUtil.h
 *
 *  Created on: Aug 27, 2009
 *      Author: sumeet
 */


#include "Reflection.h"
#include "Constants.h"
#include "HttpRequest.h"
#include "CastUtil.h"
#include "Logger.h"


#ifndef AFCUTIL_H_
#define AFCUTIL_H_

class AfcUtil {
	static Logger logger;
public:
	AfcUtil();
	virtual ~AfcUtil();
	static string generateJsObjects(strVec,string,string&,string,string &,strVec);
	static string generateJsInterfaces(strVec,string,string&,string,string &,string);
	static string updateAjaxInterface(strVec,string,string,string,string);
	static void writeTofile(string,string,bool);
	static string execute(HttpRequest);
	static string camelCased(const string&);
	static string reverseCamelCased(const string&);
	static string generateJsObjectsAll(vector<string>,strVec,vector<bool>,string &,string &,string &,vector<string>);
};

#endif /* AFCUTIL_H_ */
