/*
 * SocketInterface.cpp
 *
 *  Created on: 03-Dec-2014
 *      Author: sumeetc
 */

#include "SocketInterface.h"

SocketInterface::SocketInterface(const SOCKET& fd, SSL* ssl, BIO* io) {
	http2 = false;
	closed = false;
	this->ssl = ssl;
	this->io = io;
	eh = NULL;
	reqPos = 0;
	current = 0;
	address = StringUtil::toHEX((long long)this);
	this->fd = fd;
	http2 = SSLHandler::getAlpnProto(fd).find("h2")==0;
	wtl.insert(0, new ResponseData());
}

bool SocketInterface::init(const SOCKET& fd, SSL*& ssl, BIO*& io, Logger& logger) {
	if(SSLHandler::getInstance()->getIsSSL())
	{
		BIO* sbio = BIO_new_socket(fd, BIO_NOCLOSE);
		ssl = SSL_new(SSLHandler::getInstance()->getCtx());
		SSL_set_bio(ssl, sbio, sbio);

		io = BIO_new(BIO_f_buffer());
		BIO* ssl_bio = BIO_new(BIO_f_ssl());
		BIO_set_ssl(ssl_bio, ssl, BIO_CLOSE);
		BIO_push(io, ssl_bio);

		if(SSL_accept(ssl)<=0)
		{
			logger << "SSL accept error" << std::endl;
			closeSocket();
			return false;
		}
		if (SSLHandler::getInstance()->securityProperties.client_auth==2 || SSLHandler::getInstance()->securityProperties.client_auth==1)
		{
			X509* client_cert = NULL;
			/* Get the client's certificate (optional) */
			client_cert = SSL_get_peer_certificate(ssl);
			if (client_cert != NULL)
			{
				printf ("Client certificate:\n");
				char* str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
				if(str == NULL)
				{
					logger << "Could not get client certificate subject name" << std::endl;
					closeSocket();
					return false;
				}
				printf ("\t subject: %s\n", str);
				free (str);
				str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
				if(str == NULL)
				{
					logger << "Could not get client certificate issuer name" << std::endl;
					closeSocket();
					return false;
				}
				printf ("\t issuer: %s\n", str);
				free (str);
				X509_free(client_cert);
			}
			else
			{
				logger << ("The SSL client does not have certificate.\n") << std::endl;
			}
		}
		return SSLHandler::getAlpnProto(fd).find("h2")==0;
	}
	return false;
}

SocketInterface::~SocketInterface() {
	closeSocket();
	cuckoohash_map<int, ResponseData*>::locked_table lt = wtl.lock_table();
	cuckoohash_map<int, ResponseData*>::locked_table::iterator it;
	for(it=lt.begin();it!=lt.end();++it) {
		delete it->second;
	}
}

bool SocketInterface::completeWrite() {
	Timer t;
	t.start();

	bool done = false;
	int reqPos = current + 1;
	ResponseData* rd = wtl.find(reqPos);
	int ret = writeTo(rd);
	if(ret == 0 || ret == 1) {
		endRequest(reqPos);
		delete rd;
		done = true;
	} else {
		eh->registerWrite(this);
	}

	t.end();
	CommonUtils::tsWrite += t.timerNanoSeconds();
	return done;
}

void SocketInterface::writeTo(const std::string& d, int reqPos) {
	ResponseData* rd = wtl.find(reqPos);
	rd->_b += d;
}

bool SocketInterface::pushResponse(void* request, void* response, void* context, int reqPos) {
	Timer t;
	t.start();

	bool done = false;
	ResponseData* rd = wtl.find(reqPos);
	if(isCurrentRequest(reqPos)) {
		writeResponse(request, response, context, rd->_b, reqPos);
		rd->done = true;
		int ret = writeTo(rd);
		if(ret == 1) {
			endRequest(reqPos);
			delete rd;
			while(wtl.contains(++reqPos) && (rd = wtl.find(reqPos))!=NULL && rd->done) {
				ret = writeTo(rd);
				if(ret!=1) {
					break;
				}
				endRequest(reqPos);
				delete rd;
			}
		}
		if(ret == -1) {
			eh->registerWrite(this);
		} else if(ret == 0) {
			endRequest(reqPos);
			delete rd;
		}
	} else {
		writeResponse(request, response, context, rd->_b, reqPos);
		rd->done = true;
		done = false;
	}

	t.end();
	CommonUtils::tsWrite += t.timerNanoSeconds();

	return done;
}

int SocketInterface::startRequest() {
	int rp = ++reqPos;
	ResponseData* rd = new ResponseData();
	wtl.insert(rp, rd);
	return rp;
}

int SocketInterface::endRequest(int reqPos) {
	wtl.erase(reqPos);
	return ++current;
}

bool SocketInterface::allRequestsDone() {
	return current == reqPos;
}

bool SocketInterface::isCurrentRequest(int reqp) {
	return reqp == (current + 1);
}

int SocketInterface::writeTo(ResponseData* d)
{
	if(SSLHandler::getInstance()->getIsSSL())
	{
		if(handleRenegotiation())
		{
			return 0;
		}
		outer: while (!closed)
		{
			int er  = BIO_write(io, &d->_b[d->oft] , d->_b.length()-d->oft);
			int ser = SSL_get_error(ssl, er);
			switch(ser)
			{
				case SSL_ERROR_WANT_WRITE:
				{
					return -1;
				}
				case SSL_ERROR_NONE:
				{
					d->oft += er;
					if(d->oft==d->_b.length()) {
						return 1;
					}
					break;
				}
				default:
				{
					closeSocket();
					return 0;
				}
			}
		}
	}
	else
	{
		while (!closed)
		{
			int er = send(fd, &d->_b[d->oft] , d->_b.length()-d->oft, 0);
			if (er == -1 && errno == EAGAIN)
			{
				return -1;
			}
			else if(er<=0)
			{
				closeSocket();
				return 0;
			}
			else
			{
				d->oft += er;
				if(d->oft==d->_b.length()) {
					return 1;
				}
			}
		}
	}
	return closed==true;
}

bool SocketInterface::writeFile(int fdes, int remain_data)
{
	off_t offset = 0;
#if defined(OS_DARWIN)
	off_t sent_bytes1 = BUFSIZ;
	while ((sendfile(fdes, fd, offset, &sent_bytes1, NULL, 0) == 0) && sent_bytes1>0 && (remain_data > 0))
	{
			remain_data -= sent_bytes1;
	}
#elif defined(OS_BSD)
	off_t sent_bytes1 = BUFSIZ;
	while ((sendfile(fdes, fd, offset, BUFSIZ, 0, &sent_bytes1, 0) == 0) && sent_bytes1>0 && (remain_data > 0))
	{
			remain_data -= sent_bytes1;
	}
#elif defined(IS_SENDFILE)
	int sent_bytes = 0;
	while (((sent_bytes = sendfile(fd, fdes, &offset, BUFSIZ)) > 0) && (remain_data > 0))
	{
		remain_data -= sent_bytes;
	}
#else
	while (remain_data > 0) {
		char buffer[1024];
		int bytes_read = read(fdes, buffer, sizeof(buffer));
		if (bytes_read == 0)
			break;

		if (bytes_read < 0) {
			return false;
		}

		void *p = buffer;
		while (bytes_read > 0) {
			int bytes_written = write(fd, p, bytes_read);
			if (bytes_read == 0) {
				close();
				return false;
			}
			if (bytes_written < 0) {
				return false;
			}
			remain_data -= bytes_written;
			bytes_read -= bytes_written;
			p += bytes_written;
		}
	}
#endif
	return isClosed();
}

bool SocketInterface::readFrom()
{
	if(SSLHandler::getInstance()->getIsSSL())
	{
		while (!closed)
		{
			char b[4096];
			int er  = BIO_read(io, b, 4096);
			int ser = SSL_get_error(ssl, er);
			bool flag = false;
			switch(ser)
			{
				case SSL_ERROR_WANT_READ:
				{
					return false;
				}
				case SSL_ERROR_NONE:
				{
					buffer.append(b, er);
					break;
				}
				default:
				{
					closeSocket();
					return true;
				}
			}
		}
	}
	else
	{
		while (!closed)
		{
			char b[4096];
			int er = recv(fd, b, 4096, 0);
			if (er == -1 && errno == EAGAIN)
			{
				return false;
			}
			else if(er<=0)
			{
				closeSocket();
				return true;
			}
			else
			{
				buffer.append(b, er);
			}
		}
	}
	return closed;
}

int SocketInterface::getDescriptor() {
	return fd;
}
std::string SocketInterface::getAddress() {
	return address;
}

bool SocketInterface::flush() {
	bool fl = false;
	if(!closed && BIO_flush(io)<=0 && !BIO_should_retry(io))
	{
		logger.debug("Error flushing BIO");
		closeSocket();
		fl = true;
	}
	return fl;
}

void SocketInterface::closeSocket()
{
	if(!closed)
	{
		if(SSLHandler::getInstance()->getIsSSL())
		{
			SSLHandler::getInstance()->closeSSL(fd,ssl,io);
		}
		else
		{
			close(fd);
		}
	} else {
		return;
	}
	closed = true;
}

bool SocketInterface::checkSocketWaitForTimeout(const int& writing, const int& seconds, const int& micros)
{
	#ifdef OS_MINGW
		u_long iMode = 1;
		ioctlsocket(fd, FIONBIO, &iMode);
	#else
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
	#endif

	fd_set rset, wset;
	struct timeval tv = {seconds, micros};
	int rc = 0;

	/* Guard against closed socket */
	if (fd < 0)
	{
		return false;
	}

	/* Construct the arguments to select */
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	wset = rset;

	/* See if the socket is ready */
	switch (writing)
	{
		case 0:
			rc = select(fd+1, &rset, NULL, NULL, &tv);
			break;
		case 1:
			rc = select(fd+1, NULL, &wset, NULL, &tv);
			break;
		case 2:
			rc = select(fd+1, &rset, &wset, NULL, &tv);
			break;
	}
	FD_CLR(fd, &rset);
	#ifdef OS_MINGW
		u_long bMode = 0;
		ioctlsocket(fd, FIONBIO, &bMode);
	#else
		fcntl(fd, F_SETFL, O_SYNC);
	#endif

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	/* Return SOCKET_TIMED_OUT on timeout, SOCKET_OPERATION_OK
	otherwise
	(when we are able to write or when there's something to
	read) */
	return rc <= 0 ? false : true;
}

bool SocketInterface::isBlocking()
{
	int flags = fcntl(fd, F_GETFL, 0);
	return (flags&O_NONBLOCK)!=O_NONBLOCK;
}

bool SocketInterface::handleRenegotiation()
{
	if(SSLHandler::getInstance()->securityProperties.client_auth==CLIENT_AUTH_REHANDSHAKE)
	{
		SSL_set_verify(ssl,SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);

		/* Stop the client from just resuming the un-authenticated session */
		SSL_set_session_id_context(ssl, (const unsigned char*)&SSLHandler::s_server_auth_session_id_context,
				sizeof(SSLHandler::s_server_auth_session_id_context));

		if(SSL_renegotiate(ssl)<=0)
		{
			logger.debug("SSL renegotiation error");
			closeSocket();
			return true;
		}
		if(SSL_do_handshake(ssl)<=0)
		{
			logger.debug("SSL renegotiation error");
			closeSocket();
			return true;
		}
#if OPENSSL_VERSION_NUMBER < 0x10100000L
		ssl->state = SSL_ST_ACCEPT;
#else
		SSL_set_accept_state(ssl);
#endif
		if(SSL_do_handshake(ssl)<=0)
		{
			logger.debug("SSL handshake error");
			closeSocket();
			return true;
		}
	}
	return false;
}

std::string SocketInterface::getAlpnProto() {
	return SSLHandler::getAlpnProto(fd);
}

bool SocketInterface::isHttp2() {
	return http2;
}
