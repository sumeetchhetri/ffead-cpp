/*
 * DataSourceManager.cpp
 *
 *  Created on: 14-May-2014
 *      Author: sumeetc
 */

#include "DataSourceManager.h"

map<string, DataSourceManager*> DataSourceManager::dsns;
string DataSourceManager::defDsnName;

void DataSourceManager::initDSN(const ConnectionProperties& props, const Mapping& mapping)
{
	string name = StringUtil::trimCopy(props.getName());
	if(name=="")
	{
		throw "Data Source Name cannot be blank";
	}
	string appName = CommonUtils::getAppName(mapping.getAppName());
	name = appName + name;
	if(dsns.find(name)!=dsns.end())
	{
		throw "Data Source Already exists";
	}
	if(props.getProperty("_isdefault_")=="true") {
		defDsnName = StringUtil::trimCopy(props.getName());
	}
	DataSourceManager* dsnMgr = new DataSourceManager(props, mapping);
	dsns[name] = dsnMgr;
}

void DataSourceManager::destroy()
{
	map<string, DataSourceManager*>::iterator it;
	for(it=dsns.begin();it!=dsns.end();++it)
	{
		if(it->second!=NULL)
		{
			delete it->second;
		}
	}
	dsns.clear();
}

DataSourceManager::DataSourceManager(const ConnectionProperties& props, const Mapping& mapping) {
	this->pool = NULL;
	this->props = props;
	this->mapping = mapping;
	if(StringUtil::toLowerCopy(props.getType()) == "sql") {
#ifdef INC_SDORM_SQL
		this->pool = new SQLConnectionPool(props);
#endif
	} else if(StringUtil::toLowerCopy(props.getType()) == "mongo") {
#ifdef INC_SDORM_MONGO
		this->pool = new MongoDBConnectionPool(props);
#endif
	}
}

DataSourceManager::~DataSourceManager() {
	if(this->pool!=NULL) {
		delete this->pool;
	}
}

DataSourceInterface* DataSourceManager::getImpl(string name) {
	string appName = CommonUtils::getAppName();
	StringUtil::trim(name);
	if(name=="") {
		name = defDsnName;
	}
	name = appName + name;
	if(dsns.find(name)==dsns.end())
	{
		throw "Data Source Not found...";
	}
	DataSourceManager* dsnMgr = dsns[name];
	DataSourceInterface* t = NULL;
	if(StringUtil::toLowerCopy(dsnMgr->props.getType())=="sql")
	{
#ifdef INC_SDORM_SQL
		t = new SQLDataSourceImpl(dsnMgr->pool, &dsnMgr->mapping);
#endif
	}
	else if(StringUtil::toLowerCopy(dsnMgr->props.getType())=="mongo")
	{
#ifdef INC_SDORM_MONGO
		t = new MongoDBDataSourceImpl(dsnMgr->pool, &dsnMgr->mapping);
#endif
	}
	if(t == NULL)
	{
		return NULL;
	}
	t->appName = dsnMgr->mapping.getAppName();
	t->dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(t->dlib == NULL)
	{
		cerr << dlerror() << endl;
		throw "Cannot load application shared library";
	}
	t->reflector = new Reflector(t->dlib);
	map<string, DataSourceEntityMapping>::iterator it;
	for(it=dsnMgr->mapping.getDseMap().begin();it!=dsnMgr->mapping.getDseMap().end();++it)
	{
		DataSourceEntityMapping dsemp = it->second;
		if(dsemp.isIdGenerate())
		{
			t->init(dsemp, true);
		}
	}
	return t;
}
