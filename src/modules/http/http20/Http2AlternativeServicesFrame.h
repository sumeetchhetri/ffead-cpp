/*
 * Http2AlternativeServicesFrame.h
 *
 *  Created on: 22-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2ALTERNATIVESERVICESFRAME_H_
#define HTTP2ALTERNATIVESERVICESFRAME_H_
#include "Http2Frame.h"

class Http2AlternativeServicesFrame : public Http2Frame {
	unsigned int maxAge;
	uint16_t port;
	unsigned char protoLength;
	std::string protocolId;
	unsigned char hostLength;
	std::string host;
	std::string origin;
	Http2AlternativeServicesFrame(const std::string& data, Http2FrameHeader& header);
	friend class Http2Handler;
	friend class Http2StreamHandler;
public:
	Http2AlternativeServicesFrame();
	virtual ~Http2AlternativeServicesFrame();
	std::string getFrameData();
	const std::string& getHost() const;
	unsigned char getHostLength() const;
	unsigned int getMaxAge() const;
	const std::string& getOrigin() const;
	uint16_t getPort() const;
	const std::string& getProtocolId() const;
	unsigned char getProtoLength() const;
};

#endif /* HTTP2ALTERNATIVESERVICESFRAME_H_ */
