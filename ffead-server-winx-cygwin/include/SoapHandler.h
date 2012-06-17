/*
 * SoapHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef SOAPHANDLER_H_
#define SOAPHANDLER_H_
#include "Reflector.h"
#include <dlfcn.h>
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Document.h"
#include "Logger.h"
#include "XmlParser.h"

class SoapHandler {
public:
	SoapHandler();
	virtual ~SoapHandler();
	static void handle(HttpRequest* req, HttpResponse& res, void* dlib, string xmlcnttype);
};

#endif /* SOAPHANDLER_H_ */
