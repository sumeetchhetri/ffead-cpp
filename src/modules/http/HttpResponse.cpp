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
string HttpResponse::DateHeader				 = "Date";
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
string HttpResponse::Upgrade = 			 "Upgrade";
string HttpResponse::SecWebSocketAccept = "Sec-WebSocket-Accept";
string HttpResponse::SecWebSocketVersion = "Sec-WebSocket-Version";
string HttpResponse::AltSvc = "Alt-Svc";

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

string HttpResponse::generateResponse(const string& httpMethod, HttpRequest *req, const bool& appendHeaders /*= true*/)
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
		return generateResponse(appendHeaders);
	}
}

string HttpResponse::generateOnlyHeaderResponse(HttpRequest *req)
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
		return generateHeadResponse();
	}
}

string HttpResponse::generateResponse(const bool& appendHeaders /*= true*/)
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

string HttpResponse::generateHeadResponse()
{
	addHeaderValue("Server", "FFEAD 2.0");
	bool isTE = isHeaderValue("Transfer-Encoding", "chunked");
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
		headers["Content-Type"] += "; boundary=\"" + boundary + "\"";
	}
	if(!isTE && getHeader(ContentLength)=="")
	{
		addHeaderValue(ContentLength, CastUtil::lexical_cast<string>((int)content.length()));
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
	addHeaderValue("Server", "FFEAD 2.0");
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
	addHeaderValue("Server", "FFEAD 2.0");
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
		resp += "TRACE " + req->getActUrl() + " " + req->getHttpVersion();
		resp += "\r\n";
		map<string,string>::iterator it;
		for(it=headers.begin();it!=headers.end();++it)
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
	this->httpVers = req->httpVers;
	this->httpVersion = req->getHttpVersion();
}

void HttpResponse::setHTTPResponseStatus(const HTTPResponseStatus& status)
{
	this->statusCode = CastUtil::lexical_cast<string>(status.getCode());
	this->statusMsg = status.getMsg();
}

string HttpResponse::getStatusCode() const
{
	return statusCode;
}

void HttpResponse::setStatusCode(const string& statusCode)
{
	this->statusCode = statusCode;
}

string HttpResponse::getStatusMsg() const
{
	return statusMsg;
}

void HttpResponse::setStatusMsg(const string& statusMsg)
{
	this->statusMsg = statusMsg;
}

string HttpResponse::getContent() const
{
	return content;
}

void HttpResponse::setContent(const string& content)
{
	this->content = content;
	if(content!="") {
		hasContent = true;
	}
}

void HttpResponse::addCookie(const string& cookie)
{
	this->cookies.push_back(cookie);
}

void HttpResponse::addContent(const MultipartContent& content)
{
	contentList.push_back(content);
}

void HttpResponse::addHeaderValue(string header, const string& value)
{
	StringUtil::toLower(header);
	if(header!="")
	{
		if(VALID_RESPONSE_HEADERS.find(","+header+",")!=string::npos)
		{
			if(headers.find(header)!=headers.end()) {
				headers[header] += "," + value;
			} else {
				headers[header] = value;
			}
		}
		else
		{
			cout << ("Non standard Header string " + header) << endl;
			vector<string> matres = RegexUtil::search(header, "^[a-zA-Z]+[-|a-zA-Z]+[a-zA-Z]*[a-zA-Z]$");
			if(matres.size()==0)
			{
				cout << ("Invalid Header string " + header) << endl;
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

bool HttpResponse::isHeaderValue(string header, const string& value, const bool& ignoreCase)
{
	StringUtil::toLower(header);
	return header!="" && headers.find(header)!=headers.end()
			&& (headers[header]==value ||
					(ignoreCase && StringUtil::toLowerCopy(headers[header])==StringUtil::toLowerCopy(value)));
}

string HttpResponse::getHeader(string header)
{
	StringUtil::toLower(header);
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

void HttpResponse::setCompressed(const bool& compressed)
{
	this->compressed = compressed;
}

bool HttpResponse::getCompressed()
{
	return this->compressed;
}

vector<string> HttpResponse::getCookies() const {
	return cookies;
}

string HttpResponse::getStatusLine() const {
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
		ifstream myfile;
		myfile.open(fileName, ios::binary | ios::ate);
		if (myfile.is_open())
		{
			myfile.seekg(0, ios::end);
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

string HttpResponse::getContent(const char *fileName, const int& start, const int& end)
{
	string all;
	if(fileName!=NULL)
	{
		ifstream myfile;
		myfile.open(fileName, ios::in | ios::binary);
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

	string ext = req->getExt();

	HttpResponse *res = this;
	vector<string> rangesVec;
	vector<vector<int> > rangeValuesLst = req->getRanges(rangesVec);

	string url = req->getUrl();
	string locale = CommonUtils::getLocale(StringUtil::toLowerCopy(req->getDefaultLocale()));
	string type = CommonUtils::getMimeType(ext);

	string all;
    string fname = url;
	if (url=="/")
    {
		res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
		return false;
    }

	/*ifstream myfile;
    if(locale.find("english")==string::npos && (ext==".html" || ext==".htm"))
    {
    	string tfname = fname;
    	StringUtil::replaceFirst(tfname, "." , ("_" + locale+"."));
    	ifstream gzipdfile(tfname.c_str(), ios::binary);
		if(gzipdfile.good())
		{
			fname = tfname;
			gzipdfile.close();
		}
    }*/

	if(req->getMethod()=="HEAD")
	{
		res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<string>(getContentSize(fname.c_str())));
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
			string lastmodDate = df.format(filemodifieddate);

			bool isifmodsincvalid = false;

			string ifmodsincehdr = req->getHeader(HttpRequest::IfModifiedSince);

			bool forceLoadFile = false;
			if(ifmodsincehdr!="")
			{
				Date* ifmodsince = NULL;
				try {
					ifmodsince = df.parse(ifmodsincehdr);
					isifmodsincvalid = true;
					cout << "Parsed date success" << endl;
				} catch(...) {
					isifmodsincvalid = false;
				}

				if(ifmodsince!=NULL)
				{
					cout << "IfModifiedSince header = " + ifmodsincehdr + ", date = " + ifmodsince->toString() << endl;
					cout << "Lastmodifieddate value = " + lastmodDate + ", date = " + filemodifieddate.toString() << endl;
					cout << "Date Comparisons = " +CastUtil::lexical_cast<string>(*ifmodsince>=filemodifieddate)  << endl;

					if(isifmodsincvalid && *ifmodsince>=filemodifieddate)
					{
						res->addHeaderValue(HttpResponse::LastModified, ifmodsincehdr);
						cout << ("File not modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::NotModified);
						return false;
					}
					else if(isifmodsincvalid && *ifmodsince<filemodifieddate)
					{
						cout << ("File modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << endl;
						forceLoadFile = true;
					}
					delete ifmodsince;
				}
			}

			res->addHeaderValue(HttpResponse::LastModified, lastmodDate);

			if(isCEGzip)
			{
				bool gengzipfile = true;
				string ofname = req->getContextHome() + "/temp/" + req->getFile() + ".gz";
				if(!forceLoadFile)
				{
					ifstream gzipdfile(ofname.c_str(), ios::binary);
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
				string ofname = req->getContextHome() + "/temp/" + req->getFile() + ".z";
				if(!forceLoadFile)
				{
					ifstream gzipdfile(ofname.c_str(), ios::binary);
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
				res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<string>(totlen));
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
						string cont = getContent(fname.c_str(), start, end);
						MultipartContent conte(cont);
						conte.addHeaderValue(MultipartContent::ContentType, type);
						conte.addHeaderValue(HttpResponse::ContentRange, "bytes "+rangesVec.at(var)+"/"+CastUtil::lexical_cast<string>(totlen));
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
				res->addHeaderValue(ContentLength, CastUtil::lexical_cast<string>(totlen));
			}
		}
		else
		{
			res->content = getContent(fname.c_str());
			res->addHeaderValue(ContentLength, CastUtil::lexical_cast<string>((int)res->content.length()));
		}
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
		hasContent = true;
	}
	return hasContent;
}

string HttpResponse::getRemainingContent(const string& fname, const bool& isFirst) {
	string rem;
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

string HttpResponse::toPluginString() {
	string text = (this->statusCode + "\n");
	text += (this->statusMsg + "\n");
	text += (CastUtil::lexical_cast<string>(this->httpVersion) + "\n");
	text += (this->outFileName + "\n");

	text += (CastUtil::lexical_cast<string>(this->content.length()) + "\n");
	text += (this->content);

	text += (CastUtil::lexical_cast<string>(this->preamble.length()) + "\n");
	text += (this->preamble);

	text += (CastUtil::lexical_cast<string>(this->epilogue.length()) + "\n");
	text += (this->epilogue);

	map<string,string>::iterator it;
	text += (CastUtil::lexical_cast<string>(this->headers.size()) + "\n");
	for(it=this->headers.begin();it!=this->headers.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::lexical_cast<string>(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::lexical_cast<string>(this->multipartFormData.size()) + "\n");
	FMap::iterator fit;
	for(fit=this->multipartFormData.begin();fit!=this->multipartFormData.end();++fit)
	{
		text += fit->second.name + "\n";
		text += fit->second.fileName + "\n";
		text += fit->second.tempFileName + "\n";
		text += (CastUtil::lexical_cast<string>(fit->second.content.length()) + "\n");
		text += (fit->second.content);
		text += (CastUtil::lexical_cast<string>(fit->second.headers.size()) + "\n");
		for(it=fit->second.headers.begin();it!=fit->second.headers.end();++it)
		{
			text += it->first + "\n";
			text += CastUtil::lexical_cast<string>(it->second.length()) + "\n";
			text += it->second;
		}
	}

	text += (CastUtil::lexical_cast<string>(this->contentList.size()) + "\n");
	for(int k=0;k<(int)this->contentList.size();k++)
	{
		text += this->contentList.at(k).name + "\n";
		text += this->contentList.at(k).fileName + "\n";
		text += this->contentList.at(k).tempFileName + "\n";
		text += (CastUtil::lexical_cast<string>(this->contentList.at(k).content.length()) + "\n");
		text += (this->contentList.at(k).content);
		text += (CastUtil::lexical_cast<string>(this->contentList.at(k).headers.size()) + "\n");
		for(it=this->contentList.at(k).headers.begin();it!=this->contentList.at(k).headers.end();++it)
		{
			text += it->first + "\n";
			text += CastUtil::lexical_cast<string>(it->second.length()) + "\n";
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

string HttpResponse::getFileExtension(const string& file)
{
	if(file.find_last_of(".")!=string::npos)return file.substr(file.find_last_of("."));
	return file;
}
