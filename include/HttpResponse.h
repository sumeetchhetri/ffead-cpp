/*
	Copyright 2009-2012, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (the "License"); 
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
#include "CastUtil.h"
#include "HttpRequest.h"
#include "HTTPResponseStatus.h"
#include "ContentTypes.h"
#include "Timer.h"
#include "RegexUtil.h"
#include "CompressionUtil.h"
#include "MultipartContent.h"
using namespace std;
typedef vector<unsigned char> Cont;
class HttpResponse {
public:
	static string AccessControlAllowOrigin,AccessControlAllowHeaders,AccessControlAllowCredentials,
				  AccessControlAllowMethods,AccessControlMaxAge,AcceptRanges,Age,Allow,CacheControl,
				  Connection,ContentEncoding,ContentLanguage,ContentLength,ContentLocation,ContentMD5,
				  ContentDisposition,ContentRange,ContentType,Date,ETag,Expires,LastModified,Link,
				  Location,P3P,Pragma,ProxyAuthenticate,Refresh,RetryAfter,Server,SetCookie,Status,
				  StrictTransportSecurity,Trailer,TransferEncoding,Vary,Via,Warning,WWWAuthenticate;
	HttpResponse();
	virtual ~HttpResponse();
    void setCompressed(bool compressed);
    string getHttpVersion() const;
    void update(HttpRequest* req);
    void setHTTPResponseStatus(HTTPResponseStatus status);
    string getStatusCode() const;
    void setStatusCode(string statusCode);
    string getStatusMsg() const;
    void setStatusMsg(string statusMsg);
    string getContent() const;
	//void setContent(Cont content);
	void setContent(string content);
	void addCookie(string cookie);
	string generateResponse(string httpMethod, HttpRequest *req);
    string generateResponse();
    string generateHeadResponse();
    string generateOptionsResponse();
    string generateTraceResponse(HttpRequest* req);
    void addContent(MultipartContent content);
    void addHeaderValue(string header, string value);
    bool isHeaderValue(string header, string value, bool ignoreCase = true);
    bool isNonBinary();
    string getHeader(string);
    bool getCompressed();
    friend class ServiceTask;
    friend class HttpResponseParser;
private:
    Logger logger;
    static string VALID_RESPONSE_HEADERS;
	string httpVersion;
	string statusCode;
	string statusMsg;
	string preamble;
	string epilogue;
	bool compressed;
	vector<MultipartContent> contentList;
	string content;
	vector<string> cookies;
	map<string,string> headers;
};

#endif /* HTTPRESPONSE_H_ */
