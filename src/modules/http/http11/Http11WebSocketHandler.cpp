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

bool Http11WebSocketHandler::writeData(const WebSocketData& wsdata) {
	Http11WebSocketDataFrame frame;
	frame.fin = true;
	frame.rsv1 = false;
	frame.rsv2 = false;
	frame.rsv3 = false;
	frame.opcode = wsdata.dataType;
	frame.mask = false;
	frame.payloadLength = wsdata.data.length();
	//We will not set application data here, we will directly send the data as
	//we have computed other control options
	//sockUtil.sendData(frame.getFrameData());
	//sockUtil.sendData(wsdata.data);
	return false;
}

Http11WebSocketHandler::~Http11WebSocketHandler() {
	// TODO Auto-generated destructor stub
}

std::string Http11WebSocketHandler::getProtocol(void* context) {
	return "WS1.1";
}

int Http11WebSocketHandler::getTimeout() {
	return -1;
}

Http11WebSocketDataFrame* Http11WebSocketHandler::nextFrame()
{
	Http11WebSocketDataFrame* framer = NULL;
	if(buffer.length()>=2)
	{
		Http11WebSocketDataFrame frame;
		int count = 0;
		unsigned char f = buffer.at(0);
		unsigned char s = buffer.at(1);
		frame.fin = ((f >> 7) & 0x01);
		frame.rsv1 = ((f >> 6) & 0x01);
		frame.rsv2 = ((f >> 5) & 0x01);
		frame.rsv3 = ((f >> 4) & 0x01);
		frame.opcode = f & 0x0F;
		frame.mask = ((s >> 7) & 0x01);
		frame.payloadLength = s & 0x7F;
		unsigned long long dataLength = frame.payloadLength;
		count = 2;
		if(buffer.length()>4 && frame.payloadLength==126) {
			frame.extendedPayloadLength = CommonUtils::charArrayToULongLong(buffer.substr(2, 2));
			dataLength = frame.extendedPayloadLength;
			count += 2;
		} else if(buffer.length()>10 && frame.payloadLength==127) {
			frame.extendedPayloadLength = CommonUtils::charArrayToULongLong(buffer.substr(2, 8));
			dataLength = frame.extendedPayloadLength;
			count += 8;
		} else if(frame.payloadLength>125) {
			return framer;
		}

		if((frame.mask && buffer.length()>=(dataLength+4+count)) || buffer.length()>=(dataLength+count))
		{
			if(frame.mask) {
				frame.maskingKey = CommonUtils::charArrayToULongLong(buffer.substr(count, 4));
				count += 4;
			}
			frame.applicationData = buffer.substr(count, dataLength);
			if(frame.mask) {
				frame.applicationData = CommonUtils::xorEncryptDecrypt(frame.applicationData, frame.maskingKey);
			}
			buffer = buffer.substr(count+dataLength);
			framer = new Http11WebSocketDataFrame();
			*framer = frame;
		}
	}
	return framer;
}

bool Http11WebSocketHandler::processFrame(Http11WebSocketDataFrame* frame, void*& request)
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
	else if(frame->opcode==0 || frame->opcode==1 || frame->opcode==2) {
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
			WebSocketData* wsd = new WebSocketData();
			wsd->data = dataframes[it->first];
			wsd->dataType = frame->opcode;
			request = wsd;
			logger << "Message is " << wsd->data << std::endl;
			dataframes[it->first].clear();
			dataframesComplete[it->first] = false;
			break;
		}
	}

	return false;
}

void* Http11WebSocketHandler::readRequest(void*& context, int& pending, int& reqPos) {
	if(readFrom())return NULL;

	void* request = NULL;
	Http11WebSocketDataFrame* frame = NULL;
	while((frame=nextFrame())!=NULL)
	{
		if(processFrame(frame, request)) {
			//closed = true;
			closeSocket();
			break;
		}
		if(request!=NULL) {
			reqPos = startRequest();
			break;
		}

		delete frame;
		frame = NULL;
	}

	if(pending==(int)buffer.length())
	{
		pending = 0;
	}
	else
	{
		pending = buffer.length();
	}
	return request;
}

bool Http11WebSocketHandler::writeResponse(void* req, void* res, void* context, std::string& data, int reqPos) {
	WebSocketData* wsdata  = static_cast<WebSocketData*>(res);

	if(isClosed()) {
		if(req!=NULL) {
			//delete (WebSocketData*)req;
		}
		//delete wsdata;
		return true;
	}

	Http11WebSocketDataFrame frame;
	frame.fin = true;
	frame.rsv1 = false;
	frame.rsv2 = false;
	frame.rsv3 = false;
	frame.opcode = wsdata->dataType;
	frame.mask = false;
	frame.payloadLength = wsdata->data.length();
	//We will not set application data here, we will directly send the data as
	//we have computed other control options
	data += frame.getFrameData();
	if(req!=NULL) {
		//delete (WebSocketData*)req;
	}
	data += wsdata->data;
	//delete wsdata;
	return true;
}

void Http11WebSocketHandler::addHandler(SocketInterface* handler) {}
void Http11WebSocketHandler::onOpen(){}
void Http11WebSocketHandler::onClose(){}
