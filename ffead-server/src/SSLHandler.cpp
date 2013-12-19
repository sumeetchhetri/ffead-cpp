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
 * SSLHandler.cpp
 *
 *  Created on: 20-Jun-2012
 *      Author: sumeetc
 */

#include "SSLHandler.h"

SSLHandler* SSLHandler::instance = NULL;
char* SSLHandler::pass = NULL;
int SSLHandler::s_server_session_id_context = 1;
int SSLHandler::s_server_auth_session_id_context = 2;

SSLHandler::SSLHandler() {
	logger = LoggerFactory::getLogger("SSLHandler");
	ctx = NULL;
}

SSLHandler::~SSLHandler() {
	// TODO Auto-generated destructor stub
}

void SSLHandler::initInstance() {
	getInstance()->init();
}

void SSLHandler::setIsSSL(bool isSSLEnabled) {
	getInstance()->isSSLEnabled = isSSLEnabled;
}

SSLHandler* SSLHandler::getInstance() {
	if(instance==NULL) {
		instance = new SSLHandler;
	}
	return instance;
}

bool SSLHandler::getIsSSL() const {
	return isSSLEnabled;
}

SSL_CTX* SSLHandler::getCtx() const {
	return ctx;
}

/*The password code is not thread safe*/
int SSLHandler::password_cb(char *buf,int num, int rwflag,void *userdata)
{
	if(num<(int)(strlen(pass)+1))
		return(0);

	strcpy(buf, pass);
	return(strlen(pass));
}

void SSLHandler::init() {
	if(isSSLEnabled)
	{
		string sslConfsettings = "Server setup with SSL enabled, CERTFILE = ";
		sslConfsettings.append(ConfigurationData::getInstance()->cert_file);
		sslConfsettings.append(", KEYFILE = ");
		sslConfsettings.append(ConfigurationData::getInstance()->key_file);
		sslConfsettings.append(", PASSWORD = ");
		sslConfsettings.append(ConfigurationData::getInstance()->sec_password);
		sslConfsettings.append(", DHFILE = ");
		sslConfsettings.append(ConfigurationData::getInstance()->dh_file);
		sslConfsettings.append(", CA_LIST = ");
		sslConfsettings.append(ConfigurationData::getInstance()->ca_list);
		sslConfsettings.append(", ISDH_PARAMS = ");
		sslConfsettings.append(CastUtil::lexical_cast<string>(ConfigurationData::getInstance()->isDHParams));
		sslConfsettings.append(", CLIENT_SEC_LEVEL = ");
		sslConfsettings.append(CastUtil::lexical_cast<string>(ConfigurationData::getInstance()->client_auth));
		logger << sslConfsettings << endl;

		/* Build our SSL context*/
		ctx = SSLCommon::initialize_ctx(logger, true);

		pass = (char*)ConfigurationData::getInstance()->sec_password.c_str();
		SSL_CTX_set_default_passwd_cb(ctx, SSLHandler::password_cb);

		SSLCommon::loadCerts(ctx, (char*)ConfigurationData::getInstance()->cert_file.c_str(),
					(char*)ConfigurationData::getInstance()->key_file.c_str(),
					ConfigurationData::getInstance()->ca_list, logger, true);

		if(ConfigurationData::getInstance()->isDHParams)
		{
			SSLCommon::load_dh_params(ctx,(char*)ConfigurationData::getInstance()->dh_file.c_str(), logger);
		}
		else
		{
			SSLCommon::load_ecdh_params(ctx, logger);
		}

		SSL_CTX_set_session_id_context(ctx,
				(const unsigned char*)&s_server_session_id_context,
				sizeof s_server_session_id_context);

		/* Set our cipher list */
		if(SSLCommon::ciphers){
			SSL_CTX_set_cipher_list(ctx,SSLCommon::ciphers);
		}

		if(ConfigurationData::getInstance()->client_auth==2)
		{
			/* Set to require peer (client) certificate verification */
			SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
			/* Set the verification depth to 1 */
            SSL_CTX_set_verify_depth(ctx,1);
		}
		else if(ConfigurationData::getInstance()->client_auth==1)
		{
			/* Set to require peer (client) certificate verification */
			SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
			/* Set the verification depth to 1 */
            SSL_CTX_set_verify_depth(ctx,1);
		}
		else
		{
			SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
		}
	}
}

void SSLHandler::clear() {
	if(instance!=NULL)
	{
		if(instance->ctx!=NULL)
		{
			SSL_CTX_free(instance->ctx);
		}
		if(pass!=NULL)
		{
			delete SSLHandler::pass;
		}
		delete instance;
	}
}

void SSLHandler::closeSSL(int fd, SSL *ssl, BIO* bio)
{
	SSLCommon::closeSSL(fd, ssl, bio);
}
