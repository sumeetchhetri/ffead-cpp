/*
 * ExtHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef EXTHANDLER_H_
#define EXTHANDLER_H_
#include "AfcUtil.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TemplateHandler.h"
#include "DynamicView.h"
#include "Reflector.h"
#include "Document.h"
#include "View.h"
#include <dlfcn.h>

typedef string (*DCPPtr) ();
typedef ClassInfo (*FunPtr) ();

class ExtHandler {
public:
	ExtHandler();
	virtual ~ExtHandler();
	static string getContentStr(string url,string locale,string ext);
	static bool handle(HttpRequest* req, HttpResponse& res, void* dlib, string resourcePath,
			map<string, string> tmplMap, map<string, string> vwMap,string ext, map<string, string> props);
};

#endif /* EXTHANDLER_H_ */
