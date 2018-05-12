/*
 * WebSocket.cpp
 *
 *  Created on: 30-Nov-2014
 *      Author: sumeetc
 */

#include "Http11WebSocketHandler.h"

Http11WebSocketHandler::Http11WebSocketHandler(const std::string& url, const bool& isServer, const int& fd) {
	init(fd);
	logger = LoggerFactory::getLogger("Http11WebSocketHandler");
	this->url = url;
}

std::string Http11WebSocketHandler::getUrl() {
	return this->url;
}

Http11WebSocketDataFrame Http11WebSocketHandler::readFrame() {
	Http11WebSocketDataFrame frame;
	std::vector<unsigned char> f2bytes;
	if(!sockUtil.readData(2, f2bytes))
	{
		frame.opcode = 8;
		return frame;
	}
	unsigned char f = f2bytes.at(0);
	unsigned char s = f2bytes.at(1);
	frame.fin = ((f >> 7) & 0x01);
	frame.rsv1 = ((f >> 6) & 0x01);
	frame.rsv2 = ((f >> 5) & 0x01);
	frame.rsv3 = ((f >> 4) & 0x01);
	frame.opcode = f & 0x0F;
	frame.mask = ((s >> 7) & 0x01);
	frame.payloadLength = s & 0x7F;
	unsigned long long dataLength = frame.payloadLength;
	if(frame.payloadLength==126) {
		std::vector<unsigned char> extdlenbytes;
		if(!sockUtil.readData(2, extdlenbytes))
		{
			frame.opcode = 8;
			return frame;
		}
		frame.extendedPayloadLength = CommonUtils::charArrayToULongLong(extdlenbytes);
		dataLength = frame.extendedPayloadLength;
	} else if(frame.payloadLength==127) {
		std::vector<unsigned char> extdlenbytes;
		if(!sockUtil.readData(8, extdlenbytes))
		{
			frame.opcode = 8;
			return frame;
		}
		frame.extendedPayloadLength = CommonUtils::charArrayToULongLong(extdlenbytes);
		dataLength = frame.extendedPayloadLength;
	}
	if(frame.mask) {
		std::vector<unsigned char> maskingbytes;
		if(!sockUtil.readData(4, maskingbytes))
		{
			frame.opcode = 8;
			return frame;
		}
		frame.maskingKey = CommonUtils::charArrayToULongLong(maskingbytes);
	}
	if(!sockUtil.readData(dataLength, frame.applicationData))
	{
		frame.opcode = 8;
		return frame;
	}
	if(frame.mask) {
		frame.applicationData = CommonUtils::xorEncryptDecrypt(frame.applicationData, frame.maskingKey);
	}
	return frame;
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

/*int WebSocket::doIt(const Reflector& reflector, void* _temp, const Method& method, const std::string& cntxtName) {
	std::map<int, std::string> dataframes;
	std::map<int, bool> dataframesComplete;
	while(true)
	{
		Http11WebSocketDataFrame frame = readFrame();
		logger << "read new websocket frame " << frame.opcode << std::endl;
		if(frame.opcode==8) {
			return frame.opcode;
		}
		else if(frame.opcode==9) {
			replyPong();
			continue;
		}
		else if(frame.opcode==10) {
			//TODO don't know what to do...
			continue;
		}
		else if(frame.opcode==0 || frame.opcode==1 || frame.opcode==2) {
			for(int u=0;u<(int)frame.applicationData.size();u++) {
				dataframes[frame.opcode].push_back(frame.applicationData.at(u));
			}
			dataframesComplete[frame.opcode] = frame.fin;
		}
		else {
			//TODO don't know what to do...
			continue;
		}

		std::map<int, std::string>::iterator it;
		for(it=dataframes.begin();it!=dataframes.end();++it)
		{
			if(dataframesComplete[it->first])
			{
				WebSocketData wsd;
				wsd.data = dataframes[it->first];
				wsd.dataType = frame.opcode;
				//handle the onMessage method in controller
				if(method.getMethodName()!="")
				{
					vals valus;
					valus.push_back(&wsd);
					WebSocketData data = reflector.invokeMethod<WebSocketData>(_temp,method,valus,cntxtName);
					if(data.data!="")
					{
						writeData(data);
					}
					logger << "WebSocket Controller onMessage called" << std::endl;
				}
				else
				{
					logger << "Invalid WebSocket Controller" << std::endl;
				}

				dataframes[it->first].clear();
				dataframesComplete[it->first] = false;
			}
		}
	}
	sockUtil.closeSocket();
	delete sockUtil;
}*/

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
	return "HTTP1.1WS";
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
			close();
			break;
		}
		if(request!=NULL) {
			reqPos = startRequest();
			break;
		}
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

bool Http11WebSocketHandler::writeResponse(void* req, void* res, void* context) {
	WebSocketData* wsdata  = static_cast<WebSocketData*>(res);

	if(isClosed()) {
		if(req!=NULL) {
			delete (WebSocketData*)req;
		}
		delete wsdata;
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
	writeTo(frame.getFrameData());
	if(req!=NULL) {
		delete (WebSocketData*)req;
	}
 	bool fl = writeTo(wsdata->data);
	delete wsdata;
	return fl;
}

void Http11WebSocketHandler::onOpen(){}
void Http11WebSocketHandler::onClose(){}
