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
#include "Logger.h"
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
    string getAppTableColMapping(string table,string propertyName)
    {
    	if(this->appTableColMapping.find(table)!=this->appTableColMapping.end())
    	{
    		if(this->appTableColMapping[table].find(propertyName)!=this->appTableColMapping[table].end())
    			return this->appTableColMapping[table][propertyName];
    		else
    			return "";
    	}
    	return "";
    }

    string getTableAppColMapping(string table,string columnName)
	{
		if(this->tableAppColMapping.find(table)!=this->tableAppColMapping.end())
		{
			if(this->tableAppColMapping[table].find(columnName)!=this->tableAppColMapping[table].end())
				return this->tableAppColMapping[table][columnName];
			else
				return "";
		}
		return "";
	}

    strMap getAppTableColMapping(string table)
	{
		if(this->appTableColMapping.find(table)!=this->appTableColMapping.end())
		{
			return this->appTableColMapping[table];
		}
		strMap tem;
		return tem;
	}

    strMap getTableAppColMapping(string table)
	{
		if(this->tableAppColMapping.find(table)!=this->tableAppColMapping.end())
		{
			return this->tableAppColMapping[table];
		}
		strMap tem;
		return tem;
	}

    void setAppTableColMapping(smstrMap appTableColMapping)
    {
        this->appTableColMapping = appTableColMapping;
        smstrMap::iterator it;
        for(it=appTableColMapping.begin();it!=appTableColMapping.end();it++) {
        	strMap tempo;
        	strMap::iterator ite;
        	for(ite=it->second.begin();ite!=it->second.end();ite++) {
        		tempo[ite->second] = ite->first;
        	}
        	this->tableAppColMapping[appTableClassMapping[it->first]] = tempo;
		}
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
        smstrMap::iterator it;
		for(it=appTableColMapping.begin();it!=appTableColMapping.end();it++) {
			strMap tempo;
			strMap::iterator ite;
			for(ite=it->second.begin();ite!=it->second.end();ite++) {
				tempo[ite->second] = ite->first;
			}
			this->tableAppColMapping[appTableClassMapping[it->first]] = tempo;
		}
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
	Logger logger;
	map<string,CibernateConnectionPool*> cpools;
	map<string,Mapping*> mappings;
	static CibernateConnPools* get();
	bool initialized;
public:
	CibernateConnPools();
	virtual ~CibernateConnPools();
	static void addPool(int,string,string,string,string,string);
	static void addMapping(string,Mapping*);
	static bool isInitialized();
	static CibernateConnectionPool* getPool(string);
	static Mapping* getMapping(string);
	static map<string,Mapping*> getMappings()
	{
		map<string,Mapping*> temp;
		if(get()!=NULL)
			return get()->mappings;
		return temp;
	}
	static void destroy();
};

#endif /* CIBERNATECONNPOOLS_H_ */
