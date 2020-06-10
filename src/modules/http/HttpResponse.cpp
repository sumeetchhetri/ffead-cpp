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
 * HttpResponse.cpp
 *
 *  Created on: Aug 19, 2009
 *      Author: sumeet
 */

#include "HttpResponse.h"

std::string HttpResponse::VALID_RESPONSE_HEADERS = ",access-control-allow-origin,access-control-allow-headers,access-control-allow-credentials,access-control-allow-methods,access-control-max-age,accept-ranges,age,allow,cache-control,connection,content-encoding,content-language,content-length,content-location,content-md5,content-disposition,content-range,content-type,date,etag,expires,last-modified,link,location,p3p,pragma,proxy-authenticate,refresh,retry-after,server,set-cookie,status,strict-transport-security,trailer,transfer-encoding,vary,via,warning,www-authenticate,";
std::string HttpResponse::AccessControlAllowOrigin		 = "Access-Control-Allow-Origin";
std::string HttpResponse::AccessControlAllowHeaders		 = "Access-Control-Allow-Headers";
std::string HttpResponse::AccessControlAllowCredentials	 = "Access-Control-Allow-Credentials";
std::string HttpResponse::AccessControlAllowMethods		 = "Access-Control-Allow-Methods";
std::string HttpResponse::AccessControlMaxAge		 = "Access-Control-Max-Age";
std::string HttpResponse::AcceptRanges			 = "Accept-Ranges";
std::string HttpResponse::Age				 = "Age";
std::string HttpResponse::Allow				 = "Allow";
std::string HttpResponse::CacheControl			 = "Cache-Control";
std::string HttpResponse::Connection				 = "Connection";
std::string HttpResponse::ContentEncoding			 = "Content-Encoding";
std::string HttpResponse::ContentLanguage			 = "Content-Language";
std::string HttpResponse::ContentLength			 = "Content-Length";
std::string HttpResponse::ContentLocation			 = "Content-Location";
std::string HttpResponse::ContentMD5				 = "Content-MD5";
std::string HttpResponse::ContentDisposition			 = "Content-Disposition";
std::string HttpResponse::ContentRange			 = "Content-Range";
std::string HttpResponse::ContentType			 = "Content-Type";
std::string HttpResponse::DateHeader				 = "Date";
std::string HttpResponse::ETag				 = "ETag";
std::string HttpResponse::Expires				 = "Expires";
std::string HttpResponse::LastModified			 = "Last-Modified";
std::string HttpResponse::Link				 = "Link";
std::string HttpResponse::Location				 = "Location";
std::string HttpResponse::P3P				 = "P3P";
std::string HttpResponse::Pragma				 = "Pragma";
std::string HttpResponse::ProxyAuthenticate			 = "Proxy-Authenticate";
std::string HttpResponse::Refresh				 = "Refresh";
std::string HttpResponse::RetryAfter				 = "Retry-After";
std::string HttpResponse::Server				 = "Server";
std::string HttpResponse::SetCookie				 = "Set-Cookie";
std::string HttpResponse::Status				 = "Status";
std::string HttpResponse::StrictTransportSecurity		 = "Strict-Transport-Security";
std::string HttpResponse::Trailer				 = "Trailer";
std::string HttpResponse::TransferEncoding			 = "Transfer-Encoding";
std::string HttpResponse::Vary				 = "Vary";
std::string HttpResponse::Via				 = "Via";
std::string HttpResponse::Warning				 = "Warning";
std::string HttpResponse::WWWAuthenticate			 = "WWW-Authenticate";
std::string HttpResponse::Upgrade = 			 "Upgrade";
std::string HttpResponse::SecWebSocketAccept = "Sec-WebSocket-Accept";
std::string HttpResponse::SecWebSocketVersion = "Sec-WebSocket-Version";
std::string HttpResponse::AltSvc = "Alt-Svc";

RiMap HttpResponse::HDRS_SW_CODES;

void HttpResponse::init() {
	std::string t = VALID_RESPONSE_HEADERS.substr(1, VALID_RESPONSE_HEADERS.length()-1);
	std::vector<std::string> vt;
	StringUtil::split(vt, t, ",");
	for(int i=0;i<(int)vt.size();i++) {
		HDRS_SW_CODES[vt.at(i)] = i;
	}
}

void HttpResponse::reset() {
	content.clear();
	preamble.clear();
	epilogue.clear();
	multipartFormData.clear();
	contentList.clear();
	cookies.clear();
	outFileName.clear();
	headers.clear();
	hasContent = false;
	tecurrpart = 0;
	teparts = 0;
	techunkSiz = 0;
	intCntLen = -1;
	httpVers = 0;
	done = false;
	status = &HTTPResponseStatus::NotFound;
	httpVersion = "HTTP/1.1";
}

HttpResponse::HttpResponse() {
	httpVersion = "HTTP/1.1";
	compressed = false;
	tecurrpart = 0;
	teparts = 0;
	techunkSiz = 0;
	hasContent = false;
	intCntLen = -1;
	httpVers = 0;
	done = false;
	status = &HTTPResponseStatus::NotFound;
}

HttpResponse::~HttpResponse() {
}

void HttpResponse::generateResponse(const std::string& httpMethod, HttpRequest *req, std::string& data, const bool& appendHeaders /*= true*/)
{
	if(httpMethod=="HEAD" && appendHeaders)
	{
		return generateHeadResponse(data);
	}
	else if(httpMethod=="OPTIONS" && appendHeaders)
	{
		return generateOptionsResponse(data);
	}
	else if(httpMethod=="TRACE" && appendHeaders)
	{
		return generateTraceResponse(req, data);
	}
	else
	{
		if(appendHeaders)
		{
			generateHeadResponse(data);
			data += this->content;
		}
		else
		{
			generateHeadResponse(data);
		}
	}
}

void HttpResponse::generateResponse(HttpRequest *req, std::string& data, const bool& appendHeaders /*= true*/)
{
	if(req->getMethod().at(0)=='o' || req->getMethod().at(0)=='O')
	{
		generateOptionsResponse(data);
	}
	else if(req->getMethod().at(0)=='t' || req->getMethod().at(0)=='T')
	{
		generateTraceResponse(req, data);
	}
	else
	{
		if(appendHeaders)
		{
			generateHeadResponse(data);
			data.append(this->content);
		}
		else
		{
			generateHeadResponse(data);
		}
	}
}

std::string HttpResponse::generateResponse(const bool& appendHeaders /*= true*/)
{
	if(appendHeaders)
	{
		std::string data;
		generateHeadResponse(data);
		return data + this->content;
	}
	else
	{
		std::string data;
		generateHeadResponse(data);
		return this->content;
	}
}

const std::string HttpResponse::HDR_SRV = "Server: FFEAD 2.0\r\n";
const std::string HttpResponse::HDR_SEP = ": ";
const std::string HttpResponse::HDR_SEPT = ":";
const std::string HttpResponse::HDR_END = "\r\n";
const std::string HttpResponse::HDR_FIN = "\r\n\r\n";

std::string& HttpResponse::generateNginxApacheResponse() {
	std::string boundary;
	if(this->contentList.size()>0)
	{
		content.clear();
		boundary = "FFEAD_SERVER_";
		boundary.append(CastUtil::fromNumber(Timer::getCurrentTime()));
		for (int var = 0; var < (int)contentList.size(); ++var) {
			content.append("--");
			content.append(boundary);
			content.append(HDR_END);
			RMap headers = contentList.at(var).getHeaders();
			RMap::iterator it;
			for(it=headers.begin();it!=headers.end();++it)
			{
				content.append(it->first);
				content.append(HDR_SEP);
				content.append(it->second);
				content.append(HDR_END);
			}
			content.append(HDR_END);
			content.append(contentList.at(var).getContent());
			content.append(HDR_END);
		}
		content.append("--");
		content.append(boundary);
		content.append("--");
		content.append(HDR_END);
		if(!hasHeader(ContentType) && this->contentList.size()>0)
		{
			this->addHeader(ContentType, "multipart/mixed");
		}
		if(hasHeader(ContentType) && boundary!="")
		{
			headers[ContentType].append("; boundary=\"");
			headers[ContentType].append(boundary);
			headers[ContentType].append("\"");
		}
	}
	return content;
}

const std::string& HttpResponse::getHeadersStr(const std::string& server, bool status_line, bool with_content, bool with_serverline)
{
	if(_headers_str.length()==0) {
		bool isTE = isHeaderValue(TransferEncoding, "chunked");
		std::string boundary;
		if(this->contentList.size()>0)
		{
			content.clear();
			boundary = "FFEAD_SERVER_";
			boundary.append(CastUtil::fromNumber(Timer::getCurrentTime()));
			for (int var = 0; var < (int)contentList.size(); ++var) {
				content.append("--");
				content.append(boundary);
				content.append(HDR_END);
				RMap headers = contentList.at(var).getHeaders();
				RMap::iterator it;
				for(it=headers.begin();it!=headers.end();++it)
				{
					content.append(it->first);
					content.append(HDR_SEP);
					content.append(it->second);
					content.append(HDR_END);
				}
				content.append(HDR_END);
				content.append(contentList.at(var).getContent());
				content.append(HDR_END);
			}
			content.append("--");
			content.append(boundary);
			content.append("--");
			content.append(HDR_END);
		}
		if(status_line) {
			_headers_str.append(httpVersion);
			_headers_str.append(" ");
			_headers_str.append(status->getSCode());
			_headers_str.append(" ");
			_headers_str.append(status->getMsg());
			_headers_str.append(HDR_END);
		}
		if(server.length()>0) {
			_headers_str.append("Server: ");
			_headers_str.append(server);
			_headers_str.append(HDR_END);
		} else if(with_serverline) {
			_headers_str.append(HDR_SRV);
		}
		if(!hasHeader(ContentType) && this->contentList.size()>0)
		{
			this->addHeader(ContentType, "multipart/mixed");
		}
		if(hasHeader(ContentType) && boundary!="")
		{
			headers[ContentType].append("; boundary=\"");
			headers[ContentType].append(boundary);
			headers[ContentType].append("\"");
		}
		if(!isTE && !hasHeader(ContentLength))
		{
			addHeader(ContentLength, CastUtil::fromNumber((int)content.length()));
		}
		RMap::iterator it;
		for(it=headers.begin();it!=headers.end();++it)
		{
			_headers_str.append(it->first);
			_headers_str.append(HDR_SEP);
			_headers_str.append(it->second);
			_headers_str.append(HDR_END);
		}
		for (int var = 0; var < (int)this->cookies.size(); var++)
		{
			_headers_str.append(SetCookie);
			_headers_str.append(HDR_SEP);
			_headers_str.append(this->cookies.at(var));
			_headers_str.append(HDR_END);
		}
		_headers_str.append(HDR_END);
		if(with_content) {
			_headers_str.append(content);
		}
	}
	return _headers_str;
}

void HttpResponse::generateHeadResponse(std::string& resp)
{
	bool isTE = isHeaderValue(TransferEncoding, "chunked");
	if(this->contentList.size()>0)
	{
		std::string boundary;
		content.clear();
		boundary = "FFEAD_SERVER_";
		boundary.append(CastUtil::fromNumber(Timer::getCurrentTime()));
		for (int var = 0; var < (int)contentList.size(); ++var) {
			content.append("--");
			content.append(boundary);
			content.append(HDR_END);
			RMap headers = contentList.at(var).getHeaders();
			RMap::iterator it;
			for(it=headers.begin();it!=headers.end();++it)
			{
				content.append(it->first);
				content.append(HDR_SEP);
				content.append(it->second);
				content.append(HDR_END);
			}
			content.append(HDR_END);
			content.append(contentList.at(var).getContent());
			content.append(HDR_END);
		}
		content.append("--");
		content.append(boundary);
		content.append("--");
		content.append(HDR_END);

		if(!hasHeader(ContentType) && this->contentList.size()>0)
		{
			this->addHeader(ContentType, "multipart/mixed");
		}
		if(hasHeader(ContentType) && boundary!="")
		{
			headers[ContentType].append("; boundary=\"");
			headers[ContentType].append(boundary);
			headers[ContentType].append("\"");
		}
	}
	resp.append(httpVersion);
	resp.append(" ");
	resp.append(status->getSCode());
	resp.append(" ");
	resp.append(status->getMsg());
	resp.append(HDR_END);
	resp.append(HDR_SRV);
	if(!isTE && !hasHeader(ContentLength))
	{
		resp.append(ContentLength);
		resp.append(HDR_SEP);
		resp.append(CastUtil::fromNumber((int)content.length()));
		resp.append(HDR_END);
	}
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		resp.append(it->first);
		resp.append(HDR_SEP);
		resp.append(it->second);
		resp.append(HDR_END);
	}
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp.append(SetCookie);
		resp.append(HDR_SEP);
		resp.append(this->cookies.at(var));
		resp.append(HDR_END);
	}
	resp.append(HDR_END);
}

const std::string HttpResponse::HDR_CORS_ALW = "Allow: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE\r\n";

void HttpResponse::generateOptionsResponse(std::string& resp)
{
	resp.append(httpVersion);
	resp.append(" ");
	resp.append(status->getSCode());
	resp.append(" ");
	resp.append(status->getMsg());
	resp.append(HDR_END);
	resp.append(HDR_SRV);
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		resp.append(it->first);
		resp.append(HDR_SEP);
		resp.append(it->second);
		resp.append(HDR_END);
	}
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp.append(SetCookie);
		resp.append(HDR_SEP);
		resp.append(this->cookies.at(var));
		resp.append(HDR_END);
	}
	resp.append(HDR_CORS_ALW);
	resp.append(HDR_END);
}

void HttpResponse::generateTraceResponse(HttpRequest* req, std::string& resp)
{
	resp.append(httpVersion);
	resp.append(" ");
	resp.append(status->getSCode());
	resp.append(" ");
	resp.append(status->getMsg());
	resp.append(HDR_END);
	resp.append(HDR_SRV);
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		resp.append(it->first);
		resp.append(HDR_SEP);
		resp.append(it->second);
		resp.append(HDR_END);
	}
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp.append(SetCookie);
		resp.append(HDR_SEP);
		resp.append(this->cookies.at(var));
		resp.append(HDR_END);
	}
	resp.append(HDR_END);
	if(req!=NULL)
	{
		resp.append("TRACE ");
		resp.append(req->getActUrl());
		resp.append(" ");
		resp.append(req->getHttpVersion());
		resp.append(HDR_END);
		RMap::iterator it;
		for(it=headers.begin();it!=headers.end();++it)
		{
			resp.append(it->first);
			resp.append(HDR_SEP);
			resp.append(it->second);
			resp.append(HDR_END);
		}
	}
}


std::string HttpResponse::getHttpVersion() const
{
	return httpVersion;
}

void HttpResponse::update(HttpRequest* req)
{
	this->httpVers = req->httpVers;
	this->httpVersion = req->httpVersion;
	//addHeader(HttpResponse::AcceptRanges, "none");
}

void HttpResponse::setHTTPResponseStatus(HTTPResponseStatus& status)
{
	this->status = &status;
}

std::string HttpResponse::getStatusCode() const
{
	return status->getSCode();
}

int HttpResponse::getCode() const
{
	return status->getCode();
}

const std::string& HttpResponse::getStatusMsg()
{
	return status->getMsg();
}

const std::string& HttpResponse::getContent()
{
	return content;
}

void HttpResponse::setContent(const std::string& content)
{
	this->content = content;
	if(content.length()>0) {
		hasContent = true;
	}
}

void HttpResponse::setUrl(const std::string& url) {
	this->_url_str = url;
}

const std::string& HttpResponse::getUrl() {
	return _url_str;
}

void HttpResponse::addCookie(const std::string& cookie)
{
	this->cookies.push_back(cookie);
}

void HttpResponse::addContent(const MultipartContent& content)
{
	contentList.push_back(content);
}

void HttpResponse::addHeader(std::string header, const std::string& value)
{
	if(headers.find(header)!=headers.end()) {
		headers[header].append(",");
		headers[header].append(value);
	} else {
		headers[header] = value;
	}
}

void HttpResponse::setContentType(const std::string& value) {
	headers[ContentType] = value;
}

void HttpResponse::addHeaderValue(std::string header, const std::string& value)
{
	if(header.length()>0)
	{
		if(HDRS_SW_CODES.find(header)!=HDRS_SW_CODES.end())
		{
			if(headers.find(header)!=headers.end()) {
				headers[header].append(",");
				headers[header].append(value);
			} else {
				headers[header] = value;
			}
		}
		else
		{
			//std::cout << ("Non standard Header string " + header) << std::endl;
			std::vector<std::string> matres = RegexUtil::search(header, "^[a-zA-Z]+[-|a-zA-Z]+[a-zA-Z]*[a-zA-Z]$");
			if(matres.size()==0)
			{
				//std::cout << ("Invalid Header string " + header) << std::endl;
				return;
			}
			if(headers.find(header)!=headers.end()) {
				headers[header].append(",");
				headers[header].append(value);
			} else {
				headers[header] = value;
			}
		}
	}
}

bool HttpResponse::hasHeader(const std::string& header)
{
	return headers.find(header)!=headers.end() && headers[header].length()>0;
}

bool HttpResponse::isHeaderValue(std::string header, const std::string& value, const bool& ignoreCase)
{
	return headers.find(header)!=headers.end()
			&& (strcmp(headers[header].c_str(), value.c_str())==0 || (ignoreCase && strcasecmp(headers[header].c_str(), value.c_str())==0));
}

std::string HttpResponse::getHeader(std::string header)
{
	if(header.length()>0 && headers.find(header)!=headers.end())
		return headers[header];
	return "";
}

bool HttpResponse::isNonBinary()
{
	std::string contType = StringUtil::toLowerCopy(getHeader(ContentType));
	return (contType.find("text")!=std::string::npos || contType.find("css")!=std::string::npos
			|| contType.find("x-javascript")!=std::string::npos || contType.find("json")!=std::string::npos
			|| contType.find("xml")!=std::string::npos || contType.find("html")!=std::string::npos);
}

void HttpResponse::setCompressed(const bool& compressed)
{
	this->compressed = compressed;
}

bool HttpResponse::getCompressed()
{
	return this->compressed;
}

const std::vector<std::string> HttpResponse::getCookies() const {
	return cookies;
}

std::string HttpResponse::getStatusLine() const {
	std::string r;
	r.append(httpVersion);
	r.append(" ");
	r.append(status->getSCode());
	r.append(" ");
	r.append(status->getMsg());
	return r;
}

unsigned int HttpResponse::getContentSize(const char *fileName)
{
	if((int)intCntLen!=-1) {
		return intCntLen;
	}
	unsigned int siz = 0;
	if(fileName!=NULL)
	{
		std::ifstream myfile;
		myfile.open(fileName, std::ios::binary | std::ios::ate);
		if (myfile.is_open())
		{
			myfile.seekg(0, std::ios::end);
			siz = myfile.tellg();
			myfile.close();
		}
	}
	else
	{
		siz = content.length();
	}
	intCntLen = siz;
	return siz;
}

std::string HttpResponse::getContent(const char *fileName, const int& start, const int& end)
{
	std::string all;
	if(fileName!=NULL)
	{
		std::ifstream myfile;
		myfile.open(fileName, std::ios::in | std::ios::binary);
		if (myfile.is_open())
		{
			if(start==-1 && end==-1)
			{
				std::string content((std::istreambuf_iterator<char>(myfile)), (std::istreambuf_iterator<char>()));
				all = content;
			}
			else
			{
				myfile.seekg(start);
				std::string s(end, '\0');
				myfile.read(&s[0], end);
				all = s;
			}
			myfile.close();
		}
	}
	else
	{
		if(start==-1 && end==-1)
		{
			all = content;
		}
		else
		{
			all = content.substr(start, end-start);
		}
	}
	return all;
}

bool HttpResponse::updateContent(HttpRequest* req, const uint32_t& techunkSiz)
{
	hasContent = false;
	this->httpVersion = req->getHttpVersion();
	this->httpVers = req->httpVers;

	std::string ext = req->getExt();

	HttpResponse *res = this;
	std::vector<std::string> rangesVec;
	std::vector<std::vector<int> > rangeValuesLst = req->getRanges(rangesVec);

	std::string fname = req->getUrl();
	std::string locale = CommonUtils::getLocale(StringUtil::toLowerCopy(req->getDefaultLocale()));
	std::string type = CommonUtils::getMimeType(ext);

	std::string all;
	if (fname=="/")
    {
		res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
		return false;
    }

	/*ifstream myfile;
    if(locale.find("english")==std::string::npos && (ext==".html" || ext==".htm"))
    {
    	std::string tfname = fname;
    	StringUtil::replaceFirst(tfname, "." , ("_" + locale+"."));
    	std::ifstream gzipdfile(tfname.c_str(), std::ios::binary);
		if(gzipdfile.good())
		{
			fname = tfname;
			gzipdfile.close();
		}
    }*/

	if(req->getMethod()[0]=='H')
	{
		res->addHeader(HttpResponse::ContentLength, CastUtil::fromNumber(getContentSize(fname.c_str())));
		res->addHeader(HttpResponse::AcceptRanges, "bytes");
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeader(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
	}
	else if(req->getMethod()[0]=='O' || req->getMethod()[0]=='T')
	{
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	}
	else
	{
		bool isCEGzip = isHeaderValue("Content-Encoding", "gzip");
		bool isCEDef = isHeaderValue("Content-Encoding", "deflate");
		if(this->contentList.size()==0 && res->content=="")
		{
			struct tm tim;
			struct stat attrib;
			if(stat(fname.c_str(), &attrib)!=0)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
				return false;
			}
			//as per suggestion at http://stackoverflow.com/questions/10446526/get-last-modified-time-of-file-in-linux
			gmtime_r(&(attrib.st_mtime), &tim);


			Date filemodifieddate(&tim);

			bool isifmodsincvalid = false;

			std::string ifmodsincehdr = req->getHeader(HttpRequest::IfModifiedSince);

			bool forceLoadFile = false;
			if(ifmodsincehdr!="")
			{
				Date* ifmodsince = NULL;
				try {
					DateFormat df("%a, %d %b %Y %H:%M:%S GMT");
					ifmodsince = df.parse(ifmodsincehdr);
					isifmodsincvalid = true;
					//std::cout << "Parsed date success" << std::endl;
				} catch(const std::exception& e) {
					isifmodsincvalid = false;
				}

				if(ifmodsince!=NULL)
				{
					//std::cout << "IfModifiedSince header = " + ifmodsincehdr + ", date = " + ifmodsince->toString() << std::endl;
					//std::cout << "Lastmodifieddate value = " + lastmodDate + ", date = " + filemodifieddate.toString() << std::endl;
					//std::cout << "Date Comparisons = " +CastUtil::fromBool(*ifmodsince>=filemodifieddate)  << std::endl;

					if(isifmodsincvalid && *ifmodsince>=filemodifieddate)
					{
						res->addHeader(HttpResponse::LastModified, ifmodsincehdr);
						//std::cout << ("File not modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << std::endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::NotModified);
						return false;
					}
					else if(isifmodsincvalid && *ifmodsince<filemodifieddate)
					{
						//std::cout << ("File modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << std::endl;
						forceLoadFile = true;
					}
					delete ifmodsince;
				}
			}

			char buffer[31];
			strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &tim);
			res->addHeader(HttpResponse::LastModified, std::string(buffer));

			if(isCEGzip)
			{
				bool gengzipfile = true;
				std::string ofname = req->getCntxt_root() + "/temp/" + req->getFile() + ".gz";
				if(!forceLoadFile)
				{
					std::ifstream gzipdfile(ofname.c_str(), std::ios::binary);
					if(gzipdfile.good())
					{
						gzipdfile.close();
						gengzipfile = false;
					}
				}
				if(gengzipfile)
				{
					CompressionUtil::gzipCompressFile((char*)fname.c_str(), false, (char*)ofname.c_str());
				}
				fname = ofname;
				res->setCompressed(true);
				req->setUrl(fname);
			}
			else if(isCEDef)
			{
				bool genzlibfile = true;
				std::string ofname = req->getCntxt_root() + "/temp/" + req->getFile() + ".z";
				if(!forceLoadFile)
				{
					std::ifstream gzipdfile(ofname.c_str(), std::ios::binary);
					if(gzipdfile.good())
					{
						gzipdfile.close();
						genzlibfile = false;
					}
				}
				if(genzlibfile)
				{
					CompressionUtil::zlibCompressFile((char*)fname.c_str(), false, (char*)ofname.c_str());
				}
				fname = ofname;
				res->setCompressed(true);
				req->setUrl(fname);
			}

			if(req->getHttpVers()<1.1 && rangeValuesLst.size()>0)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::InvalidReqRange);
				return false;
			}
			else if(rangeValuesLst.size()>0)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::PartialContent);
				res->addHeader(HttpResponse::ContentType, "multipart/byteranges");
				unsigned int totlen = getContentSize(fname.c_str());
				res->addHeader(HttpResponse::ContentLength, CastUtil::fromNumber(totlen));
				for (int var = 0; var <(int)rangeValuesLst.size(); ++var) {
					int start = rangeValuesLst.at(var).at(0);
					int end = rangeValuesLst.at(var).at(1);
					if(end!=-1 && end>=(int)totlen && start!=-1 && start<(int)totlen)
					{
						//error
					}
					else
					{
						if(start==-1)
							start = 0;
						if(end==-1)
							end = totlen - 1;
						else
							end += 1;
						std::string cont = getContent(fname.c_str(), start, end);
						MultipartContent conte(cont);
						conte.addHeader(MultipartContent::ContentType, type);
						conte.addHeader(HttpResponse::ContentRange, "bytes "+rangesVec.at(var)+"/"+CastUtil::fromNumber(totlen));
						res->addContent(conte);
					}
				}
			}
		}
		else if(this->contentList.size()==0)
		{
			if(isCEGzip)
			{
				this->content = CompressionUtil::gzipCompress(this->content, true);
			}
			if(isCEDef)
			{
				this->content = CompressionUtil::zlibCompress(this->content, true);
			}
		}

		unsigned int totlen = getContentSize(fname.c_str());
		if(techunkSiz>0 && techunkSiz<totlen)
		{
			res->techunkSiz = techunkSiz;
			float parts = techunkSiz!=0?(float)totlen/techunkSiz:0;
			parts = (floor(parts)<parts?floor(parts)+1:floor(parts));
			res->teparts = (int)parts;
			res->content = "";
			if(res->httpVers>=1.1 && res->httpVers<1.2) {
				res->addHeader(HttpResponse::TransferEncoding, "chunked");
			} else {
				res->addHeader(ContentLength, CastUtil::fromNumber(totlen));
			}
		}
		else
		{
			res->content = getContent(fname.c_str());
			res->addHeader(ContentLength, CastUtil::fromNumber((int)res->content.length()));
		}
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeader(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
		hasContent = true;
	}
	return hasContent;
}

bool HttpResponse::getRemainingContent(const std::string& fname, const bool& isFirst, std::string& rem) {
	if(isContentRemains() && httpVers>=1.1) {
		unsigned int totlen = getContentSize(fname.c_str());
		unsigned int len = totlen - techunkSiz*tecurrpart;
		if((int)len>techunkSiz)
		{
			len = techunkSiz;
		}
		if(httpVers<2.0)
		{
			rem.append(StringUtil::toHEX(len));
			rem.append("\r\n");
			rem.append(getContent(fname.c_str(), techunkSiz*tecurrpart, len));
			rem.append("\r\n");
			if(tecurrpart+1==teparts) {
				rem.append("0\r\n\r\n");
			}
		}
		else
		{
			rem = getContent(fname.c_str(), techunkSiz*tecurrpart, len);
		}
		tecurrpart++;
		return true;
	} else if(isFirst || (httpVers>=1.0 && httpVers<1.1)) {
		rem = content;
		return true;
	}
	return false;
}

bool HttpResponse::isContentRemains() {
	return teparts>0 && tecurrpart<teparts;
}

std::string HttpResponse::toPluginString() {
	std::string text = (this->status->getSCode() + "\n");
	text += (this->status->getMsg() + "\n");
	text += this->httpVersion + "\n";
	text += (this->outFileName + "\n");

	text += (CastUtil::fromNumber(this->content.length()) + "\n");
	text += (this->content);

	text += (CastUtil::fromNumber(this->preamble.length()) + "\n");
	text += (this->preamble);

	text += (CastUtil::fromNumber(this->epilogue.length()) + "\n");
	text += (this->epilogue);

	std::map<std::string,std::string,cicomp>::iterator it;
	text += (CastUtil::fromNumber(this->headers.size()) + "\n");
	for(it=this->headers.begin();it!=this->headers.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::fromNumber(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::fromNumber(this->multipartFormData.size()) + "\n");
	FMap::iterator fit;
	for(fit=this->multipartFormData.begin();fit!=this->multipartFormData.end();++fit)
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

bool HttpResponse::isDone() const {
	return done;
}

void HttpResponse::setDone(const bool& done) {
	this->done = done;
}

std::string HttpResponse::getFileExtension(const std::string& file)
{
	if(file.find_last_of(".")!=std::string::npos)return file.substr(file.find_last_of("."));
	return file;
}

const RMap& HttpResponse::getCHeaders() const {
	return headers;
}
RMap HttpResponse::getHeaders() const {
	return headers;
}
