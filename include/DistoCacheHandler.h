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
 * DistoCacheHandler.h
 *
 *  Created on: Mar 26, 2010
 *      Author: sumeet
 */

#ifndef DISTOCACHEHANDLER_H_
#define DISTOCACHEHANDLER_H_
#include "string"
#include "NBServer.h"
#include "fstream"
#include "Thread.h"
#include "map"
#include "LoggerFactory.h"
#include "Constants.h"
#include "DistoCacheServiceHandler.h"
#include "PooledDistoCacheConnectionFactory.h"

#define BACKLOG1 500
#define MAXBUFLEN1 1024
using namespace std;
class DistoCacheHandler {
	static DistoCacheHandler* instance;
	Logger logger;
	static void* service(void* arg);
	NBServer server;
	bool running;
	static void init();
	DistoCacheHandler();
	virtual ~DistoCacheHandler();
public:
	static void trigger(string, int);
	static void stop();
};

#endif /* COMPONENTHANDLER_H_ */
