/*
	Copyright 2009-2012, Sumeet Chhetri

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
 * ConfigurationHandler.h
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#ifndef CONFIGURATIONHANDLER_H_
#define CONFIGURATIONHANDLER_H_
#include "AppDefines.h"
#include "TemplateEngine.h"
#include "Reflection.h"
#include "XmlParser.h"
#ifdef INC_COMP
#include "ComponentGen.h"
#endif
#ifdef INC_APPFLOW
#include "ApplicationUtil.h"
#endif
#ifdef INC_DCP
#include "DCPGenerator.h"
#endif
#include "ConfigurationData.h"
#include "LoggerFactory.h"
#ifdef INC_TPE
#include "TemplateGenerator.h"
#endif
#include "FFEADContext.h"
#ifdef INC_WEBSVC
#include "WsUtil.h"
#endif
#ifdef INC_JOBS
#include "JobScheduler.h"
#endif
#include "SSLHandler.h"
#include "ScriptHandler.h"
#include "fstream"
#include "iostream"

class ConfigurationHandler {
public:
	ConfigurationHandler();
	virtual ~ConfigurationHandler();
	static void handle(strVec webdirs,strVec webdirs1,string incpath,string rtdcfpath,
			string serverRootDirectory,string respath);
	static void listi(string cwd,string type,bool apDir,strVec &folders,bool showHidden);
	static void configureCibernate(string, string);
	static void destroyCibernate();
};

#endif /* CONFIGURATIONHANDLER_H_ */
