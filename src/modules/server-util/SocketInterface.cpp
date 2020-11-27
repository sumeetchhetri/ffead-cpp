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
 * SocketInterface.cpp
 *
 *  Created on: 03-Dec-2014
 *      Author: sumeetc
 */

#include "SocketInterface.h"

std::atomic<int> SocketInterface::openSocks = 0;

SocketInterface::SocketInterface() {
	eh = NULL;
	http2 = false;
	closed = false;
	ssl = NULL;
	io = NULL;
	reqPos = 0;
	current = 0;
	http2 = false;
	fd = -1;
	tid = -1;
	rdTsk = NULL;
	wrTsk = NULL;
	srvTsk = NULL;
	useCounter = 0;
}

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
	openSocks++;
	tid = -1;
	rdTsk = NULL;
	wrTsk = NULL;
	srvTsk = NULL;
	useCounter = 0;
}

void SocketInterface::use() {
	useCounter++;
}

void SocketInterface::unUse() {
	useCounter--;
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
			close(fd);
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
					close(fd);
					return false;
				}
				printf ("\t subject: %s\n", str);
				free (str);
				str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
				if(str == NULL)
				{
					logger << "Could not get client certificate issuer name" << std::endl;
					close(fd);
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
	openSocks--;
}

int SocketInterface::writeWsData(void* d) {
	return -1;
}

bool SocketInterface::isClosed() {
	return closed;
}

int SocketInterface::completeWrite() {
	//Timer to;
	//to.start();

	int done = 1;
	int reqPos = current + 1;

	//wm.lock();
	bool allRequestsDoneFl = false;
	while(!(allRequestsDoneFl = allRequestsDone())) {
		ResponseData& rd = wtl[reqPos];

		//Timer t;
		//t.start();

		done = writeTo(&rd);

		//t.end();
		//CommonUtils::tsResSockWrite += t.timerNanoSeconds();

		if(done == 0 || done == 1) {
			endRequest(reqPos);
			if(done==0) {
				break;
			}
		} else {
			//eh->registerWrite(this);
			break;
		}
	}
	if(allRequestsDoneFl) {
		if(!eh->unRegisterWrite(this)) {
			return 0;
		}
	}
	//wm.unlock();

	//to.end();
	//CommonUtils::tsResTotal += to.timerNanoSeconds();
	return done;
}

void SocketInterface::writeTo(const std::string& d, int reqPos) {
	//wm.lock();
	ResponseData& rd = wtl[reqPos];
	//wm.unlock();
	rd._b += d;
}

int SocketInterface::pushResponse(void* request, void* response, void* context, int reqPos) {
	//Timer to;
	//to.start();

	//wm.lock();
	ResponseData& rd = wtl[reqPos];
	//wm.unlock();

	writeResponse(request, response, context, rd._b, reqPos);
	//eh->registerWrite(this);

	//Timer t;
	//t.start();

	int done = writeTo(&rd);

	/*int state = 0;
	setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
	state = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));*/

	//t.end();
	//CommonUtils::tsResSockWrite += t.timerNanoSeconds();

	if(done >= 0) {
		endRequest(reqPos);
	} else if(done == -1) {
		eh->registerWrite(this);
	}

	//to.end();
	//CommonUtils::tsResTotal += to.timerNanoSeconds();

	return 1;
}

int SocketInterface::startRequest() {
	int rp = ++reqPos;
	//wm.lock();
	wtl[rp] = ResponseData();
	//wm.unlock();
	return rp;
}

int SocketInterface::endRequest(int reqPos) {
	//wm.lock();
	wtl.erase(reqPos);
	//wm.unlock();
	return ++current;
}

bool SocketInterface::allRequestsDone() {
	return current == reqPos;
}

bool SocketInterface::isCurrentRequest(int reqp) {
	return reqp == (current + 1);
}

int SocketInterface::writeDirect(const std::string& d, int off) {
	if(ssl==NULL) {
		int er = send(fd, &d[off] , d.length()-off, 0);
		switch(er) {
			case -1:
			case 0:
				if (er == -1 && errno == EAGAIN) {
					return -1;
				} else {
					closeSocket();
					return 0;
				}
			default:
				return er;
		}
	} else {
		if(handleRenegotiation()) {
			return 0;
		}
		int er  = BIO_write(io, &d[off] , d.length()-off);
		int ser = SSL_get_error(ssl, er);
		switch(ser) {
			case SSL_ERROR_WANT_WRITE:
				return -1;
			case SSL_ERROR_NONE:
				return er;
			default:
				closeSocket();
				return 0;
		}
	}
	return -1;
}

int SocketInterface::writeTo(ResponseData* d)
{
	if(ssl==NULL) {
		int er = 0;
		do {
			er = send(fd, &d->_b[d->oft] , d->_b.length()-d->oft, 0);
			switch(er) {
				case -1:
				case 0:
					if (er == -1 && errno == EAGAIN) {
						return -1;
					} else {
						closeSocket();
						return 0;
					}
				default:
					d->oft += er;
					if(d->oft==(int)d->_b.length()) {
						return 1;
					}
					break;
			}
		} while(er>0);
	} else {
		if(handleRenegotiation()) {
			return 0;
		}
		int er = 0;
		int ser = 0;
		do {
			er  = BIO_write(io, &d->_b[d->oft] , d->_b.length()-d->oft);
			ser = SSL_get_error(ssl, er);
			switch(ser) {
				case SSL_ERROR_WANT_WRITE:
					return -1;
				case SSL_ERROR_NONE:
					d->oft += er;
					if(d->oft==(int)d->_b.length()) {
						return 1;
					}
					break;
				default:
					closeSocket();
					return 0;
			}
		} while(er!=0);
	}
	return 1;
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
#elif defined(IS_SENDFILE) && !defined(OS_MINGW) && !defined(CYGWIN)
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
				closeSocket();
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

int SocketInterface::readFrom()
{
	if(ssl==NULL) {
		int er = 0;
		char b[8192];
		do {
			er = recv(fd, b, 8192, 0);
			switch(er) {
				case -1:
				case 0:
					if (er == -1 && errno == EAGAIN) {
						return -1;
					} else {
						closeSocket();
						return 0;
					}
				default:
					buffer.append(b, er);
					break;
			}
		} while(er>0);
	} else {
		int er = 0;
		int ser = 0;
		char b[8192];
		do {
			er  = BIO_read(io, b, 4096);
			ser = SSL_get_error(ssl, er);
			switch(ser) {
				case SSL_ERROR_WANT_READ:
					return -1;
				case SSL_ERROR_NONE:
					buffer.append(b, er);
					break;
				default:
					closeSocket();
					return 0;
			}
		} while(er>0);
	}
	return 1;
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
	#ifndef OS_MINGW
		int flags = fcntl(fd, F_GETFL, 0);
		return (flags&O_NONBLOCK)!=O_NONBLOCK;
	#else
		return false;
	#endif
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

ResponseData::ResponseData() {
	oft = 0;
}

ResponseData::~ResponseData() {
}
