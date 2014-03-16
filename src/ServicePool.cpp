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
 * ServicePool.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: sumeet
 */

#include "ServicePool.h"
static ServicePool *instance=NULL;
static map<string,Service> servicePool;
ServicePool::ServicePool()
{
	instance = NULL;
}

ServicePool::~ServicePool() {
	// TODO Auto-generated destructor stub
}

ServicePool* ServicePool::getInstance()
{
	if(instance==NULL)
		instance = new ServicePool;
	return instance;
}

string ServicePool::registerService(string name,Service service)// will return a unique identification for this service
{
	time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
	timeinfo = localtime(&rawtime);
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);

	string yr = CastUtil::lexical_cast<string>(timeinfo->tm_year);
	string mo = CastUtil::lexical_cast<string>(timeinfo->tm_mon);
	string da = CastUtil::lexical_cast<string>(timeinfo->tm_mday);
	string hr = CastUtil::lexical_cast<string>(timeinfo->tm_hour);
	string mm = CastUtil::lexical_cast<string>(timeinfo->tm_min);
	string ms = CastUtil::lexical_cast<string>(((en.tv_sec * 1000000000) + en.tv_nsec)/1000000);

	string regName = (name+yr+mo+da+hr+mm+ms);
	servicePool[regName] = service;
	return regName;
}

bool ServicePool::unRegisterService(string name)//unregister will require the unique id
{
	map<string,Service>::iterator it;
	if(it==servicePool.end())
		return false;
	it = servicePool.find(name);
	servicePool.erase(it);
	return true;
}

vector<string> ServicePool::getServices(string access)//return a list of available services
{
	vector<string> services;
	map<string,Service>::iterator it;
	for(it=servicePool.begin();it!=servicePool.end();++it)
	{
		services.push_back(it->first);
	}
	return services;
}

Service ServicePool::getService(string regName)
{
	return servicePool[regName];
}
