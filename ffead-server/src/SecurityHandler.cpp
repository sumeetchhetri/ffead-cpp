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

bool SecurityHandler::handle(HttpRequest* req, HttpResponse& res, long sessionTimeout)
{
	Logger logger = LoggerFactory::getLogger("SecurityHandler");
	string ip_addr = ConfigurationData::getInstance()->ip_address;
	map<string, Security> securityObjectMap = ConfigurationData::getInstance()->securityObjectMap;
	map<string, string> cntMap = ConfigurationData::getInstance()->cntMap;
	bool isContrl = false;
	string serverUrl = "";//"http://" + ip_addr;
	if(req->getCntxt_name()!="default" && cntMap[req->getCntxt_name()]!="true")
		serverUrl += "/" + req->getCntxt_name();
	string actUrl = serverUrl + req->getActUrl();
	string userRole = req->getSession()->getAttribute("_FFEAD_USER_ACCESS_ROLE");
	if(userRole=="")
	{
		userRole = "ROLE_ANONYMOUS";
	}
	string claz;
	Security securityObject = securityObjectMap[req->getCntxt_name()];
	//long sessionTimeoutVar = sessionTimeout;
	if(securityObject.isLoginConfigured())
	{
		//sessionTimeoutVar = securityObject.sessTimeout;
	}
	SecureAspect aspect = securityObject.matchesPath(req->getActUrl());
	if(securityObject.isLoginConfigured() && ((aspect.path!="" && aspect.role!="ROLE_ANONYMOUS")
			|| (securityObject.isLoginPage(serverUrl, actUrl) && req->getRequestParam("_ffead_security_cntxt_username")!="")))
	{
		logger << ("Matched secure path " + aspect.path + ", which requires role " + aspect.role) << endl;
		if(!securityObject.isLoginPage(serverUrl, actUrl) && aspect.role!=userRole)
		{
			res.setHTTPResponseStatus(HTTPResponseStatus::TempRedirect);
			res.addHeaderValue(HttpResponse::Location, serverUrl+"/"+securityObject.loginUrl);
			isContrl = true;
		}
		else if(securityObject.isLoginPage(serverUrl, actUrl) && req->getRequestParam("_ffead_security_cntxt_username")!="")
		{
			claz = securityObject.loginProvider;
			bool validUser = false;
			if(claz.find("file:")!=string::npos)
			{
				claz = req->getCntxt_root()+"/"+claz.substr(claz.find(":")+1);
				logger << ("Auth handled by file " + claz) << endl;
				FileAuthController* authc = new FileAuthController(claz,":");
				if(authc->isInitialized())
				{
					if(authc->authenticateSecurity(req->getRequestParam("_ffead_security_cntxt_username"),
							req->getRequestParam("_ffead_security_cntxt_password")))
					{
						userRole = authc->getUserRole(req->getRequestParam("_ffead_security_cntxt_username"));
						logger << ("Valid user " + req->getRequestParam("_ffead_security_cntxt_username")
								+ ", role is "  + userRole) << endl;
						validUser = true;
					}
					else
					{
						logger << "Invalid user" << endl;
						res.setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
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

				void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("login-handler_"+req->getCntxt_name()+claz, req->getCntxt_name());
				AuthController* loginc = (AuthController*)_temp;
				if(loginc!=NULL && loginc->authenticateSecurity(req->getRequestParam("_ffead_security_cntxt_username"),
					req->getRequestParam("_ffead_security_cntxt_password")))
				{
					userRole = loginc->getUserRole(req->getRequestParam("_ffead_security_cntxt_username"));
					logger << ("Valid user " + req->getRequestParam("_ffead_security_cntxt_username")
							+ ", role is "  + userRole) << endl;
					validUser = true;
				}
				else if(loginc!=NULL)
				{
					logger << "Invalid user" << endl;
					res.setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
					isContrl = true;
				}
				else
				{
					logger << "Invalid Login handler" << endl;
					res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
					isContrl = true;
				}
				logger << "Login controller called" << endl;

			}
			if(validUser && (aspect.role==userRole || securityObject.isLoginPage(serverUrl, actUrl)))
			{
				req->getSession()->setAttribute("_FFEAD_USER_ACCESS_ROLE", userRole);
				res.setHTTPResponseStatus(HTTPResponseStatus::TempRedirect);
				res.addHeaderValue(HttpResponse::Location, serverUrl+"/"+securityObject.welocmeFile);
				logger << ("Valid role " + userRole + " for path " + req->getActUrl()) << endl;
				isContrl = true;
			}
			else if(!validUser)
			{
				req->getSession()->setAttribute("_FFEAD_USER_ACCESS_ROLE", "ROLE_ANONYMOUS");
				res.setHTTPResponseStatus(HTTPResponseStatus::Unauthorized);
				isContrl = true;
			}
		}
	}
	return isContrl;
}
