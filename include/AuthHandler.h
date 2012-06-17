/*
 * AuthHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef AUTHHANDLER_H_
#define AUTHHANDLER_H_
#include "FileAuthController.h"
#include "Reflector.h"

#include <dlfcn.h>

typedef ClassInfo (*FunPtr) ();

class AuthHandler {
public:
	AuthHandler();
	virtual ~AuthHandler();
	static string getFileExtension(const string& file);
	static bool handle(map<string, string> autMap, map<string, string> autpattMap, HttpRequest* req, HttpResponse& res, map<string, vector<string> > filterMap, void* dlib,
			string ext);
};

#endif /* AUTHHANDLER_H_ */
