/*
 * WebSockHandler.cpp
 *
 *  Created on: 20-Apr-2020
 *      Author: sumeetc
 */
#include "WebSockHandler.h"

int WebSockHandler::writeToPeer(WebSocketRespponseData* response, SocketInterface* sif) {
	if(!response->isEmpty()) {
		std::cout << "WS:Start:Writing" << std::endl;
		std::cout << "WS:Packets:" << response->getMore().size() << std::endl;
		for (int var = 0; var < (int)response->getMore().size(); ++var) {
			if(!sif->isClosed() && response->getMore()[var].hasData()) {
				ResponseData rd;
				Http11WebSocketDataFrame::getFramePdu(&response->getMore()[var], rd._b);
				int ret = sif->writeTo(&rd);
				if(ret == 0) {
					return 0;
				}
			}
		}
		std::cout << "WS:End:Writing" << std::endl;
	}
	return 1;
}

int WebSockHandler::writeTo(WebSocketRespponseData *response) {
	return writeToPeer(response, sif);
}

bool WebSockHandler::isWriteControl() {
	return false;
}

SocketInterface* WebSockHandler::getSif() {
	return sif;
}

WebSockHandler::~WebSockHandler() {
}
