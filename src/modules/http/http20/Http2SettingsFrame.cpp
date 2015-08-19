/*
 * Http2SettingsFrame.cpp
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#include "Http2SettingsFrame.h"

uint16_t Http2SettingsFrame::SETTINGS_HEADER_TABLE_SIZE = 1;
uint16_t Http2SettingsFrame::SETTINGS_ENABLE_PUSH = 2;
uint16_t Http2SettingsFrame::SETTINGS_MAX_CONCURRENT_STREAMS = 3;
uint16_t Http2SettingsFrame::SETTINGS_INITIAL_WINDOW_SIZE = 4;
uint16_t Http2SettingsFrame::SETTINGS_MAX_FRAME_SIZE = 5;
uint16_t Http2SettingsFrame::SETTINGS_MAX_HEADER_LIST_SIZE = 6;
uint16_t Http2SettingsFrame::SETTINGS_WEBSOCKET_CAPABLE = 7;//just a default value for now will change once spec if finalized

Http2SettingsFrame::Http2SettingsFrame() {
	header.type = 4;
	header.streamIdentifier = 0;
}

Http2SettingsFrame::Http2SettingsFrame(const string& data, Http2FrameHeader& aheader) {
	header = aheader;
	header.type = 4;
	header.streamIdentifier = 0;
	size_t indx = 0;
	while((indx+6)<=data.length())
	{
		uint16_t sk = (uint16_t)CommonUtils::charArrayToULongLong(data.substr(indx, 2));
		indx += 2;
		uint32_t sv = (uint32_t)CommonUtils::charArrayToULongLong(data.substr(indx, 4));
		indx += 4;
		settings[sk] = sv;
	}
}

const map<uint16_t, uint32_t>& Http2SettingsFrame::getSettings() const {
	return settings;
}

Http2SettingsFrame::~Http2SettingsFrame() {
	// TODO Auto-generated destructor stub
}

string Http2SettingsFrame::getFrameData() {
	string edata;
	map<uint16_t, uint32_t>::iterator it;
	for (it=settings.begin();it!=settings.end();++it) {
		edata.append(CommonUtils::ulonglongTocharArray(it->first, 2));
		edata.append(CommonUtils::ulonglongTocharArray(it->second, 4));
	}
	return edata;
}

