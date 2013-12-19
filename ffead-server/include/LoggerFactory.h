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
 * LoggerFactory.h
 *
 *  Created on: 16-Jul-2013
 *      Author: sumeetc
 */

#ifndef LOGGERFACTORY_H_
#define LOGGERFACTORY_H_
#include "Logger.h"
#include "XmlParser.h"

class LoggerFactory {
	LoggerFactory();
	static map<string, LoggerConfig*> configs;
	static map<string, string> dupLogNames;
	static bool inited;
public:
	virtual ~LoggerFactory();
	static void clear();
	static void init(string configFile, string serverRootDirectory);
	static Logger getLogger(string className);
	static Logger getLogger(string loggerName, string className);
};

#endif /* LOGGERFACTORY_H_ */
