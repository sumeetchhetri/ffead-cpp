/*
 * DefaultWebSocketHandler.h
 *
 *  Created on: 04-Dec-2014
 *      Author: sumeetc
 */

#ifndef DEFAULTWEBSOCKETHANDLER_H_
#define DEFAULTWEBSOCKETHANDLER_H_
#include "Http11WebSocketDataFrame.h"

class DefaultWebSocketHandler {
public:
	DefaultWebSocketHandler();
	virtual ~DefaultWebSocketHandler();
	bool isWriteControl();
	bool onOpen(WebSocketData* req, WebSocketRespponseData* res, std::string uniqueAddress, void* hreq);
	void onClose(std::string uniqueAddress);
	bool onMessage(WebSocketData* req, WebSocketRespponseData* res, std::string uniqueAddress);
};

#endif /* DEFAULTWEBSOCKETHANDLER_H_ */
