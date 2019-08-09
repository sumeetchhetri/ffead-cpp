/*
 * Http11Handler.h
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#ifndef HTTP11HANDLER_H_
#define HTTP11HANDLER_H_
#include "SocketInterface.h"
#include "SocketUtil.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ConfigurationData.h"

class Http11Handler : public SocketInterface {
	bool isHeadersDone;
	int bytesToRead;
	std::string webpath;
	int chunkSize;
	int connKeepAlive;
	bool isTeRequest;
	int maxReqHdrCnt;
	int maxEntitySize;
	SocketInterface* handler;
	friend class CHServer;
public:
	void addHandler(SocketInterface* handler);
	void onOpen();
	void onClose();
	std::string getProtocol(void* context);
	int getTimeout();
	void* readRequest(void*& context, int& pending, int& reqPos);
	bool writeResponse(void* req, void* res, void* context);
	void init(const std::string& webpath, const int& chunkSize, const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize);
	Http11Handler(const std::string& webpath, const int& chunkSize, const int& connKeepAlive, const int& maxReqHdrCnt, const int& maxEntitySize);
	virtual ~Http11Handler();
};

#endif /* HTTP11HANDLER_H_ */
