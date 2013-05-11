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
public:
	string name;
	string type;
	string from;
};

class RestFunction
{
public:
	string name;
	string alias;
	string clas;
	string meth;
	string baseUrl;
	string icontentType;
	string ocontentType;
	vector<RestFunctionParams> params;
};

typedef map<string, RestFunction> resFuncMap;

class SecureAspect
{
public:
	string path;
	string role;
};

class Security
{
	Logger logger;
public:
	Security();
	~Security();
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
};


class CorsConfig {
public:
	string allwdOrigins;
	string allwdMethods;
	string allwdHeaders;
	bool allwdCredentials;
	string exposedHeaders;
	long maxAge;
	bool isOriginAllowed(strVec reqOrgLst);
	bool isMethodAllowed(string method);
	bool isHeaderAllowed(strVec reqHdrLst, string& erheadr);
};

class ConfigurationData {
public:
	string key_file,dh_file,ca_list,rand_file,sec_password,srv_auth_prvd,srv_auth_mode,srv_auth_file,ip_address,resourcePath;
	int client_auth;
	map<string, vector<string> > filterMap;
	resFuncMap rstCntMap;
	map<string, string> handoffs;
	map<string, Security> securityObjectMap;
	map<string, Element> formMap;
	strVec cmpnames;
	map<string, string> sprops,props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap,mapMap,mappattMap,autMap,autpattMap,wsdlmap,fviewmap,ajaxIntfMap,dcpsss, tpes;
	long sessionTimeout;
	bool sessatserv;
	FFEADContext* ffeadContext;
	CorsConfig corsConfig;
	ConfigurationData();
	virtual ~ConfigurationData();
};

#endif /* CONFIGURATIONDATA_H_ */
