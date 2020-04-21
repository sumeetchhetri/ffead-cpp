/*
 * WebSocketDatathis->cpp
 *
 *  Created on: 30-Nov-2014
 *      Author: sumeetc
 */

#include "Http11WebSocketDataFrame.h"

Http11WebSocketDataFrame::Http11WebSocketDataFrame() {
	fin = false;
	rsv1 = false;
	rsv2 = false;
	rsv3 = false;
	mask = false;
	opcode = 0;
	payloadLength = 0;
	extendedPayloadLength = 0;
	maskingKey = 0;
}

Http11WebSocketDataFrame::~Http11WebSocketDataFrame() {
}

std::string Http11WebSocketDataFrame::getPayloadData() const {
	if(extensionData.size()==0)
		return applicationData;
	std::string tpayloadData = extensionData;
	tpayloadData.append(applicationData);
	return tpayloadData;
}

const std::string& Http11WebSocketDataFrame::getApplicationData() const {
	return applicationData;
}

void Http11WebSocketDataFrame::setApplicationData(const std::string& applicationData) {
	this->applicationData = applicationData;
}

const std::string& Http11WebSocketDataFrame::getExtensionData() const {
	return extensionData;
}

uint64_t Http11WebSocketDataFrame::getExtendedPayloadLength() const {
	return extendedPayloadLength;
}

bool Http11WebSocketDataFrame::isFin() const {
	return fin;
}

bool Http11WebSocketDataFrame::isMask() const {
	return mask;
}

uint64_t Http11WebSocketDataFrame::getMaskingKey() const {
	return maskingKey;
}

short Http11WebSocketDataFrame::getOpcode() const {
	return opcode;
}

uint8_t Http11WebSocketDataFrame::getPayloadLength() const {
	return payloadLength;
}

bool Http11WebSocketDataFrame::isRsv1() const {
	return rsv1;
}

bool Http11WebSocketDataFrame::isRsv2() const {
	return rsv2;
}

bool Http11WebSocketDataFrame::isRsv3() const {
	return rsv3;
}

void Http11WebSocketDataFrame::getFrameData(std::string &finaldata) {
	std::bitset<8> firstBits;
	firstBits.set(7, this->fin);
	firstBits.set(6, this->rsv1);
	firstBits.set(5, this->rsv2);
	firstBits.set(4, this->rsv3);
	firstBits.set(3, (this->opcode >> 3) & 0x01);
	firstBits.set(2, (this->opcode >> 2) & 0x01);
	firstBits.set(1, (this->opcode >> 1) & 0x01);
	firstBits.set(0, this->opcode & 0x01);
	finaldata.push_back((unsigned char)firstBits.to_ulong());
	firstBits.reset();
	firstBits.set(7, this->mask);
	for(int u=6;u>=0;u--) {
		firstBits.set(u, (this->payloadLength >> u) & 0x01);
	}
	finaldata.push_back((unsigned char)firstBits.to_ulong());
	if(this->payloadLength==126) {
		finaldata.append(CommonUtils::ulonglongTocharArray(this->extendedPayloadLength, 2));
	} else if(this->payloadLength==127) {
		finaldata.append(CommonUtils::ulonglongTocharArray(this->extendedPayloadLength, 8));
	}
	if(this->mask) {
		finaldata.append(CommonUtils::ulonglongTocharArray(this->maskingKey, 4));
		this->applicationData = CommonUtils::xorEncryptDecrypt(this->applicationData, this->maskingKey);
	}
	for(int u=0;u<(int)this->applicationData.size();u++) {
		finaldata.push_back(this->applicationData.at(u));
	}
}

std::string Http11WebSocketDataFrame::getFrameData() {
	std::string finaldata;
	getFrameData(finaldata);
	return finaldata;
}

WebSocketData::WebSocketData(){}

bool WebSocketData::hasData() {
	return textData.length()>0 || binaryData.length()>0;
}

WebSocketData::~WebSocketData() {}

void WebSocketData::collectText(const std::string& data) {
	textData.append(data);
}

void WebSocketData::collectBinary(const std::string& data) {
	binaryData.append(data);
}

std::string WebSocketData::getTextData() const {
	return textData;
}

std::string WebSocketData::getBinaryData() const {
	return binaryData;
}

std::string WebSocketData::getUrl() const {
	return url;
}

std::string WebSocketData::getCntxt_name() const {
	return cnxtName;
}

void WebSocketRespponseData::pushText(const std::string &textData) {
	more.push_back(WebSocketData());
	more.at(more.size()-1).textData = textData;
}

void WebSocketRespponseData::pushBinary(const std::string &binaryData) {
	more.push_back(WebSocketData());
	more.at(more.size()-1).binaryData = binaryData;
}

std::vector<WebSocketData> WebSocketRespponseData::getMore() {
	return more;
}

bool WebSocketRespponseData::isEmpty() {
	return more.size()==0;
}

WebSocketRespponseData::WebSocketRespponseData() {}

void WebSocketRespponseData::reset() {
	more.clear();
}

WebSocketRespponseData::~WebSocketRespponseData() {
}

void Http11WebSocketDataFrame::getFramePdu(WebSocketData* wres, std::string& data) {
	Http11WebSocketDataFrame frame;
	frame.fin = true;
	frame.rsv1 = false;
	frame.rsv2 = false;
	frame.rsv3 = false;
	frame.mask = false;
	if(wres->textData.length()>0) {
		frame.opcode = 1;
		int payloadLength = wres->textData.length();
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
		data += wres->textData;
	}
	if(wres->binaryData.length()>0) {
		frame.opcode = 2;
		int payloadLength = wres->binaryData.length();
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
		data += wres->binaryData;
	}
}
