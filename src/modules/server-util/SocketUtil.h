/*
 * SocketUtil.h
 *
 *  Created on: 03-Dec-2014
 *      Author: sumeetc
 */

#ifndef SOCKETUTIL_H_
#define SOCKETUTIL_H_
#include "Compatibility.h"
#include "vector"
/*HTTPS related*/
#include <unistd.h>
#include <sys/types.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "string"
#include "Mutex.h"
#include <fcntl.h>
#include "SSLHandler.h"
#include "SelEpolKqEvPrt.h"

#define MAXBUFLENM 32768
#define BUFSIZZ 1024

class SocketUtil {
	SSL *ssl;
	BIO *sbio;
	BIO *io, *ssl_bio;
	SOCKET fd;
	std::atomic<bool> closed;
	SelEpolKqEvPrt *sel;
	bool inited, http2;
	Logger logger;
	SocketUtil();
	bool isBlocking();
	bool handleSSlErrors(const int& er);
	bool handleRenegotiation();
	void init(const SOCKET& fd);
	friend class Http2Handler;
	friend class Http11Handler;
	friend class SocketInterface;
	friend class Http11WebSocketHandler;
	friend class RequestReaderHandler;
public:
	std::string getAlpnProto();
	bool isHttp2();
	SocketUtil(const SOCKET& fd);
	virtual ~SocketUtil();
	int writeData(const std::string& data, const bool& flush, const int& offset= 0);
	bool flush(const bool& lk= true);
	void closeSocket(const bool& lk= true);
	int readLine(std::string& line);
	int readData(int cntlen, std::string& content);
	int readData(int cntlen, std::vector<unsigned char>& content);
	bool checkSocketWaitForTimeout(const int& writing, const int& seconds, const int& micros= 0);
};

#endif /* SOCKETUTIL_H_ */
