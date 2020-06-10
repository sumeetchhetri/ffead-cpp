/*
	Copyright 2009-2020, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (const the& "License"); 
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
 * ServerInitUtil.h
 *
 *  Created on: 28-Apr-2020
 *      Author: sumeetc
 */

#ifndef SRC_SERVER_SERVERINITUTIL_H_
#define SRC_SERVER_SERVERINITUTIL_H_

#include "dlfcn.h"
#include "sstream"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <queue>
#ifdef INC_COMP
#include "ComponentGen.h"
#include "ComponentHandler.h"
#endif
#ifdef INC_MSGH
#include "MessageHandler.h"
#endif
#ifdef INC_MI
#include "MethodInvoc.h"
#endif
#ifdef INC_COMP
#include "AppContext.h"
#endif
#include "Logger.h"
#include "ConfigurationHandler.h"
#include "ServiceTask.h"
#include "PropFileReader.h"
#include "XmlParseException.h"
#include "HttpClient.h"
#undef strtoul
#ifdef WINDOWS
#include <direct.h>
#define pwd _getcwd
#else
#include <unistd.h>
#define pwd getcwd
#endif

class ServerInitUtil {
	static Logger loggerIB;
public:
	static void bootstrap(std::string, Logger& logger, SERVER_BACKEND type);
	static void init(Logger& logger);

	static void bootstrapIB(std::string, SERVER_BACKEND type);
	static void initIB();

	static void cleanUp();
};

#endif /* SRC_SERVER_SERVERINITUTIL_H_ */
