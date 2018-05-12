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
	std::string provKey = isLoginPage(req->getCntxt_name(), req->getCurl());
	if(provKey=="")
	{
		return;
	}
	std::map<std::string, std::map<std::string, Security> >& securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	Security securityObject = securityObjectMap[req->getCntxt_name()][provKey];
	std::string userfld = securityObject.securityFieldNames["username"];
	std::string passfld = securityObject.securityFieldNames["password"];
	std::string userfrom = securityObject.securityFieldFrom["username"];
	std::string passfrom = securityObject.securityFieldFrom["password"];

	std::string username, password;

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

std::string SecurityHandler::isLoginPage(const std::string& cntxtName, const std::string& actUrl)
{
	std::map<std::string, std::map<std::string, Security> >& securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	std::map<std::string, Security> securityProv = securityObjectMap[cntxtName];
	std::map<std::string, Security>::iterator it;
	for (it=securityProv.begin();it!=securityProv.end();++it) {
		Security provider = it->second;
		if(provider.isLoginPage(cntxtName, actUrl))
		{
			return it->first;
		}
	}
	return "";
}

bool SecurityHandler::hasSecurity(const std::string& cntxtName) {
	return ConfigurationData::getInstance()->securityObjectMap[cntxtName].size()>0;
}

std::string SecurityHandler::validateSecurePath(const std::string& cntxtName, const std::string& actUrl, const std::string& username)
{
	std::map<std::string, std::map<std::string, Security> >& securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	std::map<std::string, Security> securityProv = securityObjectMap[cntxtName];
	std::map<std::string, Security>::iterator it;
	for (it=securityProv.begin();it!=securityProv.end();++it) {
		Security provider = it->second;
		SecureAspect aspect = provider.matchesPath(cntxtName, actUrl);
		if(provider.isLoginConfigured() && ((aspect.path!="" && aspect.role!="ROLE_ANONYMOUS") || (provider.isLoginPage(cntxtName, actUrl) && username!="")))
		{
			return it->first;
		}
	}
	return "";
}

bool SecurityHandler::handle(HttpRequest* req, HttpResponse* res, const long& sessionTimeout, Reflector& reflector)
{
	std::map<std::string, std::map<std::string, Security> >& securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	Logger logger = LoggerFactory::getLogger("SecurityHandler");

	bool isContrl = false;

	std::string userRole = req->getSession()->getAttribute("_FFEAD_USER_ACCESS_ROLE");
	if(userRole=="")
	{
		userRole = "ROLE_ANONYMOUS";
	}
	std::string claz;

	std::string username = req->userName;
	std::string password = req->password;

	std::string provKey = validateSecurePath(req->getCntxt_name(), req->getCurl(), username);
	if(provKey!="")
	{
		Security securityObject = securityObjectMap[req->getCntxt_name()][provKey];
		SecureAspect aspect = securityObject.matchesPath(req->getCntxt_name(), req->getCurl());
		//long sessionTimeoutVar = sessionTimeout;
		if(securityObject.isLoginConfigured())
		{
			//sessionTimeoutVar = securityObject.sessTimeout;
		}
		if(!securityObject.isLoginPage(req->getCntxt_name(), req->getCurl()))
		{
			if(aspect.path!="")
			{
				logger << ("Matched secure path " + aspect.path + ", which requires role " + aspect.role) << std::endl;
			}
			else if(aspect.role!=userRole)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::TempRedirect);
				res->addHeaderValue(HttpResponse::Location, "/"+req->getCntxt_name()+"/"+securityObject.loginUrl);
				res->setDone(true);
				isContrl = true;
			}
		}
		else if(securityObject.isLoginPage(req->getCntxt_name(), req->getCurl()) && username!="")
		{
			claz = securityObject.loginProvider;
			bool validUser = false;
			if(claz.find("file:")!=std::string::npos)
			{
				claz = req->getContextHome()+"/"+claz.substr(claz.find(":")+1);
				logger << ("Auth handled by file " + claz) << std::endl;
				FileAuthController authc(claz,":");
				if(authc.isInitialized())
				{
					if(authc.authenticate(username, password))
					{
						userRole = authc.getUserRole(username);
						logger << ("Valid user " + username
								+ ", role is "  + userRole) << std::endl;
						validUser = true;
					}
					else
					{
						logger << "Invalid user" << std::endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
						isContrl = true;
					}
				}
				else
				{
					logger << "Invalid user repo defined" << std::endl;
				}
			}
			else if(claz.find("class:")!=std::string::npos)
			{
				claz = claz.substr(claz.find(":")+1);
				logger << ("Auth handled by class " + claz) << std::endl;

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
					logger << ("AuthController class needs to implement all 3 methods namely, authenticate, getUserRole and isInitialized") << std::endl;
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
						std::string userRole = reflector.invokeMethod<std::string>(_temp,methGetUserRole,valusg);
						logger << ("Valid user " + username + ", role is "  + userRole) << std::endl;
						validUser = true;
					}
					else
					{
						logger << "Invalid user" << std::endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
						isContrl = true;
					}
					logger << "AuthController called" << std::endl;
				}
				else
				{
					logger << "AuthController not initialized" << std::endl;
				}
				ConfigurationData::getInstance()->ffeadContext.release(_temp, "login-handler_"+claz, req->getCntxt_name());
			}
			if(validUser && (aspect.role==userRole || securityObject.isLoginPage(req->getCntxt_name(), req->getCurl())))
			{
				req->getSession()->setAttribute("_FFEAD_USER_ACCESS_ROLE", userRole);
				res->setHTTPResponseStatus(HTTPResponseStatus::TempRedirect);
				res->addHeaderValue(HttpResponse::Location, "/"+req->getCntxt_name()+"/"+securityObject.welcomeFile);
				logger << ("Valid role " + userRole + " for path " + req->getCurl()) << std::endl;
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
