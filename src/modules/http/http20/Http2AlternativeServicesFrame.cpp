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
 * Http2AlternativeServicesFrame.cpp
 *
 *  Created on: 22-Dec-2014
 *      Author: sumeetc
 */

#include "Http2AlternativeServicesFrame.h"

Http2AlternativeServicesFrame::Http2AlternativeServicesFrame() {
	header.type = 10;
	protoLength = 0;
	hostLength = 0;
	maxAge = 0;
	port = 0;
}

const std::string& Http2AlternativeServicesFrame::getHost() const {
	return host;
}

unsigned char Http2AlternativeServicesFrame::getHostLength() const {
	return hostLength;
}

unsigned int Http2AlternativeServicesFrame::getMaxAge() const {
	return maxAge;
}

const std::string& Http2AlternativeServicesFrame::getOrigin() const {
	return origin;
}

uint16_t Http2AlternativeServicesFrame::getPort() const {
	return port;
}

const std::string& Http2AlternativeServicesFrame::getProtocolId() const {
	return protocolId;
}

unsigned char Http2AlternativeServicesFrame::getProtoLength() const {
	return protoLength;
}

Http2AlternativeServicesFrame::Http2AlternativeServicesFrame(const std::string& data, Http2FrameHeader& aheader) {
	header = aheader;
	header.type = 10;
	maxAge = (unsigned int)CommonUtils::charArrayToULongLong(data.substr(0, 4));
	port = (uint16_t)CommonUtils::charArrayToULongLong(data.substr(4, 2));
	protoLength = (unsigned char)data.at(6);
	protocolId = data.substr(7, protoLength);
	size_t index = 7 + protoLength;
	hostLength = (unsigned char)data.at(index);
	index++;
	host = data.substr(index, hostLength);
	index += hostLength;
	origin = data.substr(index);
}

Http2AlternativeServicesFrame::~Http2AlternativeServicesFrame() {
	
}

std::string Http2AlternativeServicesFrame::getFrameData() {
	std::string edata;
	edata.append(CommonUtils::ulonglongTocharArray(maxAge, 4));
	edata.append(CommonUtils::ulonglongTocharArray(port, 2));
	edata.push_back(protoLength);
	edata.append(protocolId);
	edata.push_back(hostLength);
	edata.append(host);
	edata.append(origin);
	return edata;
}
