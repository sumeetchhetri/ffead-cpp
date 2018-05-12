/*
 * CacheManager.cpp
 *
 *  Created on: 12-Oct-2014
 *      Author: sumeetc
 */

#include "CacheManager.h"

std::map<std::string, CacheManager*> CacheManager::caches;
std::string CacheManager::defDsnName;

void CacheManager::initCache(const ConnectionProperties& props, const std::string& appName) {
	std::string name = StringUtil::trimCopy(props.getName());
	if(name=="")
	{
		throw "Cache Name cannot be blank";
	}
	std::string appNameN = CommonUtils::getAppName(appName);
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

	void* dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(dlib == NULL)
	{
		std::cerr << dlerror() << std::endl;
		throw "Cannot load application shared library";
	}
	Reflector ref(dlib);
	if(props.getProperty("init")!="") {
		std::string meth = props.getProperty("init");
		std::vector<std::string> v;
		StringUtil::split(v, meth, ".");
		if(v.size()==2) {
			CommonUtils::setAppName(appName);
			ClassInfo clas = ref.getClassInfo(v.at(0), appName);
			if(clas.getClassName()!="") {
				args argus;
				vals valus;
				const Constructor& ctor = clas.getConstructor(argus);
				void* _temp = ref.newInstanceGVP(ctor);
				try {
					if(_temp!=NULL) {
						const Method& meth = clas.getMethod(v.at(1), argus);
						if(meth.getMethodName()!="")
						{
							ref.invokeMethodGVP(_temp, meth, valus);
						}
					}
				} catch(...) {
				}
				ref.destroy(_temp, v.at(0), appName);
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
}

CacheInterface* CacheManager::getImpl(std::string name) {
	std::string appName = CommonUtils::getAppName();
	StringUtil::trim(name);
	if(name=="") {
		name = defDsnName;
	}
	name = appName + name;
	if(caches.find(name)==caches.end())
	{
		throw "Cache Not found...";
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
	return t;
}

CacheManager::CacheManager(const ConnectionProperties& props) {
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

