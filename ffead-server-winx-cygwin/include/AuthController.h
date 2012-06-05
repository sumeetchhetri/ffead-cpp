/*
 * AuthController.h
 *
 *  Created on: Nov 23, 2010
 *      Author: sumeet
 */

#ifndef AUTHCONTROLLER_H_
#define AUTHCONTROLLER_H_
#include "string"
#include "map"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace std;
class AuthController {
public:
	AuthController();
	virtual ~AuthController();
	virtual bool authenticate(string user,string password)=0;
	virtual bool authenticate(map<string,string>)=0;
	virtual bool handle(HttpRequest* req,HttpResponse* res)=0;
	virtual string getUserRole(string username)=0;
	virtual bool isInitialized()=0;
};

#endif /* AUTHCONTROLLER_H_ */
