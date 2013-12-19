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
 * CibernateConnPools.cpp
 *
 *  Created on: Mar 16, 2010
 *      Author: sumeet
 */

#include "CibernateConnPools.h"
CibernateConnPools *CibernateConnPools::instance = NULL;

CibernateConnPools* CibernateConnPools::get()
{
	if(instance==NULL)
		instance = new CibernateConnPools;
	return instance;
}

void CibernateConnPools::destroy()
{
	if(instance!=NULL)
	{
		delete instance;
	}
}

CibernateConnPools::CibernateConnPools()
{
	logger = LoggerFactory::getLogger("CibernateConnPools");
}

CibernateConnPools::~CibernateConnPools() {
	map<string,CibernateConnectionPool*>::iterator it1;
	for (it1=cpools.begin();it1!=cpools.end();it1++) {
		delete it1->second;
	}
	map<string,Mapping*>::iterator it2;
	for (it2=mappings.begin();it2!=mappings.end();it2++) {
		delete it2->second;
	}
	logger << "\nDestructed CibernateConnPools" << flush;
}

void CibernateConnPools::addPool(int size,string uid,string pwd,string dsn,string appName,string dialect)
{
	get()->cpools[appName] = new CibernateConnectionPool(size,dsn,uid,pwd,dialect);
	instance->initialized = get()->cpools[appName]->initialized;
}

bool CibernateConnPools::isInitialized()
{
	return instance->initialized;
}
CibernateConnectionPool* CibernateConnPools::getPool(string appName)
{
	if(instance->cpools.find(appName)!=instance->cpools.end())
		return instance->cpools[appName];
	return NULL;
}

Mapping* CibernateConnPools::getMapping(string appName)
{
	if(instance->mappings.find(appName)!=instance->mappings.end())
		return instance->mappings[appName];
	return NULL;
}

void CibernateConnPools::addMapping(string appName,Mapping* mapping)
{
	get()->mappings[appName] = mapping;
}

string Mapping::getAppTableColMapping(string table,string propertyName)
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

string Mapping::getTableAppColMapping(string table,string columnName)
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

strMap Mapping::getAppTableColMapping(string table)
{
	if(this->appTableColMapping.find(table)!=this->appTableColMapping.end())
	{
		return this->appTableColMapping[table];
	}
	strMap tem;
	return tem;
}

strMap Mapping::getTableAppColMapping(string table)
{
	if(this->tableAppColMapping.find(table)!=this->tableAppColMapping.end())
	{
		return this->tableAppColMapping[table];
	}
	strMap tem;
	return tem;
}

void Mapping::setAppTableColMapping(smstrMap appTableColMapping)
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

string Mapping::getAppTableClassMapping(string claz)
{
	if(this->appTableClassMapping.find(claz)!=this->appTableClassMapping.end())
		return this->appTableClassMapping[claz];
	else
		return "";
}

void Mapping::setAppTableClassMapping(strMap appTableClassMapping)
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

vector<DBRel> Mapping::getAppTablerelMapping(string claz)
{
	if(this->appTableRelMapping.find(claz)!=this->appTableRelMapping.end())
		return this->appTableRelMapping[claz];
	else
	{
		vector<DBRel> rel;
		return rel;
	}
}

void Mapping::setAppTableRelMapping(relMap appTableRelMapping)
{
	this->appTableRelMapping = appTableRelMapping;
}

map<string,Mapping*> CibernateConnPools::getMappings()
{
	map<string,Mapping*> temp;
	if(get()!=NULL)
		return get()->mappings;
	return temp;
}
