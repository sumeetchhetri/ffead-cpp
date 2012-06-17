/*
 * FilterHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef FILTERHANDLER_H_
#define FILTERHANDLER_H_
#include "Filter.h"
#include "Reflector.h"
#include <dlfcn.h>

typedef ClassInfo (*FunPtr) ();

class FilterHandler {
public:
	FilterHandler();
	virtual ~FilterHandler();
	static void handleIn(HttpRequest* req, HttpResponse& res, map<string, vector<string> > filterMap, void* dlib,
			string ext);
	static void handleOut(HttpRequest* req, HttpResponse& res, map<string, vector<string> > filterMap, void* dlib,
			string ext);
};

#endif /* FILTERHANDLER_H_ */
