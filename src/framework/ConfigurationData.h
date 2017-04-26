/*
	Copyright 2009-2012, Sumeet Chhetri

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
	std::string from;
	std::string defValue;
	friend class ConfigurationHandler;
	friend class ControllerHandler;
};

class RestFunction
{
	std::string name;
	std::string path;
	std::string clas;
	std::string meth;
	std::string statusCode;
	std::string icontentType;
	std::string ocontentType;
	std::vector<RestFunctionParams> params;
	friend class ConfigurationHandler;
	friend class ControllerHandler;
};

typedef std::map<std::string, std::vector<RestFunction> > resFuncMap;

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
	bool isLoginUrl(const std::string& cntxtName, const std::string& actUrl);
	bool isLoginPage(const std::string& cntxtName, const std::string& actUrl);
	SecureAspect matchesPath(const std::string& cntxtName, std::string actUrl);
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
	CoreServerProperties(){}
	CoreServerProperties(std::string serverRootDirectory, std::string resourcePath, std::string webPath, propMap sprops, long sessionTimeout, bool sessatserv) {
		isMainServerProcess = false;
		sessservdistocache = false;
		this->serverRootDirectory = serverRootDirectory;
		this->resourcePath = resourcePath;
		this->webPath = webPath;
		this->sprops = sprops;
		this->sessionTimeout = sessionTimeout;
		this->sessatserv = sessatserv;
	}
};

class ConfigurationData {
	ConfigurationData();
	static ConfigurationData* instance;
	std::map<std::string, std::string> appAliases;
	std::map<std::string, std::vector<std::string> > filterMap;
	std::map<std::string, std::map<std::string, std::vector<RestFunction> > > rstCntMap;
	std::map<std::string, std::string> handoffs;
	std::map<std::string, std::map<std::string, Security> > securityObjectMap;
	std::map<std::string, std::map<std::string, std::string> > controllerObjectMap;
	std::map<std::string, std::map<std::string, std::string> > mappingObjectMap;
	std::map<std::string, std::map<std::string, std::string> > mappingextObjectMap;
	std::map<std::string, std::map<std::string, std::vector<std::string> > > filterObjectMap;
	std::map<std::string, std::map<std::string, std::string> > viewMappingMap;
	std::map<std::string, std::map<std::string, std::string> > ajaxInterfaceMap;
	std::map<std::string, std::map<std::string, std::string> > fviewMappingMap;
	std::map<std::string, std::map<std::string, std::string> > wsdlMap;
	std::map<std::string, std::map<std::string, Element> > fviewFormMap;
	std::map<std::string, std::map<std::string, std::string> > templateMappingMap;
	std::map<std::string, std::map<std::string, std::string> > dcpMappingMap;
	std::map<std::string, std::map<std::string, std::string> > websocketMappingMap;
	std::map<std::string, std::string> dynamicCppPagesMap;
	std::map<std::string, std::string> templateFilesMap;
	std::map<std::string, bool> applicationFlowMap;
	std::map<std::string, bool> servingContexts;
	std::vector<std::string> componentNames;
	FFEADContext ffeadContext;
	CorsConfig corsConfig;
	SecurityProperties securityProperties;
	CoreServerProperties coreServerProperties;
	std::map<std::string, std::map<std::string, ConnectionProperties> > sdormConnProperties;
	std::map<std::string, std::map<std::string, Mapping> > sdormEntityMappings;
	std::map<std::string, std::map<std::string, ConnectionProperties> > cacheConnProperties;
	std::map<std::string, std::vector<WsDetails> > webserviceDetailMap;
	std::map<std::string, std::map<std::string, ClassStructure> > classStructureMap;
	Logger logger;
	ThreadLocal httpRequest;
	ThreadLocal httpResponse;
	void* dlib;
	void* ddlib;
	static void clearInstance();
	friend class ExtHandler;
	friend class FilterHandler;
	friend class ConfigurationHandler;
	friend class ControllerHandler;
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
	friend class SocketUtil;
	static bool urlMatchesPath(const std::string& cntxtName, std::string pathurl, std::string url);
public:
	static bool isServingContext(const std::string& cntxtName);
	static ConfigurationData* getInstance();
	static SecurityProperties const& getSecurityProperties();
	static CoreServerProperties const& getCoreServerProperties();
	static HttpRequest* getHttpRequest();
	static HttpResponse* getHttpResponse();
	static void initializeAllSingletonBeans() {
		getInstance()->ffeadContext.initializeAllSingletonBeans(getInstance()->servingContexts);
	}
	static void clearAllSingletonBeans() {
		getInstance()->ffeadContext.clearAllSingletonBeans(getInstance()->servingContexts);
	}
	static void setCoreServerProperties(CoreServerProperties coreServerProperties) {
		getInstance()->coreServerProperties = coreServerProperties;
	}
	virtual ~ConfigurationData();
};

#endif /* CONFIGURATIONDATA_H_ */
