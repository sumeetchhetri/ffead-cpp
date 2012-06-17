/*
 * SecurityHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef SECURITYHANDLER_H_
#define SECURITYHANDLER_H_
#include "FileAuthController.h"
#include "Reflector.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

typedef ClassInfo (*FunPtr) ();

class SecureAspect
{
public:
	string path;
	string role;
};

class Security
{
public:
	vector<SecureAspect> secures;
	string loginProvider;
	string loginUrl;
	string welocmeFile;
	long sessTimeout;
	bool isLoginConfigured()
	{
		return (loginProvider!="" && loginUrl!="");
	}
	bool isSecureConfigured()
	{
		return secures.size()!=0;
	}
	bool isLoginUrl(string url, string actUrl)
	{
		return (actUrl==(url+"/_ffead_security_cntxt_login_url"));
	}
	bool isLoginPage(string url, string actUrl)
	{
		return (actUrl==(url+"/"+loginUrl));
	}
	SecureAspect matchesPath(string url)
	{
		bool pathval = false;
		SecureAspect aspect;
		for (int var = 0; var < secures.size(); ++var) {
			SecureAspect secureAspect = secures.at(var);
			string pathurl = secureAspect.path;
			cout << "checking security path " << pathurl << " against url " << url << endl;
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
};


class SecurityHandler {
public:
	SecurityHandler();
	virtual ~SecurityHandler();
	static bool handle(string ip_addr, HttpRequest* req, HttpResponse& res, map<string, Security> securityObjectMap,
			long sessionTimeout, void* dlib, map<string, string> cntMap);
};

#endif /* SECURITYHANDLER_H_ */
