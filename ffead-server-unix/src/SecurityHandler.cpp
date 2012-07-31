/*
	Copyright 2010, Sumeet Chhetri

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

bool SecurityHandler::handle(string ip_addr, HttpRequest* req, HttpResponse& res, map<string, Security> securityObjectMap,
		long sessionTimeout, void* dlib, map<string, string> cntMap)
{
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
	logger << actUrl << endl;
	string claz;
	Security securityObject = securityObjectMap[req->getCntxt_name()];
	long sessionTimeoutVar = sessionTimeout;
	if(securityObject.isLoginConfigured())
	{
		sessionTimeoutVar = securityObject.sessTimeout;
	}
	SecureAspect aspect = securityObject.matchesPath(req->getActUrl());
	if(securityObject.isLoginConfigured() && ((aspect.path!="" && aspect.role!="ROLE_ANONYMOUS")
			|| (securityObject.isLoginPage(serverUrl, actUrl) && req->getRequestParam("_ffead_security_cntxt_username")!="")))
	{
		logger << "matched secure path " << aspect.path << ", which requires role " << aspect.role << endl;
		if(!securityObject.isLoginPage(serverUrl, actUrl) && aspect.role!=userRole)
		{
			res.setStatusCode("307");
			res.setStatusMsg("Temporary Redirect");
			res.setLocation(serverUrl+"/"+securityObject.loginUrl);
			isContrl = true;
		}
		else if(securityObject.isLoginPage(serverUrl, actUrl) && req->getRequestParam("_ffead_security_cntxt_username")!="")
		{
			claz = securityObject.loginProvider;
			bool validUser = false;
			if(claz.find("file:")!=string::npos)
			{
				claz = req->getCntxt_root()+"/"+claz.substr(claz.find(":")+1);
				logger << "auth handled by file " << claz << endl;
				FileAuthController* authc = new FileAuthController(claz,":");
				if(authc->isInitialized())
				{
					if(authc->authenticateSecurity(req->getRequestParam("_ffead_security_cntxt_username"),
							req->getRequestParam("_ffead_security_cntxt_password")))
					{
						userRole = authc->getUserRole(req->getRequestParam("_ffead_security_cntxt_username"));
						logger << "valid user " << req->getRequestParam("_ffead_security_cntxt_username")
								<< ", role is "  << userRole << endl;
						validUser = true;
					}
					else
					{
						logger << "invalid user" << endl;
						res.setStatusCode("401");
						res.setStatusMsg("Unauthorized\r\nWWW-Authenticate: Invalid authentication details");
						isContrl = true;
					}
				}
				else
				{
					logger << "invalid user repo defined" << endl;
				}
				delete authc;
			}
			else if(claz.find("class:")!=string::npos)
			{
				claz = claz.substr(claz.find(":")+1);
				claz = "getReflectionCIFor" + claz;
				logger << "auth handled by class " << claz << endl;
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
						logger << "valid user " << req->getRequestParam("_ffead_security_cntxt_username")
								<< ", role is "  << userRole << endl;
						validUser = true;
					}
					else
					{
						logger << "invalid user" << endl;
						res.setStatusCode("401");
						res.setStatusMsg("Unauthorized\r\nWWW-Authenticate: Invalid authentication details");
						isContrl = true;
					}
					logger << "login controller called" << endl;
					delete loginc;
				}
			}
			if(validUser && (aspect.role==userRole || securityObject.isLoginPage(serverUrl, actUrl)))
			{
				req->getSession()->setAttribute("_FFEAD_USER_ACCESS_ROLE", userRole);
				res.setStatusCode("307");
				res.setStatusMsg("Temporary Redirect");
				res.setLocation(serverUrl+"/"+securityObject.welocmeFile);
				logger << "valid role " << userRole << " for path " << req->getActUrl();
				isContrl = true;
			}
			else if(!validUser)
			{
				req->getSession()->setAttribute("_FFEAD_USER_ACCESS_ROLE", "ROLE_ANONYMOUS");
				res.setStatusCode("401");
				res.setStatusMsg("Unauthorized\r\nWWW-Authenticate: Invalid authentication details");
				isContrl = true;
			}
		}
	}
	return isContrl;
}

Security::Security()
{
	logger = Logger::getLogger("Security");
}

Security::~Security()
{

}

SecureAspect Security::matchesPath(string url)
{
	bool pathval = false;
	SecureAspect aspect;
	for (int var = 0; var < secures.size(); ++var) {
		SecureAspect secureAspect = secures.at(var);
		string pathurl = secureAspect.path;
		logger << "checking security path " << pathurl << " against url " << url << endl;
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
