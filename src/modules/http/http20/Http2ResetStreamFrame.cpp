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

