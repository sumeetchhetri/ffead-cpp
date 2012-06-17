/*
 * FviewHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef FVIEWHANDLER_H_
#define FVIEWHANDLER_H_
#include "HttpResponse.h"
#include "HttpRequest.h"

class FviewHandler {
public:
	FviewHandler();
	virtual ~FviewHandler();
	static string handle(HttpRequest* req, HttpResponse& res, map<string, string> fviewmap);
};

#endif /* FVIEWHANDLER_H_ */
