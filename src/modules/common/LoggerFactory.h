/*
	Copyright 2010, Sumeet Chhetri

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
 * LoggerFactory.h
 *
 *  Created on: 16-Jul-2013
 *      Author: sumeetc
 */

#ifndef LOGGERFACTORY_H_
#define LOGGERFACTORY_H_
#include "Logger.h"
#include "XmlParser.h"
#include "CommonUtils.h"

class LoggerFactory {
	std::map<std::string, LoggerConfig*> configs;
	std::map<std::string, std::string> dupLogNames;
	int vhostNumber;
	static bool isLoggingEnabled;
	static LoggerFactory* instance;
	static Logger _l;
	LoggerFactory();
	static void setVhostNumber(const int& vhn);
	static void init();
	static void configureDefaultLogger(const std::string& appName);
	friend class CHServer;
public:
	virtual ~LoggerFactory();
	static void clear();
	static void init(const std::string& configFile, const std::string& serverRootDirectory, const std::string& appName = "", const bool& isLoggingEnabled = true);
	static Logger getLogger(const std::string& className);
	static Logger getLogger(const std::string& loggerName, const std::string& className);
};

#endif /* LOGGERFACTORY_H_ */
