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
 * CibernateConnPools.h
 *
 *  Created on: Mar 16, 2010
 *      Author: sumeet
 */

#ifndef CIBERNATECONNPOOLS_H_
#define CIBERNATECONNPOOLS_H_
#include "CibernateConnectionPool.h"
#include "map"
#include "LoggerFactory.h"
typedef map<string,string> strMap;
typedef map<string,strMap> smstrMap;

class DBRel
{
public:
	int type;//1->1-1 2->1-N 3->N-N
	string fk;
	string pk;
	string clsName;
	string field;
};
typedef map<string,vector<DBRel> > relMap;
class Mapping
{
	smstrMap appTableColMapping;
	smstrMap tableAppColMapping;
	strMap appTableClassMapping;
	relMap appTableRelMapping;
public:
    string getAppTableColMapping(string table,string propertyName);
    string getTableAppColMapping(string table,string columnName);
    strMap getAppTableColMapping(string table);
    strMap getTableAppColMapping(string table);
    void setAppTableColMapping(smstrMap appTableColMapping);
    string getAppTableClassMapping(string claz);
    void setAppTableClassMapping(strMap appTableClassMapping);
    vector<DBRel> getAppTablerelMapping(string claz);
    void setAppTableRelMapping(relMap appTableRelMapping);
};
class CibernateConnPools {
	Logger logger;
	map<string,CibernateConnectionPool*> cpools;
	map<string,Mapping*> mappings;
	static CibernateConnPools* get();
	bool initialized;
	static CibernateConnPools* instance;
public:
	CibernateConnPools();
	virtual ~CibernateConnPools();
	static void addPool(int,string,string,string,string,string);
	static void addMapping(string,Mapping*);
	static bool isInitialized();
	static CibernateConnectionPool* getPool(string);
	static Mapping* getMapping(string);
	static map<string,Mapping*> getMappings();
	static void destroy();
};

#endif /* CIBERNATECONNPOOLS_H_ */
