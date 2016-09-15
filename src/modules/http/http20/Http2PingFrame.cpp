/*
 * Http2PingFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2PingFrame.h"

Http2PingFrame::Http2PingFrame() {
	header.type = 6;
	opaqueData = -1;
}

Http2PingFrame::Http2PingFrame(const std::string& data, Http2FrameHeader& aheader) {
	header = aheader;
	header.type = 6;
	opaqueData = (uint64_t)CommonUtils::charArrayToULongLong(data.substr(0, 8));
}

uint64_t Http2PingFrame::getOpqueData() const {
	return opaqueData;
}

Http2PingFrame::~Http2PingFrame() {
	// TODO Auto-generated destructor stub
}

std::string Http2PingFrame::getFrameData() {
	std::string edata;
	edata.append(CommonUtils::ulonglongTocharArray(opaqueData, 8));
	return edata;
}
