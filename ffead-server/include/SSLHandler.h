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
 * SSLHandler.h
 *
 *  Created on: 20-Jun-2012
 *      Author: sumeetc
 */

#ifndef SSLHANDLER_H_
#define SSLHANDLER_H_
#include "SSLCommon.h"
#include "LoggerFactory.h"
#include "ConfigurationData.h"
#define CLIENT_AUTH_REQUEST 1
#define CLIENT_AUTH_REQUIRE 2
#define CLIENT_AUTH_REHANDSHAKE 3

using namespace std;

class SSLHandler {
	static char *pass;
	SSL_CTX *ctx;
	Logger logger;
	bool isSSLEnabled;
	static SSLHandler* instance;
	SSLHandler();
	void init();
public:
	bool getIsSSL() const;
	SSL_CTX* getCtx() const;
	static int s_server_session_id_context;
	static int s_server_auth_session_id_context;
	static void initInstance();
	static void clear();
	static void setIsSSL(bool isSSLEnabled);
	static SSLHandler* getInstance();
	virtual ~SSLHandler();
	static int password_cb(char *buf, int num, int rwflag, void *userdata);
	void closeSSL(int fd, SSL *ssl, BIO* bio);
};

#endif /* SSLHANDLER_H_ */
