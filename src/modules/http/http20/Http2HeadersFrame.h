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
 * Http2HeadersFrame.h
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2HEADERSFRAME_H_
#define HTTP2HEADERSFRAME_H_
#include "Http2Frame.h"

class Http2HeadersFrame : public Http2Frame {
	unsigned char padLength;
	bool exclusive;
	int streamDependency;
	unsigned char weight;
	std::string headerBlockFragment;
	std::string padding;
	Http2HeadersFrame(std::string data, Http2FrameHeader& header);
	friend class Http2Handler;
	friend class Http2StreamHandler;
public:
	Http2HeadersFrame();
	virtual ~Http2HeadersFrame();
	bool isExclusive() const;
	const std::string& getHeaderBlockFragment() const;
	void setHeaderBlockFragment(const std::string& headerBlockFragment);
	const std::string& getPadding() const;
	unsigned char getPadLength() const;
	int getStreamDependency() const;
	unsigned char getWeight() const;
	std::string getFrameData();
};

#endif /* HTTP2HEADERSFRAME_H_ */
