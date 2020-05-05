/*
	Copyright 2009-2020, Sumeet Chhetri 
  
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


typedef std::vector<std::string> strVec;
typedef std::map<std::string, std::string> propMap;

#ifndef AFCUTIL_H_
#define AFCUTIL_H_

class ClassStructure;
class Reflection;

class AfcUtil {
public:
	AfcUtil();
	virtual ~AfcUtil();
	static std::string generateJsObjects(const strVec& obj, ClassStructure& classstruc, std::string &headers, std::string &objs, strVec pobj, const bool& isOpForSet, std::string& typrefs, const strVec& minfo, const std::string& app, const std::string& clspth, Reflection& ref);
	static std::string generateReadObjects(const std::string& type, const std::string& name, const bool& priv, const bool& ptr, const std::string& typ, const std::string& app, ClassStructure& classstruc, Reflection& ref);
	static std::string generateReadVectorObjects(const std::string& type, const std::string& name, const bool& priv, const bool& ptr, const std::string& typ, const std::string& conttype, const std::string& app, ClassStructure& classstruc, Reflection& ref);
	static std::string generateToJSONObjects(const std::string& type, const std::string& name, const bool& priv, const bool& end, std::string &retu, std::string &headers, std::string &objs, const std::string& typ, const bool& ptr, const std::string& app, ClassStructure& classstruc, Reflection& ref);
	static std::string generateToJSONVectorObjects(const std::string& type, const std::string& name, const bool& priv, std::string &retu, std::string &headers, std::string &objs, const std::string& typ, const bool& ptr, const std::string& stlcnttyp, const std::string& app, ClassStructure& classstruc, Reflection& ref);

	static std::string generateJsObjectsAll(std::map<std::string, ClassStructure, std::less<> >& allclsmap);
	static std::string generateJsObjects(strVec obj, const std::string& claz, strVec pobj, const strVec& minfo);

	static void writeTofile(const std::string&, const std::string&, const bool&);
	static std::string camelCased(const std::string&);
	static std::string reverseCamelCased(const std::string&);

	static std::string generateJsInterfacessAll(std::map<std::string, ClassStructure, std::less<> >& allclsmap, std::string &infjs, std::map<std::string, std::string, std::less<> >& ajintpthMap, strVec& afcd, Reflection& ref);
	static std::string generateJsInterfaces(const strVec& obj, ClassStructure& classstruc, const std::string& path, std::string &infjs, const std::string& appName, std::map<std::string, std::string, std::less<> >& ajintpthMap, Reflection& ref);
	static std::string updateAjaxInterface(const strVec& emp, ClassStructure& classstruc, const std::string& pars, const std::string& parswt, const std::string& types, const std::string& appName, Reflection& ref);


};

#endif /* AFCUTIL_H_ */
