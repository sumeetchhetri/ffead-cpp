/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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
 * Http2StreamHandler.h
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2STREAMHANDLER_H_
#define HTTP2STREAMHANDLER_H_
#include "Http2DataFrame.h"
#include "Http2GoAwayFrame.h"
#include "Http2HeadersFrame.h"
#include "Http2PingFrame.h"
#include "Http2PriorityFrame.h"
#include "Http2PushPromiseFrame.h"
#include "Http2ResetStreamFrame.h"
#include "Http2SettingsFrame.h"
#include "Http2WindowUpdateFrame.h"
#include "Http2ContinuationFrame.h"
#include "Http2AlternativeServicesFrame.h"
#include "Http2HPACKContext.h"
#include "CommonUtils.h"
#include "Http2ReadWriteUtil.h"
#include "HttpRequest.h"
#include "Http11WebSocketDataFrame.h"

class Http2StreamHandler {
	enum StreamState {Idle, Reserved_L, Reserved_R, Open, Half_Closed_L, Half_Closed_R, Closed};
	StreamState state;
	int streamIdentifier;
	Http2HPACKContext* context;

	HttpRequest* request;
	bool isHeadersDone;
	std::string headerBlockFragment;
	bool endofstream;

	WebSocketData* wsrequest;
	bool endofsegment;

	Http2RequestResponseData pendingSendData;
	int lastFrameType;
	bool isWebSocket;
	int senderFlowControlWindow;
	int receiverFlowControlWindow;
	void* getRequestAndReInit();
	void* getWsRequestAndReInit();
	void closeConnection(const int& lastStreamIdentifier, Http2ReadWriteUtil* handler);
	bool isWebSocketRequest();
	void* handleWebSocketRequest(Http2HPACKContext* context, Http2Frame* frame, Http2ReadWriteUtil* handler, std::map<uint16_t, uint32_t>& settings);
	void sendPushPromiseFrames(Http2HPACKContext* context, Http2Frame* frame, Http2ReadWriteUtil* handler, std::map<uint16_t, uint32_t>& settings);
	friend class Http2Handler;
public:
	Http2StreamHandler();
	Http2StreamHandler(Http2HPACKContext* context, const int& streamIdentifier, const std::string& webpath);
	virtual ~Http2StreamHandler();
	bool handle(Http2Frame* frame, const int& precedingStreamId, std::map<uint16_t, uint32_t>& settings, Http2ReadWriteUtil* handler, std::map<int, bool>& frameAcks, void*& requestObj);
};

#endif /* HTTP2STREAMHANDLER_H_ */
