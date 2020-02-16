/*
 * Router.h
 *
 *  Created on: 04-Feb-2020
 *      Author: sumeetc
 */

#ifndef SRC_FRAMEWORK_ROUTER_H_
#define SRC_FRAMEWORK_ROUTER_H_
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ClassInfo.h"
#include "map"

class Router {
public:
	virtual void route(HttpRequest* req, HttpResponse* res, void* dlib, void* ddlib)=0;
	virtual ~Router(){}
};

#endif /* SRC_FRAMEWORK_ROUTER_H_ */
