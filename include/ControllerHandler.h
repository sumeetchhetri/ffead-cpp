/*
 * ControllerHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef CONTROLLERHANDLER_H_
#define CONTROLLERHANDLER_H_
#include "Controller.h"
#include "RestController.h"
#include "AuthHandler.h"

class RestFunctionParams
{
public:
	int pos;
	string type;
};

class RestFunction
{
public:
	string name;
	string alias;
	string clas;
	string meth;
	string baseUrl;
	vector<RestFunctionParams> params;
};

typedef map<string, RestFunction> resFuncMap;

class ControllerHandler {
public:
	ControllerHandler();
	virtual ~ControllerHandler();
	static bool handle(HttpRequest* req, HttpResponse& res, map<string, string> urlpattMap, map<string, string> mappattMap, void* dlib,
			string ext, resFuncMap rstCntMap, map<string, string> mapMap, map<string, string> urlMap, string pthwofile);
};

#endif /* CONTROLLERHANDLER_H_ */
