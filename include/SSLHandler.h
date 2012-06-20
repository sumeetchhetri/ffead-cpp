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
#define CLIENT_AUTH_REQUEST 1
#define CLIENT_AUTH_REQUIRE 2
#define CLIENT_AUTH_REHANDSHAKE 3

using namespace std;

class SSLHandler {
	static char *pass;
	static BIO *bio_err;
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
