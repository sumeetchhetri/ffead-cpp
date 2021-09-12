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
 * Reflection.h
 *
 *  Created on: Aug 21, 2009
 *      Author: sumeet
 */

#ifndef REFLECTION_H_
#define REFLECTION_H_
#include "CastUtil.h"
#include "LoggerFactory.h"
#include "RegexUtil.h"
#include "iostream"
#include "fstream"
#include "sstream"
#include "map"
#include "vector"
#include "set"
#include <stdio.h>
#include "StringUtil.h"
#include "ClassStructure.h"
#include "TemplateEngine.h"
#include "MarkerHandler.h"
#include "CommonUtils.h"
#include <regex>

class Reflection {
	//bool generateClassInfoFromDD(const std::string&);
	//bool generateClassInfo(const std::string&);
	//void collectInfo(const std::string&, const std::string&);
	std::map<std::string, bool> methallpvstats;
	static std::map<std::string,bool> validcls;
	std::map<std::string,std::string> clspaths;
	std::map<std::string,std::string> classNamespaces;
	static std::map<std::string,int> nmspcIds;
	static std::map<std::string,std::string> nmspcIdVals;
	Logger logger;
	MarkerHandler handler;
	void handleNamespace(std::string data, std::string namepsc, std::map<std::string, ClassStructure, std::less<> >& clsvec, std::map<std::string, std::vector<std::string> >& glbnmspcs, std::vector<std::string> pragmas);
	int findless(const int& a, const int& b, const int& c);
	void collectInfo(std::string data, const std::string& flag, ClassStructure& cls, const std::vector<std::string>& mrktxt);
	void emptyBlocks(std::string& data, size_t);
public:
	std::map<std::string, ClassStructure, std::less<> > getClassStructures(const std::string& className, const std::string&);
	//strVec list(const std::string&);
	static bool isValidClass(const std::string& claz, const std::string& app)
	{
		return validcls.find(app+claz)!=validcls.end();
	}
	std::string getClassPath(const std::string& claz)
	{
		if(clspaths.find(claz)!=clspaths.end())
		{
			return clspaths[claz];
		}
		return "";
	}
	static std::string getNameSpaceId(const std::string& nmsp, const std::string& app)
	{
		if(nmspcIds.find(app+nmsp)!=nmspcIds.end())
		{
			return "ns" + CastUtil::fromNumber(nmspcIds[app+nmsp]);
		}
		return "";
	}
	static std::string getNameSpaceIdValue(const std::string& nmspid)
	{
		if(nmspcIdVals.find(nmspid)!=nmspcIdVals.end())
		{
			return nmspcIdVals[nmspid];
		}
		return "";
	}
	/*string getWsTreatedClassName()
	{
		if(nmSpc!="" && nmspcIds.find(nmSpc)!=nmspcIds.end())
		{
			return "ns" + CastUtil::fromNumber(nmspcIds[nmSpc]) + ":" + classN;
		}
		return classN;
	}*/
	std::string getFullyQualifiedClassName(const std::string& classN, const std::vector<std::string>& namespaces)
	{
		for (int var = 0; var < (int)namespaces.size(); ++var) {
			std::string tempfqnmclz = namespaces.at(var)+classN;
			if(classNamespaces.find(tempfqnmclz)!=classNamespaces.end())
			{
				return tempfqnmclz;
			}
		}
		return classN;
	}
	std::string getTreatedFullyQualifiedClassName(const std::string& classN, const std::vector<std::string>& namespaces, const bool& stripns= true)
	{
		for (int var = 0; var < (int)namespaces.size(); ++var) {
			std::string tempfqnmclz = namespaces.at(var)+classN;
			if(classNamespaces.find(tempfqnmclz)!=classNamespaces.end())
			{
				if(stripns)
				{
					StringUtil::replaceAll(tempfqnmclz, "::", "");
				}
				return tempfqnmclz;
			}
		}
		return classN;
	}
	Reflection();
	virtual ~Reflection();
	std::string getXSDDefinitions(std::map<std::string, ClassStructure, std::less<> >& allclsmap, const std::string& fqcn, Reflection& ref, const std::string& appname, std::string &trgnmspc, std::set<std::string> &allnmspcs, const std::string& dfnmspc, const std::string& resp);
	strVec getAfcObjectData(ClassStructure& classStructure, const bool& object, std::vector<std::string>& privf, bool &isOpForSet);
	//strVec getAfcObjectData(const std::string&, const bool&);
	propMap getDbTableInfo(const std::string&);
	std::string generateClassDefinitionsAll(std::map<std::string, std::map<std::string, ClassStructure, std::less<> >, std::less<> >& clsstrucMaps, std::string &includeRef, const std::vector<std::string>& apps);
	std::string generateSerDefinitionAll(std::map<std::string, std::map<std::string, ClassStructure, std::less<> >, std::less<> >& clsstrucMaps, std::string &includeRef, const bool& isBinary, std::string& objs, std::string& ajaxret, std::string& headers, std::string& typerefs, const std::vector<std::string>& apps);
	std::string generateClassDefinition(std::map<std::string, ClassStructure, std::less<> >& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, std::string &opers, const std::string& app);
	std::string generateClassDefinition_Old(std::map<std::string, ClassStructure, std::less<> >& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, std::string &opers, const std::string& app);
	std::string generateSerDefinition(std::map<std::string, ClassStructure, std::less<> >& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, const std::string& app);
	std::string generateClassDefinitions(std::map<std::string, ClassStructure, std::less<> >& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, std::string &opers, const std::string& app);
	std::string generateSerDefinitions(std::map<std::string, ClassStructure, std::less<> >& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, const bool& isBinary, std::string& objs, std::string &ajaxret, std::string& headers, std::string& typerefs, const std::string& app);
	std::string generateSerDefinitionBinary(std::map<std::string, ClassStructure, std::less<> >& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, const std::string& app);
	std::string generateAllSerDefinition(std::map<std::string, ClassStructure, std::less<> >& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, const std::string& app);
	static std::string getTypeName(std::string type);
	static bool isPrimitiveDataType(std::string type);
	void addMarker(const Marker& m);
};

#endif /* REFLECTION_H_ */
