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

const string& Http2RequestResponseData::getData() const {
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
		WebSocketData* res = (WebSocketData*)incompleteResponse;
		return data.length()>0 || (res!=NULL && res->isContentRemains());
	}
}

void Http2RequestResponseData::updateContent() {
	if(!isWebSocket) {
		HttpResponse* res = (HttpResponse*)incompleteResponse;
		if(data.length()==0 && res!=NULL && res->isContentRemains()) {
			data = res->getRemainingContent(url, false);
		}
	} else {
		WebSocketData* res = (WebSocketData*)incompleteResponse;
		if(data.length()==0 && res!=NULL && res->isContentRemains()) {
			data = res->getRemainingContent();
		}
	}
}

const map<string, string>& Http2RequestResponseData::getHeaders() const {
	return preHeaders;
}

Http2ReadWriteUtil::Http2ReadWriteUtil() {
}

Http2ReadWriteUtil::~Http2ReadWriteUtil() {
}
