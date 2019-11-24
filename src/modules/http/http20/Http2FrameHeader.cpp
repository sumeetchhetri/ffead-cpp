/*
 * Http2Frame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2FrameHeader.h"

Http2FrameHeader::Http2FrameHeader() {
	streamIdentifier = 0;
	payloadLength = 0;
	reserved = false;
	type = 0;
	flags.reset();
}

std::bitset<8> Http2FrameHeader::getFlags() const {
	return flags;
}

int Http2FrameHeader::getPayloadLength() const {
	return payloadLength;
}

bool Http2FrameHeader::isReserved() const {
	return reserved;
}

int Http2FrameHeader::getStreamIdentifier() const {
	return streamIdentifier;
}

unsigned char Http2FrameHeader::getType() const {
	return type;
}

Http2FrameHeader::~Http2FrameHeader() {
	
}

bool Http2FrameHeader::isEndOfStream() {
	return flags.test(0);
}

bool Http2FrameHeader::isPadded() {
	return flags.test(3);
}

bool Http2FrameHeader::isEndHeaders() {
	return flags.test(2);
}

bool Http2FrameHeader::isPriority() {
	return flags.test(5);
}

bool Http2FrameHeader::isSettingsAck() {
	return type==4 && flags.test(0);
}

bool Http2FrameHeader::isPingAck() {
	return type==6 && flags.test(0);
}

bool Http2FrameHeader::isWsEndSegment() {
	return flags.test(6);
}
