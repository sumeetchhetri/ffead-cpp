/*
 * Http2StreamHandler.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2StreamHandler.h"

Http2StreamHandler::~Http2StreamHandler() {
	if(request!=NULL) {
		delete request;
	}
	if(wsrequest!=NULL) {
		delete wsrequest;
	}
	pendingSendData.reset();
}

Http2StreamHandler::Http2StreamHandler() {
	this->streamIdentifier = -1;
	this->state = Idle;
	this->lastFrameType = -1;
	this->context = NULL;
	this->isWebSocket = false;
	this->senderFlowControlWindow = 65535;
	this->receiverFlowControlWindow = 65535;
	this->endofstream = true;
	this->isHeadersDone = false;
	this->request = NULL;
	this->wsrequest = NULL;
	this->endofsegment = false;
}

Http2StreamHandler::Http2StreamHandler(Http2HPACKContext* context, const int& streamIdentifier, const std::string& webpath) {
	this->streamIdentifier = streamIdentifier;
	this->state = Idle;
	this->lastFrameType = -1;
	this->context = context;
	this->isWebSocket = false;
	this->senderFlowControlWindow = 65535;
	this->receiverFlowControlWindow = 65535;
	this->endofstream = true;
	this->isHeadersDone = false;
	this->request = new HttpRequest(webpath);
	this->wsrequest = new WebSocketData();
	this->endofsegment = false;
}

void Http2StreamHandler::closeConnection(const int& lastStreamIdentifier, Http2ReadWriteUtil* handler)
{
	Http2GoAwayFrame gframe;
	gframe.lastStreamId = 0;
	gframe.errorCode = 1;
	handler->writeInitData(&gframe);
	//handler->close();
	std::cout << "closed stream " << lastStreamIdentifier << std::endl;
}

bool Http2StreamHandler::isWebSocketRequest() {
	return request->hasHeader(":path") && request->hasHeader(":authority")
			&& request->hasHeader(":scheme") && request->hasHeader("websocket-version")
			&& request->hasHeader("websocket-origin");
}

void* Http2StreamHandler::getRequestAndReInit() {
	endofstream = false;
	headerBlockFragment.clear();
	void* temp = request;
	request = NULL;
	return temp;
}

void* Http2StreamHandler::getWsRequestAndReInit() {
	endofsegment = false;
	headerBlockFragment.clear();
	void* temp = wsrequest;
	wsrequest = new WebSocketData();
	return temp;
}

bool Http2StreamHandler::handle(Http2Frame* frame, const int& precedingStreamId, std::map<uint16_t, uint32_t>& settings, Http2ReadWriteUtil* handler, std::map<int, bool>& frameAcks, void*& requestObj) {
	if(frame->header.type==1) {
		//Header Frame
		Http2HeadersFrame* headerf  = static_cast<Http2HeadersFrame*>(frame);
		//Header frame can only be received in the Idle/Open states
		if(state==Idle || state==Open) {
			state = Open;
			//If this is the end of headers block then decode headers
			if(!isWebSocket && headerf->getHeader().isEndHeaders()) {
				request->setHttp2Headers(context->decode(headerf->headerBlockFragment));
				void* temp = handleWebSocketRequest(context, frame, handler, settings);
				if(temp!=NULL) {
					requestObj = temp;
					return false;
				}
			} else if(headerf->getHeader().isEndHeaders()) {
				std::map<std::string, std::string> wshdrs = context->decode(headerf->headerBlockFragment);
				if(wshdrs.find(":opcode")!=wshdrs.end()) {
					try {
						wsrequest->dataType = CastUtil::lexical_cast<short>(wshdrs[":opcode"]);
					} catch(const std::exception& e) {
						//TODO specify proper error
						closeConnection(frame->header.streamIdentifier, handler);
						return true;
					}
				} else {
					//TODO specify proper error
					closeConnection(frame->header.streamIdentifier, handler);
					return true;
				}
			} else {
				//else continue appending to the headerBlockFragment
				headerBlockFragment += headerf->headerBlockFragment;
			}

			if(headerf->getHeader().isWsEndSegment()) {
				endofsegment = true;
			}
			if(headerf->getHeader().isEndOfStream()) {
				endofstream = true;
				if(isWebSocket && headerf->getHeader().isEndHeaders()) {
					//TODO handle websocket close
				}
			}

			//if end of stream is reached then change state and process request
			if(!isWebSocket && headerf->getHeader().isEndHeaders() && endofstream) {
				state = Half_Closed_R;

				sendPushPromiseFrames(context, frame, handler, settings);

				requestObj = getRequestAndReInit();
			}
		} else {
			closeConnection(frame->header.streamIdentifier, handler);
			return true;
		}
	} else if(frame->header.type==9) {
		//Continuation Frame
		Http2ContinuationFrame* contf  = static_cast<Http2ContinuationFrame*>(frame);
		//Continuation frame can only be received in the Open state given the preceding stream id is same
		//and last frame type was either a Header/Continuation/PushPromise frame
		if(state==Open && precedingStreamId==contf->getHeader().getStreamIdentifier()
				&& (lastFrameType==1 || lastFrameType==5 || lastFrameType==9)) {
			if(!isWebSocket && contf->getHeader().isEndHeaders()) {
				headerBlockFragment += contf->headerBlockFragment;
				request->setHttp2Headers(context->decode(headerBlockFragment));
				void* temp = handleWebSocketRequest(context, frame, handler, settings);
				if(temp!=NULL) {
					requestObj = temp;
					return false;
				}
				//if end of stream was set in the Header frame then change state and process request
				if(endofstream) {
					state = Half_Closed_R;

					sendPushPromiseFrames(context, frame, handler, settings);

					requestObj = getRequestAndReInit();
				}
			} else if(contf->getHeader().isEndHeaders()) {
				std::map<std::string, std::string> wshdrs = context->decode(contf->headerBlockFragment);
				if(wshdrs.find(":opcode")!=wshdrs.end()) {
					try {
						wsrequest->dataType = CastUtil::lexical_cast<short>(wshdrs[":opcode"]);
					} catch(const std::exception& e) {
						//TODO specify proper error
						closeConnection(frame->header.streamIdentifier, handler);
						return true;
					}

					if(isWebSocket && endofstream && contf->getHeader().isEndHeaders()) {
						//TODO handle websocket close
					}
				} else {
					//TODO specify proper error
					closeConnection(frame->header.streamIdentifier, handler);
					return true;
				}
			} else {
				headerBlockFragment += contf->headerBlockFragment;
			}
		} else {
			closeConnection(frame->header.streamIdentifier, handler);
			return true;
		}
	} else if(frame->header.type==5) {
		closeConnection(frame->header.streamIdentifier, handler);
		return true;
	} else if(frame->header.type==4) {
		Http2SettingsFrame* settingsf  = static_cast<Http2SettingsFrame*>(frame);
		//Settings Frame
		if(streamIdentifier==0) {
			settings.insert(settingsf->settings.begin(), settingsf->settings.end());
			if(!settingsf->header.flags.test(0))
			{
				Http2SettingsFrame sframe;
				sframe.header.flags.set(0);
				handler->writeInitData(&sframe);
				std::map<uint16_t, uint32_t>::iterator itt;
				for(itt=settings.begin();itt!=settings.end();++itt) {
					std::cout << "client_settings[" << itt->first << "] = " << itt->second << std::endl;
				}
				if(settings.find(Http2SettingsFrame::SETTINGS_MAX_FRAME_SIZE)!=settings.end()) {
					handler->updateMaxFrameSize(settings[Http2SettingsFrame::SETTINGS_MAX_FRAME_SIZE]);
				}
			}
			else if(frameAcks.find(frame->header.type)!=frameAcks.end() && frameAcks[frame->header.type])
			{
				frameAcks.erase(frame->header.type);
			}
			else
			{
				//Invalid Ack received
				closeConnection(frame->header.streamIdentifier, handler);
				return true;
			}
		} else {
			closeConnection(frame->header.streamIdentifier, handler);
			return true;
		}
	} else if(frame->header.type==6) {
		Http2PingFrame* pingf  = static_cast<Http2PingFrame*>(frame);
		//Settings Frame
		if(streamIdentifier==0) {
			if(!pingf->header.flags.test(0))
			{
				Http2PingFrame pframe;
				pframe.opaqueData = pingf->opaqueData;
				pframe.header.flags.set(0);
				handler->writeInitData(&pframe);
			}
			else if(frameAcks.find(frame->header.type)!=frameAcks.end() && frameAcks[frame->header.type])
			{
				frameAcks.erase(frame->header.type);
			}
			else
			{
				//Invalid Ack received
				closeConnection(frame->header.streamIdentifier, handler);
				return true;
			}
		} else {
			closeConnection(frame->header.streamIdentifier, handler);
			return true;
		}
	} else if(frame->header.type==7) {
		Http2GoAwayFrame* goawayf  = static_cast<Http2GoAwayFrame*>(frame);
		//Go away Frame
		if(streamIdentifier==0) {
			std::cout << "Got go away frame for connection with last stream id " << goawayf->lastStreamId << " " <<
					goawayf->errorCode << " " << goawayf->additionalDebugData << std::endl;
		}
		closeConnection(frame->header.streamIdentifier, handler);
		return true;
	} else if(frame->header.type==8) {
		Http2WindowUpdateFrame* windupdf  = static_cast<Http2WindowUpdateFrame*>(frame);
		//Settings Frame
		if(streamIdentifier>=0) {
			if(streamIdentifier==0)
			{
				handler->updateSenderWindowSize(windupdf->windowSizeIncrement);
				std::cout << "Got window increment size " << windupdf->windowSizeIncrement << " for connection" << std::endl;
			}
			else
			{
				senderFlowControlWindow += windupdf->windowSizeIncrement;
				std::cout << "Got window increment size " << windupdf->windowSizeIncrement << " for stream " << streamIdentifier << std::endl;
			}

			if(pendingSendData.isDataPending())
			{
				handler->writePendingDataFrame(pendingSendData);
			}
		} else {
			closeConnection(frame->header.streamIdentifier, handler);
			return true;
		}
	} else if(frame->header.type==10) {
		Http2AlternativeServicesFrame* alsvcf  = static_cast<Http2AlternativeServicesFrame*>(frame);
		//Settings Frame
		if(streamIdentifier>=0) {
			std::cout << "Got alternative services frame for connection " << alsvcf->protocolId << std::endl;
			//TODO is it even valid for a server context?
		} else {
			closeConnection(frame->header.streamIdentifier, handler);
			return true;
		}
	} else if(state!=Idle) {
		if(frame->header.type==0) {
			//Send Window Update if current window size is not sufficient
			if(receiverFlowControlWindow<=frame->header.payloadLength) {
				Http2WindowUpdateFrame cwuframe;
				cwuframe.windowSizeIncrement = 65535 + frame->header.payloadLength;
				cwuframe.header.streamIdentifier = frame->header.streamIdentifier;
				handler->writeInitData(&cwuframe);
			}
			receiverFlowControlWindow -= frame->header.payloadLength;

			//Data Frame
			Http2DataFrame* dataf  = static_cast<Http2DataFrame*>(frame);
			//Data frame can only be received in the Open/HalfClosedL state
			if(state==Open || state==Half_Closed_L) {
				//if end of stream is reached then change state and process request
				if(!isWebSocket && dataf->getHeader().isEndOfStream()) {
					request->content += dataf->data;
					state = Half_Closed_R;

					requestObj = getRequestAndReInit();
				} else if(dataf->getHeader().isEndOfStream()) {
					wsrequest->data += dataf->data;
					//TODO handle websocket close
				} else if(isWebSocket && dataf->getHeader().isWsEndSegment()) {
					wsrequest->data += dataf->data;
					requestObj = getWsRequestAndReInit();
				} else {
					//else continue appending the data
					if(!isWebSocket)request->content += dataf->data;
					else wsrequest->data += dataf->data;
				}
			} else {
				closeConnection(frame->header.streamIdentifier, handler);
				return true;
			}
		} else if(frame->header.type==3) {
			//Reset Frame
			if(lastFrameType!=9 && frame->header.streamIdentifier!=0) {
				state = Closed;
			} else {
				closeConnection(frame->header.streamIdentifier, handler);
				return true;
			}
		}
	} else {
		closeConnection(frame->header.streamIdentifier, handler);
		return true;
	}
	lastFrameType = frame->getHeader().type;
	delete frame;
	return false;
}


void* Http2StreamHandler::handleWebSocketRequest(Http2HPACKContext* context, Http2Frame* frame, Http2ReadWriteUtil* handler, std::map<uint16_t, uint32_t>& settings)
{
	if(settings.find(Http2SettingsFrame::SETTINGS_WEBSOCKET_CAPABLE)!=settings.end() && isWebSocketRequest())
	{
		std::map<std::string, std::string> wsheaders;
		wsheaders[":status"] = "101";
		wsheaders["sec-websocket-protocol"] = "13";
		Http2HeadersFrame hframe;
		hframe.headerBlockFragment = context->encode(wsheaders);
		hframe.header.streamIdentifier = frame->header.streamIdentifier;
		hframe.header.flags.set(2);
		handler->writeInitData(&hframe);
		isWebSocket = true;
		return getRequestAndReInit();
	}
	return NULL;
}

void Http2StreamHandler::sendPushPromiseFrames(Http2HPACKContext* context, Http2Frame* frame, Http2ReadWriteUtil* handler, std::map<uint16_t, uint32_t>& settings)
{
	if(!isWebSocket && (settings.find(Http2SettingsFrame::SETTINGS_ENABLE_PUSH)==settings.end()
			|| settings[Http2SettingsFrame::SETTINGS_ENABLE_PUSH]!=0)) {
		std::vector<std::string> relEntities = handler->getRelatedEntitiesForPP(request->getHeader(":path"));
		if(relEntities.size()>0) {
			for (int var = 0; var < (int)relEntities.size(); ++var) {
				Http2PushPromiseFrame ppframe;
				ppframe.headers[":method"] = "GET";
				ppframe.headers[":path"] = relEntities.at(var);
				ppframe.headers["host"] = request->getHeader("host");
				//TODO mime-type translation from request
				std::string ext = relEntities.at(var).substr(relEntities.at(var).find(".")+1);
				ppframe.headers["accept"] = handler->getMimeType(ext);
				ppframe.header.flags.set(2);
				ppframe.header.streamIdentifier = frame->header.streamIdentifier;
				ppframe.headerBlockFragment = context->encode(ppframe.headers);
				ppframe.promisedStreamId = handler->getHighestPushPromiseStreamIdentifier();
				handler->writeInitData(&ppframe);
				Http2RequestResponseData ppdat;
				ppdat.preHeaders = ppframe.headers;
				handler->addPushPromisedRequestToQ(ppdat);
			}
		}
	}
}
