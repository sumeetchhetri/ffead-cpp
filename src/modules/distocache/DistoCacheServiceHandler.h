/*
	Copyright 2009-2013, Sumeet Chhetri

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
 * DistoCacheServiceHandler.h
 *
 *  Created on: 27-Mar-2013
 *      Author: sumeetc
 */

#ifndef DITOCACHESERVICEHANDLER_H_
#define DITOCACHESERVICEHANDLER_H_
#include "LoggerFactory.h"
#ifdef HAVE_SSLINC
#include "SSLHandler.h"
#endif
#include "AMEFDecoder.h"
#include "AMEFEncoder.h"
#include "CacheMap.h"
#define MAXBUFLENM 32768

class DistoCacheServiceHandler {
	int fd;
	bool isSSLEnabled;
#ifdef HAVE_SSLINC
	SSL_CTX *ctx;
#endif
	Logger logger;
	int getLength(const std::string& header, const int& size);
	bool validQuery(const std::vector<std::string>& parts, const int& size, const std::string& cmd1="", const std::string& cmd2="");
public:
#ifdef HAVE_SSLINC
	DistoCacheServiceHandler(const int& fd, const bool& isSSLEnabled, SSL_CTX *ctx);
#endif
	DistoCacheServiceHandler(const int& fd);
	virtual ~DistoCacheServiceHandler();
	void run();
};

#endif /* DITOCACHESERVICEHANDLER_H_ */
