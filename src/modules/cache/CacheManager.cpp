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
 * CacheManager.cpp
 *
 *  Created on: 12-Oct-2014
 *      Author: sumeetc
 */

#include "CacheManager.h"

std::map<std::string, CacheManager*> CacheManager::caches;
std::map<std::string, std::string> CacheManager::defDsnNames;
std::map<std::string, CacheInterface*> CacheManager::sevhCchImpls;
std::map<std::string, bool> CacheManager::appInitCompletionStatus;
bool CacheManager::isSinglEVH = false;

void CacheManager::init(bool issevh) {
	isSinglEVH = issevh;
}

void CacheManager::triggerAppInitCompletion(std::string appNameN) {
	std::string appName = appNameN;
	if(appName=="") {
		appName = CommonUtils::getAppName();
	} else {
		StringUtil::replaceAll(appName, "-", "_");
		RegexUtil::replace(appName, "[^a-zA-Z0-9_]+", "");
	}
	if(appInitCompletionStatus.find(appName)!=appInitCompletionStatus.end()) {
		appInitCompletionStatus[appName] = true;
	}
}

bool CacheManager::isInitCompleted() {
	bool flag = true;
	if(appInitCompletionStatus.size()>0) {
		std::map<std::string, bool>::iterator it = appInitCompletionStatus.begin();
		for(;it!=appInitCompletionStatus.end();++it) {
			flag &= it->second;
		}
	}
	return flag;
}

void CacheManager::initCache(const ConnectionProperties& props, const std::string& appNameN, GetClassBeanIns f) {
	Logger logger = LoggerFactory::getLogger("CacheManager");
	std::string name = StringUtil::trimCopy(props.getName());
	if(name=="")
	{
		throw std::runtime_error("Cache Name cannot be blank");
	}
	std::string appName = appNameN;
	StringUtil::replaceAll(appName, "-", "_");
	RegexUtil::replace(appName, "[^a-zA-Z0-9_]+", "");
	name = appName + name;

	if(StringUtil::toLowerCopy(props.getType()) != "custom") {
		if(caches.find(name)!=caches.end())
		{
			throw std::runtime_error("Cache Already exists");
		}
		if(props.getProperty("_isdefault_")=="true") {
			defDsnNames[appName] = StringUtil::trimCopy(props.getName());
		}

		try {
			CacheManager* mgr = new CacheManager(props);
			caches[name] = mgr;
		} catch (const std::exception& e) {
			logger.info("Error initializing Cache " + appNameN + "@" + props.getName() + " " + std::string(e.what()));
		}
	}

	Reflector* ref = GenericObject::getReflector();
	if(props.getProperty("init")!="") {
		std::string meth = props.getProperty("init");
		std::vector<std::string> v;
		StringUtil::split(v, meth, ".");
		if(v.size()==2) {
			CommonUtils::setAppName(appName);
			ClassBeanIns cbi;
			f(v.at(0), appNameN, &cbi);
			void* _temp = cbi.instance;
			try {
				if(_temp!=NULL) {
					args argus;
					vals valus;
					const Method& meth = cbi.clas->getMethod(v.at(1), argus);
					if(meth.getMethodName()!="")
					{
						appInitCompletionStatus[appName] = false;
						ref->invokeMethodGVP(_temp, meth, valus);
					}
				}
			} catch(const std::exception& e) {
				logger.info("Error during init call for Cache " + appNameN + "@" + props.getName() + " " + std::string(e.what()));
			}
			if(cbi.cleanUp) {
				ref->destroy(_temp, v.at(0), appName);
			}
		}
	}
}

void CacheManager::destroy()
{
	std::map<std::string, CacheManager*>::iterator it;
	for(it=caches.begin();it!=caches.end();++it)
	{
		if(it->second!=NULL)
		{
			delete it->second->pool;
			delete it->second;
		}
	}
	caches.clear();
	std::map<std::string, CacheInterface*>::iterator it1;
	for(it1=sevhCchImpls.begin();it1!=sevhCchImpls.end();++it1)
	{
		if(it1->second!=NULL)
		{
			delete it1->second;
		}
	}
	sevhCchImpls.clear();
}

void CacheManager::cleanImpl(CacheInterface* ccImpl) {
	if(!isSinglEVH) {
		if(StringUtil::toLowerCopy(ccImpl->pool->getProperties().getType())=="memory")
		{
			delete (MemoryCacheImpl*)ccImpl;
		}
		else if(StringUtil::toLowerCopy(ccImpl->pool->getProperties().getType())=="memcached")
		{
#ifdef INC_MEMCACHED
			delete (MemcachedImpl*)ccImpl;
#endif
		}
		else if(StringUtil::toLowerCopy(ccImpl->pool->getProperties().getType())=="redis")
		{
#ifdef INC_REDISCACHE
			delete (RedisCacheImpl*)ccImpl;
#endif
		}
	}
}

CacheInterface* CacheManager::getImpl(std::string name, std::string appName) {
	if(appName=="") {
		appName = CommonUtils::getAppName();
	} else {
		StringUtil::replaceAll(appName, "-", "_");
		RegexUtil::replace(appName, "[^a-zA-Z0-9_]+", "");
	}
	StringUtil::trim(name);
	if(name=="") {
		name = defDsnNames[appName];
	}
	name = appName + name;
	if(caches.find(name)==caches.end()) {
		throw std::runtime_error("Cache Not found...");
	}
	//This will cause serious issues if set/used in multi-threaded mode instead of single process mode
	if(isSinglEVH) {
		if(sevhCchImpls.find(name)!=sevhCchImpls.end()) {
			return sevhCchImpls[name];
		}
	}
	CacheManager* cchMgr = caches[name];
	CacheInterface* t = NULL;
	if(StringUtil::toLowerCopy(cchMgr->props.getType())=="memory")
	{
		t = new MemoryCacheImpl(cchMgr->pool);
	}
	else if(StringUtil::toLowerCopy(cchMgr->props.getType())=="memcached")
	{
#ifdef INC_MEMCACHED
		t = new MemcachedImpl(cchMgr->pool);
#endif
	}
	else if(StringUtil::toLowerCopy(cchMgr->props.getType())=="redis")
	{
#ifdef INC_REDISCACHE
		t = new RedisCacheImpl(cchMgr->pool);
#endif
	}
	t->init();
	//This will cause serious issues if set/used in multi-threaded mode instead of single process mode
	if(isSinglEVH) {
		sevhCchImpls[name] = t;
	}
	return t;
}

CacheManager::CacheManager(const ConnectionProperties& props) {
	logger = LoggerFactory::getLogger("CacheManager");
	this->reflector = NULL;
	this->pool = NULL;
	this->props = props;
	if(StringUtil::toLowerCopy(props.getType()) == "memory") {
		this->pool = new MemoryCacheConnectionPool(props);
	} else if(StringUtil::toLowerCopy(props.getType()) == "memcached") {
#ifdef INC_MEMCACHED
		this->pool = new MemcachedConnectionPool(props);
#endif
	} else if(StringUtil::toLowerCopy(props.getType()) == "redis") {
#ifdef INC_REDISCACHE
		this->pool = new RedisCacheConnectionPool(props);
#endif
	}
}

CacheManager::~CacheManager() {
}

