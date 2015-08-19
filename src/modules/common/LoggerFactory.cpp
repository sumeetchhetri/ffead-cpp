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
 * LoggerFactory.cpp
 *
 *  Created on: 16-Jul-2013
 *      Author: sumeetc
 */

#include "LoggerFactory.h"

LoggerFactory* LoggerFactory::instance = NULL;

LoggerFactory::LoggerFactory()
{
	vhostNumber = 0;
	if(Logger::levelMap.size()==0) {
		Logger::levelMap[Logger::LEVEL_OFF] = 1;
		Logger::levelMap[Logger::LEVEL_FATAL] = 2;
		Logger::levelMap[Logger::LEVEL_ERROR] = 3;
		Logger::levelMap[Logger::LEVEL_WARN] = 4;
		Logger::levelMap[Logger::LEVEL_INFO] = 5;
		Logger::levelMap[Logger::LEVEL_DEBUG] = 6;
		Logger::levelMap[Logger::LEVEL_TRACE] = 7;
	}
}

LoggerFactory::~LoggerFactory()
{
	dupLogNames.clear();
	map<string, LoggerConfig*>::iterator it;
	for (it=configs.begin();it!=configs.end();++it) {
		it->second->lock.lock();
		delete it->second;
	}
	configs.clear();
}

void LoggerFactory::clear() {
	if(instance==NULL)
	{
		return;
	}
	map<string, LoggerConfig*>::iterator it;
	for(it=instance->configs.begin();it!=instance->configs.end();++it) {
		cout << ("Clearing logger config for " + it->second->name) << endl;
		if(it->second->mode=="FILE" && it->second->file!="" && it->second->out!=NULL) {
			delete it->second->out;
		}
		delete it->second;
	}
	delete instance;
	cout << ("Destructed LoggerFactory") << endl;
}

void LoggerFactory::setVhostNumber(const int& vhn) {
	if(instance!=NULL)
	{
		instance->vhostNumber = vhn;
		map<string, LoggerConfig*>::iterator it;
		for(it=instance->configs.begin();it!=instance->configs.end();++it)
		{
			it->second->vhostNumber = vhn;
		}
	}
}

void LoggerFactory::configureDefaultLogger(const string& appName)
{
	string lname = "DEFAULT";
	if(appName!="")
	{
		lname = appName + "." + lname;
	}
	if(instance!=NULL && instance->configs.find(lname)==instance->configs.end()) {
		LoggerConfig* config = new LoggerConfig;
		config->name = "DEFAULT";
		config->level = "INFO";
		config->mode = "CONSOLE";
		config->file = "";
		DateFormat df("dd/mm/yyyy hh:mi:ss");
		config->datFormat = df;
		config->out = &cout;
		config->lock.lock();
		config->lock.unlock();

		instance->configs[lname] = config;
		cout << "Configuring default logger..." << endl;
	}
}

void LoggerFactory::init()
{
	if(instance!=NULL)
	{
		return;
	}
	instance = new LoggerFactory();
	configureDefaultLogger("");
}

void LoggerFactory::init(const string& configFile, const string& serverRootDirectory, const string& appName, const bool& isLoggingEnabled) {
	if(instance==NULL)
	{
		instance = new LoggerFactory();
	}
	XmlParser parser("Parser");
	Document doc;
	parser.readDocument(configFile, doc);
	Element& root = doc.getRootElement();
	if(root.getTagName()=="loggers" && root.getChildElements().size()>0)
	{
		cout << "Reading " << configFile<< " file to configure loggers..." << endl;
		ElementList eles = root.getChildElements();
		for (unsigned int apps = 0; apps < eles.size(); apps++)
		{
			if(eles.at(apps)->getTagName()=="logger")
			{
				Element* ele = eles.at(apps);
				string name = StringUtil::toUpperCopy(ele->getAttribute("name"));
				string mode = StringUtil::toUpperCopy(ele->getAttribute("mode"));
				string level = StringUtil::toUpperCopy(ele->getAttribute("level"));
				string dfstr = "dd/mm/yyyy hh:mi:ss";
				if(ele->getAttribute("dateformat")!="") {
					dfstr = StringUtil::toLowerCopy(ele->getAttribute("dateformat"));
				}
				string file, pattern;
				string logdirtype = "SERVER";
				string logDirPath = serverRootDirectory + "logs/";
				ElementList files = ele->getChildElements();
				Element* fileele = NULL;

				for(int fc=0;fc<(int)files.size();fc++) {
					if(files.at(fc)->getTagName()=="file") {
						fileele = files.at(fc);
					} else if(files.at(fc)->getTagName()=="pattern") {
						pattern = files.at(fc)->getText();
					}
				}

				if(fileele!=NULL && fileele->getTagName()=="file") {
					file = fileele->getText();
				}

				if(file!="" && fileele->getAttribute("type")!="") {
					string ftyp = StringUtil::toUpperCopy(fileele->getAttribute("type"));
					if(ftyp=="ABSOLUTE") {
						logdirtype = "ABSOLUTE";
						logDirPath = "";
					}
				}

				if(name=="")
				{
					name = "DEFAULT";
				}
				if(mode=="")
				{
					mode = "CONSOLE";
				}
				if(level=="")
				{
					level = "INFO";
				}
				if(mode=="FILE" && file=="")
				{
					cout << "Error configuring Logger, invalid mode and filepath defined..." << endl;
				}
				else if(mode=="FILE")
				{
					string logfilepath = logDirPath + file;
					ofstream ofs;
					if(isLoggingEnabled) {
						ofs.open(logfilepath.c_str());
					} else {
						ofs.open("/dev/null");
					}
					if(ofs.is_open())
					{
						ofs.close();
					}
					else
					{
						ofs.close();
						cout << "Error configuring Logger, invalid filepath defined..." << endl;
						continue;
					}
				}

				if(level!=Logger::LEVEL_OFF && level!=Logger::LEVEL_FATAL && level!=Logger::LEVEL_ERROR
						&& level!=Logger::LEVEL_WARN && level!=Logger::LEVEL_INFO && level!=Logger::LEVEL_DEBUG
						 && level!=Logger::LEVEL_TRACE)
				{
					cout << "Error configuring Logger, invalid level defined..." << endl;
					continue;
				}

				string loggerName = name;
				if(appName!="") {
					loggerName = appName + "." + loggerName;
				}
				if(instance->configs.find(loggerName)!=instance->configs.end())
				{
					cout << "Error configuring Logger " << loggerName << ", Logger already defined skipping..." << endl;
					continue;
				} else {
					bool foundDup = false;
					map<string, LoggerConfig*>::iterator it;
					for(it=instance->configs.begin();it!=instance->configs.end();++it) {
						if((mode=="FILE" && file!="" && it->second->file==file && it->second->logdirtype==logdirtype)
								|| (mode=="CONSOLE" && name!="DEFAULT")) {
							instance->dupLogNames[loggerName] = level;
							cout << "Found duplicate logger configuration for name " + loggerName << endl;
							break;
						}
					}
					if(foundDup) {
						continue;
					}
				}

				LoggerConfig* config = new LoggerConfig;
				config->name = name;
				config->level = level;
				config->mode = mode;
				config->file = file;
				config->pattern = pattern;
				config->logdirtype = logdirtype;
				config->vhostNumber = instance->vhostNumber;

				DateFormat df(dfstr);
				config->datFormat = df;

				config->lock.lock();
				if(mode=="FILE")
				{
					string logfilepath = logDirPath + file;
					if(instance->vhostNumber>0) {
						logfilepath += "." + CastUtil::lexical_cast<string>(instance->vhostNumber);
					}
					ofstream* strm = new ofstream();
					if(isLoggingEnabled) {
						strm->open(logfilepath.c_str(), ios::app | ios::binary);
					} else {
						strm->open("/dev/null", ios::app | ios::binary);
					}
					config->out = strm;
				}
				else
				{
					config->out = &cout;
				}
				config->lock.unlock();
				instance->configs[loggerName] = config;
			}
		}
		cout << "Done configuring loggers..." << endl;
	}
	configureDefaultLogger("");
}

Logger LoggerFactory::getLogger(const string& className) {
	if(instance==NULL)
	{
		init();
	}
	map<string, LoggerConfig*>::iterator it;
	for(it=instance->configs.begin();it!=instance->configs.end();++it)
	{
		if(it->second->pattern!="" && RegexUtil::find(className, it->second->pattern)!=-1) {
			return Logger(it->second, className);
		}
	}
	string appName = CommonUtils::getAppName("");
	if(appName!="")
	{
		if(instance->configs.find(appName+".DEFAULT")!=instance->configs.end())
		{
			return Logger(instance->configs[appName+".DEFAULT"], className);
		}
	}
	return Logger(instance->configs["DEFAULT"], className);
}

Logger LoggerFactory::getLogger(const string& loggerName, const string& className) {
	if(instance==NULL)
	{
		init();
	}
	string appName = CommonUtils::getAppName("");
	if(appName!="") {
		string nloggerName = appName + "." + loggerName;
		if(instance->configs.find(nloggerName)==instance->configs.end()) {
			if(instance->dupLogNames.find(nloggerName)!=instance->dupLogNames.end()) {
				return Logger(instance->configs[nloggerName], className, instance->dupLogNames[nloggerName]);
			}
		} else {
			return Logger(instance->configs[nloggerName], className);
		}
	}
	if(instance->configs.find(loggerName)==instance->configs.end()) {
		if(instance->dupLogNames.find(loggerName)!=instance->dupLogNames.end()) {
			return Logger(instance->configs[loggerName], className, instance->dupLogNames[loggerName]);
		}
		return getLogger(className);
	} else {
		return Logger(instance->configs[loggerName], className);
	}
}
