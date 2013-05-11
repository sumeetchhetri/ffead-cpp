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
 * Reflection.h
 *
 *  Created on: Aug 21, 2009
 *      Author: sumeet
 */

#ifndef REFLECTION_H_
#define REFLECTION_H_
#include "CastUtil.h"
#include "AfcUtil.h"
#include "Logger.h"
#include "RegexUtil.h"

class ClassStructure
{
	bool prosetser;
	strVec pub,pri,pro;
	string classN,baseClassN,bcvisib,nmSpc;
	vector<string> namespaces;
	friend class Reflection;
public:
	string appName;
	void toString()
	{
		cout << classN << " " << baseClassN << " " << bcvisib << " " << nmSpc << endl;
		for (int var = 0; var < (int)namespaces.size(); ++var) {
			cout << "using => " << namespaces.at(var) << endl;
		}
		for (int var = 0; var < (int)pub.size(); ++var) {
			cout << "public => " << pub.at(var) << endl;
		}
		for (int var = 0; var < (int)pri.size(); ++var) {
			cout << "private => "<< pri.at(var) << endl;
		}
		for (int var = 0; var < (int)pro.size(); ++var) {
			cout << "protected => "<< pro.at(var) << endl;
		}
	}
	string getTreatedClassName(bool flag)
	{
		if(flag)
		{
			string nm = nmSpc;
			StringUtil::replaceAll(nm, "::", "_");
			return nm+classN;
		}
		else
		{
			return classN;
		}
	}
	string getFullyQualifiedClassName()
	{
		return nmSpc+classN;
	}
};

class Reflection {
	//bool generateClassInfoFromDD(string);
	//bool generateClassInfo(string);
	//void collectInfo(string,string);
	map<string,bool> methsall;
	map<string,bool> invalidcls;
	map<string,string> clspaths;
	map<string,string> classNamespaces;
	map<string,int> nmspcIds;
	Logger logger;
	void handleNamespace(string data, string namepsc, map<string, ClassStructure>& clsvec, map<string, vector<string> >& glbnmspcs);
	int findless(int a, int b, int c);
	void collectInfo(string data, string flag, ClassStructure& cls);
public:
	map<string, ClassStructure> getClassStructures(string className);
	strVec list(string);
	bool isValidClass(string claz)
	{
		return invalidcls.find(claz)==invalidcls.end();
	}
	string getClassPath(string claz)
	{
		if(clspaths.find(claz)!=clspaths.end())
		{
			return clspaths[claz];
		}
		return "";
	}
	/*string getWsTreatedClassName()
	{
		if(nmSpc!="" && nmspcIds.find(nmSpc)!=nmspcIds.end())
		{
			return "ns" + CastUtil::lexical_cast<string>(nmspcIds[nmSpc]) + ":" + classN;
		}
		return classN;
	}*/
	string getFullyQualifiedClassName(string classN, vector<string> namespaces)
	{
		for (int var = 0; var < (int)namespaces.size(); ++var) {
			string tempfqnmclz = namespaces.at(var)+classN;
			if(classNamespaces.find(tempfqnmclz)!=classNamespaces.end())
			{
				return tempfqnmclz;
			}
		}
		return classN;
	}
	string getTreatedFullyQualifiedClassName(string classN, vector<string> namespaces)
	{
		for (int var = 0; var < (int)namespaces.size(); ++var) {
			string tempfqnmclz = namespaces.at(var)+classN;
			if(classNamespaces.find(tempfqnmclz)!=classNamespaces.end())
			{
				StringUtil::replaceAll(tempfqnmclz, "::", "");
				return tempfqnmclz;
			}
		}
		return classN;
	}
	Reflection();
	virtual ~Reflection();
	strVec getAfcObjectData(ClassStructure classStructure,bool object,vector<string>& privf, bool &isOpForSet);
	//strVec getAfcObjectData(string,bool);
	propMap getDbTableInfo(string);
	string generateClassDefinitionsAll(strVec,string &,strVec);
	string generateSerDefinitionAll(strVec all,string &includeRef, bool isBinary,string&,string&,string&,string&,strVec);
	string generateClassDefinition(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string &opers,string app);
	string generateSerDefinition(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string app);
	string generateClassDefinitions(string,string &,string &,string &,string &,string &,string app);
	string generateSerDefinitions(string,string &,string &,string &,string &,bool,string&,string &,string&,string&,string app);
	string generateSerDefinitionBinary(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string app);
};

#endif /* REFLECTION_H_ */
