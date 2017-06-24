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
	std::string getFrameData();
};

class WebSocketData {
	std::string url;
	std::string cnxtName;
	std::string data;
	short dataType;
	int techunkSiz;
	int teparts;
	int tecurrpart;
	friend class Http11WebSocketHandler;
	friend class Http2StreamHandler;
	friend class Http2Handler;
	friend class HttpServiceTask;
public:
	WebSocketData();
	WebSocketData(const std::string& data, const short& dataType);
	std::string getData() const;
	std::string getUrl() const;
	std::string getCntxt_name() const;
	virtual ~WebSocketData();
	void updateContent(const uint32_t& techunkSiz);
	bool isContentRemains();
	std::string getRemainingContent();
};

#endif /* HTTP11WEBSOCKETDATAFRAME_H_ */
