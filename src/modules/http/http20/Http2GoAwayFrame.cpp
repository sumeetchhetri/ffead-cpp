/*
 * Http2GoAwayFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2GoAwayFrame.h"

Http2GoAwayFrame::Http2GoAwayFrame() {
	header.type = 7;
	lastStreamId = -1;
	errorCode = -1;
	reserved = false;
}

const string& Http2GoAwayFrame::getAdditionalDebugData() const {
	return additionalDebugData;
}

uint32_t Http2GoAwayFrame::getErrorCode() const {
	return errorCode;
}

int Http2GoAwayFrame::getLastStreamId() const {
	return lastStreamId;
}

Http2GoAwayFrame::Http2GoAwayFrame(string data, Http2FrameHeader& aheader) {
	header = aheader;
	header.type = 7;
	reserved = ((data.at(0) >> 7) & 0x01);
	data[0] = data[0] & 0x7F;
	lastStreamId = (int)CommonUtils::charArrayToULongLong(data.substr(0, 4));
	errorCode = (uint32_t)CommonUtils::charArrayToULongLong(data.substr(4, 4));
	additionalDebugData = data.substr(8);
}

bool Http2GoAwayFrame::isReserved() const {
	return reserved;
}

Http2GoAwayFrame::~Http2GoAwayFrame() {
	// TODO Auto-generated destructor stub
}

string Http2GoAwayFrame::getFrameData() {
	string edata;
	string data = CommonUtils::ulonglongTocharArray(lastStreamId, 4);
	if(reserved)
		data[0] |= 0x01 << 0;
	else
		data[0] &= ~(0x01 << 0);
	edata.append(data);
	edata.append(CommonUtils::ulonglongTocharArray(errorCode, 4));
	edata.append(additionalDebugData);
	return edata;
}

