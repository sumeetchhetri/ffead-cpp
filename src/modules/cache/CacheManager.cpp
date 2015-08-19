/*
 * CacheManager.cpp
 *
 *  Created on: 12-Oct-2014
 *      Author: sumeetc
 */

#include "CacheManager.h"

map<string, CacheManager*> CacheManager::caches;
string CacheManager::defDsnName;

void CacheManager::initCache(const ConnectionProperties& props, const string& appName) {
	string name = StringUtil::trimCopy(props.getName());
	if(name=="")
	{
		throw "Cache Name cannot be blank";
	}
	string appNameN = CommonUtils::getAppName(appName);
	name = appNameN + name;
	if(caches.find(name)!=caches.end())
	{
		throw "Cache Already exists";
	}
	if(props.getProperty("_isdefault_")=="true") {
		defDsnName = StringUtil::trimCopy(props.getName());
	}
	CacheManager* mgr = new CacheManager(props);
	caches[name] = mgr;
}

void CacheManager::destroy()
{
	map<string, CacheManager*>::iterator it;
	for(it=caches.begin();it!=caches.end();++it)
	{
		if(it->second!=NULL)
		{
			delete it->second;
		}
	}
	caches.clear();
}

CacheInterface* CacheManager::getImpl(string name) {
	string appName = CommonUtils::getAppName();
	StringUtil::trim(name);
	if(name=="") {
		name = defDsnName;
	}
	name = appName + name;
	if(caches.find(name)==caches.end())
	{
		throw "Cache Not found...";
	}
	CacheInterface* t = NULL;
	if(StringUtil::toLowerCopy(props.getType())=="memory")
	{
		t = new MemoryCacheImpl(props);
	}
	else if(StringUtil::toLowerCopy(props.getType())=="memcached")
	{
#ifdef INC_CACHE_MEMCACHED
		t = new MemcachedImpl(props);
#endif
	}
	else if(StringUtil::toLowerCopy(props.getType())=="redis")
	{
#ifdef INC_CACHE_REDIS
		t = new RedisCacheImpl(props);
#endif
	}
	t->init();
	return t;
}

CacheManager::CacheManager(const ConnectionProperties& props) {
	this->pool = NULL;
	this->props = props;
}

CacheManager::~CacheManager() {
	// TODO Auto-generated destructor stub
}

