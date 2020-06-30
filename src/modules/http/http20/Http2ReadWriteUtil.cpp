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
 * Http2ReadWriteUtil.cpp
 *
 *  Created on: 13-Dec-2014
 *      Author: sumeetc
 */
#include "Http2ReadWriteUtil.h"

Http2RequestResponseData::Http2RequestResponseData() {
	endStream = false;
	incompleteResponse = NULL;
	isWebSocket = false;
	streamIdentifier = -1;
}

Http2RequestResponseData::~Http2RequestResponseData() {
}

const std::string& Http2RequestResponseData::getData() const {
	return data;
}

void Http2RequestResponseData::reset() {
	data = "";
	headerBlock = "";
	preHeaders.clear();
	endStream = false;
	if(incompleteResponse!=NULL) {
		if(isWebSocket)
			delete (WebSocketData*)incompleteResponse;
		else
			delete (HttpRequest*)incompleteResponse;
	}
	incompleteResponse = NULL;
}

bool Http2RequestResponseData::isDataPending() {
	if(!isWebSocket) {
		HttpResponse* res = (HttpResponse*)incompleteResponse;
		return data.length()>0 || (res!=NULL && res->isContentRemains());
	} else {
		//WebSocketData* res = (WebSocketData*)incompleteResponse;
		return data.length()>0/* || (res!=NULL && res->isContentRemains())*/;
	}
}

void Http2RequestResponseData::updateContent() {
	if(!isWebSocket) {
		HttpResponse* res = (HttpResponse*)incompleteResponse;
		if(data.length()==0 && res!=NULL && res->isContentRemains()) {
			res->getRemainingContent(url, false, data);
		}
	} else {
		//WebSocketData* res = (WebSocketData*)incompleteResponse;
		//if(data.length()==0 && res!=NULL && res->isContentRemains()) {
		//	data = res->getRemainingContent();
		//}
	}
}

const std::map<std::string, std::string, std::less<>>& Http2RequestResponseData::getHeaders() const {
	return preHeaders;
}

Http2ReadWriteUtil::Http2ReadWriteUtil() {
}

Http2ReadWriteUtil::~Http2ReadWriteUtil() {
}
