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
 * HttpRequest.h
 *
 *  Created on: Aug 10, 2009
 *      Author: sumeet
 */

#include "map"
#include "HttpSession.h"
#include "vector"
#include "sstream"
#include "fstream"
#include "StringUtil.h"
#include "RegexUtil.h"
#include "CastUtil.h"
#include "stdio.h"
#include <openssl/ssl.h>
#include "CryptoHandler.h"
#include "MultipartContent.h"
#include "Timer.h"
#include "HTTPResponseStatus.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "SocketInterface.h"
#include "string_view"

typedef std::vector<std::string_view> strvVec;
#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

typedef std::map<std::string_view, std::string_view, cicomp> RMap;
typedef std::map<std::string_view, MultipartContent, cicomp> FMap;

class HttpRequest {
	std::string headerStr;
	static std::string VALID_REQUEST_HEADERS;
	std::string_view webpath;
	std::string_view authority;
	std::string_view scheme;
	std::string_view host;
	std::string url;
	std::string curl;
	std::string ext;
	std::string_view cntxt_root;
	std::string_view cntxt_home;
	std::string cntxt_name;
	std::string_view httpVersion;
	float httpVers;
	bool corsRequest;
	std::string_view method;
	std::string content;
	std::string_view content_boundary;
	std::string_view content_tfile;
	std::string file;
	RMap requestParams;
	FMap requestParamsF;
	RMap queryParams;
	HttpSession session;
	strvVec localeInfo;
	std::string actUrl;
	std::vector<std::string_view> actUrlParts;
	std::string_view sessionID;
	bool cookie;
	std::string_view ranges;
	RMap cookieattrs;
	RMap authinfo;
	std::map<int,std::string_view> reqorderinf;
	std::map<int,std::string_view> authorderinf;
	RMap headers;
	std::vector<MultipartContent> contentList;
	std::string preamble;
	std::string epilogue;
	HTTPResponseStatus status;
	std::string_view userName;
	std::string_view password;
	std::string_view authMethod;

	void getAuthParams(std::string_view);
	void getOauthParams(std::string_view str);
	void updateFromContentStr();
	void updateFromContentFile();
	void setMethod(std::string_view);
	void setRequestParams(const RMap&);
	void setRequestParam(std::string_view, std::string_view);
	void setContent_tfile(std::string_view);
	void setQueryParams(const RMap& queryParams);
	void setAuthinfo(const RMap&);
	void normalizeUrl();
    void setCurl(std::string_view url);
	void setActUrl(std::string_view);
	void setCntxt_name(std::string_view);
	void setCntxt_root(std::string_view);
	void setContent_boundary(std::string_view);
	void setQueryParam(std::string_view name, std::string_view value);
	void setSessionID(std::string_view);
	std::string_view toPluginString();
	void setHttp2Headers(RMap headers);
	void setContextHome(std::string_view);
    void addHeader(std::string_view header, std::string_view value);
	friend class ServiceTask;
	friend class Http11Handler;
	friend class Http2Handler;
	friend class Http2StreamHandler;
	friend class HttpResponse;
	friend class HttpServiceHandler;
	friend class HttpServiceTask;
	friend class ControllerHandler;
	friend class ExtHandler;
	friend class FviewHandler;
	friend class ScriptHandler;
	friend class SecurityHandler;
	friend class SoapHandler;
	friend class HttpClient;
	friend class SolrSearch;
	friend class CORSHandler;
	friend class HttpRequestBuffered;
	static const std::string VALID_METHODS;
public:
	enum {
		PREFLIGHT, CORS, OTHER
	};
	static std::string Accept,AcceptCharset,AcceptEncoding,AcceptLanguage,AcceptDatetime,
				  AccessControlRequestHeaders,AccessControlRequestMethod,Authorization,
				  CacheControl,Connection,Cookie,ContentLength,ContentMD5,ContentType,
				  Date,Expect,From,Host,IfMatch,IfModifiedSince,IfNoneMatch,IfRange,TransferEncoding,
				  IfUnmodifiedSince,MaxForwards,Origin,Pragma,ProxyAuthorization,Range,
				  Referer,TE,Upgrade,UserAgent,Via,Warning,SecWebSocketKey,SecWebSocketVersion,
				  SecWebSocketAccept,SecWebSocketProtocol,SecWebSocketExtensions,AltUsed,Http2Settings;
	HttpRequest();
	HttpRequest(std::string_view);
	void updateContent();
	virtual ~HttpRequest();
	bool isCorsRequest();
    void setUrl(std::string_view url);
    HttpSession* getSession();
    std::string_view getMethod() const;
    const std::string& getUrl() const;
    const std::string& getCurl() const;
	std::string_view getHttpVersion() const;
	float getHttpVers() const;
	std::string_view getContent_boundary() const;
	std::string_view getContent() const;
	void setContent(std::string_view);
    RMap getRequestParams() const;
    std::string_view getRequestParam(std::string_view);
    MultipartContent getMultipartContent(std::string_view);
    std::string_view getRequestParamType(std::string_view);
    std::string_view getCntxt_root() const;
    std::string_view getDefaultLocale() const;
    std::string getCntxt_name() const;
    const std::string& getFile() const;
    void setFile(std::string_view);
    const std::string& getActUrl() const;
    const std::vector<std::string>& getActUrlParts() const;
    std::string_view getSessionID() const;
    RMap getAuthinfo() const;
    void buildRequestC(const char* key, const char* value);
    void buildRequest(std::string_view key, std::string_view value);
    std::string_view toString();
#ifdef INC_SCRH
    std::string_view toPHPVariablesString(std::string_view);
	std::string_view toPerlVariablesString();
	std::string_view toRubyVariablesString();
	std::string_view toPythonVariablesString();
	std::string_view toLuaVariablesString();
	std::string_view toNodejsVariablesString();
#endif
	RMap getQueryParams() const;
	std::string_view getQueryParam(std::string_view);
	RMap getAllParams();
    bool hasCookie();
    std::map<int,std::string> getAuthOrderinfo() const;
	std::map<int,std::string> getReqOrderinfo() const;
	RMap getCookieInfo() const;
    std::string_view getAuthOrderinfoAttribute(const int& key);
    std::string_view getReqOrderinfoAttribute(const int& key);
    std::string_view getCookieInfoAttribute(std::string_view);
    std::string_view getHeader(std::string_view key);
    bool hasHeader(std::string_view key);
    RMap getHeaders();
    int getCORSRequestType();
    void addHeaderValue(std::string_view header, std::string_view value);
    strvVec parseHeaderValue(std::string_view headerValue);
    static bool isValidHttpMethod(std::string_view method);
    bool isValidHttpMethod();
    bool isAgentAcceptsCE();
    bool isHeaderValue(std::string_view header, std::string_view value, const bool& ignoreCase= true);
    bool hasHeaderValuePart(std::string_view header, std::string_view valuePart, const bool& ignoreCase= true);
    std::vector<std::vector<int> > getRanges(std::vector<std::string_view> &rangesVec);
    std::string_view getContent_tfile();
    void addMultipartFormContent(std::string_view key, const MultipartContent& content);
    void addContent(const MultipartContent& content);
    bool isNonBinary(std::string_view);
    std::string_view getParamValue(std::string_view);
    HTTPResponseStatus getRequestParseStatus() const;
    std::vector<MultipartContent> getMultiPartFileList(std::string_view);
	std::string_view getPassword() const;
	std::string_view getUserName() const;
	std::string_view getAuthMethod() const;
	void setPassword(std::string_view v);
	void setUserName(std::string_view v);
	void setAuthMethod(std::string_view v);
	std::string_view getContextHome();
	const std::string& getExt() const;
	static std::string getFileExtension(const std::string& file);
};

/*
class HttpRequestBuffered : public HttpRequest {
	friend class Http11Handler;
	std::string_view _b;
	std::string_view bv;
	static const std::string_view BLV;
	HttpRequestBuffered();
	virtual ~HttpRequestBuffered();
};*/

#endif /* HTTPREQUEST_H_ */
