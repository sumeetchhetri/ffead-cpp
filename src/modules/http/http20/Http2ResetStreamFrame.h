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
 * Http2ResetStreamFrame.h
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2RESETSTREAMFRAME_H_
#define HTTP2RESETSTREAMFRAME_H_
#include "Http2Frame.h"

class Http2ResetStreamFrame : public Http2Frame {
	unsigned int errorCode;
	Http2ResetStreamFrame(const std::string& data, Http2FrameHeader& aheader);
	friend class Http2Handler;
public:
	Http2ResetStreamFrame();
	virtual ~Http2ResetStreamFrame();
	unsigned int getErrorCode() const;
	std::string getFrameData();
};

#endif /* HTTP2RESETSTREAMFRAME_H_ */
