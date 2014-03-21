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

ConfigurationData::ConfigurationData() {
	// TODO Auto-generated constructor stub
}

ConfigurationData* ConfigurationData::getInstance() {
	if(instance==NULL) {
		instance = new ConfigurationData;
	}
	return instance;
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
bool Security::isLoginUrl(string url, string actUrl)
{
	return (actUrl==(url+"/_ffead_security_cntxt_login_url"));
}
bool Security::isLoginPage(string url, string actUrl)
{
	return (actUrl==(url+"/"+loginUrl));
}

SecureAspect Security::matchesPath(string url)
{
	bool pathval = false;
	SecureAspect aspect;
	for (int var = 0; var < (int)secures.size(); ++var) {
		SecureAspect secureAspect = secures.at(var);
		string pathurl = secureAspect.path;
		logger << ("Checking security path " + pathurl + " against url " + url) << endl;
		if(pathurl=="*")
		{
			aspect = secureAspect;
			continue;
		}
		if(pathurl.find("*")==pathurl.length()-1)
		{
			pathurl = pathurl.substr(0, pathurl.length()-1);
			pathval = true;
		}
		if(pathval && url.find(pathurl)!=string::npos)
		{
			aspect = secureAspect;
		}
		else if(!pathval && pathurl==url)
		{
			aspect = secureAspect;
		}
	}
	return aspect;
}


bool CorsConfig::isOriginAllowed(strVec reqOrgLst)
{
	strVec orgLst;
	StringUtil::trim(allwdOrigins);
	if(allwdOrigins=="*")
	{
		return true;
	}
	StringUtil::split(orgLst, allwdOrigins, (","));
	for (int var = 0; var < (int)reqOrgLst.size(); ++var) {
		for (int var1 = 0; var1 < (int)orgLst.size(); ++var1) {
			if(StringUtil::toLowerCopy(orgLst.at(var1))==StringUtil::toLowerCopy(reqOrgLst.at(var)))
			{
				return true;
			}
		}
	}
	return false;
}
bool CorsConfig::isMethodAllowed(string method)
{
	if(method=="")
	{
		return false;
	}
	strVec methLst;
	StringUtil::trim(allwdMethods);
	StringUtil::split(methLst, allwdMethods, (","));
	for (int var = 0; var < (int)methLst.size(); ++var) {
		if(StringUtil::toLowerCopy(method)==StringUtil::toLowerCopy(methLst.at(var)))
		{
			return true;
		}
	}
	return false;
}
bool CorsConfig::isHeaderAllowed(strVec reqHdrLst, string& erheadr)
{
	strVec hdrLst;
	StringUtil::trim(allwdHeaders);
	if(allwdHeaders=="*")
	{
		return true;
	}
	StringUtil::split(hdrLst, allwdHeaders, (","));
	for (int var = 0; var < (int)reqHdrLst.size(); ++var) {
		for (int var1 = 0; var1 < (int)hdrLst.size(); ++var1) {
			if(StringUtil::toLowerCopy(hdrLst.at(var1))==StringUtil::toLowerCopy(reqHdrLst.at(var)))
			{
				return true;
			}
		}
		erheadr = StringUtil::toUpperCopy(reqHdrLst.at(var));
		break;
	}
	return false;
}
