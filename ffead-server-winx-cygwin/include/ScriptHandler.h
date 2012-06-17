/*
 * ScriptHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef SCRIPTHANDLER_H_
#define SCRIPTHANDLER_H_
#include "AfcUtil.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <sys/wait.h>
#include "Timer.h"
#include <unistd.h>

class ScriptHandler {
public:
	ScriptHandler();
	static bool handle(HttpRequest* req, HttpResponse& res, map<string, string> handoffs, void* dlib,
			string ext, map<string, string> props);
	static int pcloseRWE(int pid, int *rwepipe);
	static int popenRWE(int *rwepipe, const char *exe, const char *const argv[],string tmpf);
	virtual ~ScriptHandler();
};

#endif /* SCRIPTHANDLER_H_ */
