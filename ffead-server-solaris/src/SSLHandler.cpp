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
 * SSLHandler.cpp
 *
 *  Created on: 20-Jun-2012
 *      Author: sumeetc
 */

#include "SSLHandler.h"

SSLHandler::SSLHandler() {
	logger = Logger::getLogger("SSLHandler");
}

SSLHandler::~SSLHandler() {
	// TODO Auto-generated destructor stub
}

char* SSLHandler::pass = NULL;
BIO* SSLHandler::bio_err = NULL;

int SSLHandler::s_server_session_id_context = 1;
int SSLHandler::s_server_auth_session_id_context = 2;
Logger SSLHandler::logger;

/*The password code is not thread safe*/
int SSLHandler::password_cb(char *buf,int num, int rwflag,void *userdata)
  {
    if(num<(int)(strlen(pass)+1))
      return(0);

    strcpy(buf,pass);
    return(strlen(pass));
  }

void SSLHandler::load_dh_params(SSL_CTX *ctx,char *file)
{
    DH *ret=0;
    BIO *bio;

    if ((bio=BIO_new_file(file,"r")) == NULL)
    	logger << "Couldn't open DH file" << flush;

    ret=PEM_read_bio_DHparams(bio,NULL,NULL,
      NULL);
    BIO_free(bio);
    if(SSL_CTX_set_tmp_dh(ctx,ret)<0)
    	logger << "Couldn't set DH parameters" << flush;
  }

void SSLHandler::sigpipe_handle(int x){
}

SSL_CTX *SSLHandler::initialize_ctx(char *keyfile,char *password, string ca_list)
  {
    SSL_METHOD *meth;
    SSL_CTX *ctx;

    if(!bio_err){
      /* Global system initialization*/
      SSL_library_init();
      SSL_load_error_strings();

      /* An error write context */
      bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler */
    signal(SIGPIPE,sigpipe_handle);

    /* Create our context*/
    meth=(SSL_METHOD*)SSLv23_method();
    ctx=SSL_CTX_new(meth);

    /* Load our keys and certificates*/
    if(!(SSL_CTX_use_certificate_chain_file(ctx,
      keyfile)))
    	logger << "Can't read certificate file" << flush;

    pass=password;
    SSL_CTX_set_default_passwd_cb(ctx,
      password_cb);
    if(!(SSL_CTX_use_PrivateKey_file(ctx,
      keyfile,SSL_FILETYPE_PEM)))
    	logger << "Can't read key file" << flush;

    /* Load the CAs we trust*/
    if(!(SSL_CTX_load_verify_locations(ctx,
      ca_list.c_str(),0)))
    	logger << "Can't read CA list" << flush;
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(ctx,1);
#endif

    return ctx;
  }

void SSLHandler::destroy_ctx(SSL_CTX *ctx)
  {
    SSL_CTX_free(ctx);
  }

void SSLHandler::error_occurred(char *error,int fd,SSL *ssl)
{
	logger << error << flush;
	close(fd);
	int r=SSL_shutdown(ssl);
	if(!r){
	  /* If we called SSL_shutdown() first then
		 we always get return value of '0'. In
		 this case, try again, but first send a
		 TCP FIN to trigger the other side's
		 close_notify*/
	  shutdown(fd,1);
	  r=SSL_shutdown(ssl);
	}
	switch(r){
	  case 1:
		break; /* Success */
	  case 0:
	  case -1:
	  default:
		  logger << "shutdown failed" << flush;
		  break;
	}
	SSL_free(ssl);
}

void SSLHandler::closeSSL(int fd,SSL *ssl,BIO* bio)
{
	BIO_free(bio);
	int r=SSL_shutdown(ssl);
	if(!r){
	  /* If we called SSL_shutdown() first then
		 we always get return value of '0'. In
		 this case, try again, but first send a
		 TCP FIN to trigger the other side's
		 close_notify*/
	  shutdown(fd,1);
	  r=SSL_shutdown(ssl);
	}
	switch(r){
	  case 1:
		break; /* Success */
	  case 0:
	  case -1:
	  default:
		  logger << "shutdown failed" << flush;
		  break;
	}
	SSL_free(ssl);
}
