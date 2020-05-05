/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
/*
 * Http2HeadersFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2HeadersFrame.h"

Http2HeadersFrame::Http2HeadersFrame() {
	header.type = 1;
	padLength = 0;
	streamDependency = 0;
	exclusive = false;
	weight = 0;
}

bool Http2HeadersFrame::isExclusive() const {
	return exclusive;
}

const std::string& Http2HeadersFrame::getHeaderBlockFragment() const {
	return headerBlockFragment;
}

void Http2HeadersFrame::setHeaderBlockFragment(
		const std::string& headerBlockFragment) {
	this->headerBlockFragment = headerBlockFragment;
}

const std::string& Http2HeadersFrame::getPadding() const {
	return padding;
}

unsigned char Http2HeadersFrame::getPadLength() const {
	return padLength;
}

int Http2HeadersFrame::getStreamDependency() const {
	return streamDependency;
}

Http2HeadersFrame::Http2HeadersFrame(std::string data, Http2FrameHeader& aheader) {
	header = aheader;
	header.type = 1;
	padLength = 0;
	int stind = 0;
	if(header.isPadded()) {
		padLength = data.at(0);
		stind++;
	}
	if(header.isPriority()) {
		exclusive = ((data.at(stind) >> 7) & 0x01);
		data[stind] = data[stind] & 0x7F;
		streamDependency = (int)CommonUtils::charArrayToULongLong(data.substr(stind, 4));
		stind += 4;
		weight = data.at(stind);
		stind++;
	}
	if(header.isPadded()) {
		headerBlockFragment = data.substr(stind, data.length()-padLength-stind);
		padding = data.substr(data.length()-padLength);
	} else {
		headerBlockFragment = data.substr(stind);
	}
}

unsigned char Http2HeadersFrame::getWeight() const {
	return weight;
}

Http2HeadersFrame::~Http2HeadersFrame() {
	
}

std::string Http2HeadersFrame::getFrameData() {
	std::string edata;
	if(header.isPadded()) {
		edata.push_back(padLength);
	}
	if(header.isPriority()) {
		std::string data = CommonUtils::ulonglongTocharArray(streamDependency, 4);
		if(exclusive)
			data[0] |= 0x01 << 7;
		else
			data[0] &= ~(0x01 << 7);
		edata.append(data);
		edata.push_back(weight);
	}
	edata.append(headerBlockFragment);
	if(header.isPadded()) {
		edata.append(padding);
	}
	return edata;
}
