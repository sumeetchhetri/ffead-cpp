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
 * Http2DataFrame.h
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2DATAFRAME_H_
#define HTTP2DATAFRAME_H_
#include "Http2Frame.h"

class Http2DataFrame : public Http2Frame {
	unsigned char padLength;
	std::string data;
	std::string padding;
	Http2DataFrame(const std::string& data, Http2FrameHeader& aheader);
	friend class Http2Handler;
	friend class Http2StreamHandler;
public:
	Http2DataFrame();
	virtual ~Http2DataFrame();
	const std::string& getData() const;
	const std::string& getPadding() const;
	unsigned char getPadLength() const;
	std::string getFrameData();
};

#endif /* HTTP2DATAFRAME_H_ */
