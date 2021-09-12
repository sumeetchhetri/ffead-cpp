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
	BaseSocket* sif;
	friend class HttpServiceTask;
	friend class ServiceTask;
public:
	int writeTo(WebSocketRespponseData* res);
	int writeToPeer(WebSocketRespponseData* res, BaseSocket* sif);
	virtual bool onOpen(WebSocketData* req, WebSocketRespponseData* res, std::string uniqueAddress, void* hreq)=0;
	virtual void onClose(std::string uniqueAddress)=0;
	virtual bool onMessage(WebSocketData* req, WebSocketRespponseData* res, std::string uniqueAddress)=0;
	virtual bool isWriteControl();
	BaseSocket* getSif();
	virtual ~WebSockHandler();
};

#endif /* SRC_FRAMEWORK_WEBSOCKHANDLER_H_ */
