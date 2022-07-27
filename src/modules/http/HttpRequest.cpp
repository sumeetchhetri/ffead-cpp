/*
	Copyright 2009-2020, Sumeet Chhetri 
  
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
 * HttpRequest.cpp
 *
 *  Created on: Aug 10, 2009
 *      Author: sumeet
 */

#include "HttpRequest.h"

std::string HttpRequest::VALID_REQUEST_HEADERS = ",accept,accept-charset,accept-encoding,accept-language,accept-datetime,access-control-request-headers,access-control-request-method,authorization,cache-control,connection,cookie,content-length,content-md5,content-type,date,expect,from,host,if-match,if-modified-since,if-none-match,if-range,if-unmodified-since,max-forwards,origin,pragma,proxy-authorization,range,referer,te,upgrade,user-agent,via,warning,transfer-encoding,sec-websocket-key,sec-websocket-version,sec-websocket-accept,sec-websocket-protocol,sec-websocket-extensions,alt-used,http2-settings,";

const std::string HttpRequest::DEFAULT_CTX = "default";
const std::string HttpRequest::BLANK = "";
bool HttpRequest::isLazyHeaderParsing = false;

std::string HttpRequest::Accept =			 "Accept";
std::string HttpRequest::AcceptCharset = 		 "Accept-Charset";
std::string HttpRequest::AcceptEncoding = 		 "Accept-Encoding";
std::string HttpRequest::AcceptLanguage = 		 "Accept-Language";
std::string HttpRequest::AcceptDatetime = 		 "Accept-Datetime";
std::string HttpRequest::AccessControlRequestHeaders = "Access-Control-Request-Headers";
std::string HttpRequest::AccessControlRequestMethod =  "Access-Control-Request-Method";
std::string HttpRequest::Authorization = 		 "Authorization";
std::string HttpRequest::CacheControl = 		 "Cache-Control";
std::string HttpRequest::Connection = 			 "Connection";
std::string HttpRequest::Cookie = 			 "Cookie";
std::string HttpRequest::ContentLength = 		 "Content-Length";
std::string HttpRequest::ContentMD5 = 			 "Content-MD5";
std::string HttpRequest::ContentType = 			 "Content-Type";
std::string HttpRequest::Date = 				 "Date";
std::string HttpRequest::Expect = 			 "Expect";
std::string HttpRequest::From = 				 "From";
std::string HttpRequest::Host = 				 "Host";
std::string HttpRequest::IfMatch = 			 "If-Match";
std::string HttpRequest::IfModifiedSince = 		 "If-Modified-Since";
std::string HttpRequest::IfNoneMatch = 		 "If-None-Match";
std::string HttpRequest::IfRange = 			 "If-Range";
std::string HttpRequest::TransferEncoding = "Transfer-Encoding";
std::string HttpRequest::IfUnmodifiedSince = 		 "If-Unmodified-Since";
std::string HttpRequest::MaxForwards = 			 "Max-Forwards";
std::string HttpRequest::Origin = 			 "Origin";
std::string HttpRequest::Pragma = 			 "Pragma";
std::string HttpRequest::ProxyAuthorization = 		 "Proxy-Authorization";
std::string HttpRequest::Range = 			 "Range";
std::string HttpRequest::Referer = 			 "Referer";
std::string HttpRequest::TE = 				 "TE";
std::string HttpRequest::Upgrade = 			 "Upgrade";
std::string HttpRequest::UserAgent = 			 "User-Agent";
std::string HttpRequest::Via = 				 "Via";
std::string HttpRequest::Warning = 			 "Warning";
std::string HttpRequest::SecWebSocketKey = "Sec-WebSocket-Key";
std::string HttpRequest::SecWebSocketVersion = "Sec-WebSocket-Version";
std::string HttpRequest::SecWebSocketAccept = "Sec-WebSocket-Accept";
std::string HttpRequest::SecWebSocketProtocol = "Sec-WebSocket-Protocol";
std::string HttpRequest::SecWebSocketExtensions = "Sec-WebSocket-Extensions";
std::string HttpRequest::AltUsed = "Alt-Used";
std::string HttpRequest::Http2Settings = "HTTP2-Settings";

RiMap HttpRequest::HDRS_SW_CODES;

void HttpRequest::init(bool isLazyHeaderParsingT) {
	isLazyHeaderParsing = isLazyHeaderParsingT;
	std::string t = VALID_REQUEST_HEADERS.substr(1, VALID_REQUEST_HEADERS.length()-1);
	std::vector<std::string> vt;
	StringUtil::split(vt, t, ",");
	for(int i=0;i<(int)vt.size();i++) {
		HDRS_SW_CODES[vt.at(i)] = i;
	}
	HDRS_SW_CODES["content"] = 42;
	HDRS_SW_CODES["method"] = 43;
	HDRS_SW_CODES["httpversion"] = 44;
	HDRS_SW_CODES["getarguments"] = 45;
	HDRS_SW_CODES["url"] = 46;
	HDRS_SW_CODES["httpline"] = 47;
}

void HttpRequest::getAuthParams(std::string str)
{
	authMethod = (str.substr(0,str.find(" ")));
	str = str.substr(str.find(" ")+1);
	if(strcasecmp(authMethod.c_str(), "basic")==0)
	{
		unsigned char *input = (unsigned char *)str.c_str();
		int length = str.length();
		std::string temp = CryptoHandler::base64decode(input,length);
		userName = (temp.substr(0,temp.find(":")));
		temp = temp.substr(temp.find(":")+1);
		password = (temp);
	}
	else
	{
		strVec tempv;
		StringUtil::split(tempv, str, (","));
		for(unsigned int i=0;i<tempv.size();i++)
		{
			strVec tempvv;
			StringUtil::split(tempvv, tempv.at(i), ("="));
			std::string temr = tempvv.at(1);
			StringUtil::trim(temr);
			if(temr.length()>1 && (temr.at(0)=='"' || temr.at(0)=='\'') && (temr.at(temr.length()-1)=='"' || temr.at(temr.length()-1)=='\'')) {
				temr = temr.substr(1, temr.length()-2);
			}
			//temr = temr.substr(0,temr.length()-1);
			authinfo[tempvv.at(0)] = temr;
			authorderinf[authorderinf.size()+1] = tempvv.at(0);
		}
	}
}

void HttpRequest::getOauthParams(std::string str)
{
	authinfo["Method"] = str.substr(0,str.find(" "));
	str = str.substr(str.find(" ")+1);

	strVec tempv;
	StringUtil::split(tempv, str, (","));
	for(unsigned int i=0;i<tempv.size();i++)
	{
		strVec tempvv;
		StringUtil::split(tempvv, tempv.at(i), ("="));
		StringUtil::replaceFirst(tempvv.at(0),"\r",BLANK);
		StringUtil::replaceFirst(tempvv.at(0),"\n",BLANK);
		std::string temr = tempvv.at(1);
		temr = temr.substr(temr.find("\"")+1);
		temr = temr.substr(0,temr.find("\""));
		authinfo[tempvv.at(0)] = temr;
		authorderinf[authorderinf.size()+1] = CryptoHandler::urlDecode(tempvv.at(0));
	}
}

void HttpRequest::addNginxApacheHeader(const char* kp, size_t kl, const char* vp, size_t vl) {
	headers_list[num_headers].name = kp;
	headers_list[num_headers].name_len = kl;
	headers_list[num_headers].value = vp;
	headers_list[num_headers].value_len = vl;
	num_headers++;
}

HttpRequest::HttpRequest(const char* pp, size_t pl, const char* qp, size_t ql, const char* mp, size_t ml, const std::string& data, unsigned int hv)  {
	pathv = std::string_view{pp, pl};
	queryv = ql>0?std::string_view{qp, ql}:std::string_view(BLANK);
	methodv = std::string_view{mp, ml};
	httpVers = 1 + (float)hv/10;
	httpVersion = hv==1?"HTTP/1.1":"HTTP/1.0";
	num_headers = 0;
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = 1;
	num_params = 0;
	content = data;
	bool cnset = false;
	size_t ids = pathv.find_first_not_of("/");
	if(ids != std::string::npos) {
		size_t ide = pathv.find("/", ids);
		if(ide != std::string::npos) {
			cntxt_name = pathv.substr(ids, ide-ids);
			cnset = true;
		} else if(pathv.find(".")==std::string::npos) {
			cntxt_name = pathv.substr(ids);
			cnset = true;
		}
	}
	if(!cnset) {
		cntxt_name = HttpRequest::BLANK;
	}
	if(pathv.find(".")!=std::string::npos) {
		ext = pathv.substr(pathv.find("."));
		file = pathv.substr(pathv.find_last_of("/")+1);
	}
	parser_pos = -1;
}

HttpRequest::HttpRequest(const char* mp, size_t ml, const char* pp, size_t pl, const char* qp, size_t ql, const char* hp, size_t hl, const char* bp, size_t bl, int hv)  {
	pathv = std::string_view{pp, pl};
	queryv = ql>0?std::string_view{qp, ql}:std::string_view(BLANK);
	methodv = std::string_view{mp, ml};
	num_headers = 100;
	int temp;
	phr_parse_headers_fcp(hp, hl, headers_list, &num_headers, 0, &temp);	
	content = std::string(bp, bl);
	httpVers = 1 + (float)hv/10;
	httpVersion = hv==1?"HTTP/1.1":"HTTP/1.0";
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = 1;
	num_params = 0;

	bool cnset = false;
	size_t ids = pathv.find_first_not_of("/");
	if(ids != std::string::npos) {
		size_t ide = pathv.find("/", ids);
		if(ide != std::string::npos) {
			cntxt_name = pathv.substr(ids, ide-ids);
			cnset = true;
		} else if(pathv.find(".")==std::string::npos) {
			cntxt_name = pathv.substr(ids);
			cnset = true;
		}
	}
	if(!cnset) {
		cntxt_name = HttpRequest::BLANK;
	}
	if(pathv.find(".")!=std::string::npos) {
		ext = pathv.substr(pathv.find("."));
		file = pathv.substr(pathv.find_last_of("/")+1);
	}
	parser_pos = -1;
}



HttpRequest::HttpRequest(const char* mp, size_t ml, const char* pp, size_t pl, const char* bp, size_t bl, int hv)  {
	pathv = std::string_view{pp, pl};
	queryv = std::string_view(BLANK);
	methodv = std::string_view{mp, ml};
	num_headers = 0;
	content = std::string(bp, bl);
	httpVers = 1 + (float)hv/10;
	httpVersion = hv==1?"HTTP/1.1":"HTTP/1.0";
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = 1;
	num_params = 0;

	bool cnset = false;
	size_t ids = pathv.find_first_not_of("/");
	if(ids != std::string::npos) {
		size_t ide = pathv.find("/", ids);
		if(ide != std::string::npos) {
			cntxt_name = pathv.substr(ids, ide-ids);
			cnset = true;
		} else if(pathv.find(".")==std::string::npos) {
			cntxt_name = pathv.substr(ids);
			cnset = true;
		}
	}
	if(!cnset) {
		cntxt_name = HttpRequest::BLANK;
	}
	if(pathv.find(".")!=std::string::npos) {
		ext = pathv.substr(pathv.find("."));
		file = pathv.substr(pathv.find_last_of("/")+1);
	}
	parser_pos = -1;
}

HttpRequest::HttpRequest(const char* cnt, size_t cntlen, const std::unordered_map<std::string, std::string>& header_map, const std::string& url, const std::string& query, const char* method, int hv) {
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = hv;
	num_params = 0;
	this->num_headers = 0;
	content = std::string(cnt, cntlen);

	std::unordered_map<std::string, std::string>::const_iterator it;
	this->num_headers = 0;
	for(it=header_map.begin();it!=header_map.end();++it,this->num_headers++) {
		headers_list[this->num_headers].name = it->first.c_str();
		headers_list[this->num_headers].name_len = it->first.length();
		headers_list[this->num_headers].value = it->second.c_str();
		headers_list[this->num_headers].value_len = it->second.length();
	}

	httpVers = 1 + (float)hv/10;
	httpVersion = minor_version==1?"HTTP/1.1":"HTTP/1.0";
	this->methodv = std::string_view{method, strlen(method)};

	pathv = url;
	queryv = query;
	bool cnset = false;
	size_t ids = pathv.find_first_not_of("/");
	if(ids != std::string::npos) {
		size_t ide = pathv.find("/", ids);
		if(ide != std::string::npos) {
			cntxt_name = pathv.substr(ids, ide-ids);
			cnset = true;
		} else if(pathv.find(".")==std::string::npos) {
			cntxt_name = pathv.substr(ids);
			cnset = true;
		}
	}
	if(!cnset) {
		cntxt_name = HttpRequest::BLANK;
	}
	if(pathv.find(".")!=std::string::npos) {
		ext = pathv.substr(pathv.find("."));
		file = pathv.substr(pathv.find_last_of("/")+1);
	}
	parser_pos = -1;
}

HttpRequest::HttpRequest(std::unordered_map<std::string_view, std::string_view> header_map, std::string_view url, std::string_view qv, std::string_view method, std::string_view hv, std::string_view cnt) {
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = 0;
	num_params = 0;
	content = std::string(cnt);

	this->num_headers = 0;
	std::unordered_map<std::string_view, std::string_view>::const_iterator it;
	for(it=header_map.begin();it!=header_map.end();++it,this->num_headers++) {
		headers_list[this->num_headers].name = it->first.data();
		headers_list[this->num_headers].name_len = it->first.length();
		headers_list[this->num_headers].value = it->second.data();
		headers_list[this->num_headers].value_len = it->second.length();
	}

	httpVersion = hv;
	httpVers = CastUtil::toFloat(std::string(hv.substr(5)));
	minor_version = (httpVers-1)*10;
	this->methodv = method;

	pathv = url;
	queryv = qv.length()!=std::string::npos?qv:std::string_view(BLANK);
	bool cnset = false;
	size_t ids = pathv.find_first_not_of("/");
	if(ids != std::string::npos) {
		size_t ide = pathv.find("/", ids);
		if(ide != std::string::npos) {
			cntxt_name = pathv.substr(ids, ide-ids);
			cnset = true;
		} else if(pathv.find(".")==std::string::npos) {
			cntxt_name = pathv.substr(ids);
			cnset = true;
		}
	}
	if(!cnset) {
		cntxt_name = HttpRequest::BLANK;
	}
	if(pathv.find(".")!=std::string::npos) {
		ext = pathv.substr(pathv.find("."));
		file = pathv.substr(pathv.find_last_of("/")+1);
	}
	parser_pos = -1;
}

HttpRequest::HttpRequest(void* thdrlist, size_t num_headers, std::string_view rawUrl, std::string_view method, int hv, std::string_view cnt) {
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = hv;
	num_params = 0;
	this->num_headers = num_headers;
	phr_header_fcp* hdrlist = (phr_header_fcp*)thdrlist;
	for(int c=0;c<(int)num_headers;c++) {
		headers_list[c].name = hdrlist[c].name;
		headers_list[c].name_len = hdrlist[c].name_len;
		headers_list[c].value = hdrlist[c].value;
		headers_list[c].value_len = hdrlist[c].value_len;
	}
	httpVers = 1 + (float)hv/10;
	httpVersion = hv==1?"HTTP/1.1":"HTTP/1.0";
	this->methodv = method;
	content = std::string(cnt);
	queryv = std::string_view(BLANK);

	pathv = rawUrl;
	size_t qidx = pathv.find("?");
	if(pathv.find("?")!=std::string::npos) {
		queryv = pathv.substr(qidx+1);
		pathv = pathv.substr(0, qidx);
	}
	bool cnset = false;
	size_t ids = pathv.find_first_not_of("/");
	if(ids != std::string::npos) {
		size_t ide = pathv.find("/", ids);
		if(ide != std::string::npos) {
			cntxt_name = pathv.substr(ids, ide-ids);
			cnset = true;
		} else if(pathv.find(".")==std::string::npos) {
			cntxt_name = pathv.substr(ids);
			cnset = true;
		}
	}
	if(!cnset) {
		cntxt_name = HttpRequest::BLANK;
	}
	if(pathv.find(".")!=std::string::npos) {
		ext = pathv.substr(pathv.find("."));
		file = pathv.substr(pathv.find_last_of("/")+1);
	}
	parser_pos = -1;
}

HttpRequest::HttpRequest(void* thdrlist, size_t num_headers, std::string_view rawUrl, std::string_view qv, std::string_view method, int hv, std::string_view cnt) {
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = hv;
	num_params = 0;
	this->num_headers = num_headers;
	phr_header_fcp* hdrlist = (phr_header_fcp*)thdrlist;
	for(int c=0;c<(int)num_headers;c++) {
		headers_list[c].name = hdrlist[c].name;
		headers_list[c].name_len = hdrlist[c].name_len;
		headers_list[c].value = hdrlist[c].value;
		headers_list[c].value_len = hdrlist[c].value_len;
	}
	httpVers = 1 + (float)hv/10;
	httpVersion = hv==1?"HTTP/1.1":"HTTP/1.0";
	this->methodv = method;
	content = std::string(cnt);

	pathv = rawUrl;
	queryv = qv.length()!=std::string::npos?qv:std::string_view(BLANK);;
	bool cnset = false;
	size_t ids = pathv.find_first_not_of("/");
	if(ids != std::string::npos) {
		size_t ide = pathv.find("/", ids);
		if(ide != std::string::npos) {
			cntxt_name = pathv.substr(ids, ide-ids);
			cnset = true;
		} else if(pathv.find(".")==std::string::npos) {
			cntxt_name = pathv.substr(ids);
			cnset = true;
		}
	}
	if(!cnset) {
		cntxt_name = HttpRequest::BLANK;
	}
	if(pathv.find(".")!=std::string::npos) {
		ext = pathv.substr(pathv.find("."));
		file = pathv.substr(pathv.find_last_of("/")+1);
	}
	parser_pos = -1;
}

HttpRequest::HttpRequest(std::string_view rawUrl, std::string_view method, int hv, const char *headers, size_t headers_len, const char *body, size_t body_len) {
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = hv;
	num_params = 0;
	int temp;
	num_headers = 100;
	parser_pos = phr_parse_headers_fcp(headers, headers_len, headers_list, &num_headers, 0, &temp);
	httpVers = 1 + (float)hv/10;
	httpVersion = hv==1?"HTTP/1.1":"HTTP/1.0";
	this->methodv = method;
	content = std::string(body, body_len);
	queryv = std::string_view(BLANK);

	pathv = rawUrl;
	size_t qidx = pathv.find("?");
	if(pathv.find("?")!=std::string::npos) {
		queryv = pathv.substr(qidx+1);
		pathv = pathv.substr(0, qidx);
	}
	bool cnset = false;
	size_t ids = pathv.find_first_not_of("/");
	if(ids != std::string::npos) {
		size_t ide = pathv.find("/", ids);
		if(ide != std::string::npos) {
			cntxt_name = pathv.substr(ids, ide-ids);
			cnset = true;
		} else if(pathv.find(".")==std::string::npos) {
			cntxt_name = pathv.substr(ids);
			cnset = true;
		}
	}
	if(!cnset) {
		cntxt_name = HttpRequest::BLANK;
	}
	if(pathv.find(".")!=std::string::npos) {
		ext = pathv.substr(pathv.find("."));
		file = pathv.substr(pathv.find_last_of("/")+1);
	}
}

HttpRequest::HttpRequest(const char *headers, size_t headers_len, const char *body, size_t body_len) {
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	num_headers = 0;
	num_params = 0;
	queryv = std::string_view(BLANK);

	if(headers_len>0) {
		char *method, *path;
		size_t method_len, path_len;
		int content_length;
		num_headers = 100;
		if((parser_pos = phr_parse_request_fcp(headers, headers_len, (const char **)&method, &method_len, (const char **)&path, &path_len,
					&minor_version, headers_list, &num_headers, 0, &content_length))<0) {
			status = &HTTPResponseStatus::BadRequest;
		} else {
			content = std::string(body, body_len);
			methodv = std::string_view{method, method_len};
			pathv = std::string_view{path, path_len};
			size_t qidx = pathv.find("?");
			if(pathv.find("?")!=std::string::npos) {
				queryv = pathv.substr(qidx+1);
				pathv = pathv.substr(0, qidx);
			}
			httpVers = 1 + (float)minor_version/10;
			httpVersion = minor_version==1?"HTTP/1.1":"HTTP/1.0";

			bool cnset = false;
			size_t ids = pathv.find_first_not_of("/");
			if(ids != std::string::npos) {
				size_t ide = pathv.find("/", ids);
				if(ide != std::string::npos) {
					cntxt_name = pathv.substr(ids, ide-ids);
					cnset = true;
				} else if(pathv.find(".")==std::string::npos) {
					cntxt_name = pathv.substr(ids);
					cnset = true;
				}
			}
			if(!cnset) {
				cntxt_name = HttpRequest::BLANK;
			}
			if(pathv.find(".")!=std::string::npos) {
				ext = pathv.substr(pathv.find("."));
				file = pathv.substr(pathv.find_last_of("/")+1);
			}
		}
	}
}

HttpRequest::HttpRequest(std::string &&data, int* content_length) : headers_data{std::move(data)} {
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = 1;
	num_headers = 0;
	num_params = 0;
	queryv = std::string_view(BLANK);

	if(headers_data.length()>0) {
		char *method, *path;
		size_t method_len, path_len;
		num_headers = sizeof(headers_list) / sizeof(headers_list[0]);
		if((parser_pos=phr_parse_request_fcp(headers_data.c_str(), headers_data.length(), (const char **)&method, &method_len, (const char **)&path, &path_len,
					&minor_version, headers_list, &num_headers, 0, content_length))<0) {
			status = &HTTPResponseStatus::BadRequest;
		} else {
			methodv = std::string_view{method, method_len};
			pathv = std::string_view{path, path_len};
			size_t qidx = pathv.find("?");
			if(pathv.find("?")!=std::string::npos) {
				queryv = pathv.substr(qidx+1);
				pathv = pathv.substr(0, qidx);
			}
			httpVers = 1 + (float)minor_version/10;
			httpVersion = minor_version==1?"HTTP/1.1":"HTTP/1.0";

			bool cnset = false;
			size_t ids = pathv.find_first_not_of("/");
			if(ids != std::string::npos) {
				size_t ide = pathv.find("/", ids);
				if(ide != std::string::npos) {
					cntxt_name = pathv.substr(ids, ide-ids);
					cnset = true;
				} else if(pathv.find(".")==std::string::npos) {
					cntxt_name = pathv.substr(ids);
					cnset = true;
				}
			}
			if(!cnset) {
				cntxt_name = HttpRequest::BLANK;
			}
			if(pathv.find(".")!=std::string::npos) {
				ext = pathv.substr(pathv.find("."));
				file = pathv.substr(pathv.find_last_of("/")+1);
			}
		}
	}
}

bool HttpRequest::parseHeaders(int* content_length) {
	if(headers_data.length()>0 && httpVers>0) {
		int r;
		const char *buf = headers_data.c_str()+parser_pos, *buf_end = headers_data.c_str() + headers_data.length();
		return parse_headers_fcp(buf, buf_end, headers_list, &num_headers, num_headers, &r, content_length)!=NULL;
	}
	return false;
}

void HttpRequest::reset(std::string&& data, int* content_length) {
	cntxt_root = NULL;
	headers_data = data;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = 1;
	num_headers = 0;
	num_params = 0;
	ext.clear();
	file.clear();
	queryv = std::string_view(BLANK);

	if(headers_data.length()>0) {
		char *method, *path;
		size_t method_len, path_len;
		num_headers = sizeof(headers_list) / sizeof(headers_list[0]);
		if((parser_pos=phr_parse_request_fcp(headers_data.c_str(), headers_data.length(), (const char **)&method, &method_len, (const char **)&path, &path_len,
					&minor_version, headers_list, &num_headers, 0, content_length, isLazyHeaderParsing)<0)) {
			status = &HTTPResponseStatus::BadRequest;
		} else {
			methodv = std::string_view{method, method_len};
			pathv = std::string_view{path, path_len};
			size_t qidx = pathv.find("?");
			if(pathv.find("?")!=std::string::npos) {
				queryv = pathv.substr(qidx+1);
				pathv = pathv.substr(0, qidx);
			}
			httpVers = 1 + (float)minor_version/10;
			httpVersion = minor_version==1?"HTTP/1.1":"HTTP/1.0";

			bool cnset = false;
			size_t ids = pathv.find_first_not_of("/");
			if(ids != std::string::npos) {
				size_t ide = pathv.find("/", ids);
				if(ide != std::string::npos) {
					cntxt_name = pathv.substr(ids, ide-ids);
					cnset = true;
				} else if(pathv.find(".")==std::string::npos) {
					cntxt_name = pathv.substr(ids);
					cnset = true;
				}
			}
			if(!cnset) {
				cntxt_name = HttpRequest::BLANK;
			}
			if(pathv.find(".")!=std::string::npos) {
				ext = pathv.substr(pathv.find("."));
				file = pathv.substr(pathv.find_last_of("/")+1);
			}
		}
	}

	cookie = false;
	corsRequest = false;
	isInit = true;
	content.clear();
	content_boundary.clear();
	content_tfile.clear();
	localeInfo.clear();
	ranges.clear();
	userName.clear();
	password.clear();
	authMethod.clear();
	actUrlParts.clear();
	reqorderinf.clear();
	authorderinf.clear();
	contentList.clear();
	requestParams.clear();
	requestParamsF.clear();
	queryParams.clear();
	cookieattrs.clear();
	authinfo.clear();
	headers.clear();
	url.clear();
	curl.clear();
	actUrl.clear();
}

const std::string_view& HttpRequest::getPath() {
	return pathv;
}

const std::string_view& HttpRequest::getQueryStr() {
	return queryv;
}

HttpRequest::HttpRequest() {
	cntxt_root = NULL;
	resp = NULL;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = false;
	status = NULL;
	minor_version = 1;
	num_headers = 0;
	num_params = 0;
	parser_pos = -1;
}

HttpRequest::HttpRequest(const std::string& path) {
	cntxt_root = NULL;
	resp = NULL;
	this->webpath = path;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
	isInit = true;
	minor_version = 1;
	num_headers = 0;
	status = NULL;
	num_params = 0;
	parser_pos = -1;
}

void HttpRequest::updateContent()
{
	corsRequest = hasHeader(Origin);
	if(methodv=="GET" || methodv=="get")return;
	if(this->content!=BLANK)
	{
		updateFromContentStr();
	}
	else if(this->content_tfile!=BLANK)
	{
		updateFromContentFile();
	}
}

//@TODO -- need to change this only for one pass of reading request body
void HttpRequest::updateFromContentStr()
{
	//cout << this->getContent() << std::flush;
	if(this->getHeader(ContentType).find("application/x-www-form-urlencoded")==0)
	{
		strVec params;
		std::string valu = CryptoHandler::urlDecode(this->getContent());
		StringUtil::split(params,valu , ("&"));
		std::map<std::string ,int> indices;
		RMap::iterator it;
		for(unsigned j=0;j<params.size();j++)
		{
			strVec param;
			StringUtil::split(param, params.at(j), ("="));
			if(param.size()==2)
			{
				std::string attN = param.at(0);
				StringUtil::replaceFirst(attN,"\r",BLANK);
				StringUtil::replaceFirst(attN,"\t",BLANK);
				StringUtil::replaceFirst(attN," ",BLANK);
				if(attN.find("[")!=std::string::npos && attN.find("]")!=std::string::npos)
				{
					if(indices.find(attN)==indices.end())
					{
						indices[attN] = 0;
					}
					else
					{
						indices[attN] = indices[attN] + 1;
					}
					this->requestParams[attN.substr(0, attN.find("[")+1)
							  + CastUtil::fromNumber(indices[attN])
							  + "]"] = CryptoHandler::urlDecode(param.at(1));
				}
				else
				{
					this->setRequestParam(attN,CryptoHandler::urlDecode(param.at(1)));
				}
				reqorderinf[reqorderinf.size()+1] = attN;
			}
		}
	}
	else if(this->getContent()!=BLANK && this->getContent_boundary()!=BLANK)
	{
		std::string contemp = this->getContent();

		bool bcontstarts = false, bhdrstarts = false, bcontends = false;
		std::string filen;
		std::ofstream ofile;
		std::string temp;
		std::string delb = this->getContent_boundary();
		std::string delend = this->getContent_boundary()+"--";
		std::string cont;
		std::vector<std::string> hdrs;
		std::map<std::string ,int> indices;
		//std::cout << delb << std::endl;
		//std::cout << delend << std::endl;
		while(contemp!=BLANK)
		{
			if(contemp.find("\n")!=std::string::npos)
			{
				temp = contemp.substr(0, contemp.find("\n"));
				contemp = contemp.substr(contemp.find("\n")+1);
			}
			else
			{
				temp = contemp;
				contemp = BLANK;
			}
			if(bcontends)
			{
				epilogue.append(temp+"\n");
			}
			else if(hdrs.size()>0 && temp.find(delb)==0)
			{
				bcontstarts = false;
				bhdrstarts = true;
				MultipartContent content(hdrs);
				if(ofile.is_open())
				{
					content.setTempFileName(filen);
					ofile.close();
					ofile.clear();
				}
				else
				{
					content.setContent(cont);
				}
				if(content.getName()==BLANK)
				{
					addContent(content);
				}
				else
				{
					std::string attN = CryptoHandler::urlDecode(content.getName());
					if(attN.find("[")!=std::string::npos && attN.find("]")!=std::string::npos)
					{
						if(indices.find(attN)==indices.end())
						{
							indices[attN] = 0;
						}
						else
						{
							indices[attN] = indices[attN] + 1;
						}
						addMultipartFormContent(attN.substr(0, attN.find("[")+1)
								  + CastUtil::fromNumber(indices[attN])
								  + "]", content);
					}
					else if(indices.find(attN)!=indices.end() || requestParamsF.find(attN)!=requestParamsF.end())
					{
						if(requestParamsF.find(attN)!=requestParamsF.end())
						{
							MultipartContent content = requestParamsF[attN];
							requestParamsF.erase(attN);

							if(indices.find(attN)==indices.end())
							{
								indices[attN] = 0;
							}
							else
							{
								indices[attN] = indices[attN] + 1;
							}
							addMultipartFormContent(attN+"["
									  + CastUtil::fromNumber(indices[attN])
									  + "]", content);
						}

						if(indices.find(attN)==indices.end())
						{
							indices[attN] = 0;
						}
						else
						{
							indices[attN] = indices[attN] + 1;
						}
						addMultipartFormContent(attN+"["
								  + CastUtil::fromNumber(indices[attN])
								  + "]", content);
						/*std::cout << ("creating array from similar params" + attN+"["
										  + CastUtil::fromNumber(indices[attN])
										  + "]") << std::endl;*/

					}
					else
					{
						addMultipartFormContent(attN, content);
					}
					reqorderinf[reqorderinf.size()+1] = attN;
				}
				hdrs.clear();
				cont = BLANK;
			}
			else if(temp.find(delend)==0)
			{
				bcontends = true;
			}
			else if(bhdrstarts)
			{
				if(temp=="\r" || temp==BLANK)
				{
					bcontstarts = true;
					bhdrstarts = false;
				}
				else
				{
					hdrs.push_back(temp);
					if(StringUtil::toLowerCopy(temp).find("content-disposition: ")!=std::string::npos
							&& StringUtil::toLowerCopy(temp).find("filename")!=std::string::npos)
					{
						filen = this->getContent_boundary();
						StringUtil::replaceAll(filen, "-", BLANK);
						filen = this->getCntxt_root() + "/temp/"+ filen + CastUtil::fromNumber(Timer::getCurrentTime());
						ofile.open(filen.c_str(), std::ios::binary | std::ios::app);
					}
				}
			}
			else if(hdrs.size()==0 && temp.find(delb)==0)
			{
				bhdrstarts = true;
			}
			else if(bcontstarts)
			{
				temp += "\n";
				if(ofile.is_open())
				{
					ofile.write(temp.c_str(), temp.length());
				}
				else
				{
					cont.append(temp);
				}
			}
			else
			{
				preamble.append(temp+"\n");
			}
		}
		content = BLANK;
	}
}

void HttpRequest::updateFromContentFile()
{
	std::ifstream infile(this->content_tfile.c_str(), std::ios::binary);
	if(infile.is_open())
	{
		if(this->getContent_boundary()!=BLANK)
		{
			bool bcontstarts = false, bhdrstarts = false, bcontends = false;
			std::string filen;
			std::ofstream ofile;
			std::string temp;
			std::string delb = this->getContent_boundary();
			std::string delend = this->getContent_boundary()+"--";
			std::string cont;
			std::vector<std::string> hdrs;
			std::map<std::string ,int> indices;
			//std::cout << delb << std::endl;
			//std::cout << delend << std::endl;
			while(getline(infile, temp))
			{
				if(bcontends)
				{
					epilogue.append(temp+"\n");
				}
				else if(hdrs.size()>0 && temp.find(delb)==0)
				{
					bcontstarts = false;
					bhdrstarts = true;
					MultipartContent content(hdrs);
					if(ofile.is_open())
					{
						content.setTempFileName(filen);
						ofile.close();
						ofile.clear();
					}
					else
					{
						content.setContent(cont);
					}
					if(content.getName()==BLANK)
					{
						addContent(content);
					}
					else
					{
						std::string attN = CryptoHandler::urlDecode(content.getName());
						if(attN.find("[")!=std::string::npos && attN.find("]")!=std::string::npos)
						{
							if(indices.find(attN)==indices.end())
							{
								indices[attN] = 0;
							}
							else
							{
								indices[attN] = indices[attN] + 1;
							}
							addMultipartFormContent(attN.substr(0, attN.find("[")+1)
									  + CastUtil::fromNumber(indices[attN])
									  + "]", content);
						}
						else if(indices.find(attN)!=indices.end() || requestParamsF.find(attN)!=requestParamsF.end())
						{
							if(requestParamsF.find(attN)!=requestParamsF.end())
							{
								MultipartContent content = requestParamsF[attN];
								requestParamsF.erase(attN);

								if(indices.find(attN)==indices.end())
								{
									indices[attN] = 0;
								}
								else
								{
									indices[attN] = indices[attN] + 1;
								}
								addMultipartFormContent(attN+"["
										  + CastUtil::fromNumber(indices[attN])
										  + "]", content);
							}

							if(indices.find(attN)==indices.end())
							{
								indices[attN] = 0;
							}
							else
							{
								indices[attN] = indices[attN] + 1;
							}
							addMultipartFormContent(attN+"["
									  + CastUtil::fromNumber(indices[attN])
									  + "]", content);

						}
						else
						{
							addMultipartFormContent(attN, content);
						}
						reqorderinf[reqorderinf.size()+1] = attN;
					}
					hdrs.clear();
					cont = BLANK;
				}
				else if(temp.find(delend)==0)
				{
					bcontends = true;
				}
				else if(bhdrstarts)
				{
					if(temp=="\r" || temp==BLANK)
					{
						bcontstarts = true;
						bhdrstarts = false;
					}
					else
					{
						hdrs.push_back(temp);
						if(StringUtil::toLowerCopy(temp).find("content-disposition: ")!=std::string::npos
								&& StringUtil::toLowerCopy(temp).find("filename")!=std::string::npos)
						{
							filen = this->getContent_boundary();
							StringUtil::replaceAll(filen, "-", BLANK);
							filen = this->getCntxt_root() + "/temp/"+ filen + CastUtil::fromNumber(Timer::getCurrentTime());
							ofile.open(filen.c_str(), std::ios::binary | std::ios::app);
						}
					}
				}
				else if(hdrs.size()==0 && temp.find(delb)==0)
				{
					bhdrstarts = true;
				}
				else if(bcontstarts)
				{
					temp += "\n";
					if(ofile.is_open())
					{
						ofile.write(temp.c_str(), temp.length());
					}
					else
					{
						cont.append(temp);
					}
				}
				else
				{
					preamble.append(temp+"\n");
				}
			}
		}
		else
		{
			std::string temp;
			while(getline(infile, temp))
			{
				content.append(temp);
			}
		}
		infile.close();
	}
}

void HttpRequest::buildRequestC(const char *keyc, const char *valuec)
{
	buildRequest(std::string(keyc), std::string(valuec));
}

void HttpRequest::buildRequest(std::string key, std::string value)
{
	int hdrswc = -1;
	if(HDRS_SW_CODES.find(key)!=HDRS_SW_CODES.end()) {
		hdrswc = HDRS_SW_CODES[key];
	}
	switch(hdrswc) {
		case 3:
		{
			StringUtil::trim(value);
			strVec lemp;
			StringUtil::split(lemp, value, (","));
			for(unsigned int li=0;li<lemp.size();li++)
			{
				if(lemp.at(li).find(";")==std::string::npos && lemp.at(li)!=BLANK)
				{
					std::string t = lemp.at(li);
					size_t s = t.find_first_not_of(" ");
					size_t e = t.find_last_not_of(" ")+1;
					t = t.substr(s,e-s);
					this->localeInfo.push_back(t);
				}
				else if(lemp.at(li)!=BLANK)
				{
					std::string t = lemp.at(li);
					size_t s = t.find_first_not_of(" ");
					size_t e = t.find(";");
					t = t.substr(s,e-s);
					e = t.find_last_not_of(" ")+1;
					t = t.substr(0,e);
					this->localeInfo.push_back(t);
				}
			}
			addHeader(key, value);
			break;
		}
		case 7:
		{
			StringUtil::trim(value);
			this->getAuthParams(value);
			addHeader(key, value);
			break;
		}
		case 10:
		{
			StringUtil::trim(value);
			this->cookie = true;
			strVec results;
			StringUtil::split(results, value, ("; "));
			for(int j=0;j<(int)results.size();j++)
			{
				strVec results1;
				StringUtil::split(results1, results.at(j), ("="));
				if(results1.size()==2)
					cookieattrs[results1.at(0)] = results1.at(1);
				else
					cookieattrs[results1.at(0)] = "true";
			}
			//addHeader(key, value);
			break;
		}
		case 13:
		{
			StringUtil::trim(value);
			std::string tempi(value);
			size_t s = tempi.find("boundary");
			if(s!=std::string::npos)
			{
				addHeader(key, tempi.substr(0,s));
				tempi = tempi.substr(s);
				strVec results;
				StringUtil::split(results, tempi, ("="));
				if(results.size()==2)
				{
					std::string bound = "--" + results.at(1).substr(0,results.at(1).length());
					this->setContent_boundary(bound);
				}
			}
			else
			{
				addHeader(key, value);
			}
			break;
		}
		case 42:
		{
			content.append(value);
			break;
		}
		case 43:
		{
			this->setMethod(value);
			break;
		}
		case 44:
		{
			this->httpVersion = value;
			std::string versionStr = StringUtil::replaceFirstCopy(StringUtil::toLowerCopy(value), "http/", BLANK);
			StringUtil::trim(versionStr);
			float version = -1;
			try {
				version = CastUtil::toFloat(versionStr);
				this->httpVers = version;
			} catch(const std::exception& e) {
			}
			break;
		}
		case 45:
		{
			strVec params;
			std::map<std::string ,int> indices;
			value = CryptoHandler::urlDecode(value);
			StringUtil::split(params, value, ("&"));
			for(unsigned j=0;j<params.size();j++)
			{
				strVec param;
				StringUtil::split(param, params.at(j), ("="));
				if(param.size()==2)
				{
					std::string attN = param.at(0);
					StringUtil::replaceFirst(attN,"\r",BLANK);
					StringUtil::replaceFirst(attN,"\t",BLANK);
					StringUtil::replaceFirst(attN," ",BLANK);
					if(attN.find("[")!=std::string::npos && attN.find("]")!=std::string::npos)
					{
						if(indices.find(attN)==indices.end())
						{
							indices[attN] = 0;
						}
						else
						{
							indices[attN] = indices[attN] + 1;
						}
						this->queryParams[attN.substr(0, attN.find("[")+1)
								  + CastUtil::fromNumber(indices[attN])
								  + "]"] = CryptoHandler::urlDecode(param.at(1));
					}
					else
					{
						this->setQueryParam(attN,param.at(1));
					}
					reqorderinf[reqorderinf.size()+1] = attN;
				}
			}
			break;
		}
		case 46:
		{
			//strVec memp;
			this->setActUrl(value);
			//StringUtil::split(memp, value, ("/"));
			/*int fs = value.find_first_of("/");
			int es = value.find_last_of("/");
			if(fs==es)
			{
				this->setCntxt_name(BLANK);
				this->setFile(value.substr(es+1));
			}
			else
			{
				int ss = value.substr(fs+1).find("/");
				if(ss>fs)
				{
					this->setCntxt_name(value.substr(fs+1,ss-fs));
					this->setFile(value.substr(es+1));
				}
			}
			this->setCntxt_root(webpath + "/" + cntxt_name);*/
			break;
		}
		case 47:
		{
			strVec vemp;
			StringUtil::split(vemp, value, (" "));
			if(vemp.size()<3)
			{
				status = &HTTPResponseStatus::BadRequest;
				return;
			}
			else if(!isValidHttpMethod(vemp.at(0)))
			{
				status = &HTTPResponseStatus::InvalidMethod;
				return;
			}
			else
			{
				std::string versionStr = StringUtil::replaceFirstCopy(StringUtil::toLowerCopy(vemp.at(2)), "http/", BLANK);
				StringUtil::trim(versionStr);
				float version = -1;
				try {
					version = CastUtil::toFloat(versionStr);
					this->httpVers = version;
				} catch(const std::exception& e) {
					status = &HTTPResponseStatus::HttpVersionNotSupported;
					return;
				}
				if(version<1.0 && version>1.1)
				{
					status = &HTTPResponseStatus::HttpVersionNotSupported;
					return;
				}
				if(version<1.1 && StringUtil::toLowerCopy(vemp.at(0))=="options")
				{
					status = &HTTPResponseStatus::InvalidMethod;
					return;
				}
			}

			if(!isValidHttpMethod(vemp.at(0)))
			{
				status = &HTTPResponseStatus::InvalidMethod;
				return;
			}
			this->setMethod(StringUtil::toUpperCopy(vemp.at(0)));
			vemp.erase(vemp.begin());
			StringUtil::replaceFirst(vemp.at(1),"\r",BLANK);
			this->httpVersion = vemp.at(1);
			StringUtil::replaceFirst(vemp.at(0)," ",BLANK);
			if(vemp.at(0).find("?")!=std::string ::npos)
			{
				std::string valu(vemp.at(0));
				vemp[0] = valu.substr(0,vemp.at(0).find("?"));
				valu = CryptoHandler::urlDecode(valu.substr(valu.find("?")+1));
				strVec params;
				std::map<std::string ,int> indices;
				StringUtil::split(params, valu, ("&"));
				for(unsigned j=0;j<params.size();j++)
				{
					strVec param;
					StringUtil::split(param, params.at(j), ("="));
					if(param.size()==2)
					{
						std::string attN = param.at(0);
						StringUtil::replaceFirst(attN,"\r",BLANK);
						StringUtil::replaceFirst(attN,"\t",BLANK);
						StringUtil::replaceFirst(attN," ",BLANK);
						if(attN.find("[")!=std::string::npos && attN.find("]")!=std::string::npos)
						{
							if(indices.find(attN)==indices.end())
							{
								indices[attN] = 0;
							}
							else
							{
								indices[attN] = indices[attN] + 1;
							}
							this->queryParams[attN.substr(0, attN.find("[")+1)
									  + CastUtil::fromNumber(indices[attN])
									  + "]"] = CryptoHandler::urlDecode(param.at(1));
						}
						else
						{
							this->setQueryParam(attN,param.at(1));
						}
						reqorderinf[reqorderinf.size()+1] = attN;
					}
				}
			}
			this->setActUrl(vemp.at(0));
			break;
		}
		case -1:
		{
			addHeaderValue(key, value);
			break;
		}
		default:
		{
			addHeader(key, value);
			break;
		}
	}
}

std::string HttpRequest::toString()
{
	std::string ret;
	if(num_headers>0) {
		for(int i=0;i<(int)num_headers;++i) {
			ret += "\n" + std::string(headers_list[i].name, headers_list[i].name_len) + ": " + std::string(headers_list[i].value, headers_list[i].value_len);
		}
	} else {
		RMap::iterator it;
		for(it=headers.begin();it!=headers.end();++it)
		{
			ret += "\n" + it->first + ": " + it->second;
		}
	}
	ret += "\nContent: "+this->getContent();
	ret += "\nHttp Version: "+this->getHttpVersion();
	ret += "\nMethod: "+std::string(this->getMethod());
	ret += "\nUrl: "+this->getUrl();
	ret += "\nFile: "+this->getFile();
	ret += "\nContext Name: ";
	ret.append(this->getCntxt_name());
	ret += "\nContext Root: "+this->getCntxt_root();
	ret += "\nDefault Locale: "+this->getDefaultLocale();
	ret += "\nContent Boundary: "+this->getContent_boundary();
	std::string vals;
	std::cout << "logging request data " << std::endl;
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			vals+= ("\nKey: "+iter->first + " Value: "+iter->second);
		}
	}
	std::cout << "logging multipart data " << std::endl;
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();++iter)
		{
			MultipartContent dat = iter->second;
			vals+= ("\nKey: "+iter->first);
			std::cout << dat.getHeaders().size() << std::endl;
			RMap::iterator it1;
			for(it1=dat.headers.begin();it1!=dat.headers.end();++it1)
			{
				vals += "\n\t" + it1->first + ": " + it1->second;
			}
			vals+= ("\n\tFileName: "+dat.fileName);
			vals+= ("\n\tTempFileName: "+dat.tempFileName);
		}
	}
	ret += "\nRequest Parameters "+vals;//CastUtil::fromNumber(this->getRequestParams().size());
	return ret;
}


HttpRequest::~HttpRequest() {
}

const std::string_view& HttpRequest::getMethod() const
{
	return methodv;
}

void HttpRequest::setMethod(const std::string& method)
{
	//this->method = method;
}

HttpSession* HttpRequest::getSession()
{
	return &(this->session);
}

std::string HttpRequest::getUrl() const
{
	return this->url;
}

void HttpRequest::setCurl(std::string curl)
{
	this->curl = curl;
}

std::string HttpRequest::getCurl() const
{
	return this->curl;
}

float HttpRequest::getHttpVers() const
{
	return this->httpVers;
}

std::string HttpRequest::getHttpVersion() const
{
	return this->httpVersion;
}

std::string HttpRequest::getContent_boundary() const
{
	return content_boundary;
}

void HttpRequest::setContent_boundary(const std::string& content_boundary)
{
	this->content_boundary = content_boundary;
}

std::string HttpRequest::getContent() const
{
	return content;
}

void HttpRequest::setContent(std::string &data)
{
	content = data;
}

void HttpRequest::setContent(std::string &&data)
{
	content = data;
}

RMap HttpRequest::getRequestParams() const
{
	return requestParams;
}

void HttpRequest::setRequestParams(const RMap& requestParams)
{
	this->requestParams = requestParams;
}

std::string HttpRequest::getRequestParam(const std::string& key)
{
	if(this->requestParams.find(key)!=this->requestParams.end())
		return this->requestParams[key];
	else if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].getContent();
	else
		return BLANK;
}

MultipartContent HttpRequest::getMultipartContent(const std::string& key)
{
	MultipartContent cont;
	if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key];
	else
		return cont;
}


std::string HttpRequest::getRequestParamType(const std::string& key)
{
	if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].getHeader("Content-Type");
	else
		return BLANK;
}

void HttpRequest::setRequestParam(const std::string& key, const std::string& value)
{
	this->requestParams[key] = value;
}

std::string HttpRequest::getCntxt_root() const
{
	return *cntxt_root;
}

void HttpRequest::setCntxt_root(const std::string& cntxt_root)
{
	this->cntxt_root = &cntxt_root;
}

std::string HttpRequest::getDefaultLocale() const
{
	if(this->localeInfo.size()>0)
		return this->localeInfo.at(0);
	else
		return "en";
}

const std::string_view& HttpRequest::getCntxt_name() const
{
	return cntxt_name;
}

void HttpRequest::setCntxt_name(std::string_view cntxt_name)
{
	this->cntxt_name = cntxt_name;
}

std::string HttpRequest::getFile() const
{
	return file;
}

void HttpRequest::setFile(const std::string& file)
{
	if(this->file!=BLANK && this->url.find(this->file)!=std::string::npos
			&& this->url.find("/")!=std::string::npos)
	{
		this->url = this->url.substr(0, this->url.find_last_of("/")+1) +  file;
	}
	else
	{
		this->url +=  "/" + file;
	}
	this->file = file;
	this->ext = getFileExtension(file);
}

void HttpRequest::setUrl(std::string url)
{
	StringUtil::replaceFirst(url,"//","/");
	this->url = url;
	this->ext = getFileExtension(url);
}

std::string HttpRequest::getActUrl() const
{
	return actUrl;
}

void HttpRequest::setActUrl(const std::string& actUrl)
{
	std::string au = actUrl;
	if(au.at(0)=='/') {
		au = au.substr(1);
	}
	if(au!=BLANK) {
		StringUtil::split(actUrlParts, au, ("/"));
		if(actUrlParts.size()>1 || (actUrlParts.size()==1 && actUrlParts[0].find(".")==std::string::npos)) {
			cntxt_name = actUrlParts[0];
		}
		if(actUrlParts.size()>0 && actUrlParts[actUrlParts.size()-1].find(".")!=std::string::npos) {
			file = actUrlParts[actUrlParts.size()-1];
		}
	}
	if(actUrl.at(0)!='/') {
		this->actUrl = "/" + actUrl;
	} else {
		this->actUrl = actUrl;
	}
}

void HttpRequest::normalizeUrl()
{
	if(pathv.at(0)!='/') {
		this->actUrl.append(pathv.substr(1));
		StringUtil::split(actUrlParts, actUrl, "/");
		this->actUrl = "/" + actUrl;
	} else {
		this->actUrl.append(pathv);
		StringUtil::split(actUrlParts, actUrl.substr(1), "/");
	}
	if(cntxt_name!=BLANK && actUrlParts.size()>0 && actUrlParts.at(0)!=cntxt_name) {
		actUrlParts.insert(actUrlParts.begin(), std::string(cntxt_name));
		curl = "/";
		curl.append(cntxt_name);
		curl.append(actUrl);
		url = *this->cntxt_root + actUrl;
	} else {
		curl = actUrl;
		url = *this->cntxt_root;
		if(url.at(url.length()-1)=='/') {
			url = url.substr(0, url.length()-1);
		}
		url = url.substr(0, url.find_last_of("/")) + actUrl;
	}
	if(queryv.size()>0) {
		std::string qd = CryptoHandler::urlDecode(std::string(queryv));
		num_params = yuarel_parse_query((char*)qd.c_str(), qd.size(), params, 100);
		for(int i=0;i<num_params;i++) {
			queryParams[std::string(params[i].key, params[i].key_len)] = std::string(params[i].val, params[i].val_len);
		}
	}
}

const std::vector<std::string>& HttpRequest::getActUrlParts() const
{
	return actUrlParts;
}

RMap HttpRequest::getAuthinfo() const
{
	return authinfo;
}

void HttpRequest::setAuthinfo(const RMap& authinfo)
{
	this->authinfo = authinfo;
}

#ifdef INC_SCRH
std::string HttpRequest::toPHPVariablesString(const std::string& def)
{
	std::string ret;
	ret = "<?php";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string header = StringUtil::toUpperCopy(it->first);
		StringUtil::replaceAll(header, "-", "_");
		ret += "$_SERVER['HTTP_"+header+"'] = '"+it->second + "';\n";
	}
	ret += "$HTTP_RAW_POST_DATA = '"+this->getContent();
	ret += "';\n$_SERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "';\n$_SERVER['REQUEST_METHOD'] = '"+this->getMethod();
	std::string requri = this->getActUrl();
	StringUtil::replaceFirst(requri, ("/"+this->getCntxt_name()), BLANK);
	if(requri==BLANK)
		requri = "/";
	ret += "';\n$_SERVER['REQUEST_URI'] = '"+requri;
	ret += "';\n$_SERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "';\n$_SERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	if(def==BLANK)
		ret += "';\n$_SERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	else
		ret += "';\n$_SERVER['DOCUMENT_ROOT'] = '/"+this->getCntxt_name();
	ret += "';\n$_SERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "';\n$_SERVER['HTTP_CONTENT_BOUNDARY'] = '"+this->getContent_boundary() + "';\n";
	if(this->queryParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->queryParams.begin();iter!=this->queryParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				ret += "\nif(!isset($_GET['"+iter->first.substr(0, iter->first.find("["))+"']))\n{\n$_GET['"+iter->first.substr(0, iter->first.find("["))+"']=array();\n}\n";
				ret += ("\n$_GET['"+iter->first.substr(0, iter->first.find("["))+"']" +
					iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "';");
			}
			else
				ret += "\n$_GET['"+iter->first+"'] = '"+ iter->second + "';";
		}
	}
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				ret += "\nif(!isset($_POST['"+iter->first.substr(0, iter->first.find("["))+"']))\n{\n$_POST['"+iter->first.substr(0, iter->first.find("["))+"']=array();\n}\n";
				ret += ("\n$_POST['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "';");
			}
			else
				ret += "\n$_POST['"+iter->first+"'] = '"+iter->second + "';";
		}
	}
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();iter++)
		{
			MultipartContent dat = iter->second;
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				ret += "\nif(!isset($_FILES['"+iter->first.substr(0, iter->first.find("["))+"']))\n{\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']=array()\n}\n";
				ret += "\nif(!isset($_FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"))\n"
						+ "{\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"=array();\n}\n";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "';";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "';";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::fromNumber(dat.getContent().length()) + ";";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.getTempFileName() + "';";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nif(!isset($_FILES['"+iter->first+"']))\n{\n$_FILES['"+iter->first+"']=array();\n}\n";
				ret += "\n$_FILES['"+iter->first+"']['name'] = '"+ dat.getFileName() + "';";
				ret += "\n$_FILES['"+iter->first+"']['type'] = '"+ dat.getHeader("Content-Type") + "';";
				ret += "\n$_FILES['"+iter->first+"']['size'] = "+ CastUtil::fromNumber(dat.getContent().length()) + ";";
				ret += "\n$_FILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "';";
				ret += "\n$_FILES['"+iter->first+"']['error'] = 0;";
			}
		}
	}
	if(def==BLANK)
		ret += "\ninclude_once('"+this->getUrl()+"');";
	else
		ret += "\ninclude_once('"+def+"');";
	ret += "?>";
	return ret;
}

std::string HttpRequest::toPerlVariablesString()
{
	std::string ret;
	ret += "$_SERVER = {};";
	ret += "\n$_SERVER_EX = {};";
	ret += "\n$_GET = {};";
	ret += "\n$_POST = {};";
	ret += "\n$_FILES = {};\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string header = StringUtil::toUpperCopy(it->first);
		StringUtil::replaceAll(header, "-", "_");
		ret += "$_SERVER['HTTP_"+header+"'] = '"+it->second + "';\n";
	}
	ret += "';\n$HTTP_RAW_POST_DATA = '"+this->getContent();
	ret += "';\n$_SERVER_EX{'HTTP_VERSION'} = '"+this->getHttpVersion();
	ret += "';\n$_SERVER{'REQUEST_METHOD'} = '"+this->getMethod();
	ret += "';\n$_SERVER{'REQUEST_URI'} = '"+this->getUrl();
	ret += "';\n$_SERVER_EX{'HTTP_REQ_FILE'} = '"+this->getFile();
	ret += "';\n$_SERVER_EX{'CONTEXT_NAME'} = '"+this->getCntxt_name();
	ret += "';\n$_SERVER{'DOCUMENT_ROOT'} = '"+this->getCntxt_root();
	ret += "';\n$_SERVER_EX{'DEFAULT_LOCALE'} = '"+this->getDefaultLocale();
	ret += "';\n$_SERVER{'HTTP_CONTENT_BOUNDARY'} = '"+this->getContent_boundary() + "';\n";
	if(this->queryParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->queryParams.begin();iter!=this->queryParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first;
				StringUtil::replaceFirst(key,"[","{");
				StringUtil::replaceFirst(key,"]","}");
				ret += "\nif(!exists $_GET{'"+key.substr(0, key.find("{"))+"'})\n{\n$_GET{'"+key.substr(0, key.find("{"))+"'}={}\n}\n";
				ret += ("\n$_GET{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + " = '"+iter->second + "';");
			}
			else
				ret += "\n$_GET{'"+iter->first+"'} = '"+ iter->second + "';";
		}
	}
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first;
				StringUtil::replaceFirst(key,"[","{");
				StringUtil::replaceFirst(key,"]","}");
				ret += "\nif(!exists $_POST{'"+key.substr(0, key.find("{"))+"'})\n{\n$_POST{'"+key.substr(0, key.find("{"))+"'}={}\n}\n";
				ret += ("\n$_POST{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + " = '"+iter->second + "';");
			}
			else
				ret += "\n$_POST{'"+iter->first+"'} = '"+iter->second + "';";
		}
	}
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();iter++)
		{
			MultipartContent dat = iter->second;
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first;
				StringUtil::replaceFirst(key,"[","{");
				StringUtil::replaceFirst(key,"]","}");
				ret += "\nif(!exists $_FILES{'"+key.substr(0, key.find("{"))+"'})\n{\n$_FILES{'"+key.substr(0, key.find("{"))+"'}={}\n}\n";
				ret += "\nif(!exists $_FILES{'"+key.substr(0, key.find("{"))+"'}{'"+key.substr(key.find("{"))+"'})\n"
						+ "{\n$_FILES{'"+key.substr(0, key.find("{"))+"'}{'"+key.substr(key.find("{"))+"'}={}\n}\n";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'name'} = '"+ dat.getFileName() + "';";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'type'} = '"+ dat.getHeader("Content-Type") + "';";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'size'} = "+ CastUtil::fromNumber(dat.getContent().length()) + ";";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'tmp_name'} = '"+ dat.getTempFileName() + "';";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'error'} = 0;";
			}
			else
			{
				ret += "\nif(!exists $_FILES{'"+iter->first+"'})\n{\n$_FILES{'"+iter->first+"'}={}\n}\n";
				ret += "\n$_FILES{'"+iter->first+"'}{'name'} = '"+ dat.getFileName() + "';";
				ret += "\n$_FILES{'"+iter->first+"'}{'type'} = '"+ dat.getHeader("Content-Type") + "';";
				ret += "\n$_FILES{'"+iter->first+"'}{'size'} = "+ CastUtil::fromNumber(dat.getContent().length()) + ";";
				ret += "\n$_FILES{'"+iter->first+"'}{'tmp_name'} = '"+ dat.getTempFileName() + "';";
				ret += "\n$_FILES{'"+iter->first+"'}{'error'} = 0;";
			}
		}
	}
	ret += "\n\n";
	return ret;
}

std::string HttpRequest::toRubyVariablesString()
{
	std::string ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string header = StringUtil::toUpperCopy(it->first);
		StringUtil::replaceAll(header, "-", "_");
		ret += "SERVER['HTTP_"+header+"'] = '"+it->second + "';\n";
	}
	ret += "'\nHTTP_RAW_POST_DATA = '"+this->getContent();
	ret += "'\nSERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "'\nSERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "'\nSERVER['REQUEST_URI'] = '"+this->getUrl();
	ret += "'\nSERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "'\nSERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	ret += "'\nSERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	ret += "'\nSERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "'\nSERVER['HTTP_CONTENT_BOUNDARY'] = '"+this->getContent_boundary() + "'\n";
	if(this->queryParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->queryParams.begin();iter!=this->queryParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(0, iter->first.find("["));
				ret += "\nif(!GET.has_key?('"+key+"'))\nGET['"+key+"']={}\nend";
				ret += ("\nGET['"+iter->first.substr(0, iter->first.find("["))+"']" +
					iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "'");
			}
			else
				ret += "\nGET['"+iter->first+"'] = '"+ iter->second + "'";
		}
	}
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(0, iter->first.find("["));
				ret += "\nif(!POST.has_key?('"+key+"'))\nPOST['"+key+"']={}\nend";
				ret += ("\nPOST['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "'");
			}
			else
				ret += "\nPOST['"+iter->first+"'] = '"+iter->second + "'";
		}
	}
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();iter++)
		{
			MultipartContent dat = iter->second;
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(iter->first.find("["));
				StringUtil::replaceFirst(key,"[",BLANK);
				StringUtil::replaceFirst(key,"]",BLANK);
				ret += "\nif(!FILES.has_key?('"+iter->first.substr(0, iter->first.find("["))
						+"'))\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']={}\nend";
				ret += "\nif(!FILES['"+iter->first.substr(0, iter->first.find("["))+"'].has_key?('"+key+"'))"
						+ "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']['"+key+"']={}\nend";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::fromNumber(dat.getContent().length());
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nFILES['"+iter->first+"'] = {}";
				ret += "\nFILES['"+iter->first+"']['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::fromNumber(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\nrequire ('"+this->getUrl()+"')\n";
	return ret;
}

std::string HttpRequest::toPythonVariablesString()
{
	std::string ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string header = StringUtil::toUpperCopy(it->first);
		StringUtil::replaceAll(header, "-", "_");
		ret += "SERVER['HTTP_"+header+"'] = '"+it->second + "';\n";
	}
	ret += "'\nHTTP_RAW_POST_DATA = '"+this->getContent();
	ret += "'\nSERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "'\nSERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "'\nSERVER['REQUEST_URI'] = '"+this->getUrl();
	ret += "'\nSERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "'\nSERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	ret += "'\nSERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	ret += "'\nSERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "'\nSERVER['HTTP_CONTENT_BOUNDARY'] = '"+this->getContent_boundary() + "'\n";
	if(this->queryParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->queryParams.begin();iter!=this->queryParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(0, iter->first.find("["));
				ret += "\nif '"+key+"' not in GET:\n\tGET['"+key+"']={}\n";
				ret += ("\nGET['"+iter->first.substr(0, iter->first.find("["))+"']" +
					iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "'");
			}
			else
				ret += "\nGET['"+iter->first+"'] = '"+ iter->second + "'";
		}
	}
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(0, iter->first.find("["));
				ret += "\nif '"+key+"' not in POST:\n\tPOST['"+key+"']={}\n";
				ret += ("\nPOST['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "'");
			}
			else
				ret += "\nPOST['"+iter->first+"'] = '"+iter->second + "'";
		}
	}
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();iter++)
		{
			MultipartContent dat = iter->second;
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(iter->first.find("["));
				StringUtil::replaceFirst(key,"[",BLANK);
				StringUtil::replaceFirst(key,"]",BLANK);
				ret += "\nif '"+iter->first.substr(0, iter->first.find("["))
						+"' not in FILES:\n\tFILES['"+iter->first.substr(0, iter->first.find("["))+"']={}\n";
				ret += "\nif '"+key+"' not in FILES['"+iter->first.substr(0, iter->first.find("["))+"']:\n"
						+ "\n\tFILES['"+iter->first.substr(0, iter->first.find("["))+"']['"+key+"']={}\n";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::fromNumber(dat.getContent().length());
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nFILES['"+iter->first+"'] = {}";
				ret += "\nFILES['"+iter->first+"']['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::fromNumber(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\n\n";
	return ret;
}

std::string HttpRequest::toLuaVariablesString()
{
	std::string ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string header = StringUtil::toUpperCopy(it->first);
		StringUtil::replaceAll(header, "-", "_");
		ret += "SERVER['HTTP_"+header+"'] = '"+it->second + "';\n";
	}
	ret += "'\nHTTP_RAW_POST_DATA = '"+this->getContent();
	ret += "'\nSERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "'\nSERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "'\nSERVER['REQUEST_URI'] = '"+this->getUrl();
	ret += "'\nSERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "'\nSERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	ret += "'\nSERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	ret += "'\nSERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "'\nSERVER['HTTP_CONTENT_BOUNDARY'] = '"+this->getContent_boundary() + "'\n";
	if(this->queryParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->queryParams.begin();iter!=this->queryParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(0, iter->first.find("["));
				ret += "\nif GET['"+key+"'] == nil then\nGET['"+key+"']={}\nend\n";
				ret += ("\nGET['"+iter->first.substr(0, iter->first.find("["))+"']" +
					iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "'");
			}
			else
				ret += "\nGET['"+iter->first+"'] = '"+ iter->second + "'";
		}
	}
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(0, iter->first.find("["));
				ret += "\nif POST['"+key+"'] == nil then\nPOST['"+key+"']={}\nend\n";
				ret += ("\nPOST['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "'");
			}
			else
				ret += "\nPOST['"+iter->first+"'] = '"+iter->second + "'";
		}
	}
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();iter++)
		{
			MultipartContent dat = iter->second;
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				std::string key = iter->first.substr(iter->first.find("["));
				StringUtil::replaceFirst(key,"[",BLANK);
				StringUtil::replaceFirst(key,"]",BLANK);
				ret += "\nif FILES['"+iter->first.substr(0, iter->first.find("["))+"'] == nil then"
						+ "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']={}\nend\n";
				ret += "\nif FILES['"+iter->first.substr(0, iter->first.find("["))+"']['"+key+"']  == nil then"
						+ "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']['"+key+"']={}\nend\n";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::fromNumber(dat.getContent().length());
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nFILES['"+iter->first+"'] = {}";
				ret += "\nFILES['"+iter->first+"']['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::fromNumber(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\ndofile(\""+this->getUrl()+"\")\n";
	return ret;
}

std::string HttpRequest::toNodejsVariablesString()
{
	std::string ret;
	ret += "SERVER = {};";
	ret += "\nSERVER_EX = {};";
	ret += "\nGET = {};";
	ret += "\nPOST = {};";
	ret += "\nFILES = {};\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string header = StringUtil::toUpperCopy(it->first);
		StringUtil::replaceAll(header, "-", "_");
		ret += "SERVER['HTTP_"+header+"'] = '"+it->second + "';\n";
	}
	ret += "';\nHTTP_RAW_POST_DATA = '"+this->getContent();
	ret += "';\nSERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "';\nSERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "';\nSERVER['REQUEST_URI'] = '"+this->getUrl();
	ret += "';\nSERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "';\nSERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	ret += "';\nSERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	ret += "';\nSERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "';\nSERVER['HTTP_CONTENT_BOUNDARY'] = '"+this->getContent_boundary() + "';\n";
	if(this->queryParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->queryParams.begin();iter!=this->queryParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				ret += "\nif((GET['"+iter->first.substr(0, iter->first.find("["))+"'])==undefined)\n{\nGET['"+iter->first.substr(0, iter->first.find("["))+"']={};\n}\n";
				ret += ("\nGET['"+iter->first.substr(0, iter->first.find("["))+"']" +
					iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "';");
			}
			else
				ret += "\nGET['"+iter->first+"'] = '"+ iter->second + "';";
		}
	}
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				ret += "\nif((POST['"+iter->first.substr(0, iter->first.find("["))+"'])==undefined)\n{\nPOST['"+iter->first.substr(0, iter->first.find("["))+"']={};\n}\n";
				ret += ("\nPOST['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + " = '"+iter->second + "';");
			}
			else
				ret += "\nPOST['"+iter->first+"'] = '"+iter->second + "';";
		}
	}
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();iter++)
		{
			MultipartContent dat = iter->second;
			if(iter->first.find("[")!=std::string::npos && iter->first.find("]")!=std::string::npos)
			{
				ret += "\nif((FILES['"+iter->first.substr(0, iter->first.find("["))+"'])==undefined)\n{\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']={};\n}\n";
				ret += "\nif((FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+")==undefined)\n"
						+ "{\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"={};\n}\n";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "';";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "';";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::fromNumber(dat.getContent().length()) + ";";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.getTempFileName() + "';";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nif(!isset(FILES['"+iter->first+"']))\n{\nFILES['"+iter->first+"']={};\n}\n";
				ret += "\nFILES['"+iter->first+"']['name'] = '"+ dat.getFileName() + "';";
				ret += "\nFILES['"+iter->first+"']['type'] = '"+ dat.getHeader("Content-Type") + "';";
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::fromNumber(dat.getContent().length()) + ";";
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "';";
				ret += "\nFILES['"+iter->first+"']['error'] = 0;";
			}
		}
	}
	ret += "\nrequire('"+this->getUrl()+"');";
	return ret;
}
#endif

RMap HttpRequest::getAllParams()
{
	std::map<std::string,std::string,std::less<>>::iterator it;
	std::map<std::string,std::string,std::less<>> reqparams = this->getRequestParams();
	std::map<std::string,std::string,std::less<>> qryparams = this->getQueryParams();
	for(it=qryparams.begin();it!=qryparams.end();it++)
	{
		reqparams[it->first] = it->second;
	}
	return reqparams;
}

RMap HttpRequest::getQueryParams() const {
	return queryParams;
}

void HttpRequest::setQueryParams(const RMap& queryParams) {
	this->queryParams = queryParams;
}

void HttpRequest::setQueryParam(const std::string& name, const std::string& value)
{
	this->queryParams[name] = value;
}

std::string HttpRequest::getQueryParam(const std::string& key)
{
	if(this->queryParams.find(key)!=this->queryParams.end())
		return this->queryParams[key];
	return BLANK;
}

bool HttpRequest::hasCookie()
{
	std::string ffeadid;
	if(cookieattrs.find("FFEADID")!=cookieattrs.end())
		ffeadid = cookieattrs["FFEADID"];
	StringUtil::trim(ffeadid);
	return this->cookie && ffeadid!=BLANK;
}

std::map<int,std::string> HttpRequest::getAuthOrderinfo() const
{
	return authorderinf;
}

std::map<int,std::string> HttpRequest::getReqOrderinfo() const
{
	return reqorderinf;
}

RMap HttpRequest::getCookieInfo() const
{
	return cookieattrs;
}

std::string HttpRequest::getAuthOrderinfoAttribute(const int& key)
{
	if(authorderinf.find(key)!=authorderinf.end())
		return authorderinf[key];
	else return BLANK;
}

std::string HttpRequest::getReqOrderinfoAttribute(const int& key)
{
	if(reqorderinf.find(key)!=reqorderinf.end())
		return reqorderinf[key];
	else return BLANK;
}

std::string HttpRequest::getCookieInfoAttribute(const std::string& key)
{
	if(cookieattrs.find(key)!=cookieattrs.end())
		return cookieattrs[key];
	else return BLANK;
}

std::string HttpRequest::getHeader(std::string_view key)
{
	if(num_headers>0) {
		for(int i=0;i<(int)num_headers;++i) {
			if(headers_list[i].name_len==key.length() && strncasecmp(headers_list[i].name, key.data(), headers_list[i].name_len)==0) {
				return std::string(headers_list[i].value, headers_list[i].value_len);
			}
		}
		return BLANK;
	}
	if(this->headers.find(key)!=this->headers.end())
		return this->headers.find(key)->second;
	return BLANK;
}

bool HttpRequest::hasHeader(std::string_view key)
{
	if(num_headers>0) {
		for(int i=0;i<(int)num_headers;++i) {
			if(headers_list[i].name_len==key.length() && strncasecmp(headers_list[i].name, key.data(), headers_list[i].name_len)==0) {
				return true;
			}
		}
		return false;
	}
	if(this->headers.find(key)!=this->headers.end())
		return true;
	return false;
}

RMap HttpRequest::getHeaders()
{
	return headers;
}

bool HttpRequest::isCorsRequest() {
	return corsRequest;
}

int HttpRequest::getCORSRequestType()
{
	if(isCorsRequest())
	{
		if(methodv.length()==7 && strncasecmp("options", methodv.data(), 7)==0 && hasHeader(AccessControlRequestMethod))
		{
			//CORS Preflight request
			//std::cout << ("CORS Preflight request") << std::endl;
			return PREFLIGHT;
		}
		else
		{
			//Actual CORS request
			//std::cout << ("Actual CORS request") << std::endl;
			return CORS;
		}
	}
	return -1;
}

void HttpRequest::addHeader(const std::string& header, const std::string& value)
{
	if(headers.find(header)!=headers.end()) {
		headers[header] += "," + value;
	} else {
		headers[header] = value;
	}
}

void HttpRequest::addHeaderValue(std::string header, const std::string& value)
{
	if(header.length()>0)
	{
		if(HDRS_SW_CODES.find(header)!=HDRS_SW_CODES.end())
		{
			if(headers.find(header)!=headers.end()) {
				headers[header] += "," + value;
			} else {
				headers[header] = value;
			}
		}
		else
		{
			//std::cout << ("Non standard Header std::string " + header) << std::endl;
			if(!RegexUtil::matches(header, "^[a-zA-Z]+[-|a-zA-Z0-9]*"))
			{
				//std::cout << ("Invalid Header std::string " + header) << std::endl;
				return;
			}
			if(headers.find(header)!=headers.end()) {
				headers[header] += "," + value;
			} else {
				headers[header] = value;
			}
		}
	}
}

std::vector<std::string> HttpRequest::parseHeaderValue(std::string headerValue)
{
	RegexUtil::replace(headerValue, "\\s*,\\s*|\\s+", ",");
	return StringUtil::splitAndReturn<std::vector<std::string> >(headerValue, ",");
}

const std::string HttpRequest::VALID_METHODS = ",get,post,options,delete,head,put,trace,";
bool HttpRequest::isValidHttpMethod(const std::string& method)
{
	std::string lmeth = ","+method+",";
	return strcasecmp(method.c_str(), "get") || strcasecmp(method.c_str(), "post") || strcasecmp(method.c_str(), "options")
			|| strcasecmp(method.c_str(), "delete") || strcasecmp(method.c_str(), "head") || strcasecmp(method.c_str(), "put")
			|| strcasecmp(method.c_str(), "trace");
}

bool HttpRequest::isValidHttpMethod()
{
	std::string lmeth;
	lmeth.append(methodv);
	return strcasecmp(lmeth.c_str(), "get") || strcasecmp(lmeth.c_str(), "post") || strcasecmp(lmeth.c_str(), "options")
			|| strcasecmp(lmeth.c_str(), "delete") || strcasecmp(lmeth.c_str(), "head") || strcasecmp(lmeth.c_str(), "put")
			|| strcasecmp(lmeth.c_str(), "trace");
}

bool HttpRequest::isAgentAcceptsCE()
{
	std::string lmeth = StringUtil::toLowerCopy(getHeader(AcceptEncoding));
	return lmeth.find("gzip")!=std::string::npos || lmeth.find("deflate")!=std::string::npos;
}

bool HttpRequest::isHeaderValue(std::string header, const std::string& value, const bool& ignoreCase)
{
	if(num_headers>0) {
		for(int i=0;i<(int)num_headers;++i) {
			if(header.length()==headers_list[i].name_len && strncasecmp(headers_list[i].name, header.c_str(), headers_list[i].name_len)==0) {
				return strncasecmp(headers_list[i].value, value.c_str(), headers_list[i].value_len)==0;
			}
		}
		return false;
	}
	return headers.find(header)!=headers.end()
			&& (headers[header]==value || (ignoreCase && strcasecmp(headers[header].c_str(), value.c_str())==0));
}

bool HttpRequest::isClose() {
	if(num_headers>0) {
		for(int i=0;i<(int)num_headers;++i) {
			if(HttpRequest::Connection.length()==headers_list[i].name_len && strncasecmp(headers_list[i].name, HttpRequest::Connection.c_str(), headers_list[i].name_len)==0) {
				/*std::string_view hv{headers_list[i].value, headers_list[i].value_len};
				size_t s = hv.find_first_not_of(" \t");
				size_t e = hv.find_last_not_of(" \t");
				hv = hv.substr(s, e-s);
				if(hv.length()>=5 && (hv.at(0)=='c' || hv.at(0)=='C')
						 && (hv.at(1)=='l' || hv.at(1)=='L')
						 && (hv.at(2)=='o' || hv.at(2)=='O')
						 && (hv.at(3)=='s' || hv.at(3)=='S')
						 && (hv.at(4)=='e' || hv.at(4)=='E')) {
					return true;
				}
				break;*/
				return strncasecmp(headers_list[i].value, "close", headers_list[i].value_len)==0;
			}
		}
		return false;
	} else {
		return hasHeaderValuePart(HttpRequest::Connection, "close", true);
	}
}

bool HttpRequest::isKeepAlive() {
	if(num_headers>0) {
		for(int i=0;i<(int)num_headers;++i) {
			if(HttpRequest::Connection.length()==headers_list[i].name_len && strncasecmp(headers_list[i].name, HttpRequest::Connection.c_str(), headers_list[i].name_len)==0) {
				/*std::string_view hv{headers_list[i].value, headers_list[i].value_len};
				size_t s = hv.find_first_not_of(" \t");
				size_t e = hv.find_last_not_of(" \t");
				hv = hv.substr(s, e-s);
				if(hv.length()>=10 && (hv.at(0)=='k' || hv.at(0)=='K')
						 && (hv.at(1)=='e' || hv.at(1)=='E')
						 && (hv.at(2)=='e' || hv.at(2)=='E')
						 && (hv.at(3)=='p' || hv.at(3)=='P')
						 && hv.at(4)=='-'
						 && (hv.at(5)=='a' || hv.at(5)=='A')
						 && (hv.at(6)=='l' || hv.at(6)=='L')
						 && (hv.at(7)=='i' || hv.at(7)=='I')
						 && (hv.at(8)=='v' || hv.at(8)=='V')
						 && (hv.at(9)=='e' || hv.at(9)=='E')) {
					return true;
				}
				break;*/
				return strncasecmp(headers_list[i].value, "keep-alive", headers_list[i].value_len)==0;
			}
		}
		return false;
	} else {
		return hasHeaderValuePart(HttpRequest::Connection, "keep-alive", true);
	}
}

bool HttpRequest::isUpgrade() {
	if(num_headers>0) {
		for(int i=0;i<(int)num_headers;++i) {
			if(HttpRequest::Connection.length()==headers_list[i].name_len && strncasecmp(headers_list[i].name, HttpRequest::Connection.c_str(), headers_list[i].name_len)==0) {
				/*std::string_view hv{headers_list[i].value, headers_list[i].value_len};
				size_t s = hv.find_first_not_of(" \t");
				size_t e = hv.find_last_not_of(" \t");
				hv = hv.substr(s, e-s);
				if(hv.length()>=7 && (hv.at(0)=='u' || hv.at(0)=='U')
						 && (hv.at(1)=='p' || hv.at(1)=='P')
						 && (hv.at(2)=='g' || hv.at(2)=='G')
						 && (hv.at(3)=='r' || hv.at(3)=='R')
						 && (hv.at(4)=='a' || hv.at(4)=='A')
						 && (hv.at(5)=='d' || hv.at(5)=='D')
						 && (hv.at(6)=='e' || hv.at(6)=='E')) {
					return true;
				}
				break;*/
				return strncasecmp(headers_list[i].value, "upgrade", headers_list[i].value_len)==0;
			}
		}
		return false;
	} else {
		return hasHeaderValuePart(HttpRequest::Connection, "upgrade", true);
	}
}

bool HttpRequest::hasHeaderValuePart(std::string header, std::string valuePart, const bool& ignoreCase)
{
	std::string hvalue;
	if(num_headers>0) {
		for(int i=0;i<(int)num_headers;++i) {
			if(header.length()==headers_list[i].name_len && strncasecmp(headers_list[i].name, header.c_str(), headers_list[i].name_len)==0) {
				hvalue = std::string(headers_list[i].value, headers_list[i].value_len);
				break;
			}
		}
	}
	if(headers.find(header)!=headers.end())
	{
		hvalue = headers[header];
	}
	if(hvalue==valuePart || (ignoreCase && StringUtil::toLowerCopy(hvalue)==StringUtil::toLowerCopy(valuePart)))
	{
		return true;
	}
	else if(ignoreCase)
	{
		StringUtil::toLower(hvalue);
		StringUtil::toLower(valuePart);
		std::vector<std::string> hvec = StringUtil::splitAndReturn<std::vector<std::string> >(hvalue, ",");
		for(int yy=0;yy<(int)hvec.size();yy++)
		{
			std::string vp = StringUtil::toLowerCopy(hvec.at(yy));
			StringUtil::trim(vp);
			if(vp==valuePart) {
				return true;
			}
		}
	}
	else
	{
		std::vector<std::string> hvec = StringUtil::splitAndReturn<std::vector<std::string> >(hvalue, ",");
		for(int yy=0;yy<(int)hvec.size();yy++)
		{
			std::string vp = hvec.at(yy);
			StringUtil::trim(vp);
			if(vp==valuePart) {
				return true;
			}
		}
	}
	return false;
}

std::vector<std::vector<int> > HttpRequest::getRanges(std::vector<std::string> &rangesVec)
{
	std::vector<std::vector<int> > rangeValuesLst;
	std::string ranges = getHeader(Range);
	if(ranges.find("bytes=")!=0)
	{
		return rangeValuesLst;
	}
	StringUtil::replaceFirst(ranges, "bytes=", BLANK);
	StringUtil::split(rangesVec, ranges, (","));
	for (int var = 0; var <(int)rangesVec.size(); ++var) {
		std::string range = rangesVec.at(var);
		StringUtil::trim(range);
		std::vector<std::string> rangeVals;
		StringUtil::split(rangeVals, range, ("-"));
		if(rangeVals.size()>2)
		{
			//error
		}
		else
		{
			if(rangeVals.at(0)!=BLANK && rangeVals.at(1)!=BLANK)
			{
				try {
					int start = CastUtil::toInt(rangeVals.at(0));
					int end = CastUtil::toInt(rangeVals.at(1));

					if(start<0 || start>end)
						throw std::runtime_error("1");
					std::vector<int> values;
					values.push_back(start);
					values.push_back(end - start);
					rangeValuesLst.push_back(values);
				} catch(const std::exception& e) {
					//error
				}
			}
			else if(rangeVals.at(0)!=BLANK)
			{
				try {
					int start = CastUtil::toInt(rangeVals.at(0));
					if(start<0)
						throw std::runtime_error("1");
					std::vector<int> values;
					values.push_back(start);
					values.push_back(-1);
					rangeValuesLst.push_back(values);
				} catch(const std::exception& e) {
					//error
				}
			}
			else if(rangeVals.at(1)!=BLANK)
			{
				try {
					int end = CastUtil::toInt(rangeVals.at(1));
					if(end<0)
						throw std::runtime_error("1");
					std::vector<int> values;
					values.push_back(-1);
					values.push_back(end);
					rangeValuesLst.push_back(values);
				} catch(const std::exception& e) {
					//error
				}
			}
		}
	}
	return rangeValuesLst;
}

void HttpRequest::setContent_tfile(const std::string& tfile)
{
	content_tfile = tfile;
}

std::string HttpRequest::getContent_tfile()
{
	return content_tfile;
}

void HttpRequest::addMultipartFormContent(const std::string& key, const MultipartContent& content)
{
	requestParamsF[key] = content;
}

void HttpRequest::addContent(const MultipartContent& content)
{
	contentList.push_back(content);
}

bool HttpRequest::isNonBinary(const std::string& mimeType)
{
	std::string contType = StringUtil::toLowerCopy(mimeType);
	return (contType.find("text")!=std::string::npos || contType.find("css")!=std::string::npos
			|| contType.find("x-javascript")!=std::string::npos || contType.find("json")!=std::string::npos
			|| contType.find("xml")!=std::string::npos || contType.find("html")!=std::string::npos);
}

std::string HttpRequest::getParamValue(const std::string& key)
{
	if(this->queryParams.find(key)!=this->queryParams.end()) {
		return this->queryParams[key];
	} else if(this->requestParams.find(key)!=this->requestParams.end())
		return this->requestParams[key];
	else if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].getContent();
	else
		return BLANK;
}

HTTPResponseStatus* HttpRequest::getRequestParseStatus() {
	return status;
}

void HttpRequest::setSessionID(const std::string& sessionID)
{
	this->sessionID = sessionID;
}

std::string HttpRequest::getSessionID() const
{
	return sessionID;
}

std::vector<MultipartContent> HttpRequest::getMultiPartFileList(const std::string& name)
{
	std::vector<MultipartContent> filevec;
	FMap::iterator it;
	if(name!=BLANK)
	{
		for (it=this->requestParamsF.begin();it!=this->requestParamsF.end();++it) {
			if(it->second.getTempFileName()!=BLANK && (it->second.getName()==name || it->second.getName().find(name+"[")==0))
			{
				filevec.push_back(it->second);
			}
		}
	}
	else
	{
		filevec.insert(filevec.end(), contentList.begin(), contentList.end());
	}
	return filevec;
}

std::string HttpRequest::getPassword() const {
	return password;
}

std::string HttpRequest::getUserName() const {
	return userName;
}

std::string HttpRequest::getAuthMethod() const {
	return authMethod;
}

void HttpRequest::setPassword(std::string v) {
	this->password = v;
}

void HttpRequest::setUserName(std::string v) {
	this->userName = v;
}

void HttpRequest::setAuthMethod(std::string v) {
	this->authMethod = v;
}

std::string HttpRequest::toPluginString() {
	std::string text;
	text.append(this->cntxt_name);
	text += "\n";
	text += (this->host + "\n");
	text += (this->url + "\n");
	text += (this->file + "\n");
	text += this->httpVersion + "\n";
	text.append(methodv);
	text += "\n";
	text += (this->getDefaultLocale() + "\n");
	text += (this->sessionID + "\n");
	text += (this->actUrl + "\n");

	text += (CastUtil::fromNumber(this->content.length()) + "\n");
	text += (this->content);

	text += (CastUtil::fromNumber(this->preamble.length()) + "\n");
	text += (this->preamble);

	text += (CastUtil::fromNumber(this->epilogue.length()) + "\n");
	text += (this->epilogue);

	text += (CastUtil::fromNumber(this->requestParams.size()) + "\n");
	RMap::iterator it;
	for(it=this->requestParams.begin();it!=this->requestParams.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::fromNumber(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::fromNumber(this->queryParams.size()) + "\n");
	for(it=this->queryParams.begin();it!=this->queryParams.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::fromNumber(it->second.length()) + "\n";
		text += it->second;
	}

	if(num_headers>0) {
		text += (CastUtil::fromNumber(this->num_headers) + "\n");
		for(int i=0;i<(int)num_headers;++i) {
			text += std::string(headers_list[i].name, headers_list[i].name_len) + "\n";
			text += CastUtil::fromNumber(headers_list[i].value_len) + "\n";
			text += std::string(headers_list[i].value, headers_list[i].value_len);
		}
	} else {
		text += (CastUtil::fromNumber(this->headers.size()) + "\n");
		for(it=this->headers.begin();it!=this->headers.end();++it)
		{
			text += it->first + "\n";
			text += CastUtil::fromNumber(it->second.length()) + "\n";
			text += it->second;
		}
	}

	text += (CastUtil::fromNumber(this->requestParamsF.size()) + "\n");
	FMap::iterator fit;
	for(fit=this->requestParamsF.begin();fit!=this->requestParamsF.end();++fit)
	{
		text += fit->second.name + "\n";
		text += fit->second.fileName + "\n";
		text += fit->second.tempFileName + "\n";
		text += (CastUtil::fromNumber(fit->second.content.length()) + "\n");
		text += (fit->second.content);
		text += (CastUtil::fromNumber(fit->second.headers.size()) + "\n");
		for(it=fit->second.headers.begin();it!=fit->second.headers.end();++it)
		{
			text += it->first + "\n";
			text += CastUtil::fromNumber(it->second.length()) + "\n";
			text += it->second;
		}
	}

	text += (CastUtil::fromNumber(this->contentList.size()) + "\n");
	for(int k=0;k<(int)this->contentList.size();k++)
	{
		text += this->contentList.at(k).name + "\n";
		text += this->contentList.at(k).fileName + "\n";
		text += this->contentList.at(k).tempFileName + "\n";
		text += (CastUtil::fromNumber(this->contentList.at(k).content.length()) + "\n");
		text += (this->contentList.at(k).content);
		text += (CastUtil::fromNumber(this->contentList.at(k).headers.size()) + "\n");
		for(it=this->contentList.at(k).headers.begin();it!=this->contentList.at(k).headers.end();++it)
		{
			text += it->first + "\n";
			text += CastUtil::fromNumber(it->second.length()) + "\n";
			text += it->second;
		}
	}

	return text;
}

void HttpRequest::setHttp2Headers(RMap headers)
{
	methodv = std::string{headers[":method"], headers[":method"].length()};
	authority = headers[":authority"];
	httpVers = 2.0;
	httpVersion = "http/2";
	scheme = headers[":scheme"];
	buildRequest("url", headers[":path"]);
	//headers.erase(":method");
	headers.erase(":path");
	headers.erase(":authority");
	headers.erase(":scheme");
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		buildRequest(it->first, it->second);
	}
}

std::string HttpRequest::getFileExtension(const std::string& file)
{
	if(file.find_last_of(".")!=std::string::npos)return file.substr(file.find_last_of("."));
	return file;
}

std::string HttpRequest::getExt() const
{
	return ext;
}
