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

typedef std::vector<std::string> strVec;
#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

typedef std::map<std::string, std::string, cicomp> RMap;
typedef std::map<std::string, MultipartContent, cicomp> FMap;

class HttpRequest {
	static std::string VALID_REQUEST_HEADERS;
	std::string webpath;
	std::string authority;
	std::string scheme;
	std::string host;
	std::string url;
	std::string curl;
	std::string ext;
	std::string cntxt_root;
	std::string cntxt_home;
	std::string cntxt_name;
	std::string httpVersion;
	float httpVers;
	bool corsRequest;
	std::string method;
	std::string content;
	std::string content_boundary;
	std::string content_tfile;
	std::string file;
	RMap requestParams;
	FMap requestParamsF;
	RMap queryParams;
	HttpSession session;
	strVec localeInfo;
	std::string actUrl;
	std::vector<std::string> actUrlParts;
	std::string sessionID;
	bool cookie;
	std::string ranges;
	RMap cookieattrs;
	RMap authinfo;
	std::map<int,std::string> reqorderinf;
	std::map<int,std::string> authorderinf;
	RMap headers;
	std::vector<MultipartContent> contentList;
	std::string preamble;
	std::string epilogue;
	HTTPResponseStatus status;
	std::string userName;
	std::string password;
	std::string authMethod;

	void getAuthParams(std::string);
	void getOauthParams(std::string str);
	void updateFromContentStr_Old();
	void updateFromContentStr();
	void updateFromContentFile();
	void setMethod(const std::string& method);
	void setRequestParams(const RMap&);
	void setRequestParam(const std::string&, const std::string&);
	void setContent_tfile(const std::string& tfile);
	void setQueryParams(const RMap& queryParams);
	void setAuthinfo(const RMap&);
	void normalizeUrl();
    void setCurl(std::string url);
	void setActUrl(const std::string&);
	void setCntxt_name(const std::string&);
	void setCntxt_root(const std::string&);
	void setContent_boundary(const std::string&);
	void setQueryParam(const std::string& name, const std::string& value);
	void setSessionID(const std::string& sessionID);
	std::string toPluginString();
	void setHttp2Headers(RMap headers);
	void setContextHome(const std::string& home);
    void addHeader(const std::string& header, const std::string& value);
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
	HttpRequest(const strVec&, const std::string&);
	HttpRequest(const std::string&);
	void updateContent();
	virtual ~HttpRequest();
	bool isCorsRequest();
    void setUrl(std::string url);
    HttpSession* getSession();
    std::string getMethod() const;
	std::string getUrl() const;
	std::string getCurl() const;
	std::string getHttpVersion() const;
	float getHttpVers() const;
	std::string getContent_boundary() const;
	std::string getContent() const;
	void setContent(const std::string&);
    RMap getRequestParams() const;
    std::string getRequestParam(const std::string&);
    MultipartContent getMultipartContent(const std::string& key);
    std::string getRequestParamType(const std::string& key);
    std::string getCntxt_root() const;
    std::string getDefaultLocale() const;
    std::string getCntxt_name() const;
    std::string getFile() const;
    void setFile(const std::string&);
    std::string getActUrl() const;
    const std::vector<std::string>& getActUrlParts() const;
    std::string getSessionID() const;
    RMap getAuthinfo() const;
    void buildRequestC(const char* key, const char* value);
    void buildRequest(std::string key, std::string value);
    std::string toString();
#ifdef INC_SCRH
    std::string toPHPVariablesString(const std::string&);
	std::string toPerlVariablesString();
	std::string toRubyVariablesString();
	std::string toPythonVariablesString();
	std::string toLuaVariablesString();
	std::string toNodejsVariablesString();
#endif
	RMap getQueryParams() const;
	std::string getQueryParam(const std::string& key);
	RMap getAllParams();
    bool hasCookie();
    std::map<int,std::string> getAuthOrderinfo() const;
	std::map<int,std::string> getReqOrderinfo() const;
	RMap getCookieInfo() const;
    std::string getAuthOrderinfoAttribute(const int& key);
    std::string getReqOrderinfoAttribute(const int& key);
    std::string getCookieInfoAttribute(const std::string& key);
    std::string getHeader(std::string key);
    bool hasHeader(std::string key);
    RMap getHeaders();
    int getCORSRequestType();
    void addHeaderValue(std::string header, const std::string& value);
    std::vector<std::string> parseHeaderValue(std::string headerValue);
    static bool isValidHttpMethod(const std::string& method);
    bool isValidHttpMethod();
    bool isAgentAcceptsCE();
    bool isHeaderValue(std::string header, const std::string& value, const bool& ignoreCase= true);
    bool hasHeaderValuePart(std::string header, std::string valuePart, const bool& ignoreCase= true);
    std::vector<std::vector<int> > getRanges(std::vector<std::string> &rangesVec);
    std::string getContent_tfile();
    void addMultipartFormContent(const std::string& key, const MultipartContent& content);
    void addContent(const MultipartContent& content);
    bool isNonBinary(const std::string& mimeType);
    std::string getParamValue(const std::string&);
    HTTPResponseStatus getRequestParseStatus() const;
    std::vector<MultipartContent> getMultiPartFileList(const std::string& name);
	std::string getPassword() const;
	std::string getUserName() const;
	std::string getAuthMethod() const;
	void setPassword(std::string v);
	void setUserName(std::string v);
	void setAuthMethod(std::string v);
	std::string getContextHome();
	std::string getExt() const;
	static std::string getFileExtension(const std::string& file);
};

/*
class HttpRequestBuffered : public HttpRequest {
	friend class Http11Handler;
	std::string _b;
	std::string_view bv;
	static const std::string_view BLV;
	HttpRequestBuffered();
	virtual ~HttpRequestBuffered();
};*/

#endif /* HTTPREQUEST_H_ */
