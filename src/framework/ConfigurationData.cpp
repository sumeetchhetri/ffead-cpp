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
 * ConfiguartionData.cpp
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#include "ConfigurationData.h"

ConfigurationData* ConfigurationData::instance = NULL;
//std::atomic<int> ConfigurationData::counter(0);

ConfigurationData::ConfigurationData() {
	logger = LoggerFactory::getLogger("ConfigurationData");
	dlib = NULL;
	ddlib = NULL;
	embeddedServer = false;
	apacheServer = false;
	nginxServer = false;
}

ConfigurationData* ConfigurationData::getInstance() {
	if(instance==NULL) {
		instance = new ConfigurationData;
	}
	return instance;
}

SecurityProperties const& ConfigurationData::getSecurityProperties() {
	return (getInstance()->securityProperties);
}

CoreServerProperties const& ConfigurationData::getCoreServerProperties() {
	return (getInstance()->coreServerProperties);
}

bool ConfigurationData::isServingContext(const std::string& cntxtName) {
	return getInstance()->servingContexts.find(cntxtName)!=getInstance()->servingContexts.end()
			&& getInstance()->servingContexts[cntxtName];
}

const ClassInfo ConfigurationData::getClassInfo(const std::string& cs, const std::string& app) {
	return getInstance()->ffeadContext.reflector->getClassInfo(cs, app);
}

void ConfigurationData::clearAllSingletonBeans() {
	getInstance()->ffeadContext.clearAllSingletonBeans(getInstance()->servingContexts);
}
void ConfigurationData::setCoreServerProperties(CoreServerProperties coreServerProperties) {
	getInstance()->coreServerProperties = coreServerProperties;
}

void ConfigurationData::initializeAllSingletonBeans() {
	getInstance()->dlib = dlopen(INTER_LIB_FILE, RTLD_NOW);
	if(getInstance()->dlib==NULL)
	{
		std::cout << dlerror() << std::endl;
		std::cout << "Could not load Library" << std::endl;
		exit(0);
	}

	getInstance()->ddlib = dlopen(DINTER_LIB_FILE, RTLD_NOW);
	if(getInstance()->ddlib==NULL)
	{
		std::cout << dlerror() << std::endl;
		std::cout << "Could not load dynamic Library" << std::endl;
		exit(0);
	}
	getInstance()->ffeadContext.initializeAllSingletonBeans(getInstance()->servingContexts);
}

void ConfigurationData::clearInstance() {
	if(instance!=NULL) {
		delete instance;
	}
}

ConfigurationData::~ConfigurationData() {
	// TODO Auto-generated destructor stub
}

Security::Security()
{
	sessTimeout = 0;
	logger = LoggerFactory::getLogger("Security");
}

Security::~Security()
{
}

bool Security::isLoginConfigured()
{
	return (loginProvider!="" && loginUrl!="");
}
bool Security::isSecureConfigured()
{
	return secures.size()!=0;
}
bool Security::isLoginUrl(const std::string& url, const std::string& actUrl)
{
	return (actUrl==(url+"/_ffead_security_cntxt_login_url"));
}
bool Security::isLoginPage(const std::string& cntxtName, const std::string& actUrl)
{
	return actUrl==loginUrl;
}

bool Security::addAspect(const SecureAspect& aspect)
{
	std::string pathurl = aspect.path;
	StringUtil::trim(pathurl);
	RegexUtil::replace(pathurl,"[/]+","/");
	if(secures.find("*")!=secures.end())
	{
		logger << ("Already found secure path mapping for * (All paths), cannot add any further secure paths, skipping...") << std::endl;
		return false;
	}
	else if(secures.find(pathurl)==secures.end())
	{
		bool endval = false, startval = false;
		if(pathurl.at(pathurl.length()-1)=='*')
		{
			pathurl = pathurl.substr(0, pathurl.length()-1);
			startval = true;
		}
		if(pathurl.at(0)=='*')
		{
			pathurl = pathurl.substr(1);
			endval = true;
		}
		bool exists = false;
		std::string url;
		std::map<std::string, SecureAspect>::iterator it;
		for (it=secures.begin();it!=secures.end();++it) {
			url = it->first;
			if(startval && url.find(pathurl)==0)
			{
				exists = true;
			}
			else if(endval && StringUtil::endsWith(url, pathurl))
			{
				exists = true;
			}
			else if(startval && endval && url.find(pathurl)!=std::string::npos)
			{
				exists = true;
			}
		}
		if(!exists)
		{
			secures[pathurl] = aspect;
			return true;
		}
		else
		{
			logger << ("Already found secure path mapping for " + pathurl + "(" + url + "), skipping...") << std::endl;
			return false;
		}
	}
	else
	{
		logger << ("Duplicate secure aspect found for path" + pathurl + ", skipping...") << std::endl;
		return false;
	}
}

SecureAspect Security::matchesPath(const std::string& cntxtName, std::string url)
{
	SecureAspect aspect;
	std::map<std::string, SecureAspect>::iterator it;
	for (it=secures.begin();it!=secures.end();++it) {
		SecureAspect secureAspect = it->second;
		std::string pathurl = secureAspect.path;
		//logger << ("Checking security path " + pathurl + " against url " + url) << std::endl;
		if(ConfigurationData::urlMatchesPath(cntxtName, pathurl, url)) {
			aspect = secureAspect;
		}
	}
	return aspect;
}

bool ConfigurationData::urlMatchesPath(const std::string& cntxtName, std::string pathurl, std::string url)
{
	//getInstance()->logger << ("Checking path " + pathurl + " against url " + url) << std::endl;
	if(pathurl==url)
	{
		return true;
	}
	else if(StringUtil::startsWith(pathurl, "regex(") && StringUtil::endsWith(pathurl, ")"))
	{
		std::string regurl = pathurl.substr(6, pathurl.length()-7);
		if(RegexUtil::find(url, regurl)!=-1)
		{
			return true;
		}
		return false;
	}
	else if(pathurl=="*")
	{
		return true;
	}
	else if(pathurl=="*.*" && url.find(".")!=std::string::npos)
	{
		return true;
	}

	bool urlextmtch = true;
	if(pathurl.find("*.")!=std::string::npos)
	{
		std::string pathext, urlext;
		size_t indx = pathurl.find("*.");
		pathext = pathurl.substr(indx+1);

		if(url.find(".")==std::string::npos)
		{
			return false;
		}

		if(indx==0)
		{
			urlext = url.substr(url.find("."));
			if(pathext==urlext)
			{
				return true;
			}
			return false;
		}
		else
		{
			pathurl = pathurl.substr(0, pathurl.find("*.")+1);
			urlext = url.substr(url.find("."));
			url = url.substr(0, url.find("."));
			urlextmtch = pathext==urlext;
		}
	}
	else if(StringUtil::endsWith(pathurl, ".*"))
	{
		if(url.find(".")==std::string::npos)
		{
			return false;
		}

		pathurl = pathurl.substr(0, pathurl.length()-2);
		url = url.substr(0, url.find("."));
	}

	bool endval = false, startval = false;
	if(pathurl.at(pathurl.length()-1)=='*')
	{
		pathurl = pathurl.substr(0, pathurl.length()-1);
		startval = true;
	}
	if(pathurl.at(0)=='*')
	{
		pathurl = pathurl.substr(1);
		endval = true;
	}

	if(urlextmtch && startval && endval && url.find(pathurl)!=std::string::npos)
	{
		return true;
	}
	else if(urlextmtch && startval && url.find(pathurl)==0)
	{
		return true;
	}
	else if(urlextmtch && endval && StringUtil::endsWith(url, pathurl))
	{
		return true;
	}
	else if(urlextmtch && !startval && !endval && pathurl==url)
	{
		return true;
	}
	return false;
}

HttpRequest* ConfigurationData::getHttpRequest() {
	return getInstance()->httpRequest.getPointer<HttpRequest>();
}

HttpResponse* ConfigurationData::getHttpResponse() {
	return getInstance()->httpRequest.getPointer<HttpResponse>();
}

bool ConfigurationData::isApacheServer() {
	return getInstance()->apacheServer;
}

void ConfigurationData::setApacheServer(bool isApacheServer) {
	getInstance()->apacheServer = isApacheServer;
	if(isApacheServer) {
		getInstance()->embeddedServer = false;
		getInstance()->nginxServer = false;
	}
}

bool ConfigurationData::isEmbeddedServer() {
	return getInstance()->embeddedServer;
}

void ConfigurationData::setEmbeddedServer(bool isEmbeddedServer) {
	getInstance()->embeddedServer = isEmbeddedServer;
	if(isEmbeddedServer) {
		getInstance()->apacheServer = false;
		getInstance()->nginxServer = false;
	}
}

bool ConfigurationData::isNginxServer() {
	return getInstance()->nginxServer;
}

void ConfigurationData::setNginxServer(bool isNginxServer) {
	getInstance()->nginxServer = isNginxServer;
	if(isNginxServer) {
		getInstance()->apacheServer = false;
		getInstance()->embeddedServer = false;
	}
}

int ConfigurationData::getProcessId() {
	return getpid();
}
