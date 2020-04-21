/*
 * WebSockHandler.h
 *
 *  Created on: 17-Apr-2020
 *      Author: sumeetc
 */

#ifndef SRC_FRAMEWORK_WEBSOCKHANDLER_H_
#define SRC_FRAMEWORK_WEBSOCKHANDLER_H_
#include "Http11WebSocketDataFrame.h"
#include "SocketInterface.h"

class WebSockHandler {
	SocketInterface* sif;
	friend class HttpServiceTask;
	friend class ServiceTask;
public:
	int writeTo(WebSocketRespponseData* res);
	int writeToPeer(WebSocketRespponseData* res, SocketInterface* sif);
	virtual bool onOpen(WebSocketData* req, WebSocketRespponseData* res, std::string uniqueAddress, void* hreq)=0;
	virtual void onClose(std::string uniqueAddress)=0;
	virtual bool onMessage(WebSocketData* req, WebSocketRespponseData* res, std::string uniqueAddress)=0;
	virtual bool isWriteControl();
	SocketInterface* getSif();
	virtual ~WebSockHandler();
};

#endif /* SRC_FRAMEWORK_WEBSOCKHANDLER_H_ */
