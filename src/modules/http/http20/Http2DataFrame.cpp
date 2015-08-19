/*
 * Http2DataFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2DataFrame.h"

Http2DataFrame::Http2DataFrame() {
	header.type = 0;
	padLength = 0;
}

const string& Http2DataFrame::getData() const {
	return data;
}

const string& Http2DataFrame::getPadding() const {
	return padding;
}

Http2DataFrame::Http2DataFrame(const string& data, Http2FrameHeader& aheader) {
	this->header = aheader;
	header.type = 0;
	if(header.isPadded()) {
		padLength = data.at(0);
		this->data = data.substr(1, data.length()-padLength-1);
		padding = data.substr(data.length()-padLength);
	} else {
		this->data = data;
	}
}

unsigned char Http2DataFrame::getPadLength() const {
	return padLength;
}

Http2DataFrame::~Http2DataFrame() {
	// TODO Auto-generated destructor stub
}

string Http2DataFrame::getFrameData() {
	string edata;
	if(header.isPadded()) {
		edata.push_back(padLength);
	}
	edata.append(data);
	if(header.isPadded()) {
		edata.append(padding);
	}
	return edata;
}
