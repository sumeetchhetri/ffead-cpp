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
 * CibernateConnPools.h
 *
 *  Created on: Mar 16, 2010
 *      Author: sumeet
 */

#ifndef CIBERNATECONNPOOLS_H_
#define CIBERNATECONNPOOLS_H_
#include "CibernateConnectionPool.h"
#include "map"
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
	strMap appTableClassMapping;
	relMap appTableRelMapping;
public:
    string getAppTableColMapping(string table,string column)
    {
    	if(this->appTableColMapping.find(table)!=this->appTableColMapping.end())
    	{
    		if(this->appTableColMapping[table].find(column)!=this->appTableColMapping[table].end())
    			return this->appTableColMapping[table][column];
    		else
    			return "";
    	}
    	return "";
    }

    void setAppTableColMapping(smstrMap appTableColMapping)
    {
        this->appTableColMapping = appTableColMapping;
    }

    string getAppTableClassMapping(string claz)
    {
    	if(this->appTableClassMapping.find(claz)!=this->appTableClassMapping.end())
    		return this->appTableClassMapping[claz];
    	else
    		return "";
    }

    void setAppTableClassMapping(strMap appTableClassMapping)
    {
        this->appTableClassMapping = appTableClassMapping;
    }

    vector<DBRel> getAppTablerelMapping(string claz)
	{
		if(this->appTableRelMapping.find(claz)!=this->appTableRelMapping.end())
			return this->appTableRelMapping[claz];
		else
		{
			vector<DBRel> rel;
			return rel;
		}
	}

    void setAppTableRelMapping(relMap appTableRelMapping)
    {
	    this->appTableRelMapping = appTableRelMapping;
    }

};
class CibernateConnPools {
	map<string,CibernateConnectionPool*> cpools;
	map<string,Mapping*> mappings;
	static CibernateConnPools* get();
	bool initialized;
public:
	CibernateConnPools();
	virtual ~CibernateConnPools();
	static void addPool(int,string,string,string,string);
	static void addMapping(string,Mapping*);
	static bool isInitialized();
	static CibernateConnectionPool* getPool(string);
	static Mapping* getMapping(string);
};

#endif /* CIBERNATECONNPOOLS_H_ */
