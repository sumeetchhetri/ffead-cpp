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
 * AfcUtil.h
 *
 *  Created on: Aug 27, 2009
 *      Author: sumeet
 */


#include "Reflection.h"
#include "Constants.h"
#include "CastUtil.h"
#include "iostream"
#include "fstream"
#include "sstream"
#include "map"
#include "vector"
#include "StringUtil.h"
#include "LoggerFactory.h"

using namespace std;
typedef vector<string> strVec;
typedef map<string, string> propMap;

#ifndef AFCUTIL_H_
#define AFCUTIL_H_

class ClassStructure;
class Reflection;

class AfcUtil {
public:
	AfcUtil();
	virtual ~AfcUtil();
	static string generateJsObjects(strVec obj,ClassStructure classstruc,string &headers,string &objs,strVec pobj, bool isOpForSet, string& typrefs,strVec minfo,string app,string clspth,Reflection ref);
	static string generateReadObjects(string type, string name, bool priv, bool ptr, string typ, string app, ClassStructure classstruc,Reflection ref);
	static string generateReadVectorObjects(string type, string name, bool priv, bool ptr, string typ, string conttype,string app, ClassStructure classstruc,Reflection ref);
	static string generateToJSONObjects(string type, string name, bool priv, bool end, string &retu, string &headers, string &objs, string typ, bool ptr,string app, ClassStructure classstruc,Reflection ref);
	static string generateToJSONVectorObjects(string type, string name, bool priv, string &retu, string &headers, string &objs, string typ, bool ptr, string stlcnttyp,string app, ClassStructure classstruc,Reflection ref);

	static string generateJsObjectsAll(map<string, ClassStructure> allclsmap);
	static string generateJsObjects(strVec obj,string claz,strVec pobj,strVec minfo);

	static void writeTofile(string,string,bool);
	static string camelCased(const string&);
	static string reverseCamelCased(const string&);

	static string generateJsInterfacessAll(map<string, ClassStructure> allclsmap,string &infjs,map<string, string> ajintpthMap,strVec afcd, Reflection ref);
	static string generateJsInterfaces(strVec obj,ClassStructure classstruc,string path,string &infjs,string appName,map<string, string> ajintpthMap, Reflection ref);
	static string updateAjaxInterface(strVec emp,ClassStructure classstruc,string pars,string parswt,string types,string appName, Reflection ref);


};

#endif /* AFCUTIL_H_ */
