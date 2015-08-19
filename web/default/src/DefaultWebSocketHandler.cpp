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
	cout << "Got onOpen request for WebSocket" << endl;
	WebSocketData data;
	return data;
}

WebSocketData DefaultWebSocketHandler::onMessage(WebSocketData data) {
	cout << "Got onMessage request for WebSocket" << endl;
	cout << data.getData() << endl;
	return data;
}

WebSocketData DefaultWebSocketHandler::onClose() {
	cout << "Got onClose request for WebSocket" << endl;
	WebSocketData data;
	return data;
}
