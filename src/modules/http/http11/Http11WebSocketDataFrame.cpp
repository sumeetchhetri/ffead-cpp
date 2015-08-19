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

string Http11WebSocketDataFrame::getPayloadData() const {
	if(extensionData.size()==0)
		return applicationData;
	string tpayloadData = extensionData;
	tpayloadData.append(applicationData);
	return tpayloadData;
}

const string& Http11WebSocketDataFrame::getApplicationData() const {
	return applicationData;
}

void Http11WebSocketDataFrame::setApplicationData(const string& applicationData) {
	this->applicationData = applicationData;
}

const string& Http11WebSocketDataFrame::getExtensionData() const {
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

string Http11WebSocketDataFrame::getFrameData() {
	string finaldata;
	bitset<8> firstBits;
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
	return finaldata;
}

WebSocketData::WebSocketData(const string& data, const short& dataType)
{
	this->data = data;
	this->dataType = dataType;
	this->tecurrpart = 0;
	this->teparts = 0;
	this->techunkSiz = 0;
}

WebSocketData::WebSocketData() {
	this->tecurrpart = 0;
	this->teparts = 0;
	this->techunkSiz = 0;
	dataType = -1;
}

WebSocketData::~WebSocketData() {}

string WebSocketData::getData() const {
	return data;
}

void WebSocketData::updateContent(const uint32_t& techunkSiz) {
	unsigned int totlen = data.length();
	if(techunkSiz>0 && techunkSiz<totlen)
	{
		this->techunkSiz = techunkSiz;
		float parts = techunkSiz!=0?(float)totlen/techunkSiz:0;
		parts = (floor(parts)<parts?floor(parts)+1:floor(parts));
		this->teparts = (int)parts;
	}
}

bool WebSocketData::isContentRemains() {
	return teparts>0 && tecurrpart<teparts;
}

string WebSocketData::getRemainingContent() {
	string rem;
	if(isContentRemains()) {
		unsigned int totlen = data.length();
		unsigned int len = totlen - techunkSiz*tecurrpart;
		if((int)len>techunkSiz)
		{
			len = techunkSiz;
		}
		rem = data.substr(techunkSiz*tecurrpart, len);
		tecurrpart++;
	}
	return rem;
}
