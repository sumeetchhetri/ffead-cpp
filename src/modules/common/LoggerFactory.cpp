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
	std::map<std::string, LoggerConfig*>::iterator it;
	for (it=configs.begin();it!=configs.end();++it) {
		std::cout << ("Clearing logger config for " + it->second->name) << std::endl;
		it->second->lock->lock();
		if(it->second->mode=="FILE" && it->second->file!="" && it->second->out!=NULL) {
			delete it->second->out;
		}
		it->second->lock->unlock();
		delete it->second;
	}
	configs.clear();
	std::cout << ("Destructed LoggerFactory") << std::endl;
}

void LoggerFactory::clear() {
	if(instance==NULL)
	{
		return;
	}
	delete instance;
}

void LoggerFactory::setVhostNumber(const int& vhn) {
	if(instance!=NULL)
	{
		instance->vhostNumber = vhn;
		std::map<std::string, LoggerConfig*>::iterator it;
		for(it=instance->configs.begin();it!=instance->configs.end();++it)
		{
			it->second->vhostNumber = vhn;
		}
	}
}

void LoggerFactory::configureDefaultLogger(const std::string& appName)
{
	std::string lname = "DEFAULT";
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
		config->out = &std::cout;
		config->lock = new DummyMutex;

		instance->configs[lname] = config;
		std::cout << "Configuring default logger..." << std::endl;
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

void LoggerFactory::init(const std::string& configFile, const std::string& serverRootDirectory, const std::string& appName, const bool& isLoggingEnabled) {
	if(instance==NULL)
	{
		instance = new LoggerFactory();
	}
	XmlParser parser("Parser");
	Document doc;
	parser.readDocument(configFile, doc);
	Element& root = doc.getRootElement();
	if(isLoggingEnabled && root.getTagName()=="loggers" && root.getChildElements().size()>0)
	{
		std::cout << "Reading " << configFile<< " file to configure loggers..." << std::endl;
		ElementList eles = root.getChildElements();
		for (unsigned int apps = 0; apps < eles.size(); apps++)
		{
			if(eles.at(apps).getTagName()=="logger")
			{
				Element* ele = &(eles.at(apps));
				std::string name = StringUtil::toUpperCopy(ele->getAttribute("name"));
				std::string mode = StringUtil::toUpperCopy(ele->getAttribute("mode"));
				std::string level = StringUtil::toUpperCopy(ele->getAttribute("level"));
				std::string dfstr = "dd/mm/yyyy hh:mi:ss";
				if(ele->getAttribute("dateformat")!="") {
					dfstr = StringUtil::toLowerCopy(ele->getAttribute("dateformat"));
				}
				std::string file, pattern;
				std::string logdirtype = "SERVER";
				std::string logDirPath = serverRootDirectory + "logs/";
				ElementList files = ele->getChildElements();
				Element* fileele = NULL;

				for(int fc=0;fc<(int)files.size();fc++) {
					if(files.at(fc).getTagName()=="file") {
						fileele = &(files.at(fc));
					} else if(files.at(fc).getTagName()=="pattern") {
						pattern = files.at(fc).getText();
					}
				}

				if(fileele!=NULL && fileele->getTagName()=="file") {
					file = fileele->getText();
				}

				if(file!="" && fileele->getAttribute("type")!="") {
					std::string ftyp = StringUtil::toUpperCopy(fileele->getAttribute("type"));
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
					std::cout << "Error configuring Logger, invalid mode and filepath defined..." << std::endl;
				}
				else if(mode=="FILE")
				{
					std::string logfilepath = logDirPath + file;
					std::ofstream ofs;
					//if(isLoggingEnabled) {
						ofs.open(logfilepath.c_str());
					//} else {
					//	ofs.open("/dev/null");
					//}
					if(ofs.is_open())
					{
						ofs.close();
					}
					else
					{
						ofs.close();
						std::cout << "Error configuring Logger, invalid filepath defined..." << std::endl;
						continue;
					}
				}

				if(level!=Logger::LEVEL_OFF && level!=Logger::LEVEL_FATAL && level!=Logger::LEVEL_ERROR
						&& level!=Logger::LEVEL_WARN && level!=Logger::LEVEL_INFO && level!=Logger::LEVEL_DEBUG
						 && level!=Logger::LEVEL_TRACE)
				{
					std::cout << "Error configuring Logger, invalid level defined..." << std::endl;
					continue;
				}

				std::string loggerName = name;
				if(appName!="") {
					loggerName = appName + "." + loggerName;
				}
				if(instance->configs.find(loggerName)!=instance->configs.end())
				{
					std::cout << "Error configuring Logger " << loggerName << ", Logger already defined skipping..." << std::endl;
					continue;
				} else {
					bool foundDup = false;
					std::map<std::string, LoggerConfig*>::iterator it;
					for(it=instance->configs.begin();it!=instance->configs.end();++it) {
						if((mode=="FILE" && file!="" && it->second->file==file && it->second->logdirtype==logdirtype)
								|| (mode=="CONSOLE" && name!="DEFAULT")) {
							instance->dupLogNames[loggerName] = level;
							std::cout << "Found duplicate logger configuration for name " + loggerName << std::endl;
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

				config->lock = new Mutex;
				config->lock->lock();
				if(mode=="FILE")
				{
					std::string logfilepath = logDirPath + file;
					if(instance->vhostNumber>0) {
						logfilepath += "." + CastUtil::lexical_cast<std::string>(instance->vhostNumber);
					}
					std::ofstream* strm = new std::ofstream();
					//if(isLoggingEnabled) {
						strm->open(logfilepath.c_str(), std::ios::app | std::ios::binary);
					//} else {
					//	strm->open("/dev/null", std::ios::app | std::ios::binary);
					//}
					config->out = strm;
				}
				else
				{
					config->out = &std::cout;
				}
				config->lock->unlock();
				instance->configs[loggerName] = config;
			}
		}
		std::cout << "Done configuring loggers..." << std::endl;
	}
	configureDefaultLogger("");
}

Logger LoggerFactory::getLogger(const std::string& className) {
	if(instance==NULL)
	{
		init();
	}
	std::map<std::string, LoggerConfig*>::iterator it;
	for(it=instance->configs.begin();it!=instance->configs.end();++it)
	{
		if(it->second->pattern!="" && RegexUtil::find(className, it->second->pattern)!=-1) {
			return Logger(it->second, className);
		}
	}
	std::string appName = CommonUtils::getAppName("");
	if(appName!="")
	{
		if(instance->configs.find(appName+".DEFAULT")!=instance->configs.end())
		{
			return Logger(instance->configs[appName+".DEFAULT"], className);
		}
	}
	return Logger(instance->configs["DEFAULT"], className);
}

Logger LoggerFactory::getLogger(const std::string& loggerName, const std::string& className) {
	if(instance==NULL)
	{
		init();
	}
	std::string appName = CommonUtils::getAppName("");
	if(appName!="") {
		std::string nloggerName = appName + "." + loggerName;
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
