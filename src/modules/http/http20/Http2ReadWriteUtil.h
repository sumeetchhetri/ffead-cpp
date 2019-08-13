/*
 * Http2ReadWriteUtil.h
 *
 *  Created on: 13-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2READWRITEUTIL_H_
#define HTTP2READWRITEUTIL_H_
#include "CommonUtils.h"
#include "Http2Frame.h"
#include "HttpResponse.h"
#include "Http11WebSocketDataFrame.h"

class Http2RequestResponseData {
	std::map<std::string, std::string, cicomp> preHeaders;
	std::map<std::string, std::string, cicomp> postHeaders;
	std::string data;
	std::string url;
	bool isWebSocket;
	void* incompleteResponse;
	std::string headerBlock;
	int streamIdentifier;
	bool endStream;
	friend class Http2StreamHandler;
	friend class Http2Handler;
	friend class ServiceTask;
	void reset();
public:
	void updateContent();
	bool isDataPending();
	Http2RequestResponseData();
	virtual ~Http2RequestResponseData();
	const std::string& getData() const;
	const std::map<std::string, std::string, cicomp>& getHeaders() const;
};

class Http2ReadWriteUtil {
public:
	Http2ReadWriteUtil();
	//virtual void close();
	virtual ~Http2ReadWriteUtil();
	virtual bool writePendingDataFrame(Http2RequestResponseData&, std::string& data)=0;
	//virtual bool writeData(Http2RequestResponseData& data, Http2RequestResponseData& pendingSendData, int& streamFlowControlWindowS)=0;
	virtual void writeInitData(Http2Frame* data)=0;
	virtual std::vector<std::string> getRelatedEntitiesForPP(const std::string&)=0;
	virtual int getHighestPushPromiseStreamIdentifier()=0;
	virtual int updateSenderWindowSize(const int& windowSize)=0;
	virtual void addPushPromisedRequestToQ(const Http2RequestResponseData&)=0;
	virtual void updateMaxFrameSize(const uint32_t& val)=0;
	virtual std::string getMimeType(const std::string& ext)=0;
};

#endif /* HTTP2READWRITEUTIL_H_ */
