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

Logger SecurityHandler::logger;

SecurityHandler::SecurityHandler() {
	logger = Logger::getLogger("SecurityHandler");
}

SecurityHandler::~SecurityHandler() {
	// TODO Auto-generated destructor stub
}

bool SecurityHandler::handle(ConfigurationData configData, HttpRequest* req, HttpResponse& res, long sessionTimeout, void* dlib)
{
	string ip_addr = configData.ip_address;
	map<string, Security> securityObjectMap = configData.securityObjectMap;
	map<string, string> cntMap = configData.cntMap;
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

				void *_temp = configData.ffeadContext->getBean("login-handler_"+req->getCntxt_name()+claz);
				AuthController* loginc = static_cast<AuthController*>(_temp);
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

				/*claz = "getReflectionCIFor" + claz;
				if(dlib == NULL)
				{
					cerr << dlerror() << endl;
					exit(-1);
				}
				void *mkr = dlsym(dlib, claz.c_str());
				if(mkr!=NULL)
				{
					FunPtr f =  (FunPtr)mkr;
					ClassInfo srv = f();
					args argus;
					Constructor ctor = srv.getConstructor(argus);
					Reflector ref;
					void *_temp = ref.newInstanceGVP(ctor);
					AuthController* loginc = (AuthController*)_temp;
					if(loginc->authenticateSecurity(req->getRequestParam("_ffead_security_cntxt_username"),
						req->getRequestParam("_ffead_security_cntxt_password")))
					{
						userRole = loginc->getUserRole(req->getRequestParam("_ffead_security_cntxt_username"));
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
					logger << "Login controller called" << endl;
					delete loginc;
				}*/

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
