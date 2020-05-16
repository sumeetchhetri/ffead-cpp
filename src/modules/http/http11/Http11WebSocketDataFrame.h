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
 * Http11WebSocketDataFrame.h
 *
 *  Created on: 30-Nov-2014
 *      Author: sumeetc
 */

#ifndef HTTP11WEBSOCKETDATAFRAME_H_
#define HTTP11WEBSOCKETDATAFRAME_H_
#include "CommonUtils.h"
#include "string"
#include <math.h>

/*
 * 	 |Opcode  | Meaning                             | Reference |
    -+--------+-------------------------------------+-----------|
     | 0      | Continuation Frame                  | RFC 6455  |
    -+--------+-------------------------------------+-----------|
     | 1      | Text Frame                          | RFC 6455  |
    -+--------+-------------------------------------+-----------|
     | 2      | Binary Frame                        | RFC 6455  |
    -+--------+-------------------------------------+-----------|
     | 8      | Connection Close Frame              | RFC 6455  |
    -+--------+-------------------------------------+-----------|
     | 9      | Ping Frame                          | RFC 6455  |
    -+--------+-------------------------------------+-----------|
     | 10     | Pong Frame                          | RFC 6455  |
    -+--------+-------------------------------------+-----------|
 */
class WebSocketData {
	std::string url;
	std::string cnxtName;
	std::string textData;
	std::string binaryData;
	friend class Http11WebSocketHandler;
	friend class Http2StreamHandler;
	friend class Http2Handler;
	friend class HttpServiceTask;
	friend class ServiceTask;
	friend class WebSocketRespponseData;
	friend class Http11WebSocketDataFrame;
	friend class WebSockHandler;
public:
	bool hasData();
	void collectText(const std::string& data);
	void collectBinary(const std::string& data);
	std::string getTextData() const;
	std::string getBinaryData() const;
	std::string getUrl() const;
	std::string getCntxt_name() const;
	WebSocketData();
	virtual ~WebSocketData();
};

class WebSocketRespponseData {
	std::vector<WebSocketData> more;
	friend class Http11WebSocketHandler;
	friend class Http2StreamHandler;
	friend class Http2Handler;
	friend class HttpServiceTask;
	friend class ServiceTask;
	friend class Http11WebSocketDataFrame;
public:
	bool isEmpty();
	void pushText(const std::string& textData);
	void pushBinary(const std::string& binaryData);
	std::vector<WebSocketData>& getMore();
	void reset();
	WebSocketRespponseData();
	virtual ~WebSocketRespponseData();
};

class Http11WebSocketDataFrame {
	bool fin;
	bool rsv1;
	bool rsv2;
	bool rsv3;
	short opcode;
	bool mask;
	uint8_t payloadLength;
	uint64_t extendedPayloadLength;
	uint32_t maskingKey;
	std::string extensionData;
	std::string applicationData;
	friend class Http11WebSocketHandler;
public:
	Http11WebSocketDataFrame();
	virtual ~Http11WebSocketDataFrame();
	std::string getPayloadData() const;
	const std::string& getApplicationData() const;
	void setApplicationData(const std::string& applicationData);
	const std::string& getExtensionData() const;
	uint64_t getExtendedPayloadLength() const;
	bool isFin() const;
	bool isMask() const;
	uint64_t getMaskingKey() const;
	short getOpcode() const;
	uint8_t getPayloadLength() const;
	bool isRsv1() const;
	bool isRsv2() const;
	bool isRsv3() const;
	static int getFramePdu(WebSocketData* wres, std::string& data, bool copyData);
	void getFrameData(std::string& data);
	std::string getFrameData();
};

#endif /* HTTP11WEBSOCKETDATAFRAME_H_ */
