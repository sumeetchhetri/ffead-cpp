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
 * SearchEngineManager.cpp
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#include "SearchEngineManager.h"

std::map<std::string, SearchEngineManager*> SearchEngineManager::engines;
std::string SearchEngineManager::defEngineName;

void SearchEngineManager::initCache(const ConnectionProperties& props, const std::string& appName) {
	std::string name = StringUtil::trimCopy(props.getName());
	if(name=="")
	{
		throw std::runtime_error("Search Engine Name cannot be blank");
	}
	std::string appNameN = CommonUtils::getAppName(appName);
	name = appNameN + name;
	if(engines.find(name)!=engines.end())
	{
		throw std::runtime_error("Search Engine Already exists");
	}
	if(props.getProperty("_isdefault_")=="true") {
		defEngineName = StringUtil::trimCopy(props.getName());
	}
	SearchEngineManager* mgr = new SearchEngineManager(props);
	engines[name] = mgr;

	Reflector* ref = GenericObject::getReflector();
	if(props.getProperty("init")!="") {
		std::string meth = props.getProperty("init");
		std::vector<std::string> v;
		StringUtil::split(v, meth, ".");
		if(v.size()==2) {
			std::string scappName = appName;
			StringUtil::replaceAll(scappName, "-", "_");
			RegexUtil::replace(scappName, "[^a-zA-Z0-9_]+", "");
			CommonUtils::setAppName(scappName);
			ClassInfo* clas = ref->getClassInfo(v.at(0), appName);
			if(clas->getClassName()!="") {
				args argus;
				vals valus;
				const Constructor& ctor = clas->getConstructor(argus);
				void* _temp = ref->newInstanceGVP(ctor);
				try {
					if(_temp!=NULL) {
						const Method& meth = clas->getMethod(v.at(1), argus);
						if(meth.getMethodName()!="")
						{
							ref->invokeMethodGVP(_temp, meth, valus);
						}
					}
				} catch(const std::exception& e) {
				}
				ref->destroy(_temp, v.at(0), appName);
			}
		}
	}
}

void SearchEngineManager::destroy()
{
	std::map<std::string, SearchEngineManager*>::iterator it;
	for(it=engines.begin();it!=engines.end();++it)
	{
		if(it->second!=NULL)
		{
			delete it->second->pool;
			delete it->second;
		}
	}
	engines.clear();
}

SearchEngineInterface* SearchEngineManager::getImpl(std::string name) {
	std::string appName = CommonUtils::getAppName();
	StringUtil::trim(name);
	if(name=="") {
		name = defEngineName;
	}
	name = appName + name;
	if(engines.find(name)==engines.end())
	{
		throw std::runtime_error("Search Engine Not found...");
	}
	SearchEngineManager* engine = engines[name];
	SearchEngineInterface* t = NULL;
	if(StringUtil::toLowerCopy(engine->props.getType())=="elasticsearch")
	{
#ifdef INC_MEMCACHED
		t = NULL;
#endif
	}
	else if(StringUtil::toLowerCopy(engine->props.getType())=="solr")
	{
#ifdef INC_REDISCACHE
		t = NULL;
#endif
	}
	if(t == NULL)
	{
		return NULL;
	}
	t->appName = appName;
	t->reflector = GenericObject::getReflector();
	return t;
}

SearchEngineManager::SearchEngineManager(const ConnectionProperties& props) {
	this->props = props;
	reflector = NULL;
	this->pool = new SearchEngineConnectionPool(props);
}

SearchEngineManager::~SearchEngineManager() {
}

void SearchEngineConnectionPool::initEnv() {
	HttpClient::init();
}

void* SearchEngineConnectionPool::newConnection(const bool& isWrite, const ConnectionNode& node) {
	return new HttpClient(node.getBaseUrl());
}

void SearchEngineConnectionPool::closeConnection(void* conn) {
	delete ((HttpClient*)conn);
}

void SearchEngineConnectionPool::destroy() {
}

SearchEngineConnectionPool::SearchEngineConnectionPool(const ConnectionProperties& props) {
	logger = LoggerFactory::getLogger("SearchEngineConnectionPool");
	createPool(props);
}

SearchEngineConnectionPool::~SearchEngineConnectionPool() {
	destroyPool();
}
