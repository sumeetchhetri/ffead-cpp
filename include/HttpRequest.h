/*
	Copyright 2010, Sumeet Chhetri 
  
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
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "stdio.h"
#include <openssl/ssl.h>
#include "CryptoHandler.h"

typedef vector<string> strVec;
#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_
using namespace std;
typedef map<string, string> RMap;


class FormData
{
public:
	string type;
	string value;
	string fileName;
};
typedef map<string, FormData> FMap;
class HttpRequest {
	//friend class HttpSession;
	string host;
	string url;
	string cntxt_root;
	string cntxt_name;
	string httpVersion;
	string method;
	string user_agent;
	string accept;
	string accept_lang;
	string accept_encod;
	string accept_chars;
	string keep_alive;
	string connection;
	string cache_ctrl;
	string content;
	string content_type;
	string content_boundary;
	string content_len;
	string referer;
	string pragma;
	string file;
	Map attributes;
	RMap requestParams;
	FMap requestParamsF;
	HttpSession session;
	strVec localeInfo;
	string actUrl;
	string sessionID;
	bool cookie;
	map<string,string> cookieattrs;
	map<string,string> authinfo;
	map<int,string> reqorderinf;
	map<int,string> authorderinf;
	void unbase64(string);
	void getOauthParams(string);
public:
	HttpRequest();
	HttpRequest(strVec,string);
	virtual ~HttpRequest();
    Map getAttributes() const;
    void setAttributes(Map attributes);
    string getAttribute(string key);
    void setAttribute(string key,string value);
    void setSession(HttpSession session);
    HttpSession* getSession();
    string getMethod() const;
	void setMethod(string method);
	string getUser_agent() const;
	void setUser_agent(string user_agent);
	string getAccept() const;
	void setAccept(string accept);
	string getAccept_lang() const;
	void setAccept_lang(string accept_lang);
	string getAccept_encod() const;
	void setAccept_encod(string accept_encod);
	string getAccept_chars() const;
	void setAccept_chars(string accept_chars);
	string getKeep_alive() const;
	void setKeep_alive(string keep_alive);
	string getConnection() const;
	void setConnection(string connection);
	string getCache_ctrl() const;
	void setCache_ctrl(string cache_ctrl);
	string getHost() const;
	void setHost(string host);
	void setUrl(string);
	string getUrl();
	void setHttpVersion(string);
	string getHttpVersion();
	string getContent_type() const;
	void setContent_type(string);
	string getContent_boundary() const;
	void setContent_boundary(string);
	string getContent_len() const;
	void setContent_len(string);
	string getContent() const;
	void setContent(string);
    string getReferer() const;
    void setReferer(string);
    string getPragma() const;
    void setPragma(string);
    RMap getRequestParams() const;
    void setRequestParams(RMap);
    void setRequestParam(string,string);
    void setRequestParamF(string,FormData);
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
    bool hasCookie() const{return this->cookie;}
    map<int,string> getAuthOrderinfo() const{return authorderinf;}
    map<int,string> getReqOrderinfo() const{return reqorderinf;}
    map<string,string> getCookieInfo() const{return cookieattrs;}

};

#endif /* HTTPREQUEST_H_ */
