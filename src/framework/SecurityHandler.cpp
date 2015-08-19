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
 * SecurityHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "SecurityHandler.h"

SecurityHandler::SecurityHandler() {
}

SecurityHandler::~SecurityHandler() {
	// TODO Auto-generated destructor stub
}

void SecurityHandler::populateAuthDetails(HttpRequest* req)
{
	string cntxtName = req->getCntxt_name();

	string actUrl = req->getActUrl();

	string provKey = isLoginPage(cntxtName, actUrl);
	if(provKey=="")
	{
		return;
	}
	map<string, map<string, Security> > securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	Security securityObject = securityObjectMap[cntxtName][provKey];
	string userfld = securityObject.securityFieldNames["username"];
	string passfld = securityObject.securityFieldNames["password"];
	string userfrom = securityObject.securityFieldFrom["username"];
	string passfrom = securityObject.securityFieldFrom["password"];

	string username, password;

	if(req->getAuthMethod()=="basic")
	{
		username = req->getUserName();
	}
	else if(userfrom=="reqparam")
	{
		username = req->getQueryParam(userfld);
	}
	else if(userfrom=="postparam")
	{
		username = req->getRequestParam(userfld);
	}
	else if(userfrom=="header")
	{
		username = req->getHeader(userfld);
	}
	else
	{
		username = req->getRequestParam("_ffead_security_cntxt_username");
	}

	if(req->getAuthMethod()=="basic")
	{
		password = req->getPassword();
	}
	else if(passfrom=="reqparam")
	{
		password = req->getQueryParam(passfld);
	}
	else if(passfrom=="postparam")
	{
		password = req->getRequestParam(passfld);
	}
	else if(passfrom=="header")
	{
		password = req->getHeader(passfld);
	}
	else
	{
		password = req->getRequestParam("_ffead_security_cntxt_password");
	}

	req->userName = username;
	req->password = password;
}

string SecurityHandler::isLoginPage(const string& cntxtName, const string& actUrl)
{
	map<string, map<string, Security> > securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	if(securityObjectMap.find(cntxtName)!=securityObjectMap.end())
	{
		map<string, Security> securityProv = securityObjectMap[cntxtName];
		map<string, Security>::iterator it;
		for (it=securityProv.begin();it!=securityProv.end();++it) {
			Security provider = it->second;
			if(provider.isLoginPage(cntxtName, actUrl))
			{
				return it->first;
			}
		}
	}
	return "";
}

string SecurityHandler::validateSecurePath(const string& cntxtName, const string& actUrl, const string& username)
{
	map<string, map<string, Security> > securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	if(securityObjectMap.find(cntxtName)!=securityObjectMap.end())
	{
		map<string, Security> securityProv = securityObjectMap[cntxtName];
		map<string, Security>::iterator it;
		for (it=securityProv.begin();it!=securityProv.end();++it) {
			Security provider = it->second;
			SecureAspect aspect = provider.matchesPath(cntxtName, actUrl);
			if(provider.isLoginConfigured() && ((aspect.path!="" && aspect.role!="ROLE_ANONYMOUS")
				|| (provider.isLoginPage(cntxtName, actUrl) && username!="")))
			{
				return it->first;
			}
		}
	}
	return "";
}

bool SecurityHandler::handle(HttpRequest* req, HttpResponse* res, const long& sessionTimeout, Reflector& reflector)
{
	map<string, map<string, Security> > securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	Logger logger = LoggerFactory::getLogger("SecurityHandler");

	bool isContrl = false;

	string actUrl = req->getActUrl();

	string userRole = req->getSession()->getAttribute("_FFEAD_USER_ACCESS_ROLE");
	if(userRole=="")
	{
		userRole = "ROLE_ANONYMOUS";
	}
	string claz;

	string username = req->userName;
	string password = req->password;

	string provKey = validateSecurePath(req->getCntxt_name(), actUrl, username);
	if(provKey!="")
	{
		Security securityObject = securityObjectMap[req->getCntxt_name()][provKey];
		SecureAspect aspect = securityObject.matchesPath(req->getCntxt_name(), req->getActUrl());
		//long sessionTimeoutVar = sessionTimeout;
		if(securityObject.isLoginConfigured())
		{
			//sessionTimeoutVar = securityObject.sessTimeout;
		}
		if(!securityObject.isLoginPage(req->getCntxt_name(), actUrl))
		{
			if(aspect.path!="")
			{
				logger << ("Matched secure path " + aspect.path + ", which requires role " + aspect.role) << endl;
			}
			else if(aspect.role!=userRole)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::TempRedirect);
				res->addHeaderValue(HttpResponse::Location, "/"+req->getCntxt_name()+"/"+securityObject.loginUrl);
				res->setDone(true);
				isContrl = true;
			}
		}
		else if(securityObject.isLoginPage(req->getCntxt_name(), actUrl) && username!="")
		{
			claz = securityObject.loginProvider;
			bool validUser = false;
			if(claz.find("file:")!=string::npos)
			{
				claz = req->getContextHome()+"/"+claz.substr(claz.find(":")+1);
				logger << ("Auth handled by file " + claz) << endl;
				FileAuthController* authc = new FileAuthController(claz,":");
				if(authc->isInitialized())
				{
					if(authc->authenticate(username, password))
					{
						userRole = authc->getUserRole(username);
						logger << ("Valid user " + username
								+ ", role is "  + userRole) << endl;
						validUser = true;
					}
					else
					{
						logger << "Invalid user" << endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
						isContrl = true;
					}
				}
				else
				{
					logger << "Invalid user repo defined" << endl;
				}
				delete authc;
			}
			else if(claz.find("class:")!=string::npos)
			{
				claz = claz.substr(claz.find(":")+1);
				logger << ("Auth handled by class " + claz) << endl;

				args argusi, argusa, argusg;
				vals valusi, valusa, valusg;

				void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("login-handler_"+claz, req->getCntxt_name());
				const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][claz];

				Method methIsInitialized = srv.getMethod("isInitialized", argusi);
				argusg.push_back("string");
				Method methGetUserRole = srv.getMethod("getUserRole", argusg);
				argusa.push_back("string");
				argusa.push_back("string");
				Method methAuthenticate = srv.getMethod("authenticate", argusa);

				if(methIsInitialized.getMethodName()=="" || methGetUserRole.getMethodName()=="" || methAuthenticate.getMethodName()=="")
				{
					logger << ("AuthController class needs to implement all 3 methods namely, authenticate, getUserRole and isInitialized") << endl;
					return false;
				}

				bool inited = reflector.invokeMethod<bool>(_temp,methIsInitialized,valusi);
				if(_temp!=NULL && inited)
				{
					valusa.push_back(&username);
					valusa.push_back(&password);
					bool authenticated = reflector.invokeMethod<bool>(_temp,methAuthenticate,valusa);
					if(authenticated)
					{
						valusg.push_back(&username);
						string userRole = reflector.invokeMethod<string>(_temp,methGetUserRole,valusg);
						logger << ("Valid user " + username + ", role is "  + userRole) << endl;
						validUser = true;
					}
					else
					{
						logger << "Invalid user" << endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
						isContrl = true;
					}
					logger << "AuthController called" << endl;
				}
				else
				{
					logger << "AuthController not initialized" << endl;
				}
				//if(_temp!=NULL)
				//	delete _temp;
			}
			if(validUser && (aspect.role==userRole || securityObject.isLoginPage(req->getCntxt_name(), actUrl)))
			{
				req->getSession()->setAttribute("_FFEAD_USER_ACCESS_ROLE", userRole);
				res->setHTTPResponseStatus(HTTPResponseStatus::TempRedirect);
				res->addHeaderValue(HttpResponse::Location, "/"+req->getCntxt_name()+"/"+securityObject.welcomeFile);
				logger << ("Valid role " + userRole + " for path " + req->getActUrl()) << endl;
				isContrl = true;
				res->setDone(true);
			}
			else if(!validUser)
			{
				req->getSession()->setAttribute("_FFEAD_USER_ACCESS_ROLE", "ROLE_ANONYMOUS");
				res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
				isContrl = true;
				res->setDone(true);
			}
		}
	}
	return isContrl;
}
