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

const string& Http2AlternativeServicesFrame::getHost() const {
	return host;
}

unsigned char Http2AlternativeServicesFrame::getHostLength() const {
	return hostLength;
}

unsigned int Http2AlternativeServicesFrame::getMaxAge() const {
	return maxAge;
}

const string& Http2AlternativeServicesFrame::getOrigin() const {
	return origin;
}

uint16_t Http2AlternativeServicesFrame::getPort() const {
	return port;
}

const string& Http2AlternativeServicesFrame::getProtocolId() const {
	return protocolId;
}

unsigned char Http2AlternativeServicesFrame::getProtoLength() const {
	return protoLength;
}

Http2AlternativeServicesFrame::Http2AlternativeServicesFrame(const string& data, Http2FrameHeader& aheader) {
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
	// TODO Auto-generated destructor stub
}

string Http2AlternativeServicesFrame::getFrameData() {
	string edata;
	edata.append(CommonUtils::ulonglongTocharArray(maxAge, 4));
	edata.append(CommonUtils::ulonglongTocharArray(port, 2));
	edata.push_back(protoLength);
	edata.append(protocolId);
	edata.push_back(hostLength);
	edata.append(host);
	edata.append(origin);
	return edata;
}
