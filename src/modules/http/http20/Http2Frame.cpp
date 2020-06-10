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
 * Http2Frame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2Frame.h"

Http2Frame::~Http2Frame() {
	
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
