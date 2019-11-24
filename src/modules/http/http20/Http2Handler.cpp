/*
 * Http2Handler.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2Handler.h"

Http2Frame* Http2Handler::readFrame(std::string data) {
	Http2FrameHeader header;
	header.payloadLength = (int)CommonUtils::charArrayToULongLong(data.substr(0, 3));
	header.type = data.at(3);
	header.flags = data.at(4);
	header.reserved = ((data.at(5) >> 7) & 0x01);
	data[5] = data[5] & 0x7F;
	header.streamIdentifier = (int)CommonUtils::charArrayToULongLong(data.substr(5, 4));
	std::string payload;
	if(data.length()>9)
	{
		payload = data.substr(9);
	}
	return getFrameByType(payload, header);
}

Http2Frame* Http2Handler::nextFrame() {
	if(buffer.length()>=9)
	{
		int payloadlength = (int)CommonUtils::charArrayToULongLong(buffer.substr(0, 3));
		if((int)buffer.length()>=9+payloadlength)
		{
			Http2FrameHeader header;
			header.payloadLength = payloadlength;
			header.type = buffer.at(3);
			header.flags = buffer.at(4);
			header.reserved = ((buffer.at(5) >> 7) & 0x01);
			buffer[5] = buffer[5] & 0x7F;
			header.streamIdentifier = (int)CommonUtils::charArrayToULongLong(buffer.substr(5, 4));
			std::string payload;
			payload = buffer.substr(9, payloadlength);
			buffer = buffer.substr(payloadlength+9);
			return getFrameByType(payload, header);
		}
	}
	return NULL;
}

Http2Frame* Http2Handler::getFrameByType(const std::string& data, Http2FrameHeader& header)
{
	unsigned char type = header.type;
	if(type==0) {
		return new Http2DataFrame(data, header);
	} else if(type==1) {
		return new Http2HeadersFrame(data, header);
	} else if(type==2) {
		return new Http2PriorityFrame(data, header);
	} else if(type==3) {
		return new Http2ResetStreamFrame(data, header);
	} else if(type==4) {
		return new Http2SettingsFrame(data, header);
	} else if(type==5) {
		return new Http2PushPromiseFrame(data, header);
	} else if(type==6) {
		return new Http2PingFrame(data, header);
	} else if(type==7) {
		return new Http2GoAwayFrame(data, header);
	} else if(type==8) {
		return new Http2WindowUpdateFrame(data, header);
	} else if(type==9) {
		return new Http2ContinuationFrame(data, header);
	} else if(type==10) {
		return new Http2AlternativeServicesFrame(data, header);
	}
	return NULL;
}

Http2Handler::Http2Handler(const SOCKET& fd, SSL* ssl, BIO* io, const bool& isServer, const std::string& webpath) : SocketInterface(fd, ssl, io) {
	this->highestStreamIdentifier = 0;
	this->context.huffmanEncoding = true;
	this->highestPushPromiseStreamIdentifier = 2;
	this->senderFlowControlWindow = 65535;
	this->receiverFlowControlWindow = 20971520;
	this->isConnInit = false;
	this->precedingstreamId = -1;
	this->maxDataFrameSize = 16384;
	this->webpath = webpath;
	http2 = true;
	logger = LoggerFactory::getLogger("Http2Handler");
}

Http2Handler::Http2Handler(const SOCKET& fd, SSL* ssl, BIO* io, const bool& isServer, const std::string& webpath, const std::string& settingsFrameData) : SocketInterface(fd, ssl, io) {
	this->highestStreamIdentifier = 0;
	this->context.huffmanEncoding = true;
	this->highestPushPromiseStreamIdentifier = 2;
	this->senderFlowControlWindow = 65535;
	this->receiverFlowControlWindow = 65535;
	this->isConnInit = false;
	this->precedingstreamId = -1;
	this->maxDataFrameSize = 16384;
	this->webpath = webpath;
	http2 = true;
	logger = LoggerFactory::getLogger("Http2Handler");

	Http2Frame* sframe = readFrame(settingsFrameData);
	if(sframe!=NULL)
	{
		void* temp = NULL;
		processFrame(sframe, temp, 0);

		if(!this->isConnInit) {
			Http2SettingsFrame stframe;
			stframe.settings[Http2SettingsFrame::SETTINGS_MAX_CONCURRENT_STREAMS] = 100;
			stframe.settings[Http2SettingsFrame::SETTINGS_INITIAL_WINDOW_SIZE] = 20971520;
			writeInitData(&stframe);
			frameAcks[stframe.header.type] = true;
			isConnInit = true;
		}
	}
}

int Http2Handler::getHighestPushPromiseStreamIdentifier() {
	int val = highestPushPromiseStreamIdentifier;
	highestPushPromiseStreamIdentifier += 2;
	return val;
}

bool Http2Handler::processFrame(Http2Frame* frame, void*& request, int reqPos) {
	if(frame->header.type==0) {
		//Send Window Update if current window size is not sufficient
		if(receiverFlowControlWindow<=frame->header.payloadLength) {
			updateRxFCWS = frame->header.payloadLength;
		}
		receiverFlowControlWindow -= frame->header.payloadLength;
	}

	if(precedingstreamId==-1) {
		precedingstreamId = frame->getHeader().getStreamIdentifier();
	}

	logger << "read new Http2Frame " << (int)frame->header.type << std::endl;
	int streamIdentifier = frame->header.streamIdentifier;
	if(streams.find(frame->header.streamIdentifier)==streams.end()) {
		Http2StreamHandler& stream = streams[frame->header.streamIdentifier];
		stream.request = new HttpRequest(webpath);
		stream.wsrequest = new WebSocketData();
		stream.streamIdentifier = streamIdentifier;
		stream.context = &context;
		//streams.insert(std::pair<int, Http2StreamHandler>(frame->header.streamIdentifier,
		//		Http2StreamHandler(&context, frame->header.streamIdentifier, webpath)));
	}
	bool flag = streams[streamIdentifier].handle(frame, precedingstreamId, settings, this, frameAcks, request);
	precedingstreamId = streamIdentifier;
	return flag;
}

void* Http2Handler::readRequest(void*& context, int& pending, int& reqPos) {

	if(!pushPromisedRequestQ.empty()) {
		HttpRequest* ppreq = new HttpRequest();
		context = new int(pushPromisedRequestQ.front().streamIdentifier);
		ppreq->setHttp2Headers(pushPromisedRequestQ.front().preHeaders);
		pushPromisedRequestQ.pop();
		pending = pushPromisedRequestQ.empty()?0:1;
		return ppreq;
	}

	if(readFrom())return NULL;

	if(!this->isConnInit) {
		std::string clientpreface = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
		if(buffer.find(clientpreface)!=std::string::npos)
		{
			buffer = buffer.substr(clientpreface.length());
		}

		Http2SettingsFrame sframe;
		sframe.settings[Http2SettingsFrame::SETTINGS_MAX_CONCURRENT_STREAMS] = 100;
		sframe.settings[Http2SettingsFrame::SETTINGS_INITIAL_WINDOW_SIZE] = 20971520;
		writeInitData(&sframe);
		frameAcks[sframe.header.type] = true;
		isConnInit = true;
	}

	void* request = NULL;
	Http2Frame* frame = NULL;
	while((frame=nextFrame())!=NULL)
	{
		int streamIdentifier = frame->header.streamIdentifier;
		if(processFrame(frame, request, reqPos)) {
			//closed = true;
			closeSocket();
			break;
		}
		if(request!=NULL) {
			reqPos = startRequest();
			context = new int(streamIdentifier);
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

int Http2Handler::getTimeout() {
	return -1;
}

std::string Http2Handler::serialize(Http2Frame* frame) {
	std::string frameD;
	std::string payload = frame->getFrameData();
	frame->header.payloadLength = payload.length();
	frameD.append(CommonUtils::ulonglongTocharArray(frame->header.payloadLength, 3));
	frameD.push_back(frame->header.type);
	frameD.push_back((unsigned char)frame->header.flags.to_ulong());
	std::string data = CommonUtils::ulonglongTocharArray(frame->header.streamIdentifier, 4);
	if(frame->header.reserved)
		data[0] |= 0x01 << 7;
	else
		data[0] &= ~(0x01 << 7);
	frameD.append(data);
	frameD.append(payload);
	return frameD;
}

void Http2Handler::writeInitData(Http2Frame* frame) {
	writeTo(serialize(frame), 0);
}

bool Http2Handler::writePendingDataFrame(Http2RequestResponseData& pendingSendData, std::string& data) {

	int streamIdentifier = pendingSendData.streamIdentifier;

	while(pendingSendData.isDataPending())
	{
		pendingSendData.updateContent();
		if(senderFlowControlWindow>=(int)pendingSendData.data.length()
				&& streams[streamIdentifier].senderFlowControlWindow>=(int)pendingSendData.data.length())
		{
			Http2DataFrame dframe;
			dframe.header.streamIdentifier = streamIdentifier;
			dframe.data = pendingSendData.data;
			dframe.header.flags.set(0);

			data += serialize(&dframe);

			//Handle flow control for data frames
			senderFlowControlWindow -= dframe.header.payloadLength;
			streams[streamIdentifier].senderFlowControlWindow -= dframe.header.payloadLength;
		}
		else
		{
			int minreslen = 0;
			if(senderFlowControlWindow-streams[streamIdentifier].senderFlowControlWindow>0)
				minreslen = streams[streamIdentifier].senderFlowControlWindow;
			else
				minreslen = senderFlowControlWindow;

			Http2DataFrame dframe;
			dframe.header.streamIdentifier = streamIdentifier;
			dframe.data = pendingSendData.data.substr(0, minreslen);
			//No end of stream set for this incomplete data frame
			data += serialize(&dframe);

			pendingSendData.data = pendingSendData.data.substr(minreslen);
			break;
		}
	}
	return false;
}

bool Http2Handler::writeResponse(void* req, void* res, void* si, std::string& data, int reqPos) {
	int streamIdentifier = *(int*)si;

	if(isClosed()) {
		delete (int*)si;
		return true;
	}

	Http2WindowUpdateFrame cwuframe;
	cwuframe.windowSizeIncrement = 65535 + updateRxFCWS;
	cwuframe.header.streamIdentifier = 0;
	data += serialize(&cwuframe);

	if(streams[streamIdentifier].isWebSocket) {
		return writeWebSocketResponse(req, res, si, data);
	} else {
		return writeHttpResponse(req, res, si, data);
	}
}

bool Http2Handler::writeWebSocketResponse(void* req, void* res, void* si, std::string& data) {
	WebSocketData* response = (WebSocketData*)res;
	//WebSocketData* request = (WebSocketData*)req;
	int streamIdentifier = *(int*)si;

	response->updateContent(maxDataFrameSize);

	Http2HeadersFrame hframe;
	std::map<std::string, std::string,cicomp> hdrs;
	hdrs[":opcode"] = CastUtil::fromNumber(response->dataType);
	hframe.headerBlockFragment = context.encode(hdrs);
	CommonUtils::printHEX(hframe.headerBlockFragment);
	hframe.header.streamIdentifier = streamIdentifier;
	hframe.header.flags.set(2);
	if(response->data=="") {
		hframe.header.flags.set(6);
	}
	data += serialize(&hframe);

	bool completedSend = true;
	std::string tmd;
	while(response->data!="" && (tmd = response->getRemainingContent()) != "") {
		if(senderFlowControlWindow>=(int)tmd.length()
				&& streams[streamIdentifier].senderFlowControlWindow>=(int)tmd.length())
		{
			Http2DataFrame dframe;
			dframe.header.streamIdentifier = streamIdentifier;
			dframe.data = tmd;
			if(!response->isContentRemains()) {
				dframe.header.flags.set(6);
			}

			data += serialize(&dframe);
			logger << "write tmd frame " << dframe.header.streamIdentifier  << " " << dframe.header.payloadLength << " bytes" << std::endl;

			//Handle flow control for data frames
			senderFlowControlWindow -= dframe.header.payloadLength;
			streams[streamIdentifier].senderFlowControlWindow -= dframe.header.payloadLength;
		}
		else
		{
			int minreslen = 0;
			if(senderFlowControlWindow-streams[streamIdentifier].senderFlowControlWindow>0)
				minreslen = streams[streamIdentifier].senderFlowControlWindow;
			else
				minreslen = senderFlowControlWindow;

			Http2DataFrame dframe;
			dframe.header.streamIdentifier = streamIdentifier;
			dframe.data = tmd.substr(0, minreslen);
			//No end of stream set for this incomplete data frame
			logger << "write partial tmd frame " << dframe.header.streamIdentifier << std::endl;
			data += serialize(&dframe);

			completedSend = false;
			streams[streamIdentifier].pendingSendData.isWebSocket = streams[streamIdentifier].isWebSocket;
			streams[streamIdentifier].pendingSendData.incompleteResponse = response;
			streams[streamIdentifier].pendingSendData.streamIdentifier = streamIdentifier;
			streams[streamIdentifier].pendingSendData.data = tmd.substr(minreslen);
			break;
		}
	}

	delete (int*)si;
	if(completedSend) {
	}

	return false;
}

bool Http2Handler::writeHttpResponse(void* req, void* res, void* si, std::string& data) {
	HttpResponse* response = (HttpResponse*)res;
	HttpRequest* request = (HttpRequest*)req;
	int streamIdentifier = *(int*)si;

	if(!response->isDone()) {
		response->updateContent(request, maxDataFrameSize);
	}
	response->headers[":status"] = response->statusCode;

	Http2HeadersFrame hframe;
	hframe.headerBlockFragment = context.encode(response->headers);
	CommonUtils::printHEX(hframe.headerBlockFragment);
	hframe.header.streamIdentifier = streamIdentifier;
	hframe.header.flags.set(2);
	if(!response->hasContent) {
		hframe.header.flags.set(0);
	}
	data += serialize(&hframe);
	//logger << response->generateResponse(request, false) << std::endl;
	//logger << "write header frame " << hframe.header.streamIdentifier << std::endl;

	bool completedSend = true;
	bool isFirst = true;
	std::string tmd;
	if(!response->isContentRemains()) {
		if(senderFlowControlWindow>=(int)response->getContent().length()
				&& streams[streamIdentifier].senderFlowControlWindow>=(int)response->getContent().length())
		{
			Http2DataFrame dframe;
			dframe.header.streamIdentifier = streamIdentifier;
			dframe.data = response->getContent();
			if(!response->isContentRemains()) {
				dframe.header.flags.set(0);
			}

			data += serialize(&dframe);
			logger << "write data frame " << dframe.header.streamIdentifier  << " " << dframe.header.payloadLength << " bytes" << std::endl;

			//Handle flow control for data frames
			senderFlowControlWindow -= dframe.header.payloadLength;
			streams[streamIdentifier].senderFlowControlWindow -= dframe.header.payloadLength;
		}
		else
		{
			int minreslen = 0;
			if(senderFlowControlWindow-streams[streamIdentifier].senderFlowControlWindow>0)
				minreslen = streams[streamIdentifier].senderFlowControlWindow;
			else
				minreslen = senderFlowControlWindow;

			Http2DataFrame dframe;
			dframe.header.streamIdentifier = streamIdentifier;
			dframe.data = data.substr(0, minreslen);
			//No end of stream set for this incomplete data frame
			logger << "write partial data frame " << dframe.header.streamIdentifier << std::endl;
			data += serialize(&dframe);

			completedSend = false;
			streams[streamIdentifier].pendingSendData.isWebSocket = streams[streamIdentifier].isWebSocket;
			streams[streamIdentifier].pendingSendData.incompleteResponse = response;
			streams[streamIdentifier].pendingSendData.url = request->getUrl();
			streams[streamIdentifier].pendingSendData.streamIdentifier = streamIdentifier;
			streams[streamIdentifier].pendingSendData.data = data.substr(minreslen);
		}
	} else {
		while(response->hasContent && response->getRemainingContent(request->getUrl(), isFirst, data)) {
			isFirst = false;
			if(senderFlowControlWindow>=(int)tmd.length()
					&& streams[streamIdentifier].senderFlowControlWindow>=(int)tmd.length())
			{
				Http2DataFrame dframe;
				dframe.header.streamIdentifier = streamIdentifier;
				dframe.data = tmd;
				if(!response->isContentRemains()) {
					dframe.header.flags.set(0);
				}

				data += serialize(&dframe);
				logger << "write data frame " << dframe.header.streamIdentifier  << " " << dframe.header.payloadLength << " bytes" << std::endl;

				//Handle flow control for data frames
				senderFlowControlWindow -= dframe.header.payloadLength;
				streams[streamIdentifier].senderFlowControlWindow -= dframe.header.payloadLength;
			}
			else
			{
				int minreslen = 0;
				if(senderFlowControlWindow-streams[streamIdentifier].senderFlowControlWindow>0)
					minreslen = streams[streamIdentifier].senderFlowControlWindow;
				else
					minreslen = senderFlowControlWindow;

				Http2DataFrame dframe;
				dframe.header.streamIdentifier = streamIdentifier;
				dframe.data = tmd.substr(0, minreslen);
				//No end of stream set for this incomplete data frame
				logger << "write partial data frame " << dframe.header.streamIdentifier << std::endl;
				data += serialize(&dframe);

				completedSend = false;
				streams[streamIdentifier].pendingSendData.isWebSocket = streams[streamIdentifier].isWebSocket;
				streams[streamIdentifier].pendingSendData.incompleteResponse = response;
				streams[streamIdentifier].pendingSendData.url = request->getUrl();
				streams[streamIdentifier].pendingSendData.streamIdentifier = streamIdentifier;
				streams[streamIdentifier].pendingSendData.data = tmd.substr(minreslen);
				break;
			}
		}
	}

	delete (int*)si;
	if(completedSend) {
	}

	return false;
}

bool Http2Handler::writeData(Http2RequestResponseData& data, Http2RequestResponseData& pendingSendData, int& streamFlowControlWindowS, std::string& respd) {
	Http2HeadersFrame hframe;
	hframe.headerBlockFragment = context.encode(data.preHeaders);
	CommonUtils::printHEX(hframe.headerBlockFragment);
	hframe.header.streamIdentifier = data.streamIdentifier;
	hframe.header.flags.set(2);
	respd += serialize(&hframe);
	CommonUtils::printHEX(serialize(&hframe));

	Http2DataFrame dframe;
	dframe.header.streamIdentifier = data.streamIdentifier;
	dframe.data = data.data;
	dframe.header.flags.set(0);

	std::string dfdata = serialize(&dframe);
	//Check the connection/stream sender window size before sending data frame
	if(senderFlowControlWindow>=dframe.header.payloadLength && streamFlowControlWindowS>=dframe.header.payloadLength)
	{
		respd += dfdata;
		//Handle flow control for data frames
		senderFlowControlWindow -= dframe.header.payloadLength;
		streamFlowControlWindowS -= dframe.header.payloadLength;
	}
	else
	{
		pendingSendData.streamIdentifier = data.streamIdentifier;
		pendingSendData.data += data.data;
	}

	data.reset();
	return false;
}

std::vector<std::string> Http2Handler::getRelatedEntitiesForPP(const std::string& request) {
	if(relatedEntitiesForPP.find(request)!=relatedEntitiesForPP.end())
	{
		return relatedEntitiesForPP[request];
	}
	std::vector<std::string> ents;
	return ents;
}

int Http2Handler::updateSenderWindowSize(const int& windowSize) {
	senderFlowControlWindow += windowSize;
	return senderFlowControlWindow;
}

std::string Http2Handler::getProtocol(void* context) {
	int streamIdentifier = *(int*)context;
	if(streams.find(streamIdentifier)!=streams.end() && streams[streamIdentifier].isWebSocket) {
		return "WS2.0";
	}
	return "HTTP2.0";
}

const std::string& Http2Handler::getWebpath() const {
	return webpath;
}

Http2Handler::~Http2Handler() {
}

void Http2Handler::addPushPromisedRequestToQ(const Http2RequestResponseData& ppdat) {
	pushPromisedRequestQ.push(ppdat);
}

void Http2Handler::updateMaxFrameSize(const uint32_t& val) {
	maxDataFrameSize = val;
}

std::string Http2Handler::getMimeType(const std::string& ext) {
	return CommonUtils::getMimeType(ext);
}

void Http2Handler::addHandler(SocketInterface* handler) {}
void Http2Handler::onOpen(){}
void Http2Handler::onClose(){}
