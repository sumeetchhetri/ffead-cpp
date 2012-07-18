/*
 * SSLClient.h
 *
 *  Created on: Dec 14, 2010
 *      Author: sumeet
 */

#ifndef SSLCLIENT_H_
#define SSLCLIENT_H_
#include "iostream"
#include "sstream"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
 #include <openssl/err.h>
#include <boost/thread/thread.hpp>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "string"
#include "boost/lexical_cast.hpp"
#include "cstring"
#include "openssl/ssl.h"
#include <signal.h>
#include "Logger.h"
using namespace std;
#define KEYFILE "server.pem"
#define PASSWORD "password"
#define DHFILE "dh1024.pem"
#define CA_LIST "root.pem"
#define MAXBUFLE 32768
class SSLClient {
	Logger logger;
	SSL *ssl;
	SSL_CTX *ctx;
	BIO *sbio,*io,*ssl_bio;
	int sockfd;
	bool connected;
	void *get_in_addr1(struct sockaddr *sa);
	SSL_CTX *initialize_ctx(char *keyfile,char *password);
	void destroy_ctx(SSL_CTX *ctx);
	void error_occurred(char *error);
	void closeSSL();
public:
	SSLClient();
	virtual ~SSLClient();
	bool connection(string,int);
	int sendData(string);
	string getData(string hdrdelm,string cntlnhdr);
	string getData(int cntlen);
	void closeConnection();
	bool isConnected();
};

#endif /* SSLCLIENT_H_ */
