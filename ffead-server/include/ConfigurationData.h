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
 * ConfiguartionData.h
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#ifndef CONFIGURATIONDATA_H_
#define CONFIGURATIONDATA_H_
#include "string"
#include "FFEADContext.h"
using namespace std;

typedef void* (*toObjectFromJson) (string);

class RestFunctionParams
{
	string name;
	string type;
	string from;
	friend class ConfigurationHandler;
	friend class ControllerHandler;
};

class RestFunction
{
	string name;
	string alias;
	string clas;
	string meth;
	string baseUrl;
	string icontentType;
	string ocontentType;
	vector<RestFunctionParams> params;
	friend class ConfigurationHandler;
	friend class ControllerHandler;
};

typedef map<string, RestFunction> resFuncMap;

class SecureAspect
{
	string path;
	string role;
	friend class ConfigurationHandler;
	friend class ConfigurationData;
	friend class Security;
	friend class SecurityHandler;
};

class Security
{
	Logger logger;
	vector<SecureAspect> secures;
	string loginProvider;
	string loginUrl;
	string welocmeFile;
	long sessTimeout;
	bool isLoginConfigured();
	bool isSecureConfigured();
	bool isLoginUrl(string url, string actUrl);
	bool isLoginPage(string url, string actUrl);
	SecureAspect matchesPath(string url);
	friend class ConfigurationData;
	friend class ConfigurationHandler;
	friend class SecurityHandler;
public:
	Security();
	~Security();
};


class CorsConfig {
	string allwdOrigins;
	string allwdMethods;
	string allwdHeaders;
	bool allwdCredentials;
	string exposedHeaders;
	long maxAge;
	bool isOriginAllowed(strVec reqOrgLst);
	bool isMethodAllowed(string method);
	bool isHeaderAllowed(strVec reqHdrLst, string& erheadr);
	friend class ConfigurationHandler;
	friend class CORSHandler;
};

class JobConfig
{
	string name;
	string cron;
	string clas;
	string meth;
	string app;
	friend class JobScheduler;
};

class ConfigurationData {
	ConfigurationData();
	static ConfigurationData* instance;
public:
	string cert_file,key_file,dh_file,ca_list,rand_file,sec_password,srv_auth_prvd,srv_auth_mode,srv_auth_file,ip_address,resourcePath;
	int client_auth;
	map<string, vector<string> > filterMap;
	resFuncMap rstCntMap;
	map<string, string> handoffs;
	map<string, Security> securityObjectMap;
	map<string, Element> formMap;
	strVec cmpnames;
	map<string, string> sprops,props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap,mapMap,mappattMap,autMap,autpattMap,wsdlmap,fviewmap,ajaxIntfMap,dcpsss, tpes;
	long sessionTimeout, sessionFileLockTimeout, isDHParams;
	bool sessatserv, sessservdistocache;
	FFEADContext ffeadContext;
	CorsConfig corsConfig;
	static ConfigurationData* getInstance();
	static void clearInstance();
	virtual ~ConfigurationData();
};

#endif /* CONFIGURATIONDATA_H_ */
