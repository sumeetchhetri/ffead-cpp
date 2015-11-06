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
vector<vector<unsigned char> > SSLHandler::advertisedProtos;

SSLHandler::SSLHandler() {
	logger = LoggerFactory::getLogger("SSLHandler");
	ctx = NULL;
	isValid = false;
	isSSLEnabled = false;
}

SSLHandler::~SSLHandler() {
	// TODO Auto-generated destructor stub
}

void SSLHandler::initInstance(const SecurityProperties& securityProperties) {
	getInstance()->init(securityProperties);
}

void SSLHandler::setIsSSL(const bool& isSSLEnabled) {
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
int SSLHandler::password_cb(char *buf, int num, int rwflag, void *userdata)
{
	if(num<(int)(strlen(pass)+1))
		return(0);

	strcpy(buf, pass);
	return(strlen(pass));
}

void SSLHandler::init(const SecurityProperties& securityProperties) {
	if(isSSLEnabled)
	{
		this->securityProperties = securityProperties;
		if(securityProperties.alpnEnabled && securityProperties.alpnProtoList.size()>0)
		{
			for(int pn=0;pn<(int)securityProperties.alpnProtoList.size();pn++)
			{
				string protoname = securityProperties.alpnProtoList.at(pn);
				vector<unsigned char> res = vector<unsigned char>(1 + protoname.length());
				unsigned char* p = res.data();
				*p++ = protoname.length();
				memcpy(p, protoname.c_str(), protoname.length());
				advertisedProtos.push_back(res);
			}
		}

		string sslConfsettings = "Server setup with SSL enabled, CERTFILE = ";
		sslConfsettings.append(securityProperties.cert_file);
		sslConfsettings.append(", KEYFILE = ");
		sslConfsettings.append(securityProperties.key_file);
		sslConfsettings.append(", PASSWORD = ");
		sslConfsettings.append(securityProperties.sec_password);
		sslConfsettings.append(", DHFILE = ");
		sslConfsettings.append(securityProperties.dh_file);
		sslConfsettings.append(", CA_LIST = ");
		sslConfsettings.append(securityProperties.ca_list);
		sslConfsettings.append(", ISDH_PARAMS = ");
		//sslConfsettings.append(CastUtil::lexical_cast<string>(securityProperties.isDHParams));
		//sslConfsettings.append(", CLIENT_SEC_LEVEL = ");
		//sslConfsettings.append(CastUtil::lexical_cast<string>(securityProperties.client_auth));
		logger << sslConfsettings << endl;

		/* Build our SSL context*/
		ctx = SSLCommon::initialize_ctx(true);

		pass = (char*)securityProperties.sec_password.c_str();
		SSL_CTX_set_default_passwd_cb(ctx, SSLHandler::password_cb);

		SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 |
							SSL_OP_NO_COMPRESSION | SSL_OP_SINGLE_DH_USE |
							SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION |
							SSL_OP_SINGLE_ECDH_USE | SSL_OP_NO_TICKET |
							SSL_OP_CIPHER_SERVER_PREFERENCE);
		SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
		SSL_CTX_set_mode(ctx, SSL_MODE_RELEASE_BUFFERS);

		if(securityProperties.isDHParams)
		{
			SSLCommon::load_dh_params(ctx,(char*)securityProperties.dh_file.c_str());
		}
		else
		{
			SSLCommon::load_ecdh_params(ctx);
		}

		const unsigned char sid_ctx[] = "Ffead";
		SSL_CTX_set_session_id_context(ctx, sid_ctx, sizeof(sid_ctx) - 1);
		SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_SERVER);
		/*SSL_CTX_set_session_id_context(ctx,
				(const unsigned char*)&s_server_session_id_context,
				sizeof s_server_session_id_context);
		 */

		/* Set our cipher list */
		if(SSLCommon::ciphers!=""){
			SSL_CTX_set_cipher_list(ctx, SSLCommon::ciphers.c_str());
		}

		SSLCommon::loadCerts(ctx, (char*)securityProperties.cert_file.c_str(),
				(char*)securityProperties.key_file.c_str(),
				securityProperties.ca_list, true);


		if(securityProperties.client_auth==2)
		{
			/* Set to require peer (client) certificate verification */
			SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_CLIENT_ONCE|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
			/* Set the verification depth to 1 */
			#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
            	SSL_CTX_set_verify_depth(ctx,1);
			#endif
		}
		else if(securityProperties.client_auth==1)
		{
			/* Set to require peer (client) certificate verification */
			SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
			/* Set the verification depth to 1 */
			#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
            	SSL_CTX_set_verify_depth(ctx,1);
			#endif
		}
		else
		{
			SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
		}

#if OPENSSL_VERSION_NUMBER >= 0x10002000L
		if(securityProperties.alpnEnabled)
		{
			for (int var = 0; var < (int)advertisedProtos.size(); ++var) {
				SSL_CTX_set_next_protos_advertised_cb(ctx, SSLHandler::next_proto_cb, &(advertisedProtos.at(var)));
			}


			// ALPN selection callback
			SSL_CTX_set_alpn_select_cb(ctx, SSLHandler::alpn_select_proto_cb, NULL);
		}
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L
	}
}

#if OPENSSL_VERSION_NUMBER >= 0x10002000L
int SSLHandler::alpn_select_proto_cb(SSL *ssl, const unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void *arg) {
	instance->logger << "[ALPN] client offers:" << std::endl;
	for (unsigned int i = 0; i < inlen; i += in [i] + 1) {
		instance->logger << " * ";
		instance->logger << (string(reinterpret_cast<const char *>(&in[i + 1]), in[i]));
		instance->logger << std::endl;
	}
	if (select_next_protocol(const_cast<unsigned char **>(out), outlen, in, inlen) <= 0) {
		return SSL_TLSEXT_ERR_NOACK;
	}
	int sockfd;
	BIO_get_fd(ssl->rbio, &sockfd);
	instance->socketAlpnProtoMap[sockfd] = string((char*)*out, (int)(*outlen));
	return SSL_TLSEXT_ERR_OK;
}
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L

string SSLHandler::getAlpnProto(const int& fd) {
	string retval;
	instance->lock.lock();
	if(instance->socketAlpnProtoMap.find(fd)!=instance->socketAlpnProtoMap.end()) {
		retval = instance->socketAlpnProtoMap[fd];
	}
	instance->lock.unlock();
	return retval;
}

void SSLHandler::removeAlpnProtoSocket(const int& fd) {
	instance->lock.lock();
	if(instance->socketAlpnProtoMap.find(fd)!=instance->socketAlpnProtoMap.end()) {
		instance->socketAlpnProtoMap.erase(fd);
	}
	instance->lock.unlock();
}

int SSLHandler::select_next_protocol(unsigned char **out, unsigned char *outlen, const unsigned char *in, const unsigned int& inlen) {
	int http_selected = 0;
	unsigned int i = 0;
	for (; i < inlen; i += in [i] + 1) {
		for (int var = 0; var < (int)advertisedProtos.size(); ++var) {
			unsigned char* p = advertisedProtos.at(var).data();
			if (in[i] == p[0] && i + 1 + in[i] <= inlen &&
					memcmp(&in[i + 1], &p[1], in[i]) == 0) {
				*out = (unsigned char *)&in[i + 1];
				*outlen = in[i];
				return 1;
			}
		}
		/*if (in[i] == HTTP2_PROTO_VERSION_ID_LEN && i + 1 + in[i] <= inlen &&
			memcmp(&in[i + 1], HTTP2_PROTO_VERSION_ID, in[i]) == 0) {
			 *out = (unsigned char *)&in[i + 1];
			 *outlen = in[i];
		  return 1;
		}*/
		if (in[i] == 8 && i + 1 + in[i] <= inlen &&
				memcmp(&in[i + 1], "http/1.1", in[i]) == 0) {
			http_selected = 1;
			*out = (unsigned char *)&in[i + 1];
			*outlen = in[i];
			/* Go through to the next iteration, because "HTTP/2" may be there */
		}
	}
	if (http_selected) {
		return 0;
	} else {
		return -1;
	}
}

int SSLHandler::next_proto_cb(SSL *s, const unsigned char **data, unsigned int *len, void *arg) {
	vector<unsigned char>* next_proto = static_cast<vector<unsigned char> *>(arg);
	*data = next_proto->data();
	*len = next_proto->size();
	return SSL_TLSEXT_ERR_OK;
}

vector<unsigned char> SSLHandler::getDefaultALPN() {
	vector<unsigned char> res;
	if(advertisedProtos.size()>0) {
		res = advertisedProtos.at(0);
	}
	return res;
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

void SSLHandler::closeSSL(const int& fd, SSL *ssl, BIO* bio)
{
	removeAlpnProtoSocket(fd);
	if(instance->isValid)
	{
		SSLCommon::closeSSL(fd, ssl, bio);
	}
}
