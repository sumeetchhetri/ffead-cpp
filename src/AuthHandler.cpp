/*
 * AuthHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "AuthHandler.h"

AuthHandler::AuthHandler() {
	// TODO Auto-generated constructor stub

}

AuthHandler::~AuthHandler() {
	// TODO Auto-generated destructor stub
}

string AuthHandler::getFileExtension(const string& file)
{
	string str = file;
	string ext = "";
	for(unsigned int i=0; i<str.length(); i++)
	{
		if(str[i] == '.')
		{
			for(unsigned int j = i; j<str.length(); j++)
			{
				ext += str[j];
			}
			return ext;
		}
	}
	return ext;
}

bool AuthHandler::handle(map<string, string> autMap, map<string, string> autpattMap, HttpRequest* req, HttpResponse& res, map<string, vector<string> > filterMap, void* dlib,
		string ext)
{
	bool isContrl = false;
	string claz;
	if(autpattMap[req->getCntxt_name()+"*.*"]!="" || autMap[req->getCntxt_name()+ext]!="")
	{
		if(autpattMap[req->getCntxt_name()+"*.*"]!="")
		{
			claz = autpattMap[req->getCntxt_name()+"*.*"];
		}
		else
		{
			claz = autMap[req->getCntxt_name()+ext];
		}
		AuthController *authc;
		cout << "OAUTH/HTTP Authorization requested " <<  claz << endl;
		map<string,string>::iterator it;
		map<string,string> tempmap = req->getAuthinfo();
		for(it=tempmap.begin();it!=tempmap.end();it++)
		{
			cout << it->first << " = " << it->second << endl;
		}
		map<string,string> tempmap1 = req->getRequestParams();
		for(it=tempmap1.begin();it!=tempmap1.end();it++)
		{
			cout << it->first << " = " << it->second << endl;
		}
		if(claz.find("file:")!=string::npos)
		{
			claz = req->getCntxt_root()+"/"+claz.substr(claz.find(":")+1);
			cout << "auth handled by file " << claz << endl;
			authc = new FileAuthController(claz,":");
			if(authc->isInitialized())
			{
				if(authc->authenticate(req->getAuthinfo()["Username"],req->getAuthinfo()["Password"]))
				{
					cout << "valid user" << endl;
				}
				else
				{
					cout << "invalid user" << endl;
					res.setStatusCode("401");
					res.setStatusMsg("Unauthorized\r\nWWW-Authenticate: Invalid authentication details");
					isContrl = true;
					cout << "verified request token signature is invalid" << endl;
				}
			}
			else
			{
				cout << "invalid user repo defined" << endl;
			}
		}
		else if(claz.find("class:")!=string::npos)
		{
			claz = claz.substr(claz.find(":")+1);
			claz = "getReflectionCIFor" + claz;
			cout << "auth handled by class " << claz << endl;
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
				authc = (AuthController*)_temp;
				bool isoAuthRes = authc->handle(req,&res);
				if(res.getStatusCode()!="")
					isContrl = true;
				cout << "authhandler called" << endl;
				ext = getFileExtension(req->getUrl());
				delete authc;
			}
		}
	}
	return isContrl;
}
