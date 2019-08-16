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
#include "string_view"

typedef std::vector<unsigned char> Cont;
class HttpResponse {
public:
	static const std::string_view HDR_SRV, HDR_SEP, HDR_SEPT, HDR_END, HDR_CORS_ALW, HDR_FIN;
	static std::string_view AccessControlAllowOrigin,AccessControlAllowHeaders,AccessControlAllowCredentials,
				  AccessControlAllowMethods,AccessControlMaxAge,AcceptRanges,Age,Allow,CacheControl,
				  Connection,ContentEncoding,ContentLanguage,ContentLength,ContentLocation,ContentMD5,
				  ContentDisposition,ContentRange,ContentType,DateHeader,ETag,Expires,LastModified,Link,
				  Location,P3P,Pragma,ProxyAuthenticate,Refresh,RetryAfter,Server,SetCookie,Status,
				  StrictTransportSecurity,Trailer,TransferEncoding,Vary,Via,Warning,WWWAuthenticate,
				  Upgrade,SecWebSocketAccept,SecWebSocketVersion,AltSvc;
	HttpResponse();
	virtual ~HttpResponse();
    std::string_view getHttpVersion() const;
    void setHTTPResponseStatus(const HTTPResponseStatus& status);
    std::string_view getStatusCode() const;
    void setStatusCode(std::string_view statusCode);
    std::string_view getStatusMsg() const;
    void setStatusMsg(std::string_view statusMsg);
    std::string_view getContent() const;
	//void setContent(const Cont& content);
	void setContent(std::string_view content);
	void addCookie(std::string_view cookie);
    void addContent(const MultipartContent& content);
    void addHeaderValue(std::string_view header, std::string_view value);
    bool isHeaderValue(std::string_view header, std::string_view value, const bool& ignoreCase= true);
    bool isNonBinary();
    std::string_view getHeader(std::string_view);
    bool getCompressed();
    const std::vector<std::string> getCookies() const;
	const RMap& getCHeaders() const;
	RMap getHeaders() const;
	std::string_view getStatusLine() const;
	std::string_view toPluginString();
	bool isDone() const;
	void setDone(const bool& done);
	std::string_view generateResponse(std::string_view httpMethod, HttpRequest *req, const bool& appendHeaders= true);
	std::string_view generateResponse(const bool& appendHeaders= true);
	std::string_view generateResponse(HttpRequest *req, const bool& appendHeaders= true);
private:
    bool done;
    float httpVers;
    uint32_t intCntLen;
    static std::string_view VALID_RESPONSE_HEADERS;
	std::string_view httpVersion;
	std::string_view statusCode;
	std::string_view statusMsg;
	std::string_view preamble;
	std::string_view epilogue;
	bool compressed;
	FMap multipartFormData;
	std::vector<MultipartContent> contentList;
	std::string content;
	std::string_view outFileName;
	std::vector<std::string_view> cookies;
	RMap headers;
	int techunkSiz;
	int teparts;
	int tecurrpart;
	bool hasContent;
	void setCompressed(const bool& compressed);
	void update(HttpRequest* req);
	std::string_view generateHeadResponse();
	std::string_view generateOptionsResponse();
	std::string_view generateTraceResponse(HttpRequest* req);
	bool updateContent(HttpRequest* req, const uint32_t& techunkSiz);
	unsigned int getContentSize(const char *fileName);
	std::string_view getContent(const char *fileName, const int& start= -1, const int& end= -1);
	bool isContentRemains();
	std::string_view getRemainingContent(std::string_view fname, const bool& isFirst);
	static std::string_view getFileExtension(std::string_view file);
    void addHeader(std::string_view header, std::string_view value);
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
};

#endif /* HTTPRESPONSE_H_ */
