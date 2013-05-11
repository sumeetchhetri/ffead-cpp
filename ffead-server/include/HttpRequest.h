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
#include "Logger.h"
#include "MultipartContent.h"
#include "Timer.h"
#include "HTTPResponseStatus.h"

typedef vector<string> strVec;
#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_
using namespace std;
typedef map<string, string> RMap;
typedef map<string, MultipartContent> FMap;

class HttpRequest {
	static string VALID_REQUEST_HEADERS;
	Logger logger;
	string host;
	string url;
	string cntxt_root;
	string cntxt_name;
	string httpVersion;
	float httpVers;
	string method;
	string content;
	string content_boundary;
	string content_tfile;
	string file;
	RMap requestParams;
	FMap requestParamsF;
	RMap queryParams;
	HttpSession session;
	strVec localeInfo;
	string actUrl;
	string sessionID;
	bool cookie;
	string ranges;
	map<string,string> cookieattrs;
	map<string,string> authinfo;
	map<int,string> reqorderinf;
	map<int,string> authorderinf;
	map<string,string> headers;
	vector<MultipartContent> contentList;
	string preamble;
	string epilogue;
	HTTPResponseStatus status;

	void getAuthParams(string);
	void getOauthParams(string);
	void updateFromContentStr();
	void updateFromContentFile();
public:
	enum {
		PREFLIGHT, CORS, OTHER
	};
	static string Accept,AcceptCharset,AcceptEncoding,AcceptLanguage,AcceptDatetime,
				  AccessControlRequestHeaders,AccessControlRequestMethod,Authorization,
				  CacheControl,Connection,Cookie,ContentLength,ContentMD5,ContentType,
				  Date,Expect,From,Host,IfMatch,IfModifiedSince,IfNoneMatch,IfRange,
				  IfUnmodifiedSince,MaxForwards,Origin,Pragma,ProxyAuthorization,Range,
				  Referer,TE,Upgrade,UserAgent,Via,Warning;
	HttpRequest();
	HttpRequest(strVec,string);
	void updateContent();
	virtual ~HttpRequest();
    void setSession(HttpSession session);
    HttpSession* getSession();
    string getMethod() const;
	void setMethod(string method);
	void setUrl(string);
	string getUrl();
	string getHttpVersion();
	float getHttpVers();
	string getContent_boundary() const;
	void setContent_boundary(string);
	string getContent() const;
	void setContent(string);
    RMap getRequestParams() const;
    void setRequestParams(RMap);
    void setRequestParam(string,string);
    string getRequestParam(string);
    string getRequestParamType(string key);
    string getCntxt_root() const;
    void setCntxt_root(string);
    string getDefaultLocale();
    string getCntxt_name() const;
    void setCntxt_name(string);
    string getFile() const;
    void setFile(string);
    string getActUrl() const;
    void setActUrl(string);
    string getSessionID() const{return sessionID;}
    void setSessionID(string sessionID){this->sessionID = sessionID;}
    map<string,string> getAuthinfo() const;
    void setAuthinfo(map<string,string>);
    string buildRequest(const char* key,const char* value);
    string toString();
    string toPHPVariablesString(string);
	string toPerlVariablesString();
	string toRubyVariablesString();
	string toPythonVariablesString();
	string toLuaVariablesString();
	string toNodejsVariablesString();
	RMap getQueryParams() const;
	void setQueryParams(RMap queryParams);
	void setQueryParam(string name,string value);
	string getQueryParam(string key);
	RMap getAllParams();
    bool hasCookie() const;
    map<int,string> getAuthOrderinfo() const;
	map<int,string> getReqOrderinfo() const;
	map<string,string> getCookieInfo() const;
    string getAuthOrderinfoAttribute(int key);
    string getReqOrderinfoAttribute(int key);
    string getCookieInfoAttribute(string key);
    string getHeader(string key);
    map<string,string> getHeaders();
    int getCORSRequestType();
    void addHeaderValue(string header, string value);
    vector<string> parseHeaderValue(string headerValue);
    static bool isValidHttpMethod(string method);
    bool isValidHttpMethod();
    bool isAgentAcceptsCE();
    bool isHeaderValue(string header, string value, bool ignoreCase = true);
    vector<vector<int> > getRanges(vector<string> &rangesVec);
    void setContent_tfile(string tfile);
    string getContent_tfile();
    void addMultipartFormContent(string key, MultipartContent content);
    void addContent(MultipartContent content);
    bool isNonBinary(string mimeType);
    string getParamValue(string);
    HTTPResponseStatus getRequestParseStatus() const;
};

#endif /* HTTPREQUEST_H_ */
