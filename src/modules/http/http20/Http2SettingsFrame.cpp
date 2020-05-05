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

Http2SettingsFrame::Http2SettingsFrame(const std::string& data, Http2FrameHeader& aheader) {
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

const std::map<uint16_t, uint32_t>& Http2SettingsFrame::getSettings() const {
	return settings;
}

Http2SettingsFrame::~Http2SettingsFrame() {
	
}

std::string Http2SettingsFrame::getFrameData() {
	std::string edata;
	std::map<uint16_t, uint32_t>::iterator it;
	for (it=settings.begin();it!=settings.end();++it) {
		edata.append(CommonUtils::ulonglongTocharArray(it->first, 2));
		edata.append(CommonUtils::ulonglongTocharArray(it->second, 4));
	}
	return edata;
}

