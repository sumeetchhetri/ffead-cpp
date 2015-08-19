/*
 * Http2ContinuationFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2ContinuationFrame.h"

Http2ContinuationFrame::Http2ContinuationFrame() {
	header.type = 9;
}

Http2ContinuationFrame::Http2ContinuationFrame(const string& data, Http2FrameHeader& aheader) {
	header = aheader;
	header.type = 9;
	headerBlockFragment = data;
}

const string& Http2ContinuationFrame::getHeaderBlockFragment() const {
	return headerBlockFragment;
}

Http2ContinuationFrame::~Http2ContinuationFrame() {
	// TODO Auto-generated destructor stub
}

string Http2ContinuationFrame::getFrameData() {
	return headerBlockFragment;
}
