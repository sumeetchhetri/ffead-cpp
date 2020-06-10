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

const std::string& Http2GoAwayFrame::getAdditionalDebugData() const {
	return additionalDebugData;
}

uint32_t Http2GoAwayFrame::getErrorCode() const {
	return errorCode;
}

int Http2GoAwayFrame::getLastStreamId() const {
	return lastStreamId;
}

Http2GoAwayFrame::Http2GoAwayFrame(std::string data, Http2FrameHeader& aheader) {
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
	
}

std::string Http2GoAwayFrame::getFrameData() {
	std::string edata;
	std::string data = CommonUtils::ulonglongTocharArray(lastStreamId, 4);
	if(reserved)
		data[0] |= 0x01 << 0;
	else
		data[0] &= ~(0x01 << 0);
	edata.append(data);
	edata.append(CommonUtils::ulonglongTocharArray(errorCode, 4));
	edata.append(additionalDebugData);
	return edata;
}

