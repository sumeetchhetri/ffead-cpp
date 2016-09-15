/*
 * Http2Frame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2Frame.h"

Http2Frame::~Http2Frame() {
	// TODO Auto-generated destructor stub
}

/*string Http2Frame::getFrameData() {
	std::string finaldata;
	finaldata.append(CommonUtils::ulonglongTocharArray(header.payloadLength, 3));
	finaldata.push_back(header.type);
	//finaldata.push_back(header.flags);
	std::string rstib = CommonUtils::ulonglongTocharArray(header.streamIdentifier, 4);
	if(header.reserved)
	{
		rstib[0] |= 0x01 << 1;
	}
	finaldata.append(rstib);
	finaldata.append(payload);
	return finaldata;
}*/

Http2FrameHeader& Http2Frame::getHeader() {
	return header;
}
