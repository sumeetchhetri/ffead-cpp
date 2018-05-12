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
	statusCode = -1;
}

HttpResponse::~HttpResponse() {
}

std::string HttpResponse::generateResponse(const std::string& httpMethod, HttpRequest *req, const bool& appendHeaders /*= true*/)
{
	if(httpMethod=="HEAD" && appendHeaders)
	{
		return generateHeadResponse();
	}
	else if(httpMethod=="OPTIONS" && appendHeaders)
	{
		return generateOptionsResponse();
	}
	else if(httpMethod=="TRACE" && appendHeaders)
	{
		return generateTraceResponse(req);
	}
	else
	{
		if(appendHeaders)
		{
			return generateHeadResponse() + this->content;
		}
		else
		{
			return generateHeadResponse();
		}
	}
}

std::string HttpResponse::generateResponse(HttpRequest *req, const bool& appendHeaders /*= true*/)
{
	if(req->getMethod()=="OPTIONS")
	{
		return generateOptionsResponse();
	}
	else if(req->getMethod()=="TRACE")
	{
		return generateTraceResponse(req);
	}
	else
	{
		if(appendHeaders)
		{
			return generateHeadResponse() + this->content;
		}
		else
		{
			return generateHeadResponse();
		}
	}
}

std::string HttpResponse::generateResponse(const bool& appendHeaders /*= true*/)
{
	if(appendHeaders)
	{
		return generateHeadResponse() + this->content;
	}
	else
	{
		generateHeadResponse();
		return this->content;
	}
}

std::string HttpResponse::generateHeadResponse()
{
	addHeaderValue("Server", "FFEAD 2.0");
	bool isTE = isHeaderValue("Transfer-Encoding", "chunked");
	std::string resp, boundary;
	if(this->contentList.size()>0)
	{
		content = "";
		boundary = "FFEAD_SERVER_" + CastUtil::lexical_cast<std::string>(Timer::getCurrentTime());
		for (int var = 0; var < (int)contentList.size(); ++var) {
			content += "--" + boundary + "\r\n";
			std::map<std::string,std::string> headers = contentList.at(var).getHeaders();
			std::map<std::string,std::string>::iterator it;
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
		headers["Content-Type"] += "; boundary=\"" + boundary + "\"";
	}
	if(!isTE && getHeader(ContentLength)=="")
	{
		addHeaderValue(ContentLength, CastUtil::lexical_cast<std::string>((int)content.length()));
	}
	std::map<std::string,std::string>::iterator it;
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

std::string HttpResponse::generateOptionsResponse()
{
	addHeaderValue("Server", "FFEAD 2.0");
	std::string resp;
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	std::map<std::string,std::string>::iterator it;
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

std::string HttpResponse::generateTraceResponse(HttpRequest* req)
{
	addHeaderValue("Server", "FFEAD 2.0");
	std::string resp;
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	std::map<std::string,std::string>::iterator it;
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
		resp += "TRACE " + req->getActUrl() + " " + req->getHttpVersion();
		resp += "\r\n";
		std::map<std::string,std::string>::iterator it;
		for(it=headers.begin();it!=headers.end();++it)
		{
			resp += it->first + ": " + it->second + "\r\n";
		}
	}
	return resp;
}


std::string HttpResponse::getHttpVersion() const
{
	return httpVersion;
}

void HttpResponse::update(HttpRequest* req)
{
	this->httpVers = req->httpVers;
	this->httpVersion = req->getHttpVersion();
	addHeaderValue(HttpResponse::AcceptRanges, "none");
}

void HttpResponse::setHTTPResponseStatus(const HTTPResponseStatus& status)
{
	this->statusCode = CastUtil::lexical_cast<std::string>(status.getCode());
	this->statusMsg = status.getMsg();
}

std::string HttpResponse::getStatusCode() const
{
	return statusCode;
}

void HttpResponse::setStatusCode(const std::string& statusCode)
{
	this->statusCode = statusCode;
}

std::string HttpResponse::getStatusMsg() const
{
	return statusMsg;
}

void HttpResponse::setStatusMsg(const std::string& statusMsg)
{
	this->statusMsg = statusMsg;
}

std::string HttpResponse::getContent() const
{
	return content;
}

void HttpResponse::setContent(const std::string& content)
{
	this->content = content;
	if(content!="") {
		hasContent = true;
	}
}

void HttpResponse::addCookie(const std::string& cookie)
{
	this->cookies.push_back(cookie);
}

void HttpResponse::addContent(const MultipartContent& content)
{
	contentList.push_back(content);
}

void HttpResponse::addHeaderValue(std::string header, const std::string& value)
{
	StringUtil::toLower(header);
	if(header!="")
	{
		if(VALID_RESPONSE_HEADERS.find(","+header+",")!=std::string::npos)
		{
			if(headers.find(header)!=headers.end()) {
				headers[header] += "," + value;
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
				headers[header] += "," + value;
			} else {
				headers[header] = value;
			}
		}
	}
}

bool HttpResponse::isHeaderValue(std::string header, const std::string& value, const bool& ignoreCase)
{
	StringUtil::toLower(header);
	return header!="" && headers.find(header)!=headers.end()
			&& (headers[header]==value ||
					(ignoreCase && StringUtil::toLowerCopy(headers[header])==StringUtil::toLowerCopy(value)));
}

std::string HttpResponse::getHeader(std::string header)
{
	StringUtil::toLower(header);
	if(header!="" && headers.find(header)!=headers.end())
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
	return (httpVersion + " " + statusCode + " " + statusMsg);
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

	std::string url = req->getUrl();
	std::string locale = CommonUtils::getLocale(StringUtil::toLowerCopy(req->getDefaultLocale()));
	std::string type = CommonUtils::getMimeType(ext);

	std::string all;
    std::string fname = url;
	if (url=="/")
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

	if(req->getMethod()=="HEAD")
	{
		res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<std::string>(getContentSize(fname.c_str())));
		res->addHeaderValue(HttpResponse::AcceptRanges, "bytes");
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
	}
	else if(req->getMethod()=="OPTIONS" || req->getMethod()=="TRACE")
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
			DateFormat df("ddd, dd mmm yyyy hh:mi:ss GMT");
			std::string lastmodDate = df.format(filemodifieddate);

			bool isifmodsincvalid = false;

			std::string ifmodsincehdr = req->getHeader(HttpRequest::IfModifiedSince);

			bool forceLoadFile = false;
			if(ifmodsincehdr!="")
			{
				Date* ifmodsince = NULL;
				try {
					ifmodsince = df.parse(ifmodsincehdr);
					isifmodsincvalid = true;
					//std::cout << "Parsed date success" << std::endl;
				} catch(...) {
					isifmodsincvalid = false;
				}

				if(ifmodsince!=NULL)
				{
					//std::cout << "IfModifiedSince header = " + ifmodsincehdr + ", date = " + ifmodsince->toString() << std::endl;
					//std::cout << "Lastmodifieddate value = " + lastmodDate + ", date = " + filemodifieddate.toString() << std::endl;
					//std::cout << "Date Comparisons = " +CastUtil::lexical_cast<std::string>(*ifmodsince>=filemodifieddate)  << std::endl;

					if(isifmodsincvalid && *ifmodsince>=filemodifieddate)
					{
						res->addHeaderValue(HttpResponse::LastModified, ifmodsincehdr);
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

			res->addHeaderValue(HttpResponse::LastModified, lastmodDate);

			if(isCEGzip)
			{
				bool gengzipfile = true;
				std::string ofname = req->getContextHome() + "/temp/" + req->getFile() + ".gz";
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
				std::string ofname = req->getContextHome() + "/temp/" + req->getFile() + ".z";
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
				res->addHeaderValue(HttpResponse::ContentType, "multipart/byteranges");
				unsigned int totlen = getContentSize(fname.c_str());
				res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<std::string>(totlen));
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
						conte.addHeaderValue(MultipartContent::ContentType, type);
						conte.addHeaderValue(HttpResponse::ContentRange, "bytes "+rangesVec.at(var)+"/"+CastUtil::lexical_cast<std::string>(totlen));
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
				res->addHeaderValue(HttpResponse::TransferEncoding, "chunked");
			} else {
				res->addHeaderValue(ContentLength, CastUtil::lexical_cast<std::string>(totlen));
			}
		}
		else
		{
			res->content = getContent(fname.c_str());
			res->addHeaderValue(ContentLength, CastUtil::lexical_cast<std::string>((int)res->content.length()));
		}
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
		hasContent = true;
	}
	return hasContent;
}

std::string HttpResponse::getRemainingContent(const std::string& fname, const bool& isFirst) {
	std::string rem;
	if(isContentRemains() && httpVers>=1.1) {
		unsigned int totlen = getContentSize(fname.c_str());
		unsigned int len = totlen - techunkSiz*tecurrpart;
		if((int)len>techunkSiz)
		{
			len = techunkSiz;
		}
		if(httpVers<2.0)
		{
			rem = StringUtil::toHEX(len) + "\r\n";
			rem += getContent(fname.c_str(), techunkSiz*tecurrpart, len);
			rem += "\r\n";
			if(tecurrpart+1==teparts) {
				rem += "0\r\n\r\n";
			}
		}
		else
		{
			rem = getContent(fname.c_str(), techunkSiz*tecurrpart, len);
		}
		tecurrpart++;
	} else if(isFirst || (httpVers>=1.0 && httpVers<1.1)) {
		rem = content;
	}
	return rem;
}

bool HttpResponse::isContentRemains() {
	return teparts>0 && tecurrpart<teparts;
}

std::string HttpResponse::toPluginString() {
	std::string text = (this->statusCode + "\n");
	text += (this->statusMsg + "\n");
	text += (CastUtil::lexical_cast<std::string>(this->httpVersion) + "\n");
	text += (this->outFileName + "\n");

	text += (CastUtil::lexical_cast<std::string>(this->content.length()) + "\n");
	text += (this->content);

	text += (CastUtil::lexical_cast<std::string>(this->preamble.length()) + "\n");
	text += (this->preamble);

	text += (CastUtil::lexical_cast<std::string>(this->epilogue.length()) + "\n");
	text += (this->epilogue);

	std::map<std::string,std::string>::iterator it;
	text += (CastUtil::lexical_cast<std::string>(this->headers.size()) + "\n");
	for(it=this->headers.begin();it!=this->headers.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::lexical_cast<std::string>(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::lexical_cast<std::string>(this->multipartFormData.size()) + "\n");
	FMap::iterator fit;
	for(fit=this->multipartFormData.begin();fit!=this->multipartFormData.end();++fit)
	{
		text += fit->second.name + "\n";
		text += fit->second.fileName + "\n";
		text += fit->second.tempFileName + "\n";
		text += (CastUtil::lexical_cast<std::string>(fit->second.content.length()) + "\n");
		text += (fit->second.content);
		text += (CastUtil::lexical_cast<std::string>(fit->second.headers.size()) + "\n");
		for(it=fit->second.headers.begin();it!=fit->second.headers.end();++it)
		{
			text += it->first + "\n";
			text += CastUtil::lexical_cast<std::string>(it->second.length()) + "\n";
			text += it->second;
		}
	}

	text += (CastUtil::lexical_cast<std::string>(this->contentList.size()) + "\n");
	for(int k=0;k<(int)this->contentList.size();k++)
	{
		text += this->contentList.at(k).name + "\n";
		text += this->contentList.at(k).fileName + "\n";
		text += this->contentList.at(k).tempFileName + "\n";
		text += (CastUtil::lexical_cast<std::string>(this->contentList.at(k).content.length()) + "\n");
		text += (this->contentList.at(k).content);
		text += (CastUtil::lexical_cast<std::string>(this->contentList.at(k).headers.size()) + "\n");
		for(it=this->contentList.at(k).headers.begin();it!=this->contentList.at(k).headers.end();++it)
		{
			text += it->first + "\n";
			text += CastUtil::lexical_cast<std::string>(it->second.length()) + "\n";
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

const std::map<std::string,std::string>& HttpResponse::getCHeaders() const {
	return headers;
}
std::map<std::string,std::string> HttpResponse::getHeaders() const {
	return headers;
}
