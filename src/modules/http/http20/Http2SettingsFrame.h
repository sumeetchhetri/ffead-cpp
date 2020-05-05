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
 * Http2SettingsFrame.h
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2SETTINGSFRAME_H_
#define HTTP2SETTINGSFRAME_H_
#include "Http2Frame.h"
#include "map"

class Http2SettingsFrame : public Http2Frame {
	std::map<uint16_t, uint32_t> settings;
	Http2SettingsFrame(const std::string& data, Http2FrameHeader& aheader);
	friend class Http2Handler;
	friend class Http2StreamHandler;
public:
	static uint16_t SETTINGS_HEADER_TABLE_SIZE;
	static uint16_t SETTINGS_ENABLE_PUSH;
	static uint16_t SETTINGS_MAX_CONCURRENT_STREAMS;
	static uint16_t SETTINGS_INITIAL_WINDOW_SIZE;
	static uint16_t SETTINGS_MAX_FRAME_SIZE;
	static uint16_t SETTINGS_MAX_HEADER_LIST_SIZE;

	//Not the final spec yet, change this when the real rfc is finalized
	//https://tools.ietf.org/html/draft-hirano-httpbis-websocket-over-http2-01
	static uint16_t SETTINGS_WEBSOCKET_CAPABLE;
	Http2SettingsFrame();
	virtual ~Http2SettingsFrame();
	const std::map<uint16_t, uint32_t>& getSettings() const;
	std::string getFrameData();
};

#endif /* HTTP2SETTINGSFRAME_H_ */
