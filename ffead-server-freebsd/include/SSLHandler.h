/*
	Copyright 2010, Sumeet Chhetri

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
#include <string>
#include <sys/socket.h>
#include <iostream>
/*HTTPS related*/
#include <openssl/ssl.h>
#include <signal.h>
#include "Logger.h"
#define CLIENT_AUTH_REQUEST 1
#define CLIENT_AUTH_REQUIRE 2
#define CLIENT_AUTH_REHANDSHAKE 3

using namespace std;

class SSLHandler {
	static char *pass;
	static BIO *bio_err;
	static Logger logger;
public:
	static int s_server_session_id_context;
	static int s_server_auth_session_id_context;
	SSLHandler();
	virtual ~SSLHandler();
	static int password_cb(char *buf,int num, int rwflag,void *userdata);
	void load_dh_params(SSL_CTX *ctx,char *file);
	static void sigpipe_handle(int x);
	SSL_CTX *initialize_ctx(char *keyfile,char *password, string ca_list);
	void destroy_ctx(SSL_CTX *ctx);
	void error_occurred(char *error,int fd,SSL *ssl);
	void closeSSL(int fd,SSL *ssl,BIO* bio);
};

#endif /* SSLHANDLER_H_ */
