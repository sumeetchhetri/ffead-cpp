/*
	Copyright 2009-2012, Sumeet Chhetri 
  
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
 * HttpResponse.h
 *
 *  Created on: Aug 19, 2009
 *      Author: sumeet
 */

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_
#include "string"
#include "vector"
#include <sys/stat.h>
#include "CastUtil.h"
#include "HttpRequest.h"
#include "HTTPResponseStatus.h"
#include "ContentTypes.h"
#include "Timer.h"
#include "RegexUtil.h"
#include "CompressionUtil.h"
#include "MultipartContent.h"
#include "DateFormat.h"
#include "CommonUtils.h"

typedef std::vector<unsigned char> Cont;
class HttpResponse {
public:
	static void init();
	static std::string AccessControlAllowOrigin,AccessControlAllowHeaders,AccessControlAllowCredentials,
				  AccessControlAllowMethods,AccessControlMaxAge,AcceptRanges,Age,Allow,CacheControl,
				  Connection,ContentEncoding,ContentLanguage,ContentLength,ContentLocation,ContentMD5,
				  ContentDisposition,ContentRange,ContentType,DateHeader,ETag,Expires,LastModified,Link,
				  Location,P3P,Pragma,ProxyAuthenticate,Refresh,RetryAfter,Server,SetCookie,Status,
				  StrictTransportSecurity,Trailer,TransferEncoding,Vary,Via,Warning,WWWAuthenticate,
				  Upgrade,SecWebSocketAccept,SecWebSocketVersion,AltSvc;
	HttpResponse();
	virtual ~HttpResponse();
    std::string getHttpVersion() const;
    void setHTTPResponseStatus(const HTTPResponseStatus& status);
    std::string getStatusCode() const;
    std::string getStatusMsg() const;
    std::string getContent() const;
	//void setContent(const Cont& content);
	void setContent(const std::string& content);
	void addCookie(const std::string& cookie);
    void addContent(const MultipartContent& content);
    void addHeaderValue(std::string header, const std::string& value);
    bool hasHeader(const std::string& header);
    bool isHeaderValue(std::string header, const std::string& value, const bool& ignoreCase= true);
    bool isNonBinary();
    std::string getHeader(std::string);
    bool getCompressed();
    const std::vector<std::string> getCookies() const;
	const RMap& getCHeaders() const;
	RMap getHeaders() const;
	std::string getStatusLine() const;
	std::string toPluginString();
	bool isDone() const;
	void setDone(const bool& done);
	void generateResponse(const std::string& httpMethod, HttpRequest *req, std::string& data, const bool& appendHeaders= true);
	std::string generateResponse(const bool& appendHeaders= true);
	void generateResponse(HttpRequest *req, std::string& data, const bool& appendHeaders= true);
private:
	static RiMap HDRS_SW_CODES;
	static const std::string HDR_SRV, HDR_SEP, HDR_SEPT, HDR_END, HDR_CORS_ALW, HDR_FIN;
    bool done;
    float httpVers;
    uint32_t intCntLen;
    static std::string VALID_RESPONSE_HEADERS;
	std::string httpVersion;
	const HTTPResponseStatus* status;
	std::string preamble;
	std::string epilogue;
	bool compressed;
	FMap multipartFormData;
	std::vector<MultipartContent> contentList;
	std::string content;
	std::string outFileName;
	std::vector<std::string> cookies;
	RMap headers;
	int techunkSiz;
	int teparts;
	int tecurrpart;
	bool hasContent;
	void setCompressed(const bool& compressed);
	void update(HttpRequest* req);
	void generateHeadResponse(std::string& resp);
	void generateOptionsResponse(std::string& data);
	void generateTraceResponse(HttpRequest* req, std::string& data);
	bool updateContent(HttpRequest* req, const uint32_t& techunkSiz);
	unsigned int getContentSize(const char *fileName);
	std::string getContent(const char *fileName, const int& start= -1, const int& end= -1);
	bool isContentRemains();
	bool getRemainingContent(const std::string& fname, const bool& isFirst, std::string& data);
	static std::string getFileExtension(const std::string& file);
    void addHeader(std::string header, const std::string& value);
	void reset();
	friend class ServiceTask;
	friend class HttpResponseParser;
	friend class Http11Handler;
	friend class Http2Handler;
	friend class Http2RequestResponseData;
	friend class HttpServiceHandler;
	friend class HttpServiceTask;
	friend class ControllerHandler;
	friend class ExtHandler;
	friend class FviewHandler;
	friend class ScriptHandler;
	friend class SecurityHandler;
	friend class SoapHandler;
	friend class HttpClient;
	friend class CORSHandler;
	friend class HttpRequest;
};

#endif /* HTTPRESPONSE_H_ */
