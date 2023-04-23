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
 * WebSockHandler.cpp
 *
 *  Created on: 20-Apr-2020
 *      Author: sumeetc
 */
#include "WebSockHandler.h"

int WebSockHandler::writeToPeer(WebSocketRespponseData* response, BaseSocket* sif) {
	if(!response->isEmpty()) {
		std::cout << "WS:Start:Writing" << std::endl;
		std::cout << "WS:Packets:" << response->getMore().size() << std::endl;
		for (int var = 0; var < (int)response->getMore().size(); ++var) {
			if(!sif->isClosed() && response->getMore()[var].hasData()) {
				if(sif->isEmbedded()) {
					ResponseData rd;
					WebSocketData* wres = &response->getMore()[var];
					int type = Http11WebSocketDataFrame::getFramePdu(wres, rd._b, false);
					int ret = sif->writeTo(&rd);
					if(ret == 0) {
						return 0;
					}
					if(type==1) {
						rd.oft = 0;
						rd._b = std::move(wres->textData);
						ret = sif->writeTo(&rd);
						if(ret == 0) {
							return 0;
						}
					} else {
						rd.oft = 0;
						rd._b = std::move(wres->binaryData);
						ret = sif->writeTo(&rd);
						if(ret == 0) {
							return 0;
						}
					}
				} else {
					sif->writeWsData(response);
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

BaseSocket* WebSockHandler::getSif() {
	return sif;
}

WebSockHandler::~WebSockHandler() {
}
