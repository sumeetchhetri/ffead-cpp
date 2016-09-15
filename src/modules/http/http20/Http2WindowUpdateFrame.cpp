/*
 * Http2WindowUpdateFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2WindowUpdateFrame.h"

Http2WindowUpdateFrame::Http2WindowUpdateFrame() {
	header.type = 8;
	windowSizeIncrement = 0;
	reserved = false;
}

bool Http2WindowUpdateFrame::isReserved() const {
	return reserved;
}

Http2WindowUpdateFrame::Http2WindowUpdateFrame(std::string data, Http2FrameHeader& aheader) {
	header = aheader;
	header.type = 8;
	reserved = ((data[0] >> 7) & 0x01);
	data[0] = data[0] & 0x7F;
	windowSizeIncrement = (int)CommonUtils::charArrayToULongLong(data.substr(0, 4));
}

int Http2WindowUpdateFrame::getWindowSizeIncrement() const {
	return windowSizeIncrement;
}

Http2WindowUpdateFrame::~Http2WindowUpdateFrame() {
	// TODO Auto-generated destructor stub
}

std::string Http2WindowUpdateFrame::getFrameData() {
	std::string edata;
	std::string data = CommonUtils::ulonglongTocharArray(windowSizeIncrement, 4);
	if(reserved)
		data[0] |= 0x01 << 7;
	else
		data[0] &= ~(0x01 << 7);
	edata.append(data);
	return edata;
}
