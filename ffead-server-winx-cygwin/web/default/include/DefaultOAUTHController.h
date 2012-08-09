/*
 * DefaultOAUTHController.h
 *
 *  Created on: Nov 24, 2010
 *      Author: sumeet
 */

#ifndef DEFAULTOAUTHCONTROLLER_H_
#define DEFAULTOAUTHCONTROLLER_H_
#include "AuthController.h"
#include <boost/algorithm/string.hpp>
#include "CryptoHandler.h"
#include "FileAuthController.h"
#include "Timer.h"

class DefaultOAUTHController : public AuthController
{
public:
	DefaultOAUTHController();
	virtual ~DefaultOAUTHController();
	bool handle(HttpRequest* req,HttpResponse* res);
	bool authenticate(string,string){return false;}
	bool authenticateSecurity(string user,string password){return false;};
	bool isInitialized(){return false;}
	bool authenticate(map<string,string>){return false;}
	string getUserRole(string username){return "ROLE_ANONYMOUS";}
};

#endif /* DEFAULTOAUTHCONTROLLER_H_ */
