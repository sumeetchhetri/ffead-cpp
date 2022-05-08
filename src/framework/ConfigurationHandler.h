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
#include "SimpleXmlParser.h"
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
#ifdef HAVE_SSLINC
#include "SSLHandler.h"
#endif
#include "ScriptHandler.h"
#include "fstream"
#include "iostream"
#include "DataSourceManager.h"
#include "CacheManager.h"
#include "SearchEngineManager.h"
#include "CommonUtils.h"

class ConfigurationHandler {
	static Marker getRestFunctionMarker(std::map<std::string, std::vector<Marker> >& markers);
	static Marker getRestFunctionParamMarker(std::map<std::string, std::vector<Marker> >& markers);
	static void handleRestControllerMarker(ClassStructure& cs, const std::string& appName);
	static void handleMarkerConfigurations(std::map<std::string, std::map<std::string, ClassStructure, std::less<> >, std::less<> >& clsstrucMaps, std::vector<WsDetails>& wsdvec, std::vector<bool>& stat, strVec& vecvp, strVec& pathvec, std::map<std::string, std::string, std::less<> >& ajintpthMap, std::map<std::string, std::string, std::less<> >& tpes, const std::string& serverRootDirectory, strVec& afcd, std::string& ajrt, Reflection& ref);
	static void handleDataSourceEntities(const std::string& appName, std::map<std::string, Mapping, std::less<> >& mappings, std::map<std::string, ClassStructure, std::less<> >& allclsmap);
	static void normalizeUrl(const std::string& appName, std::string& url);
public:
	ConfigurationHandler();
	virtual ~ConfigurationHandler();
	static void handle(strVec webdirs, const strVec& webdirs1, const std::string& incpath, const std::string& rtdcfpath, const std::string& serverRootDirectory, const std::string& respath);

	static void configureDataSources(const std::string& name, const std::string& configFile, std::map<std::string, ClassStructure, std::less<> >& allclsmap);
	static void initializeDataSources();
	static void destroyDataSources();

	static void configureCaches(const std::string& name, const std::string& configFile);
	static void initializeCaches();
	static void destroyCaches();

	static void configureSearches(const std::string& name, const std::string& configFile);
	static void initializeSearches();
	static void destroySearches();

	static void initializeWsdls();
	static void populateClassBeanInstanceCb(std::string& clsn, std::string appn, ClassBeanIns* cbi);
};

#endif /* CONFIGURATIONHANDLER_H_ */
