/*
 * Http2PriorityFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2PriorityFrame.h"

Http2PriorityFrame::Http2PriorityFrame() {
	header.type = 2;
	weight = 0;
	streamDependency = -1;
	exclusive = false;
}

bool Http2PriorityFrame::isExclusive() const {
	return exclusive;
}

int Http2PriorityFrame::getStreamDependency() const {
	return streamDependency;
}

Http2PriorityFrame::Http2PriorityFrame(std::string data, Http2FrameHeader& aheader) {
	header = aheader;
	header.type = 2;
	exclusive = ((data.at(0) >> 7) & 0x01);
	data[0] = data[0] & 0x7F;
	streamDependency = (int)CommonUtils::charArrayToULongLong(data.substr(0, 4));
	weight = data.at(4);
}

unsigned char Http2PriorityFrame::getWeight() const {
	return weight;
}

Http2PriorityFrame::~Http2PriorityFrame() {
	
}

std::string Http2PriorityFrame::getFrameData() {
	std::string edata;
	std::string data = CommonUtils::ulonglongTocharArray(streamDependency, 4);
	if(exclusive)
		data[0] |= 0x01 << 7;
	else
		data[0] &= ~(0x01 << 7);
	edata.append(data);
	edata.push_back(weight);
	return edata;
}

