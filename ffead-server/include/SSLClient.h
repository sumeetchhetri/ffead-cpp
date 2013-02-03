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
 * SSLClient.h
 *
 *  Created on: Dec 14, 2010
 *      Author: sumeet
 */

#ifndef SSLCLIENT_H_
#define SSLCLIENT_H_
#include "ClientInterface.h"
using namespace std;
#define KEYFILE "client.pem"
#define PASSWORD "password"
#define DHFILE "dh1024.pem"
#define CA_LIST "root.pem"

class SSLClient : public ClientInterface {
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
	Logger logger;
public:
	SSLClient();
	virtual ~SSLClient();
	bool connection(string,int);
	bool connectionUnresolv(string host,int port);
	int sendData(string);
	string getData(string hdrdelm,string cntlnhdr);
	string getData(int cntlen);
	void closeConnection();
	bool isConnected();
	string getBinaryData(int,bool);
	string getTextData(string hdrdelm,string cntlnhdr);
};

#endif /* SSLCLIENT_H_ */
