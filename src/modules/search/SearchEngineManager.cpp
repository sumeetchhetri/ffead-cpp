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
std::map<std::string, std::string> SearchEngineManager::defEngineNames;
std::map<std::string, bool> SearchEngineManager::appInitCompletionStatus;

void SearchEngineManager::triggerAppInitCompletion(std::string appNameN) {
	std::string appName = appNameN;
	if(appName=="") {
		appName = CommonUtils::getAppName();
	} else {
		StringUtil::replaceAll(appName, "-", "_");
		RegexUtil::replace(appName, "[^a-zA-Z0-9_]+", "");
	}
	std::cout << "triggerAppInitCompletion called for " << appName << std::endl;
	if(appInitCompletionStatus.find(appName)!=appInitCompletionStatus.end()) {
		appInitCompletionStatus[appName] = true;
	}
}

bool SearchEngineManager::isInitCompleted() {
	bool flag = true;
	if(appInitCompletionStatus.size()>0) {
		std::map<std::string, bool>::iterator it = appInitCompletionStatus.begin();
		for(;it!=appInitCompletionStatus.end();++it) {
			flag &= it->second;
		}
	}
	return flag;
}

void SearchEngineManager::initSearch(const ConnectionProperties& props, const std::string& appName, GetClassBeanIns f) {
	Logger logger = LoggerFactory::getLogger("SearchEngineManager");
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
		defEngineNames[appName] = StringUtil::trimCopy(props.getName());
	}
	SearchEngineManager* mgr = new SearchEngineManager(props);
	engines[name] = mgr;

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
						std::cout << "initSearch called for " << appName << std::endl;
						appInitCompletionStatus[appName] = false;
						ref->invokeMethodGVP(_temp, meth, valus);
					}
				}
			} catch(const std::exception& e) {
				logger.info("Error during init call for Seacrh " + appNameN + "@" + props.getName() + " " + std::string(e.what()));
			}
			if(cbi.cleanUp) {
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
		name = defEngineNames[appName];
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
#ifdef HAVE_ELASTIC
		t = NULL;
#endif
	}
	else if(StringUtil::toLowerCopy(engine->props.getType())=="solr")
	{
#ifdef HAVE_SOLR
		t = new SolrSearch(engine->pool);
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
	this->pool = NULL;
	if(StringUtil::toLowerCopy(props.getType())=="elasticsearch")
	{
#ifdef HAVE_ELASTIC
		this->pool = new ElasticSearchConnectionPool(props);
#endif
	}
	else if(StringUtil::toLowerCopy(props.getType())=="solr")
	{
#ifdef HAVE_SOLR
		this->pool = new SolrSearchConnectionPool(props);
#endif
	}
}

SearchEngineManager::~SearchEngineManager() {
}

