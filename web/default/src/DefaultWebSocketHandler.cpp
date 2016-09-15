/*
 * DefaultWebSocketHandler.cpp
 *
 *  Created on: 04-Dec-2014
 *      Author: sumeetc
 */

#include "DefaultWebSocketHandler.h"

DefaultWebSocketHandler::DefaultWebSocketHandler() {
	// TODO Auto-generated constructor stub

}

DefaultWebSocketHandler::~DefaultWebSocketHandler() {
	// TODO Auto-generated destructor stub
}

WebSocketData DefaultWebSocketHandler::onOpen() {
	std::cout << "Got onOpen request for WebSocket" << std::endl;
	WebSocketData data;
	return data;
}

WebSocketData DefaultWebSocketHandler::onMessage(WebSocketData data) {
	std::cout << "Got onMessage request for WebSocket" << std::endl;
	std::cout << data.getData() << std::endl;
	return data;
}

WebSocketData DefaultWebSocketHandler::onClose() {
	std::cout << "Got onClose request for WebSocket" << std::endl;
	WebSocketData data;
	return data;
}
