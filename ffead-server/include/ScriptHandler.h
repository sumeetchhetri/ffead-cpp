/*
	Copyright 2009-2012, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
/*
 * ScriptHandler.h
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#ifndef SCRIPTHANDLER_H_
#define SCRIPTHANDLER_H_
#include <stdlib.h>
#include "AfcUtil.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "Timer.h"
#include <unistd.h>
#include "Logger.h"
#include "Constants.h"

class ScriptHandler {
	static Logger logger;
	static int pcloseRWE(int pid, int *rwepipe);
	static int popenRWE(int *rwepipe, const char *exe, const char *const argv[],string tmpf);
	static int popenRWEN(int *rwepipe, const char *exe, const char** argv);
public:
	ScriptHandler();
	static string execute(string exe, bool retErrs);
	static string chdirExecute(string exe, string tmpf, bool retErrs);
	static bool handle(HttpRequest* req, HttpResponse& res, map<string, string> handoffs, void* dlib,
			string ext, map<string, string> props);
	virtual ~ScriptHandler();
};

#endif /* SCRIPTHANDLER_H_ */
