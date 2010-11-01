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
 * CibernateConnPools.cpp
 *
 *  Created on: Mar 16, 2010
 *      Author: sumeet
 */

#include "CibernateConnPools.h"
static CibernateConnPools *instance = NULL;

CibernateConnPools* CibernateConnPools::get()
{
	if(instance==NULL)
		instance = new CibernateConnPools;
	return instance;
}

CibernateConnPools::CibernateConnPools()
{

}

CibernateConnPools::~CibernateConnPools() {
	instance=NULL;
	cout << "\nDestructed CibernateConnPools" << flush;
}

void CibernateConnPools::addPool(int size,string uid,string pwd,string dsn,string appName)
{
	get()->cpools[appName] = new CibernateConnectionPool(size,dsn,uid,pwd);
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
