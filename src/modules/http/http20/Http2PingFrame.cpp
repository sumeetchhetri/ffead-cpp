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
	
}

std::string Http2PingFrame::getFrameData() {
	std::string edata;
	edata.append(CommonUtils::ulonglongTocharArray(opaqueData, 8));
	return edata;
}
