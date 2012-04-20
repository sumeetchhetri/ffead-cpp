/*
 * SSLClient.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: sumeet
 */

#include "SSLClient.h"

SSLClient::SSLClient() {
	// TODO Auto-generated constructor stub

}

SSLClient::~SSLClient() {
	// TODO Auto-generated destructor stub
}
static char *pass;
static BIO *bio_err=0;
// get sockaddr, IPv4 or IPv6:
void *SSLClient::get_in_addr1(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
static void sigpipe_handle(int x){
}

static int password_cb(char *buf,int num,
  int rwflag,void *userdata)
  {
    if(num<strlen(pass)+1)
      return(0);

    strcpy(buf,pass);
    return(strlen(pass));
  }

SSL_CTX *SSLClient::initialize_ctx(char *keyfile,char *password)
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
    	cout << "Can't read certificate file" << flush;

    pass=password;
    SSL_CTX_set_default_passwd_cb(ctx,
      password_cb);
    if(!(SSL_CTX_use_PrivateKey_file(ctx,
      keyfile,SSL_FILETYPE_PEM)))
    	cout << "Can't read key file" << flush;

    /* Load the CAs we trust*/
    if(!(SSL_CTX_load_verify_locations(ctx,
    		CA_LIST,0)))
    	cout << "Can't read CA list" << flush;
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(ctx,1);
#endif

    return ctx;
}

void SSLClient::destroy_ctx(SSL_CTX *ctx)
{
	SSL_CTX_free(ctx);
}

void SSLClient::closeSSL()
{
	int r=SSL_shutdown(ssl);
	BIO_free_all(io);
	close(sockfd);
	if(!r){
	  /*If we called SSL_shutdown() first then
	  we always get return value of '0'. In
	  this case, try again, but first send a
	  TCP FIN to trigger the other side's
	  close_notify*/
	  shutdown(sockfd,1);
	  r=SSL_shutdown(ssl);
	}
	switch(r){
	  case 1:
		break; /* Success */
	  case 0:
	  case -1:
	  default:
		  cout << "shutdown failed" << flush;
	}
	SSL_free(ssl);
}
void SSLClient::error_occurred(char *error)
{
	cout << error << flush;
}

bool SSLClient::connection(string host,int port)
{
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    string sport = boost::lexical_cast<string>(port);
    if ((rv = getaddrinfo(host.c_str(), sport.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("SSLClient: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("SSLClient: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
    	connected = false;
        fprintf(stderr, "SSLClient: failed to connect\n");
        return false;
    }

    inet_ntop(p->ai_family, get_in_addr1((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    cout << "SSLClient connecting to " << host << ":" << port << endl;

    freeaddrinfo(servinfo); // all done with this structure



    /* Build our SSL context*/
    ctx=initialize_ctx(KEYFILE,PASSWORD);

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
		error_occurred("SSL connect error");
		closeSSL();
	}
	ERR_clear_error();
	//fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    connected = true;
    //fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return true;
}

int SSLClient::sendData(string data)
{
	ERR_clear_error();
	int bytes = SSL_write(ssl,data.c_str(),data.length());
	switch(SSL_get_error(ssl,bytes)){
	  case SSL_ERROR_NONE:
		if(data.length()!=bytes)
			error_occurred("Incomplete write!");
		break;
		default:
			error_occurred("SSL write problem");
	}
	return bytes;
}

string SSLClient::getData(string hdrdelm,string cntlnhdr)
{
	int er=-1;
	bool flag = true;
	string alldat;
	int cntlen;
	char buf[MAXBUFLE];
	while(flag)
	{
		er = BIO_gets(io,buf,MAXBUFLE-1);
		switch(SSL_get_error(ssl,er))
		{
			case SSL_ERROR_NONE:
				break;
			case SSL_ERROR_ZERO_RETURN:
			{
				error_occurred("SSL error problem");
				if(io!=NULL)BIO_free_all(io);
				return "";
			}
			default:
			{
				error_occurred("SSL read problem");
				if(io!=NULL)BIO_free_all(io);
				return "";
			}
		}
		if(!strcmp(buf,hdrdelm.c_str()))
		{
			alldat += (buf);
			break;
		}
		string temp(buf);
		//cout << temp << endl;
		temp = temp.substr(0,temp.length()-1);
		alldat += (temp + "\n");
		if(temp.find(cntlnhdr)!=string::npos)
		{
			std::string cntle = temp.substr(temp.find(": ")+2);
			cntle = cntle.substr(0,cntle.length()-1);
			try
			{
				cntlen = boost::lexical_cast<int>(cntle);
			}
			catch(boost::bad_lexical_cast&)
			{
				cout << "bad lexical cast" <<endl;
			}
		}
		memset(&buf[0], 0, sizeof(buf));
	}
	while(cntlen>0)
	{
		cout << "reading conetnt " << cntlen << endl;
		er = BIO_read(io,buf,cntlen);
		switch(SSL_get_error(ssl,er))
		{
			case SSL_ERROR_NONE:
				cntlen -= er;
				break;
			case SSL_ERROR_ZERO_RETURN:
			{
				error_occurred("SSL error problem");
				if(io!=NULL)BIO_free_all(io);
				return "";
			}
			default:
			{
				error_occurred("SSL read problem");
				if(io!=NULL)BIO_free_all(io);
				return "";
			}
		}
		alldat += (buf);
		//cout << buf << endl;
		memset(&buf[0], 0, sizeof(buf));
	}
	return alldat;
}

string SSLClient::getData(int cntlen)
{
	string alldat;
	char buf[MAXBUFLE];
	int er;
	while(cntlen>0)
	{
		cout << "reading conetnt " << cntlen << endl;
		er = BIO_read(io,buf,cntlen);
		switch(SSL_get_error(ssl,er))
		{
			case SSL_ERROR_NONE:
				cntlen -= er;
				break;
			case SSL_ERROR_ZERO_RETURN:
			{
				error_occurred("SSL error problem");
				if(io!=NULL)BIO_free_all(io);
				return "";
			}
			default:
			{
				error_occurred("SSL read problem");
				if(io!=NULL)BIO_free_all(io);
				return "";
			}
		}
		alldat += (buf);
		//cout << buf << endl;
		memset(&buf[0], 0, sizeof(buf));
	}
	return alldat;
}

void SSLClient::closeConnection()
{
	connected = false;
	destroy_ctx(ctx);
	closeSSL();
}

bool SSLClient::isConnected()
{
	int numbytes;
	char buf[1];
	if ((numbytes = recv(sockfd, buf, 1, MSG_PEEK)) == 0)
	{
		return false;
	}
	return connected;
}
