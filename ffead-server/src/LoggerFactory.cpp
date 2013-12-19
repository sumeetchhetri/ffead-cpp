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

map<string, LoggerConfig*> LoggerFactory::configs;
map<string, string> LoggerFactory::dupLogNames;
bool LoggerFactory::inited = false;

LoggerFactory::LoggerFactory() {
}

LoggerFactory::~LoggerFactory() {
}

void LoggerFactory::clear() {
	if(!inited)return;
	map<string, LoggerConfig*>::iterator it;
	for(it=configs.begin();it!=configs.end();++it) {
		cout << ("Clearing logger config for " + it->second->name) << endl;
		if(it->second->mode=="FILE" && it->second->file!="" && it->second->out!=NULL) {
			delete it->second->out;
		}
		delete it->second;
	}
	cout << ("Destructed LoggerFactory") << endl;
}

void LoggerFactory::init(string configFile, string serverRootDirectory) {
	if(inited)return;
	inited = true;
	Logger logger = LoggerFactory::getLogger("LoggerFactory");
	XmlParser parser("Parser");
	Element root = parser.getDocument(configFile).getRootElement();
	if(root.getTagName()=="loggers" && root.getChildElements().size()>0)
	{
		logger << "Reading logging.xml file to configure loggers..." << endl;
		ElementList eles = root.getChildElements();
		for (unsigned int apps = 0; apps < eles.size(); apps++)
		{
			if(eles.at(apps).getTagName()=="logger")
			{
				Element ele = eles.at(apps);
				string name = StringUtil::toUpperCopy(ele.getAttribute("name"));
				string mode = StringUtil::toUpperCopy(ele.getAttribute("mode"));
				string level = StringUtil::toUpperCopy(ele.getAttribute("level"));
				string file;
				string logdirtype = "SERVER";
				string logDirPath = serverRootDirectory + "logs/";
				ElementList files = ele.getChildElements();
				if(files.size()>0 && files.at(0).getTagName()=="file") {
					file = files.at(0).getText();
				}

				if(file!="" && files.at(0).getAttribute("type")!="") {
					string ftyp = StringUtil::toUpperCopy(files.at(0).getAttribute("type"));
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
					logger << "Error configuring Logger, invalid mode and filepath defined..." << endl;
				}
				else if(mode=="FILE")
				{
					string logfilepath = logDirPath + file;
					ofstream ofs;
					ofs.open(logfilepath.c_str());
					if(ofs.is_open())
					{
						ofs.close();
					}
					else
					{
						ofs.close();
						logger << "Error configuring Logger, invalid filepath defined..." << endl;
						continue;
					}
				}

				if(level!=Logger::LEVEL_ERROR && level!=Logger::LEVEL_DEBUG && level!=Logger::LEVEL_INFO)
				{
					logger << "Error configuring Logger, invalid level defined..." << endl;
					continue;
				}

				if(configs.find(name)!=configs.end())
				{
					if(name!="DEFAULT")
					{
						logger << "Error configuring Logger, Logger already defined skipping..." << endl;
					}
					else
					{
						logger << "Default Logger already defined skipping..." << endl;
					}
					continue;
				} else {
					bool foundDup = false;
					map<string, LoggerConfig*>::iterator it;
					for(it=configs.begin();it!=configs.end();++it) {
						if((mode=="FILE" && file!="" && it->second->file==file && it->second->logdirtype==logdirtype)
								|| (mode=="CONSOLE" && name!="DEFAULT")) {
							dupLogNames[name] = level;

							logger << "Found duplicate logger configuration for name " + name << endl;

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
				config->logdirtype = logdirtype;

				DateFormat df("dd/mm/yyyy hh:mi:ss");
				config->datFormat = df;

				config->lock.lock();
				if(mode=="FILE")
				{
					string logfilepath = logDirPath + file;
					ofstream* strm = new ofstream();
					strm->open(logfilepath.c_str(), ios::app | ios::binary);
					config->out = strm;
				}
				else
				{
					config->out = &cout;
				}
				config->lock.unlock();

				configs[config->name] = config;
			}
		}
		logger << "Done configuring loggers..." << endl;
	}
}

Logger LoggerFactory::getLogger(string className) {
	if(configs.find("DEFAULT")==configs.end()) {
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

		configs[config->name] = config;
	}
	return Logger(configs["DEFAULT"], className);
}

Logger LoggerFactory::getLogger(string loggerName, string className) {
	if(configs.find(loggerName)==configs.end()) {
		if(dupLogNames.find(loggerName)!=dupLogNames.end()) {
			return Logger(configs[loggerName], className, dupLogNames[loggerName]);
		}
		return getLogger(className);
	} else {
		return Logger(configs[loggerName], className);
	}
}


