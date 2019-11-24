/*
 * Http2ResetStreamFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2ResetStreamFrame.h"

Http2ResetStreamFrame::Http2ResetStreamFrame() {
	header.type = 3;
	errorCode = -1;
}

Http2ResetStreamFrame::Http2ResetStreamFrame(const std::string& data, Http2FrameHeader& aheader) {
	header= aheader;
	header.type = 3;
	errorCode = (unsigned int)CommonUtils::charArrayToULongLong(data.substr(0, 4));
}

unsigned int Http2ResetStreamFrame::getErrorCode() const {
	return errorCode;
}

Http2ResetStreamFrame::~Http2ResetStreamFrame() {
	
}

std::string Http2ResetStreamFrame::getFrameData() {
	std::string edata;
	edata.append(CommonUtils::ulonglongTocharArray(errorCode, 4));
	return edata;
}

