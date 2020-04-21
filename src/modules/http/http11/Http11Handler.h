/*
 * Http11Handler.h
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#ifndef HTTP11HANDLER_H_
#define HTTP11HANDLER_H_
#include "SocketInterface.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class Http11Handler : public SocketInterface {
	bool isHeadersDone;
	int bytesToRead;
	bool isTeRequest;
	std::vector<HttpRequest*> requests;

	std::string webpath;
	int chunkSize;
	int connKeepAlive;
	int maxReqHdrCnt;
	int maxEntitySize;
	SocketInterface* handler;
	friend class CHServer;
	friend class HttpServiceHandler;
	friend class HttpServiceTask;
public:
	int readFrom();
	void addHandler(SocketInterface* handler);
	void onOpen();
	void onClose();
	std::string getProtocol(void* context);
	int getType(void* context);
	int getTimeout();
	HttpRequest* getAvailableRequest();
	bool readRequest(void* request, void*& context, int& pending, int& reqPos);
	bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos);
	Http11Handler(const SOCKET& fd, SSL* ssl, BIO* io, const std::string& webpath, const int& chunkSize,
			const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize);
	virtual ~Http11Handler();
};

#endif /* HTTP11HANDLER_H_ */
