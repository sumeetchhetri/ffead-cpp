/*
 * DefaultWebSocketHandler.cpp
 *
 *  Created on: 04-Dec-2014
 *      Author: sumeetc
 */

#include "DefaultWebSocketHandler.h"

DefaultWebSocketHandler::DefaultWebSocketHandler() {
}

DefaultWebSocketHandler::~DefaultWebSocketHandler() {
}

bool DefaultWebSocketHandler::onOpen(WebSocketData *req, WebSocketRespponseData *res, std::string uniqueAddress, void *hreq) {
	std::cout << "Got onOpen request for WebSocket" << std::endl;
	return true;
}

void DefaultWebSocketHandler::onClose(std::string uniqueAddress) {
	std::cout << "Got onClose request for WebSocket" << std::endl;
}

bool DefaultWebSocketHandler::isWriteControl() {
	return false;
}

bool DefaultWebSocketHandler::onMessage(WebSocketData *req, WebSocketRespponseData *res, std::string uniqueAddress) {
	std::cout << "Got onMessage request for WebSocket" << std::endl;
	std::cout << req->getTextData() << std::endl;
	res->pushText(req->getTextData());
	return true;
}
