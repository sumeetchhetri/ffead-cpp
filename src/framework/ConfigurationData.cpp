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
	logger = LoggerFactory::getLogger("ConfigurationData");
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

bool ConfigurationData::isServingContext(const string& cntxtName) {
	return getInstance()->servingContexts.find(cntxtName)!=getInstance()->servingContexts.end()
			&& getInstance()->servingContexts[cntxtName];
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
bool Security::isLoginUrl(const string& url, const string& actUrl)
{
	return (actUrl==(url+"/_ffead_security_cntxt_login_url"));
}
bool Security::isLoginPage(const string& cntxtName, const string& actUrl)
{
	string fpath = "/"+cntxtName+"/"+loginUrl;
	RegexUtil::replace(fpath,"[/]+","/");
	string lpath = "/"+loginUrl;
	RegexUtil::replace(lpath,"[/]+","/");
	logger << actUrl << " " << lpath << " " << fpath << endl;
	return actUrl==fpath || actUrl==lpath;
}

bool Security::addAspect(const SecureAspect& aspect)
{
	string pathurl = aspect.path;
	StringUtil::trim(pathurl);
	RegexUtil::replace(pathurl,"[/]+","/");
	if(secures.find("*")!=secures.end())
	{
		logger << ("Already found secure path mapping for * (All paths), cannot add any further secure paths, skipping...") << endl;
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
		string url;
		map<string, SecureAspect>::iterator it;
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
			else if(startval && endval && url.find(pathurl)!=string::npos)
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
			logger << ("Already found secure path mapping for " + pathurl + "(" + url + "), skipping...") << endl;
			return false;
		}
	}
	else
	{
		logger << ("Duplicate secure aspect found for path" + pathurl + ", skipping...") << endl;
		return false;
	}
}

SecureAspect Security::matchesPath(const string& cntxtName, string url)
{
	SecureAspect aspect;
	map<string, SecureAspect>::iterator it;
	for (it=secures.begin();it!=secures.end();++it) {
		SecureAspect secureAspect = it->second;
		string pathurl = secureAspect.path;
		StringUtil::trim(pathurl);
		RegexUtil::replace(pathurl,"[/]+","/");
		logger << ("Checking security path " + pathurl + " against url " + url) << endl;
		if(StringUtil::startsWith(url, "/"+cntxtName) && StringUtil::startsWith(pathurl, "regex(") && StringUtil::endsWith(pathurl, ")"))
		{
			string regurl = pathurl.substr(6, pathurl.length()-1);
			string cntpre = "/"+cntxtName;
			string nurl = url.substr(cntpre.length());
			if(RegexUtil::find(nurl, regurl)!=-1)
			{
				aspect = secureAspect;
			}
			continue;
		}
		else if(StringUtil::startsWith(pathurl, "regex(") && StringUtil::endsWith(pathurl, ")"))
		{
			string regurl = pathurl.substr(6, pathurl.length()-1);
			if(RegexUtil::find(url, regurl)!=-1)
			{
				aspect = secureAspect;
			}
			continue;
		}
		else if(pathurl=="*")
		{
			aspect = secureAspect;
			continue;
		}
		else if(pathurl=="*.*" && url.find(".")!=-1)
		{
			aspect = secureAspect;
			continue;
		}

		bool urlextmtch = true;
		if(pathurl.find("*.")!=string::npos)
		{
			urlextmtch = false;
			string pathext, urlext;
			size_t indx = pathurl.find("*.");
			pathext = pathurl.substr(indx+1);

			if(url.find(".")==string::npos)
				continue;

			if(indx==0)
			{
				urlext = url.substr(url.find("."));
				if(pathext==urlext)
				{
					aspect = secureAspect;
				}
				continue;
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
			if(url.find(".")==string::npos)
				continue;

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
		string fpath = "/"+cntxtName+"/"+pathurl;
		RegexUtil::replace(fpath,"[/]+","/");
		if(urlextmtch && startval && endval && ((cntxtName=="default" && url.find(pathurl)!=string::npos) || url.find(fpath)!=string::npos))
		{
			aspect = secureAspect;
		}
		else if(urlextmtch && startval && ((cntxtName=="default" && url.find(pathurl)==0) || url.find(fpath)==0))
		{
			aspect = secureAspect;
		}
		else if(urlextmtch && endval && ((cntxtName=="default" && StringUtil::endsWith(url, pathurl)) || StringUtil::endsWith(url, fpath)))
		{
			aspect = secureAspect;
		}
		else if(urlextmtch && !startval && !endval && ((cntxtName=="default" && pathurl==url) || fpath==url))
		{
			aspect = secureAspect;
		}
	}
	return aspect;
}

bool ConfigurationData::urlMatchesPath(const string& cntxtName, string pathurl, string url)
{
	StringUtil::trim(pathurl);
	RegexUtil::replace(pathurl,"[/]+","/");
	string fpath = "/"+cntxtName+"/"+pathurl;
	RegexUtil::replace(fpath,"[/]+","/");

	getInstance()->logger << ("Checking path " + pathurl + " against url " + url) << endl;
	if(StringUtil::startsWith(url, cntxtName) && StringUtil::startsWith(pathurl, "regex(") && StringUtil::endsWith(pathurl, ")"))
	{
		string regurl = pathurl.substr(6, pathurl.length()-1);
		string nurl = url.substr(cntxtName.length());
		if(RegexUtil::find(nurl, regurl)!=-1)
		{
			return true;
		}
		return false;
	}
	else if(StringUtil::startsWith(pathurl, "regex(") && StringUtil::endsWith(pathurl, ")"))
	{
		string regurl = pathurl.substr(6, pathurl.length()-1);
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
	else if(pathurl=="*.*" && url.find(".")!=-1)
	{
		return true;
	}

	bool urlextmtch = true;
	if(pathurl.find("*.")!=string::npos)
	{
		string pathext, urlext;
		size_t indx = pathurl.find("*.");
		pathext = pathurl.substr(indx+1);

		if(url.find(".")==string::npos)
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
		if(url.find(".")==string::npos)
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

	if(urlextmtch && startval && endval && ((cntxtName=="default" && url.find(pathurl)!=string::npos) || url.find(fpath)!=string::npos))
	{
		return true;
	}
	else if(urlextmtch && startval && ((cntxtName=="default" && url.find(pathurl)==0) || url.find(fpath)==0))
	{
		return true;
	}
	else if(urlextmtch && endval && ((cntxtName=="default" && StringUtil::endsWith(url, pathurl)) || StringUtil::endsWith(url, fpath)))
	{
		return true;
	}
	else if(urlextmtch && !startval && !endval && ((cntxtName=="default" && pathurl==url) || fpath==url))
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
