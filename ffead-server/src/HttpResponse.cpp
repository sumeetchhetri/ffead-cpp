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
 * HttpResponse.cpp
 *
 *  Created on: Aug 19, 2009
 *      Author: sumeet
 */

#include "HttpResponse.h"

string HttpResponse::VALID_RESPONSE_HEADERS = ",access-control-allow-origin,access-control-allow-headers,access-control-allow-credentials,access-control-allow-methods,access-control-max-age,accept-ranges,age,allow,cache-control,connection,content-encoding,content-language,content-length,content-location,content-md5,content-disposition,content-range,content-type,date,etag,expires,last-modified,link,location,p3p,pragma,proxy-authenticate,refresh,retry-after,server,set-cookie,status,strict-transport-security,trailer,transfer-encoding,vary,via,warning,www-authenticate,";
string HttpResponse::AccessControlAllowOrigin		 = "Access-Control-Allow-Origin";
string HttpResponse::AccessControlAllowHeaders		 = "Access-Control-Allow-Headers";
string HttpResponse::AccessControlAllowCredentials	 = "Access-Control-Allow-Credentials";
string HttpResponse::AccessControlAllowMethods		 = "Access-Control-Allow-Methods";
string HttpResponse::AccessControlMaxAge		 = "Access-Control-Max-Age";
string HttpResponse::AcceptRanges			 = "Accept-Ranges";
string HttpResponse::Age				 = "Age";
string HttpResponse::Allow				 = "Allow";
string HttpResponse::CacheControl			 = "Cache-Control";
string HttpResponse::Connection				 = "Connection";
string HttpResponse::ContentEncoding			 = "Content-Encoding";
string HttpResponse::ContentLanguage			 = "Content-Language";
string HttpResponse::ContentLength			 = "Content-Length";
string HttpResponse::ContentLocation			 = "Content-Location";
string HttpResponse::ContentMD5				 = "Content-MD5";
string HttpResponse::ContentDisposition			 = "Content-Disposition";
string HttpResponse::ContentRange			 = "Content-Range";
string HttpResponse::ContentType			 = "Content-Type";
string HttpResponse::Date				 = "Date";
string HttpResponse::ETag				 = "ETag";
string HttpResponse::Expires				 = "Expires";
string HttpResponse::LastModified			 = "Last-Modified";
string HttpResponse::Link				 = "Link";
string HttpResponse::Location				 = "Location";
string HttpResponse::P3P				 = "P3P";
string HttpResponse::Pragma				 = "Pragma";
string HttpResponse::ProxyAuthenticate			 = "Proxy-Authenticate";
string HttpResponse::Refresh				 = "Refresh";
string HttpResponse::RetryAfter				 = "Retry-After";
string HttpResponse::Server				 = "Server";
string HttpResponse::SetCookie				 = "Set-Cookie";
string HttpResponse::Status				 = "Status";
string HttpResponse::StrictTransportSecurity		 = "Strict-Transport-Security";
string HttpResponse::Trailer				 = "Trailer";
string HttpResponse::TransferEncoding			 = "Transfer-Encoding";
string HttpResponse::Vary				 = "Vary";
string HttpResponse::Via				 = "Via";
string HttpResponse::Warning				 = "Warning";
string HttpResponse::WWWAuthenticate			 = "WWW-Authenticate";

HttpResponse::HttpResponse() {
	httpVersion = "HTTP/1.1";
	compressed = false;
	logger = LoggerFactory::getLogger("HttpResponse");
}

HttpResponse::~HttpResponse() {
}

string HttpResponse::generateResponse(string httpMethod, HttpRequest *req)
{
	if(httpMethod=="HEAD")
	{
		return generateHeadResponse();
	}
	else if(httpMethod=="OPTIONS")
	{
		return generateOptionsResponse();
	}
	else if(httpMethod=="TRACE")
	{
		return generateTraceResponse(req);
	}
	else
	{
		return generateResponse();
	}
}

string HttpResponse::generateResponse()
{
	string resp = generateHeadResponse();
	resp += this->content;
	return resp;
}

string HttpResponse::generateHeadResponse()
{
	addHeaderValue("Server", "FFEAD 1.1");
	bool isTE = isHeaderValue("Transfer-Encoding", "chunked");
	bool isCEGzip = isHeaderValue("Content-Encoding", "gzip");
	bool isCEDef = isHeaderValue("Content-Encoding", "deflate");
	string resp, boundary;
	if(this->contentList.size()>0)
	{
		content = "";
		boundary = "FFEAD_SERVER_" + CastUtil::lexical_cast<string>(Timer::getCurrentTime());
		for (int var = 0; var < (int)contentList.size(); ++var) {
			content += "--" + boundary + "\r\n";
			map<string,string> headers = contentList.at(var).getHeaders();
			map<string,string>::iterator it;
			for(it=headers.begin();it!=headers.end();++it)
			{
				content += it->first + ": " + it->second + "\r\n";
			}
			content += "\r\n";
			content += contentList.at(var).getContent();
			content += "\r\n";
		}
		content += "--" + boundary + "--\r\n";
	}
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	if(this->getHeader("Content-Type")=="" && this->contentList.size()>0)
	{
		this->addHeaderValue("Content-Type", "multipart/mixed");
	}
	if(this->getHeader("Content-Type")!="" && boundary!="")
	{
		headers["Content-Type"] += "; boundary=" + boundary;
	}
	if(!isTE && !compressed)
	{
		if(isCEGzip)
		{
			if(this->content!="")
			{
				this->content = CompressionUtil::gzipCompress(this->content, true);
			}
		}
		if(isCEDef)
		{
			if(this->content!="")
			{
				this->content = CompressionUtil::zlibCompress(this->content, true);
			}
		}
	}
	if(!isTE && this->content!="")
	{
		headers["Content-Length"] = CastUtil::lexical_cast<string>((int)content.length());
	}
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		resp += it->first + ": " + it->second + "\r\n";
	}
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp += "Set-Cookie: " + this->cookies.at(var) + "\r\n";
	}
	resp += "\r\n";
	return resp;
}

string HttpResponse::generateOptionsResponse()
{
	addHeaderValue("Server", "FFEAD 1.1");
	string resp;
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		resp += it->first + ": " + it->second + "\r\n";
	}
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp += "Set-Cookie: " + this->cookies.at(var) + "\r\n";
	}
	resp += "Allow: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE\r\n";
	resp += "\r\n";
	return resp;
}

string HttpResponse::generateTraceResponse(HttpRequest* req)
{
	addHeaderValue("Server", "FFEAD 1.1");
	string resp;
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		resp += it->first + ": " + it->second + "\r\n";
	}
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp += "Set-Cookie: " + this->cookies.at(var) + "\r\n";
	}
	resp += "\r\n";
	if(req!=NULL)
	{
		map<string,string>::iterator it;
		for(it=headers.begin();it!=req->getHeaders().end();++it)
		{
			resp += it->first + ": " + it->second + "\r\n";
		}
	}
	return resp;
}


string HttpResponse::getHttpVersion() const
{
	return httpVersion;
}

void HttpResponse::update(HttpRequest* req)
{
	this->httpVersion = req->getHttpVersion();
}

void HttpResponse::setHTTPResponseStatus(HTTPResponseStatus status)
{
	this->statusCode = CastUtil::lexical_cast<string>(status.getCode());
	this->statusMsg = status.getMsg();
}

string HttpResponse::getStatusCode() const
{
	return statusCode;
}

void HttpResponse::setStatusCode(string statusCode)
{
	this->statusCode = statusCode;
}

string HttpResponse::getStatusMsg() const
{
	return statusMsg;
}

void HttpResponse::setStatusMsg(string statusMsg)
{
	this->statusMsg = statusMsg;
}

string HttpResponse::getContent() const
{
	return content;
}

void HttpResponse::setContent(string content)
{
	this->content = content;
}

void HttpResponse::addCookie(string cookie)
{
	this->cookies.push_back(cookie);
}

void HttpResponse::addContent(MultipartContent content)
{
	contentList.push_back(content);
}

void HttpResponse::addHeaderValue(string header, string value)
{
	header = StringUtil::camelCasedCopy(header, "-");
	if(header!="")
	{
		if(VALID_RESPONSE_HEADERS.find(","+StringUtil::toLowerCopy(header)+",")!=string::npos)
		{
			headers[header] = value;
		}
		else
		{
			logger << ("Non standard Header string " + header) << endl;
			vector<string> matres = RegexUtil::search(header, "^[a-zA-Z]+[-|a-zA-Z]+[a-zA-Z]*[a-zA-Z]$");
			if(matres.size()==0)
			{
				logger << ("Invalid Header string " + header) << endl;
				return;
			}
			headers[header] = value;
		}
	}
}

bool HttpResponse::isHeaderValue(string header, string value, bool ignoreCase)
{
	header = StringUtil::camelCasedCopy(header, "-");
	return header!="" && headers.find(header)!=headers.end()
			&& (headers[header]==value ||
					(ignoreCase && StringUtil::toLowerCopy(headers[header])==StringUtil::toLowerCopy(value)));
}

string HttpResponse::getHeader(string header)
{
	header = StringUtil::camelCasedCopy(header, "-");
	if(header!="" && headers.find(header)!=headers.end())
		return headers[header];
	return "";
}

bool HttpResponse::isNonBinary()
{
	string contType = StringUtil::toLowerCopy(getHeader(ContentType));
	return (contType.find("text")!=string::npos || contType.find("css")!=string::npos
			|| contType.find("x-javascript")!=string::npos || contType.find("json")!=string::npos
			|| contType.find("xml")!=string::npos || contType.find("html")!=string::npos);
}

void HttpResponse::setCompressed(bool compressed)
{
	this->compressed = compressed;
}

bool HttpResponse::getCompressed()
{
	return this->compressed;
}
