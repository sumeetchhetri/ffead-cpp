/*
 * FormHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef FORMHANDLER_H_
#define FORMHANDLER_H_
#include "Reflector.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "Document.h"
#include <dlfcn.h>

typedef void* (*toVoidP) (string);

class FormHandler {
public:
	FormHandler();
	virtual ~FormHandler();
	static string handle(HttpRequest* req, HttpResponse& res, map<string, Element> formMap, void* dlib);
};

#endif /* FORMHANDLER_H_ */
