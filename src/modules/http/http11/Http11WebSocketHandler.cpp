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
 * WebSocket.cpp
 *
 *  Created on: 30-Nov-2014
 *      Author: sumeetc
 */

#include "Http11WebSocketHandler.h"

Http11WebSocketHandler::Http11WebSocketHandler(const SOCKET& fd, SSL* ssl, BIO* io, const std::string& url, const bool& isServer) : SocketInterface(fd, ssl, io) {
	logger = LoggerFactory::getLogger("Http11WebSocketHandler");
	this->url = url;
	this->h = NULL;
	this->lastOpCode = -1;
}

std::string Http11WebSocketHandler::getUrl() {
	return this->url;
}

void Http11WebSocketHandler::replyPong() {
	Http11WebSocketDataFrame frame;
	frame.fin = true;
	frame.rsv1 = false;
	frame.rsv2 = false;
	frame.rsv3 = false;
	frame.opcode = 10;
	frame.mask = false;
	frame.payloadLength = 0;
	std::string pong = frame.getFrameData();
	//sockUtil.sendData(pong);
}

Http11WebSocketHandler::~Http11WebSocketHandler() {
}

std::string Http11WebSocketHandler::getProtocol(void* context) {
	return "WS1.1";
}

int Http11WebSocketHandler::getType(void* context) {
	return 2;
}

int Http11WebSocketHandler::getTimeout() {
	return -1;
}

bool Http11WebSocketHandler::nextFrame(Http11WebSocketDataFrame* frame)
{
	if(buffer.length()>=2)
	{
		int count = 0;
		unsigned char f = buffer.at(0);
		unsigned char s = buffer.at(1);
		frame->fin = ((f >> 7) & 0x01);
		frame->rsv1 = ((f >> 6) & 0x01);
		frame->rsv2 = ((f >> 5) & 0x01);
		frame->rsv3 = ((f >> 4) & 0x01);
		frame->opcode = f & 0x0F;
		if(lastOpCode!=frame->opcode && frame->opcode!=0) {
			lastOpCode = frame->opcode;
		}
		frame->mask = ((s >> 7) & 0x01);
		frame->payloadLength = s & 0x7F;
		frame->maskingKey = 0;
		frame->applicationData.clear();
		unsigned long long dataLength = frame->payloadLength;
		count = 2;
		if(buffer.length()>4 && frame->payloadLength==126) {
			frame->extendedPayloadLength = CommonUtils::charArrayToULongLong(buffer.substr(2, 2));
			dataLength = frame->extendedPayloadLength;
			count += 2;
		} else if(buffer.length()>10 && frame->payloadLength==127) {
			frame->extendedPayloadLength = CommonUtils::charArrayToULongLong(buffer.substr(2, 8));
			dataLength = frame->extendedPayloadLength;
			count += 8;
		} else if(frame->payloadLength>125) {
			return false;
		}

		if((frame->mask && buffer.length()>=(dataLength+4+count)) || buffer.length()>=(dataLength+count))
		{
			if(frame->mask) {
				frame->maskingKey = CommonUtils::charArrayToULongLong(buffer.substr(count, 4));
				count += 4;
			}
			frame->applicationData = buffer.substr(count, dataLength);
			if(frame->mask) {
				frame->applicationData = CommonUtils::xorEncryptDecrypt(frame->applicationData, frame->maskingKey);
			}
			buffer = buffer.substr(count+dataLength);
			return true;
		}
	}
	return false;
}

bool Http11WebSocketHandler::processFrame(Http11WebSocketDataFrame* frame, WebSocketData* request, bool& isReq)
{
	if(frame->opcode==8) {
		//closed = true;
		return true;
	}
	else if(frame->opcode==9) {
		replyPong();
		return false;
	}
	else if(frame->opcode==10) {
		//TODO don't know what to do...
		return false;
	}
	else if(frame->opcode==0) {
		for(int u=0;u<(int)frame->applicationData.size();u++) {
			dataframes[lastOpCode].push_back(frame->applicationData.at(u));
		}
		dataframesComplete[lastOpCode] = frame->fin;
	}
	else if(frame->opcode==1 || frame->opcode==2) {
		for(int u=0;u<(int)frame->applicationData.size();u++) {
			dataframes[frame->opcode].push_back(frame->applicationData.at(u));
		}
		dataframesComplete[frame->opcode] = frame->fin;
	}
	else {
		//TODO don't know what to do...
		return false;
	}

	std::map<int, std::string>::iterator it;
	for(it=dataframes.begin();it!=dataframes.end();++it)
	{
		if(dataframesComplete[it->first])
		{
			WebSocketData* wsd = (WebSocketData*)request;
			isReq = true;
			if(frame->opcode==1) {
				wsd->textData = std::move(dataframes[it->first]);
			} else {
				wsd->binaryData = std::move(dataframes[it->first]);
			}
			//logger << "Message is " << wsd->data << std::endl;
			dataframes[it->first].clear();
			dataframesComplete[it->first] = false;
			break;
		}
	}

	return false;
}

bool Http11WebSocketHandler::readRequest(void* request, void*& context, int& pending, int& reqPos) {
	bool fl = false;
	Http11WebSocketDataFrame frame;
	while(nextFrame(&frame))
	{
		WebSocketData* wreq = (WebSocketData*)request;
		if(processFrame(&frame, wreq, fl)) {
			//closed = true;
			closeSocket();
			break;
		}
		if(fl) {
			break;
		}
	}

	pending = buffer.length();
	return fl;
}

bool Http11WebSocketHandler::writeResponse(void* req, void* res, void* context, std::string& data, int reqPos) {
	WebSocketData* wsdata  = (WebSocketData*)res;

	Http11WebSocketDataFrame frame;
	frame.fin = true;
	frame.rsv1 = false;
	frame.rsv2 = false;
	frame.rsv3 = false;
	frame.mask = false;
	if(wsdata->textData.length()>0) {
		frame.opcode = 1;
		int payloadLength = wsdata->textData.length();
		frame.payloadLength = payloadLength;

		if(payloadLength<=125) {
			frame.extendedPayloadLength = 0;
		} else if(payloadLength<65535) {
			frame.extendedPayloadLength = payloadLength;
			frame.payloadLength = 126;
		} else {
			frame.extendedPayloadLength = payloadLength;
			frame.payloadLength = 127;
		}

		//We will not set application data here, we will directly send the data as
		//we have computed other control options
		frame.getFrameData(data);
		data += wsdata->textData;
	}
	if(wsdata->binaryData.length()>0) {
		frame.opcode = 2;
		int payloadLength = wsdata->binaryData.length();
		frame.payloadLength = payloadLength;

		if(payloadLength<=125) {
			frame.extendedPayloadLength = 0;
		} else if(payloadLength<65535) {
			frame.extendedPayloadLength = payloadLength;
			frame.payloadLength = 126;
		} else {
			frame.extendedPayloadLength = payloadLength;
			frame.payloadLength = 127;
		}

		//We will not set application data here, we will directly send the data as
		//we have computed other control options
		frame.getFrameData(data);
		data += wsdata->binaryData;
	}
	return true;
}

void Http11WebSocketHandler::addHandler(SocketInterface* handler) {}
void Http11WebSocketHandler::onOpen(){}
void Http11WebSocketHandler::onClose(){
	if(h!=NULL) {
		h->onClose(getAddress());
	}
}
