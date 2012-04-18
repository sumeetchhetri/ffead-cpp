/*
 * RestController.h
 *
 *  Created on: Feb 4, 2012
 *      Author: sumeet
 */

#ifndef RESTCONTROLLER_H_
#define RESTCONTROLLER_H_
#include "HttpResponse.h"
#include "HttpRequest.h"

class RestController {
public:
	HttpRequest* request;
	HttpResponse* response;
	RestController();
	virtual ~RestController();
};

#endif /* RESTCONTROLLER_H_ */
