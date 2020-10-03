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
 * ConfiguartionData.h
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#ifndef CONFIGURATIONDATA_H_
#define CONFIGURATIONDATA_H_
#include "string"
#include "FFEADContext.h"
#include "RegexUtil.h"
#include "DataSourceMapping.h"
#include "Connection.h"
#include "CORSHandler.h"
#include "CommonUtils.h"
#include "SSLHandler.h"
#include "ClassStructure.h"
#include "SerializeBase.h"
#include <functional>
#include "string_view"
#include "Router.h"

typedef void* (*toObjectFromJson) (const std::string&);

class WsDetails {
	std::string claz;
	std::string location;
	std::string namespc;
	std::string appname;
	std::map<std::string, std::string> outnmmp;
	friend class ConfigurationHandler;
	friend class WsUtil;
};

class RestFunctionParams
{
	std::string name;
	std::string type;
	int serOpt;
	std::string from;
	std::string defValue;
	SerCont sc;
	SerCont scm;
	Ser s;
	UnSerCont usc;
	UnSerCont uscm;
	UnSer us;
	friend class ConfigurationHandler;
	friend class ControllerHandler;
	friend class ConfigurationData;
};

class RestFunction
{
	std::string name;
	std::string path;
	std::string clas;
	std::string appName;
	std::string meth;
	std::string statusCode;
	std::string icontentType;
	std::string ocontentType;
	bool unmapped;
	int serOpt;
	std::string rtype;
	SerCont sc;
	SerCont scm;
	Ser s;
	UnSerCont usc;
	UnSerCont uscm;
	UnSer us;
	std::vector<RestFunctionParams> params;
	friend class ConfigurationHandler;
	friend class ControllerHandler;
	friend class ConfigurationData;
};

typedef std::map<std::string, std::vector<RestFunction>, std::less<> > resFuncMap;

class SecureAspect
{
	std::string path;
	std::string role;
	friend class ConfigurationHandler;
	friend class ConfigurationData;
	friend class Security;
	friend class SecurityHandler;
};

class Security
{
	Logger logger;
	std::map<std::string, SecureAspect> secures;
	std::string loginProvider;
	std::string loginUrl;
	std::string welcomeFile;
	std::string name;
	long sessTimeout;
	std::map<std::string, std::string> securityFieldNames;
	std::map<std::string, std::string> securityFieldFrom;
	bool isLoginConfigured();
	bool isSecureConfigured();
	bool isLoginUrl(std::string_view, const std::string& actUrl);
	bool isLoginPage(std::string_view, const std::string& actUrl);
	SecureAspect matchesPath(std::string_view, std::string actUrl);
	bool addAspect(const SecureAspect&);
	friend class ConfigurationData;
	friend class ConfigurationHandler;
	friend class SecurityHandler;
public:
	Security();
	~Security();
};

class UrlPattern {
	std::string pattern;
	int type;
	friend class ConfigurationHandler;
	friend class ConfigurationData;
	friend class Security;
	friend class SecurityHandler;
public:
	static enum {ANY, ANY_EXT, END_EXT, STARTSWITH, ENDSWITH, REGEX} PATTERN_TYPE;
};

class CoreServerProperties {
	std::string serverRootDirectory;
	std::string ip_address;
	std::string resourcePath;
	std::string webPath;
	long sessionTimeout;
	long sessionFileLockTimeout;
	bool sessatserv;
	bool sessservdistocache;
	bool isMainServerProcess;
	std::map<std::string, std::string> sprops;
	friend class ExtHandler;
	friend class SecurityHandler;
	friend class FormHandler;
	friend class SoapHandler;
	friend class CHServer;
	friend class ServiceTask;
	friend class ConfigurationHandler;
	friend class WsUtil;
	friend class DCPGenerator;
	friend class TemplateGenerator;
public:
	CoreServerProperties(){
		isMainServerProcess = false;
		sessservdistocache = false;
		sessionFileLockTimeout = 0;
		sessionTimeout = -1;
		sessatserv = false;
	}
	CoreServerProperties(std::string serverRootDirectory, std::string resourcePath, std::string webPath, propMap sprops, long sessionTimeout, bool sessatserv) {
		isMainServerProcess = false;
		sessservdistocache = false;
		sessionFileLockTimeout = 0;
		this->serverRootDirectory = serverRootDirectory;
		this->resourcePath = resourcePath;
		this->webPath = webPath;
		this->sprops = sprops;
		this->sessionTimeout = sessionTimeout;
		this->sessatserv = sessatserv;
	}
};

enum SERVER_BACKEND {
	NGINX, APACHE, OPENLITESPEED, //All http server modules [0, 1, 2]
	EMBEDDED,//The embedded ffead-cpp server engine [3]
	CINATRA, LITHIUM, DROGON, //All C++ http server engines [4, 5, 6]
	C_LIBREACTOR, C_H2O, //All C http server engines [7, 8]
	V_WEB, V_PICO, //All V http server engines [9, 10]
	RUST_ACTIX, RUST_HYPER, RUST_ROCKET, RUST_THRUSTER, //All Rust http server engines [11, 12, 13, 14]
	GO_FASTHTTP, GO_ATRUEGO, GO_GNET, //All Go http server engines [15, 16, 17]
	CRYSTAL_HTTP, CRYSTAL_H2O, //All Crystal http server engines [18, 19]
	JAVA_FIRENIO, JAVA_RAPIDOID, JAVA_WIZZARDO_HTTP, //All Java http server engines [20, 21, 22]
	SWIFT_NIO, //All Swift http server engines [23]
	D_HUNT, //All D http server engines [24]
	EMBEDDED_NGHTTP2, C_LSQUICHTTP3, //HTTP2 and HTTP3 server engines [25, 26]
};

class StaticResponseData {
	std::string r;
	std::string t;
	friend class ConfigurationData;
	friend class ServiceTask;
	friend class ConfigurationHandler;
};

class ConfigurationData {
	ConfigurationData();
	static ConfigurationData* instance;
	std::map<std::string, std::string, std::less<> > appAliases;
	std::map<std::string, std::vector<std::string>, std::less<> > filterMap;
	std::map<std::string, std::map<std::string, std::vector<RestFunction>, std::less<> >, std::less<> > rstCntMap;
	std::map<std::string, std::string, std::less<> > handoffs;
	std::map<std::string, std::map<std::string, Security, std::less<>>, std::less<> > securityObjectMap;
	std::map<std::string, std::map<std::string, std::string, std::less<>>, std::less<> > controllerObjectMap;
	std::map<std::string, std::map<std::string, std::string, std::less<>>, std::less<> > mappingObjectMap;
	std::map<std::string, std::map<std::string, std::string, std::less<>>, std::less<> > mappingextObjectMap;
	std::map<std::string, std::map<std::string, std::vector<std::string>, std::less<> >, std::less<> > filterObjectMap;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> > viewMappingMap;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> > ajaxInterfaceMap;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> > fviewMappingMap;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> > wsdlMap;
	std::map<std::string, std::map<std::string, Element, std::less<> >, std::less<> > fviewFormMap;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> > templateMappingMap;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> > dcpMappingMap;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> > websocketMappingMap;
	std::map<std::string, std::string, std::less<> > dynamicCppPagesMap;
	std::map<std::string, std::string, std::less<> > templateFilesMap;
	std::map<std::string, bool, std::less<> > applicationFlowMap;
	std::map<std::string, bool, std::less<> > servingContexts;
	std::map<std::string, std::string, std::less<> > servingContextAppNames;
	std::map<std::string, std::string, std::less<> > servingContextAppRoots;
	std::map<std::string, std::string, std::less<> > servingContextRouterNames;
	std::map<std::string, Router*, std::less<> > servingContextRouters;
	std::vector<std::string> componentNames;
	FFEADContext ffeadContext;
	CorsConfig corsConfig;
	SecurityProperties securityProperties;
	CoreServerProperties coreServerProperties;
	std::map<std::string, std::map<std::string, ConnectionProperties, std::less<> >, std::less<> > sdormConnProperties;
	std::map<std::string, std::map<std::string, Mapping, std::less<> >, std::less<> > sdormEntityMappings;
	std::map<std::string, std::map<std::string, ConnectionProperties, std::less<> >, std::less<> > cacheConnProperties;
	std::map<std::string, std::vector<WsDetails>, std::less<> > webserviceDetailMap;
	std::map<std::string, std::map<std::string, ClassStructure, std::less<> >, std::less<> > classStructureMap;
	std::map<std::string, propMap, std::less<> > appPropertiesMap;
	std::map<std::string, StaticResponseData, std::less<> > staticResponsesMap;
	Logger logger;
	ThreadLocal httpRequest;
	ThreadLocal httpResponse;
	void* dlib;
	Reflector reflector;
	void* ddlib;
	SERVER_BACKEND serverType;
	bool enableCors;
	bool enableSecurity;
	bool enableFilters;
	bool enableControllers;
	bool enableExtControllers;
	bool enableContMpg;
	bool enableContPath;
	bool enableContExt;
	bool enableContRst;
	bool enableExtra;
	bool enableScripts;
	bool enableSoap;
	bool enableLogging;
	bool enableJobs;
	bool enableStaticResponses;
	static void clearInstance();
	friend class ExtHandler;
	friend class FilterHandler;
	friend class ConfigurationHandler;
	friend class ControllerHandler;
	friend class ControllerExtensionHandler;
	friend class SecurityHandler;
	friend class CORSHandler;
	friend class FormHandler;
	friend class SoapHandler;
	friend class CHServer;
	friend class ServiceTask;
	friend class SSLHandler;
	friend class FviewHandler;
	friend class WsUtil;
	friend class DCPGenerator;
	friend class TemplateGenerator;
	friend class ApplicationUtil;
	friend class FFEADContext;
	friend class SocketInterface;
	friend class GenericObject;
	friend class FFEADContext;
	friend class JobScheduler;
	friend class DataSourceManager;
	friend class CacheManager;
	friend class ServerInitUtil;
public:
	static void enableFeatures(bool enableCors, bool enableSecurity, bool enableFilters, bool enableControllers,
		bool enableContMpg, bool enableContPath, bool enableContExt,bool enableContRst, bool enableExtra,
		bool enableScripts, bool enableSoap, bool enableLogging, bool enableExtControllers, bool enableJobs,
		bool enableStaticResponses);
	static Reflector* getReflector();
	static int getProcessId();
	static ClassInfo* getClassInfo(const std::string&, const std::string& app= "");
	static ClassInfo* getClassInfo(const std::string&, std::string_view);
	static bool isServingContext(std::string_view cntxtName);
	static ConfigurationData* getInstance();
	static SecurityProperties const& getSecurityProperties();
	static CoreServerProperties const& getCoreServerProperties();
	static HttpRequest* getHttpRequest();
	static HttpResponse* getHttpResponse();
	static bool urlMatchesPath(std::string_view cntxtName, std::string pathurl, std::string url);
	static void initializeAllSingletonBeans();
	static void clearAllSingletonBeans();
	static void setCoreServerProperties(CoreServerProperties coreServerProperties);
	static propMap getAppProperties(const std::string& name = "");
	static SERVER_BACKEND getServerType();
	virtual ~ConfigurationData();
	static bool isJobsEnabled();
};

#endif /* CONFIGURATIONDATA_H_ */
