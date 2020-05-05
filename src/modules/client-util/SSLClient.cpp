/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * SSLClient.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: sumeet
 */

#include "SSLClient.h"

SSLClient::SSLClient() {
	logger = LoggerFactory::getLogger("SSLClient");
	ca_list = "root.pem";
	isDHParams = true;
	client_auth = 0;
	connected = false;
	ssl = NULL;
	ctx = NULL;
	sbio = io = ssl_bio = NULL;
	sockfd = -1;
	connected = isDHParams = false;
}

SSLClient::SSLClient(const std::string& secFile) {
	isDHParams = true;
	client_auth = 0;
	connected = false;
	ssl = NULL;
	ctx = NULL;
	sbio = io = ssl_bio = NULL;
	sockfd = -1;
	connected = isDHParams = false;
	logger = LoggerFactory::getLogger("SSLClient");
	PropFileReader pread;
	propMap sslsec = pread.getProperties(secFile);
	if(sslsec.size()>0)
	{
		cert_file = sslsec["CERTFILE"];
		key_file = sslsec["KEYFILE"];
		dh_file = sslsec["DHFILE"];
		ca_list = sslsec["CA_LIST"];
		rand_file = sslsec["RANDOM"];
		sec_password = sslsec["PASSWORD"];
		std::string tempcl = sslsec["CLIENT_SEC_LEVEL"];
		if(tempcl!="")
		{
			try
			{
				client_auth = CastUtil::toInt(tempcl);
			}
			catch(const std::exception& e)
			{
				logger << "\nInvalid client auth level defined" << std::flush;
				client_auth = 0;
			}
		}
		isDHParams = true;
		try
		{
			isDHParams = CastUtil::toBool(sslsec["ISDH_PARAMS"]);
		}
		catch(const std::exception& e)
		{
			logger << "\nInvalid boolean value for isDHParams defined" << std::flush;
		}
	}
}

SSLClient::~SSLClient() {
}

char* SSLClient::pass;

int SSLClient::password_cb(char *buf, int num, int rwflag, void *userdata)
{
	if(num<(int)(strlen(pass)+1))
		return(0);

	strcpy(buf,pass);
	return(strlen(pass));
}

void SSLClient::init()
{
	/* Build our SSL context*/
	ctx = SSLCommon::initialize_ctx(false);

	bool loadAll = false;
	if(client_auth==1)
	{
		loadAll = true;
	}

	pass = (char*)sec_password.c_str();
	SSL_CTX_set_default_passwd_cb(ctx, SSLClient::password_cb);

	SSLCommon::loadCerts(ctx, (char*)cert_file.c_str(),
			(char*)key_file.c_str(),
			ca_list, loadAll);

	if(isDHParams)
	{
		SSLCommon::load_dh_params(ctx,(char*)dh_file.c_str());
	}
	else
	{
		SSLCommon::load_ecdh_params(ctx);
	}

	/* Set our cipher list */
	if(SSLCommon::ciphers!=""){
		SSL_CTX_set_cipher_list(ctx,SSLCommon::ciphers.c_str());
	}

	//SSL_CTX_set_verify (ctx, SSL_VERIFY_PEER, NULL);
}

void SSLClient::destroy_ctx(SSL_CTX *ctx)
{
	SSL_CTX_free(ctx);
}

bool SSLClient::connection(const std::string& host, const int& port)
{
	sockfd = create_tcp_socket();

	struct sockaddr_in *remote;
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;

	if(host!="localhost" && host!="0.0.0.0" && host!="127.0.0.1") {
		char* ip = get_ip((char*)host.c_str());
		fprintf(stderr, "IP is %s\n", ip);
		int tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
		if( tmpres < 0)
		{
			free(remote);
			perror("Can't set remote->sin_addr.s_addr");
			return false;
		}
		else if(tmpres == 0)
		{
			free(remote);
			fprintf(stderr, "%s is not a valid IP address\n", ip);
			return false;
		}
		remote->sin_addr.s_addr = inet_addr(ip);
		free(ip);
	} else {
		remote->sin_addr.s_addr = INADDR_ANY;
	}

	remote->sin_port = htons(port);

	if(connect(sockfd, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0 && (errno != EINPROGRESS)){
		perror("Could not connect");
		connected = false;
	} else {
		connected = true;
	}
	free(remote);

	connected = ClientInterface::isConnected(sockfd);

	/* Build our SSL context*/
	init();

	/* Connect the SSL socket */
	ssl=SSL_new(ctx);
	sbio=BIO_new_socket(sockfd,BIO_CLOSE);
	SSL_set_bio(ssl,sbio,sbio);
	io=BIO_new(BIO_f_buffer());
	ssl_bio=BIO_new(BIO_f_ssl());
	BIO_set_ssl(ssl_bio,ssl,BIO_NOCLOSE);
	BIO_push(io,ssl_bio);

	if(SSL_connect(ssl)<=0)
	{
		logger << "SSL connect error";
		return false;
	}
	ERR_clear_error();
	connected = true;
	return true;
}

bool SSLClient::connectionNB(const std::string& host, const int& port)
{
	sockfd = create_tcp_socket();

	struct sockaddr_in *remote;
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;

	fd_set fdset;
	struct timeval tv;

	if(host!="localhost" && host!="0.0.0.0" && host!="127.0.0.1") {
		char* ip = get_ip((char*)host.c_str());
		fprintf(stderr, "IP is %s\n", ip);
		int tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
		if( tmpres < 0)
		{
			free(remote);
			perror("Can't set remote->sin_addr.s_addr");
			return false;
		}
		else if(tmpres == 0)
		{
			free(remote);
			fprintf(stderr, "%s is not a valid IP address\n", ip);
			return false;
		}
		remote->sin_addr.s_addr = inet_addr(ip);
		free(ip);
	} else {
		remote->sin_addr.s_addr = INADDR_ANY;
	}

	remote->sin_port = htons(port);

	setSocketNonBlocking(sockfd);
	if(connect(sockfd, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0 && (errno != EINPROGRESS)){
		perror("Could not connect");
		connected = false;
	} else {
		connected = true;
	}
	free(remote);

	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	int rc = select(sockfd + 1, NULL, &fdset, NULL, &tv);
	if(rc==0) {
		connected = false;
	}
	setSocketBlocking(sockfd);

	/* Build our SSL context*/
	init();

	/* Connect the SSL socket */
	ssl=SSL_new(ctx);
	sbio=BIO_new_socket(sockfd,BIO_CLOSE);
	SSL_set_bio(ssl,sbio,sbio);
	io=BIO_new(BIO_f_buffer());
	ssl_bio=BIO_new(BIO_f_ssl());
	BIO_set_ssl(ssl_bio,ssl,BIO_NOCLOSE);
	BIO_push(io,ssl_bio);

	if(SSL_connect(ssl)<=0)
	{
		logger << "SSL connect error";
		return false;
	}
	ERR_clear_error();
	connected = true;
	return true;
}

/*bool SSLClient::connectionUnresolv(const std::string& host, const int& port)
{
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	memset(s, 0, sizeof(s));
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	std::string sport = CastUtil::fromNumber(port);
	if ((rv = getaddrinfo(host.c_str(), sport.c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return false;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			closesocket(sockfd);
			perror("client: connect");
			connected = false;
			continue;
		} else {
			connected = true;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return false;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	//printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	init();

	ssl=SSL_new(ctx);
	sbio=BIO_new_socket(sockfd,BIO_CLOSE);
	SSL_set_bio(ssl,sbio,sbio);
	io=BIO_new(BIO_f_buffer());
	ssl_bio=BIO_new(BIO_f_ssl());
	BIO_set_ssl(ssl_bio,ssl,BIO_NOCLOSE);
	BIO_push(io,ssl_bio);

	if(SSL_connect(ssl)<=0)
	{
		logger << "SSL connect error";
		return false;
	}
	ERR_clear_error();
	return connected;
}*/

int SSLClient::sendData(std::string data)
{
	ERR_clear_error();
	while(data.length()>0)
	{
		int bytes = SSL_write(ssl, data.c_str(), data.length());
		switch(SSL_get_error(ssl,bytes)){
		case SSL_ERROR_NONE:
			break;
		default:
			logger << "SSL write problem" ;
			return 0;
		}
		data = data.substr(bytes);
	}
	return 1;
}

std::string SSLClient::getData(const std::string& hdrdelm, const std::string& cntlnhdr)
{
	return getTextData(hdrdelm, cntlnhdr);
}

std::string SSLClient::getTextData(const std::string& hdrdelm, const std::string& cntlnhdr)
{
	int er=-1;
	bool flag = true;
	std::string alldat;
	int cntlen = 0;
	char buf[MAXBUFLE];
	memset(buf, 0, sizeof(buf));
	while(flag)
	{
		er = BIO_gets(io,buf,MAXBUFLE-1);
		switch(SSL_get_error(ssl,er))
		{
		case SSL_ERROR_NONE:
			break;
		case SSL_ERROR_ZERO_RETURN:
		{
			logger << "SSL - Connection closed\n";
			return alldat;
		}
		default:
		{
			logger << "SSL read problem";
			return alldat;
		}
		}
		if(!strcmp(buf,hdrdelm.c_str()))
		{
			std::string tt(buf, er);
			alldat += tt;
			break;
		}
		std::string temp(buf, er);
		temp = temp.substr(0,temp.length()-1);
		alldat += (temp + "\n");
		if(temp.find(cntlnhdr)!=std::string::npos)
		{
			std::string cntle = temp.substr(temp.find(": ")+2);
			cntle = cntle.substr(0,cntle.length()-1);
			try
			{
				cntlen = CastUtil::toInt(cntle);
			}
			catch(const std::exception& e)
			{
				logger << "bad lexical cast" <<std::endl;
			}
		}
		memset(&buf[0], 0, sizeof(buf));
	}
	while(cntlen>0)
	{
		//logger << "reading conetnt " << cntlen;
		int toRead = cntlen;
		if(cntlen>MAXBUFLE)
			toRead = MAXBUFLE - 1;
		er = BIO_read(io,buf,toRead);
		switch(SSL_get_error(ssl,er))
		{
		case SSL_ERROR_NONE:
			cntlen -= er;
			break;
		case SSL_ERROR_ZERO_RETURN:
		{
			logger << "SSL - Connection closed\n";
			return alldat;
		}
		default:
		{
			logger << "SSL read problem";
			return alldat;
		}
		}
		std::string temp(buf, er);
		alldat += temp;
		memset(&buf[0], 0, sizeof(buf));
	}
	return alldat;
}

std::string SSLClient::getData(int cntlen)
{
	std::string alldat;
	char buf[MAXBUFLE];
	memset(buf, 0, sizeof(buf));
	int er;
	while(cntlen>0)
	{
		//logger << "reading content " << cntlen;
		er = BIO_read(io,buf,cntlen);
		switch(SSL_get_error(ssl,er))
		{
		case SSL_ERROR_NONE:
			cntlen -= er;
			break;
		case SSL_ERROR_ZERO_RETURN:
		{
			logger << "SSL - Connection closed\n";
			return alldat;
		}
		default:
		{
			logger << "SSL read problem\n";
			return alldat;
		}
		}
		alldat += (buf);
		memset(&buf[0], 0, sizeof(buf));
	}
	return alldat;
}

std::string SSLClient::getBinaryData(const int& len, const bool& isLengthIncluded)
{
	//logger << len;
	std::string alldat = getData(len);

	int leng = getLengthCl(alldat, len);
	if(isLengthIncluded)
	{
		leng -= len;
	}
	//logger << "done reading header length " << leng;
	alldat = getData(leng);
	logger << alldat.length();
	return alldat;
}

void SSLClient::closeConnection()
{
	connected = false;
	destroy_ctx(ctx);
	SSLCommon::closeSSL(sockfd, ssl, io);
}

bool SSLClient::isConnected()
{
	return connected && ClientInterface::isConnected(sockfd);
}
