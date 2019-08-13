/*
 * Http2Handler.h
 *
 *  Created on: 07-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2HANDLER_H_
#define HTTP2HANDLER_H_
#include "Http2ContinuationFrame.h"
#include "Http2DataFrame.h"
#include "Http2GoAwayFrame.h"
#include "Http2HeadersFrame.h"
#include "Http2PingFrame.h"
#include "Http2PriorityFrame.h"
#include "Http2PushPromiseFrame.h"
#include "Http2ResetStreamFrame.h"
#include "Http2SettingsFrame.h"
#include "Http2WindowUpdateFrame.h"
#include "Http2StreamHandler.h"
#include "Http2AlternativeServicesFrame.h"
#include "SocketInterface.h"
#include "CommonUtils.h"
#include "Http2ReadWriteUtil.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "queue"
#include "LoggerFactory.h"

class Http2Handler : public Http2ReadWriteUtil, public SocketInterface {
	Logger logger;
	int highestStreamIdentifier;
	int highestPushPromiseStreamIdentifier;
	Http2HPACKContext context;
	//All the related entities for a PUSH_PROMISE frame
	std::map<std::string, std::vector<std::string> > relatedEntitiesForPP;
	std::map<int, Http2StreamHandler> streams;
	int senderFlowControlWindow;
	int receiverFlowControlWindow;
	std::map<int, bool> frameAcks;
	std::map<uint16_t, uint32_t> settings;
	int precedingstreamId;
	bool isConnInit;
	uint32_t maxDataFrameSize;
	std::string webpath;
	std::atomic<int> updateRxFCWS;
	std::queue<Http2RequestResponseData> pushPromisedRequestQ;
	Http2Frame* readFrame(std::string data);
	Http2Frame* nextFrame();
	Http2Frame* getFrameByType(const std::string& data, Http2FrameHeader& header);
	std::string serialize(Http2Frame* frame);
	bool processFrame(Http2Frame* frame, void*& request, int reqPos);
public:
	void addHandler(SocketInterface* handler);
	void onOpen();
	void onClose();
	std::string getProtocol(void* context);
	int getTimeout();
	void* readRequest(void*& context, int& pending, int& reqPos);
	bool writeResponse(void* req, void* res, void* context, std::string& data, int reqPos);
	bool writeHttpResponse(void* req, void* res, void* si, std::string& data);
	bool writeWebSocketResponse(void* req, void* res, void* si, std::string& data);
	void doIt();
	void addPushPromisedRequestToQ(const Http2RequestResponseData& ppdat);
	void writeInitData(Http2Frame* frame);
	bool writePendingDataFrame(Http2RequestResponseData&, std::string& data);
	bool writeData(Http2RequestResponseData& data, Http2RequestResponseData& pendingSendData, int& streamFlowControlWindowS, std::string& respd);
	std::vector<std::string> getRelatedEntitiesForPP(const std::string&);
	int getHighestPushPromiseStreamIdentifier();
	int updateSenderWindowSize(const int& windowSize);
	void updateMaxFrameSize(const uint32_t& val);
	Http2Handler(const SOCKET& fd, SSL* ssl, BIO* io, const bool& isServer, const std::string& webpath);
	Http2Handler(const SOCKET& fd, SSL* ssl, BIO* io, const bool& isServer, const std::string& webpath, const std::string& settingsFrameData);
	virtual ~Http2Handler();
	const std::string& getWebpath() const;
	std::string getMimeType(const std::string& ext);
};

#endif /* HTTP2HANDLER_H_ */
