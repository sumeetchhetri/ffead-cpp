/*
	Copyright 2009-2013, Sumeet Chhetri

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
 * DistoCacheServiceHandler.h
 *
 *  Created on: 27-Mar-2013
 *      Author: sumeetc
 */

#ifndef DITOCACHESERVICEHANDLER_H_
#define DITOCACHESERVICEHANDLER_H_
#include "LoggerFactory.h"
#include "SSLHandler.h"
#include "AMEFDecoder.h"
#include "AMEFEncoder.h"
#include "CacheMap.h"
#define MAXBUFLENM 32768

class DistoCacheServiceHandler {
	int fd;
	bool isSSLEnabled;
	SSL_CTX *ctx;
	Logger logger;
	int getLength(string header,int size);
	bool validQuery(vector<string> parts, int size, string cmd1="", string cmd2="");
public:
	DistoCacheServiceHandler(int fd, bool isSSLEnabled, SSL_CTX *ctx);
	DistoCacheServiceHandler(int fd);
	virtual ~DistoCacheServiceHandler();
	void run();
};

#endif /* DITOCACHESERVICEHANDLER_H_ */
