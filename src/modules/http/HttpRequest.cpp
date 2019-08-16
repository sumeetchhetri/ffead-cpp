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
 * HttpRequest.cpp
 *
 *  Created on: Aug 10, 2009
 *      Author: sumeet
 */

#include "HttpRequest.h"

std::string_view HttpRequest::VALID_REQUEST_HEADERS = ",accept,accept-charset,accept-encoding,accept-language,accept-datetime,access-control-request-headers,access-control-request-method,authorization,cache-control,connection,cookie,content-length,content-md5,content-type,date,expect,from,host,if-match,if-modified-since,if-none-match,if-range,if-unmodified-since,max-forwards,origin,pragma,proxy-authorization,range,referer,te,upgrade,user-agent,via,warning,transfer-encoding,sec-websocket-key,sec-websocket-version,sec-websocket-accept,sec-websocket-protocol,sec-websocket-extensions,alt-used,http2-settings,";

std::string_view HttpRequest::Accept =			 "Accept";
std::string_view HttpRequest::AcceptCharset = 		 "Accept-Charset";
std::string_view HttpRequest::AcceptEncoding = 		 "Accept-Encoding";
std::string_view HttpRequest::AcceptLanguage = 		 "Accept-Language";
std::string_view HttpRequest::AcceptDatetime = 		 "Accept-Datetime";
std::string_view HttpRequest::AccessControlRequestHeaders = "Access-Control-Request-Headers";
std::string_view HttpRequest::AccessControlRequestMethod =  "Access-Control-Request-Method";
std::string_view HttpRequest::Authorization = 		 "Authorization";
std::string_view HttpRequest::CacheControl = 		 "Cache-Control";
std::string_view HttpRequest::Connection = 			 "Connection";
std::string_view HttpRequest::Cookie = 			 "Cookie";
std::string_view HttpRequest::ContentLength = 		 "Content-Length";
std::string_view HttpRequest::ContentMD5 = 			 "Content-MD5";
std::string_view HttpRequest::ContentType = 			 "Content-Type";
std::string_view HttpRequest::Date = 				 "Date";
std::string_view HttpRequest::Expect = 			 "Expect";
std::string_view HttpRequest::From = 				 "From";
std::string_view HttpRequest::Host = 				 "Host";
std::string_view HttpRequest::IfMatch = 			 "If-Match";
std::string_view HttpRequest::IfModifiedSince = 		 "If-Modified-Since";
std::string_view HttpRequest::IfNoneMatch = 		 "If-None-Match";
std::string_view HttpRequest::IfRange = 			 "If-Range";
std::string_view HttpRequest::TransferEncoding = "Transfer-Encoding";
std::string_view HttpRequest::IfUnmodifiedSince = 		 "If-Unmodified-Since";
std::string_view HttpRequest::MaxForwards = 			 "Max-Forwards";
std::string_view HttpRequest::Origin = 			 "Origin";
std::string_view HttpRequest::Pragma = 			 "Pragma";
std::string_view HttpRequest::ProxyAuthorization = 		 "Proxy-Authorization";
std::string_view HttpRequest::Range = 			 "Range";
std::string_view HttpRequest::Referer = 			 "Referer";
std::string_view HttpRequest::TE = 				 "TE";
std::string_view HttpRequest::Upgrade = 			 "Upgrade";
std::string_view HttpRequest::UserAgent = 			 "User-Agent";
std::string_view HttpRequest::Via = 				 "Via";
std::string_view HttpRequest::Warning = 			 "Warning";
std::string_view HttpRequest::SecWebSocketKey = "Sec-WebSocket-Key";
std::string_view HttpRequest::SecWebSocketVersion = "Sec-WebSocket-Version";
std::string_view HttpRequest::SecWebSocketAccept = "Sec-WebSocket-Accept";
std::string_view HttpRequest::SecWebSocketProtocol = "Sec-WebSocket-Protocol";
std::string_view HttpRequest::SecWebSocketExtensions = "Sec-WebSocket-Extensions";
std::string_view HttpRequest::AltUsed = "Alt-Used";
std::string_view HttpRequest::Http2Settings = "HTTP2-Settings";

void HttpRequest::getAuthParams(std::string_view str)
{
	authMethod = (str.substr(0,str.find(" ")));
	str = str.substr(str.find(" ")+1);
	if(strcasecmp(&authMethod[0], "basic")==0)
	{
		unsigned char *input = (unsigned char *)&str[0];
		int length = str.length();
		std::string_view temp = CryptoHandler::base64decode(input,length);
		userName = (temp.substr(0,temp.find(":")));
		temp = temp.substr(temp.find(":")+1);
		password = (temp);
	}
	else
	{
		strvVec tempv;
		StringUtil::split(tempv, str, (","));
		for(unsigned int i=0;i<tempv.size();i++)
		{
			strvVec tempvv;
			StringUtil::split(tempvv, tempv.at(i), ("="));
			std::string temr = std::string(tempvv.at(1));
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

void HttpRequest::getOauthParams(std::string_view str)
{
	authinfo["Method"] = str.substr(0,str.find(" "));
	str = str.substr(str.find(" ")+1);

	std::vector<std::string> tempv;
	StringUtil::split(tempv, std::string(str), (","));
	for(unsigned int i=0;i<tempv.size();i++)
	{
		std::vector<std::string> tempvv;
		StringUtil::split(tempvv, tempv.at(i), ("="));
		StringUtil::replaceFirst(tempvv.at(0),"\r","");
		StringUtil::replaceFirst(tempvv.at(0),"\n","");
		std::string_view temr = tempvv.at(1);
		temr = temr.substr(temr.find("\"")+1);
		temr = temr.substr(0,temr.find("\""));
		authinfo[tempvv.at(0)] = temr;
		authorderinf[authorderinf.size()+1] = CryptoHandler::urlDecode(tempvv.at(0));
	}
}

HttpRequest::HttpRequest()
{
	cookie = false;
	httpVers = 0;
	corsRequest = false;
}

HttpRequest::HttpRequest(std::string_view path)
{
	this->webpath = path;
	cookie = false;
	httpVers = 0;
	corsRequest = false;
}

void HttpRequest::updateContent()
{
	corsRequest = getHeader(Origin)!="";
	if(strcasecmp(&method[0], "get")==0)return;
	if(this->content!="")
	{
		updateFromContentStr();
	}
	else if(this->content_tfile!="")
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
		strvVec params;
		std::string valu = CryptoHandler::urlDecode(this->getContent());
		StringUtil::split(params,valu , ("&"));
		std::map<std::string_view ,int> indices;
		RMap::iterator it;
		for(unsigned j=0;j<params.size();j++)
		{
			strvVec param;
			StringUtil::split(param, params.at(j), ("="));
			if(param.size()==2)
			{
				std::string attN = std::string(param.at(0));
				StringUtil::replaceFirst(attN,"\r","");
				StringUtil::replaceFirst(attN,"\t","");
				StringUtil::replaceFirst(attN," ","");
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
							  + CastUtil::lexical_cast<std::string>(indices[attN])
							  + "]"] = CryptoHandler::urlDecode(param.at(1));
				}
				else
				{
					this->setRequestParam(attN, CryptoHandler::urlDecode(param.at(1)));
				}
				reqorderinf[reqorderinf.size()+1] = attN;
			}
		}
	}
	else if(this->getContent()!="" && this->getContent_boundary()!="")
	{
		std::string_view contemp = this->getContent();

		bool bcontstarts = false, bhdrstarts = false, bcontends = false;
		std::string filen;
		std::ofstream ofile;
		std::string temp;
		std::string_view delb = this->getContent_boundary();
		std::string_view delend = this->getContent_boundary()+"--";
		std::string cont;
		std::vector<std::string_view> hdrs;
		std::map<std::string_view ,int> indices;
		//std::cout << delb << std::endl;
		//std::cout << delend << std::endl;
		while(contemp!="")
		{
			if(contemp.find("\n")!=std::string::npos)
			{
				temp = contemp.substr(0, contemp.find("\n"));
				contemp = contemp.substr(contemp.find("\n")+1);
			}
			else
			{
				temp = contemp;
				contemp = "";
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
				if(content.getName()=="")
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
						std::string s = attN.substr(0, attN.find("[")+1) + "[" + CastUtil::lexical_cast<std::string>(indices[attN]) + "]";
						addMultipartFormContent(s, content);
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
							std::string s = attN + "[" + CastUtil::lexical_cast<std::string>(indices[attN]) + "]";
							addMultipartFormContent(s, content);
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
								  + CastUtil::lexical_cast<std::string>(indices[attN])
								  + "]", content);
						/*std::cout << ("creating array from similar params" + attN+"["
										  + CastUtil::lexical_cast<std::string>(indices[attN])
										  + "]") << std::endl;*/

					}
					else
					{
						addMultipartFormContent(attN, content);
					}
					reqorderinf[reqorderinf.size()+1] = attN;
				}
				hdrs.clear();
				cont = "";
			}
			else if(temp.find(delend)==0)
			{
				bcontends = true;
			}
			else if(bhdrstarts)
			{
				if(temp=="\r" || temp=="")
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
						StringUtil::replaceAll(filen, "-", "");
						filen = this->getContextHome() + "/temp/"+ filen + CastUtil::lexical_cast<std::string>(Timer::getCurrentTime());
						ofile.open(&filen[0], std::ios::binary | std::ios::app);
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
		content = "";
	}
}

void HttpRequest::updateFromContentFile()
{
	std::ifstream infile(&this->content_tfile[0], std::ios::binary);
	if(infile.is_open())
	{
		if(this->getContent_boundary()!="")
		{
			bool bcontstarts = false, bhdrstarts = false, bcontends = false;
			std::string filen;
			std::ofstream ofile;
			std::string temp;
			std::string_view delb = this->getContent_boundary();
			std::string_view delend = this->getContent_boundary()+"--";
			std::string cont;
			std::vector<std::string_view> hdrs;
			std::map<std::string_view ,int> indices;
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
					if(content.getName()=="")
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
									  + CastUtil::lexical_cast<std::string>(indices[attN])
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
										  + CastUtil::lexical_cast<std::string>(indices[attN])
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
									  + CastUtil::lexical_cast<std::string>(indices[attN])
									  + "]", content);

						}
						else
						{
							addMultipartFormContent(attN, content);
						}
						reqorderinf[reqorderinf.size()+1] = attN;
					}
					hdrs.clear();
					cont = "";
				}
				else if(temp.find(delend)==0)
				{
					bcontends = true;
				}
				else if(bhdrstarts)
				{
					if(temp=="\r" || temp=="")
					{
						bcontstarts = true;
						bhdrstarts = false;
					}
					else
					{
						hdrs.push_back(temp);
						if(strcasestr(&temp[0], "content-disposition: ")!=NULL && strcasestr(&temp[0], "filename")!=NULL)
						{
							filen = this->getContent_boundary();
							StringUtil::replaceAll(filen, "-", "");
							filen = this->getContextHome() + "/temp/"+ filen + CastUtil::lexical_cast<std::string>(Timer::getCurrentTime());
							ofile.open(&filen[0], std::ios::binary | std::ios::app);
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

void HttpRequest::buildRequest(std::string_view key, std::string_view value)
{
	if(strcasecmp(&key[0], "accept-language")==0)
	{
		//StringUtil::trim(value);
		strvVec lemp;
		StringUtil::split(lemp, value, (","));
		for(unsigned int li=0;li<lemp.size();li++)
		{
			if(lemp.at(li).find(";")==std::string::npos && lemp.at(li)!="")
			{
				std::string_view t = lemp.at(li);
				size_t s = t.find_first_not_of(" ");
				size_t e = t.find_last_not_of(" ")+1;
				t = t.substr(s,e-s);
				this->localeInfo.push_back(t);
			}
			else if(lemp.at(li)!="")
			{
				std::string_view t = lemp.at(li);
				size_t s = t.find_first_not_of(" ");
				size_t e = t.find(";");
				t = t.substr(s,e-s);
				e = t.find_last_not_of(" ")+1;
				t = t.substr(0,e);
				this->localeInfo.push_back(t);
			}
		}
		addHeader(key, value);
	}
	else if(strcasecmp(&key[0], "authorization")==0)
	{
		//StringUtil::trim(value);
		this->getAuthParams(value);
		addHeader(key, value);
	}
	else if(strcasecmp(&key[0], "cookie")==0)
	{
		//StringUtil::trim(value);
		this->cookie = true;
		strvVec results;
		StringUtil::split(results, value, ("; "));
		for(int j=0;j<(int)results.size();j++)
		{
			strvVec results1;
			StringUtil::split(results1, results.at(j), ("="));
			if(results1.size()==2)
				cookieattrs[results1.at(0)] = results1.at(1);
			else
				cookieattrs[results1.at(0)] = "true";
		}
		//addHeader(key, value);
	}
	else if(strcasecmp(&key[0], "content-type")==0)
	{
		//StringUtil::trim(value);
		std::string_view tempi(value);
		size_t s = tempi.find("boundary");
		if(s!=std::string::npos)
		{
			addHeader(key, tempi.substr(0,s));
			tempi = tempi.substr(s);
			strvVec results;
			StringUtil::split(results, tempi, ("="));
			if(results.size()==2)
			{
				std::string_view bound = "--" + results.at(1).substr(0,results.at(1).length());
				this->setContent_boundary(bound);
			}
		}
		else
		{
			addHeader(key, value);
		}
	}
	else if(strcasecmp(&key[0], "content")==0 && value!="")
	{
		content.append(value);
	}
	else if(strcasecmp(&key[0], "method")==0)
	{
		this->setMethod(value);
	}
	else if(strcasecmp(&key[0], "httpversion")==0)
	{
		this->httpVersion = value;
		int ix = strcasestr(&value[0], "http/") - &value[0];
		std::string_view versionStr = value.substr(ix+5);
		//StringUtil::trim(versionStr);
		float version = -1;
		try {
			version = CastUtil::lexical_cast<float>(versionStr);
			this->httpVers = version;
		} catch(const std::exception& e) {
		}
	}
	else if(strcasecmp(&key[0], "getarguments")==0)
	{
		strvVec params;
		std::map<std::string_view ,int> indices;
		value = CryptoHandler::urlDecode(value);
		StringUtil::split(params, value, ("&"));
		for(unsigned j=0;j<params.size();j++)
		{
			strvVec param;
			StringUtil::split(param, params.at(j), ("="));
			if(param.size()==2)
			{
				std::string attN = std::string(param.at(0));
				StringUtil::replaceFirst(attN,"\r","");
				StringUtil::replaceFirst(attN,"\t","");
				StringUtil::replaceFirst(attN," ","");
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
							  + CastUtil::lexical_cast<std::string>(indices[attN])
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
	else if(key.find("url")!=std::string::npos)
	{
		//strvVec memp;
		this->setActUrl(value);
		//StringUtil::split(memp, value, ("/"));
		/*int fs = value.find_first_of("/");
		int es = value.find_last_of("/");
		if(fs==es)
		{
			this->setCntxt_name("");
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
	}
	else if(strcasecmp(&key[0], "httpline")==0)
	{
		strvVec vemp;
		StringUtil::split(vemp, value, (" "));
		if(vemp.size()<3)
		{
			status = HTTPResponseStatus::BadRequest;
			return;
		}
		else if(!isValidHttpMethod(vemp.at(0)))
		{
			status = HTTPResponseStatus::InvalidMethod;
			return;
		}
		else
		{
			int ix = strcasestr(&vemp.at(2)[0], "http/") - &vemp.at(2)[0];
			std::string_view versionStr = vemp.at(2).substr(ix+5);
			float version = -1;
			try {
				version = CastUtil::lexical_cast<float>(versionStr);
				this->httpVers = version;
			} catch(const std::exception& e) {
				status = HTTPResponseStatus::HttpVersionNotSupported;
				return;
			}
			if(version<1.0 && version>1.1)
			{
				status = HTTPResponseStatus::HttpVersionNotSupported;
				return;
			}
			if(version<1.1 && strcasecmp(&vemp.at(0)[0], "options")==0)
			{
				status = HTTPResponseStatus::InvalidMethod;
				return;
			}
		}

		if(!isValidHttpMethod(vemp.at(0)))
		{
			status = HTTPResponseStatus::InvalidMethod;
			return;
		}
		this->setMethod(vemp.at(0));
		vemp.erase(vemp.begin());
		//StringUtil::replaceFirst(vemp.at(1),"\r","");
		this->httpVersion = vemp.at(1);
		//StringUtil::replaceFirst(vemp.at(0)," ","");
		if(vemp.at(0).find("?")!=std::string_view ::npos)
		{
			std::string_view valu(vemp.at(0));
			vemp[0] = valu.substr(0,vemp.at(0).find("?"));
			valu = CryptoHandler::urlDecode(valu.substr(valu.find("?")+1));
			strvVec params;
			std::map<std::string_view ,int> indices;
			StringUtil::split(params, valu, ("&"));
			for(unsigned j=0;j<params.size();j++)
			{
				strvVec param;
				StringUtil::split(param, params.at(j), ("="));
				if(param.size()==2)
				{
					std::string attN = std::string(param.at(0));
					StringUtil::replaceFirst(attN,"\r","");
					StringUtil::replaceFirst(attN,"\t","");
					StringUtil::replaceFirst(attN," ","");
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
								  + CastUtil::lexical_cast<std::string>(indices[attN])
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
	}
	else
	{
		addHeaderValue(key, value);
	}
}

std::string_view HttpRequest::toString()
{
	std::string_view ret;
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		ret += "\n" + it->first + ": " + it->second;
	}
	ret += "\nContent: "+this->getContent();
	ret += "\nHttp Version: "+this->getHttpVersion();
	ret += "\nMethod: "+this->getMethod();
	ret += "\nUrl: "+this->getUrl();
	ret += "\nFile: "+this->getFile();
	ret += "\nContext Name: "+this->getCntxt_name();
	ret += "\nContext Root: "+this->getCntxt_root();
	ret += "\nDefault Locale: "+this->getDefaultLocale();
	ret += "\nContent Boundary: "+this->getContent_boundary();
	std::string_view vals;
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
	ret += "\nRequest Parameters "+vals;//CastUtil::lexical_cast<std::string>(this->getRequestParams().size());
	return ret;
}


HttpRequest::~HttpRequest()
{
}
std::string_view HttpRequest::getMethod() const
{
	return method;
}

void HttpRequest::setMethod(std::string_view method)
{
	this->method = method;
}

HttpSession* HttpRequest::getSession()
{
	return &(this->session);
}

void HttpRequest::setUrl(std::string_view url)
{
	std::string turl = std::string(url);
	StringUtil::replaceFirst(turl,"//","/");
	this->url = turl;
	this->ext = getFileExtension(turl);
}

const std::string& HttpRequest::getUrl() const
{
	return this->url;
}

void HttpRequest::setCurl(std::string_view curl)
{
	this->curl = curl;
}

const std::string& HttpRequest::getCurl() const
{
	return this->curl;
}

float HttpRequest::getHttpVers() const
{
	return this->httpVers;
}

std::string_view HttpRequest::getHttpVersion() const
{
	return this->httpVersion;
}

std::string_view HttpRequest::getContent_boundary() const
{
	return content_boundary;
}

void HttpRequest::setContent_boundary(std::string_view content_boundary)
{
	this->content_boundary = content_boundary;
}

std::string_view HttpRequest::getContent() const
{
	return content;
}

void HttpRequest::setContent(std::string_view content)
{
	this->content = content;
}

RMap HttpRequest::getRequestParams() const
{
	return requestParams;
}

void HttpRequest::setRequestParams(const RMap& requestParams)
{
	this->requestParams = requestParams;
}

std::string_view HttpRequest::getRequestParam(std::string_view key)
{
	if(this->requestParams.find(key)!=this->requestParams.end())
		return this->requestParams[key];
	else if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].getContent();
	else
		return "";
}

MultipartContent HttpRequest::getMultipartContent(std::string_view key)
{
	MultipartContent cont;
	if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key];
	else
		return cont;
}


std::string_view HttpRequest::getRequestParamType(std::string_view key)
{
	if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].getHeader("Content-Type");
	else
		return "";
}

void HttpRequest::setRequestParam(std::string_view key, std::string_view value)
{
	this->requestParams[key] = value;
}

std::string_view HttpRequest::getCntxt_root() const
{
	return cntxt_root;
}

void HttpRequest::setCntxt_root(std::string_view cntxt_root)
{
	this->cntxt_root = cntxt_root;
	this->cntxt_home = webpath + "/" + cntxt_name;
}

std::string_view HttpRequest::getDefaultLocale() const
{
	if(this->localeInfo.size()>0)
		return this->localeInfo.at(0);
	else
		return "en";
}

std::string HttpRequest::getCntxt_name() const
{
	return cntxt_name;
}

void HttpRequest::setCntxt_name(std::string_view cntxt_name)
{
	this->cntxt_name = std::string(cntxt_name);
}

const std::string& HttpRequest::getFile() const
{
	return file;
}

void HttpRequest::setFile(std::string_view file)
{
	if(this->file.length()>0 && this->url.find(this->file)!=std::string::npos
			&& this->url.find("/")!=std::string::npos)
	{
		this->url = this->url.substr(0, this->url.find_last_of("/")+1) +  file;
	}
	else
	{
		this->url +=  "/" + file;
	}
	this->file = std::string(file);
	this->ext = getFileExtension(std::string(file));
}

const std::string& HttpRequest::getActUrl() const
{
	return actUrl;
}

void HttpRequest::setActUrl(std::string_view actUrl)
{
	std::string_view au = actUrl;
	if(au.at(0)=='/') {
		au = au.substr(1);
	}
	StringUtil::split(actUrlParts, au, ("/"));
	if(actUrlParts.size()>1 || (actUrlParts.size()==1 && actUrlParts[0].find(".")==std::string::npos)) {
		cntxt_name = actUrlParts[0];
	}
	if(actUrlParts.size()>0 && actUrlParts[actUrlParts.size()-1].find(".")!=std::string::npos) {
		file = actUrlParts[actUrlParts.size()-1];
	}
	if(actUrl.at(0)!='/') {
		this->actUrl = "/" + actUrl;
	} else {
		this->actUrl = actUrl;
	}
}

void HttpRequest::normalizeUrl()
{
	if(cntxt_name!="" && actUrlParts.at(0)!=cntxt_name) {
		actUrlParts.insert(actUrlParts.begin(), cntxt_name);
		curl = "/" + cntxt_name + actUrl;
	} else {
		curl = actUrl;
	}
	url = this->webpath + curl;
	this->ext = getFileExtension(curl);
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
std::string_view HttpRequest::toPHPVariablesString(std::string_view def)
{
	std::string_view ret;
	ret = "<?php";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string_view header = StringUtil::toUpperCopy(it->first);
		StringUtil::replaceAll(header, "-", "_");
		ret += "$_SERVER['HTTP_"+header+"'] = '"+it->second + "';\n";
	}
	ret += "$HTTP_RAW_POST_DATA = '"+this->getContent();
	ret += "';\n$_SERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "';\n$_SERVER['REQUEST_METHOD'] = '"+this->getMethod();
	std::string_view requri = this->getActUrl();
	StringUtil::replaceFirst(requri, ("/"+this->getCntxt_name()), "");
	if(requri=="")
		requri = "/";
	ret += "';\n$_SERVER['REQUEST_URI'] = '"+requri;
	ret += "';\n$_SERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "';\n$_SERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	if(def=="")
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
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length()) + ";";
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
				ret += "\n$_FILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length()) + ";";
				ret += "\n$_FILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "';";
				ret += "\n$_FILES['"+iter->first+"']['error'] = 0;";
			}
		}
	}
	if(def=="")
		ret += "\ninclude_once('"+this->getUrl()+"');";
	else
		ret += "\ninclude_once('"+def+"');";
	ret += "?>";
	return ret;
}

std::string_view HttpRequest::toPerlVariablesString()
{
	std::string_view ret;
	ret += "$_SERVER = {};";
	ret += "\n$_SERVER_EX = {};";
	ret += "\n$_GET = {};";
	ret += "\n$_POST = {};";
	ret += "\n$_FILES = {};\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string_view header = StringUtil::toUpperCopy(it->first);
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
				std::string_view key = iter->first;
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
				std::string_view key = iter->first;
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
				std::string_view key = iter->first;
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
						key.substr(key.find("{")) + "{'size'} = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length()) + ";";
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
				ret += "\n$_FILES{'"+iter->first+"'}{'size'} = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length()) + ";";
				ret += "\n$_FILES{'"+iter->first+"'}{'tmp_name'} = '"+ dat.getTempFileName() + "';";
				ret += "\n$_FILES{'"+iter->first+"'}{'error'} = 0;";
			}
		}
	}
	ret += "\n\n";
	return ret;
}

std::string_view HttpRequest::toRubyVariablesString()
{
	std::string_view ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string_view header = StringUtil::toUpperCopy(it->first);
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
				std::string_view key = iter->first.substr(0, iter->first.find("["));
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
				std::string_view key = iter->first.substr(0, iter->first.find("["));
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
				std::string_view key = iter->first.substr(iter->first.find("["));
				StringUtil::replaceFirst(key,"[","");
				StringUtil::replaceFirst(key,"]","");
				ret += "\nif(!FILES.has_key?('"+iter->first.substr(0, iter->first.find("["))
						+"'))\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']={}\nend";
				ret += "\nif(!FILES['"+iter->first.substr(0, iter->first.find("["))+"'].has_key?('"+key+"'))"
						+ "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']['"+key+"']={}\nend";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length());
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
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\nrequire ('"+this->getUrl()+"')\n";
	return ret;
}

std::string_view HttpRequest::toPythonVariablesString()
{
	std::string_view ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string_view header = StringUtil::toUpperCopy(it->first);
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
				std::string_view key = iter->first.substr(0, iter->first.find("["));
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
				std::string_view key = iter->first.substr(0, iter->first.find("["));
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
				std::string_view key = iter->first.substr(iter->first.find("["));
				StringUtil::replaceFirst(key,"[","");
				StringUtil::replaceFirst(key,"]","");
				ret += "\nif '"+iter->first.substr(0, iter->first.find("["))
						+"' not in FILES:\n\tFILES['"+iter->first.substr(0, iter->first.find("["))+"']={}\n";
				ret += "\nif '"+key+"' not in FILES['"+iter->first.substr(0, iter->first.find("["))+"']:\n"
						+ "\n\tFILES['"+iter->first.substr(0, iter->first.find("["))+"']['"+key+"']={}\n";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length());
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
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\n\n";
	return ret;
}

std::string_view HttpRequest::toLuaVariablesString()
{
	std::string_view ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string_view header = StringUtil::toUpperCopy(it->first);
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
				std::string_view key = iter->first.substr(0, iter->first.find("["));
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
				std::string_view key = iter->first.substr(0, iter->first.find("["));
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
				std::string_view key = iter->first.substr(iter->first.find("["));
				StringUtil::replaceFirst(key,"[","");
				StringUtil::replaceFirst(key,"]","");
				ret += "\nif FILES['"+iter->first.substr(0, iter->first.find("["))+"'] == nil then"
						+ "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']={}\nend\n";
				ret += "\nif FILES['"+iter->first.substr(0, iter->first.find("["))+"']['"+key+"']  == nil then"
						+ "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']['"+key+"']={}\nend\n";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length());
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
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\ndofile(\""+this->getUrl()+"\")\n";
	return ret;
}

std::string_view HttpRequest::toNodejsVariablesString()
{
	std::string_view ret;
	ret += "SERVER = {};";
	ret += "\nSERVER_EX = {};";
	ret += "\nGET = {};";
	ret += "\nPOST = {};";
	ret += "\nFILES = {};\n";
	RMap::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		std::string_view header = StringUtil::toUpperCopy(it->first);
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
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length()) + ";";
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
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<std::string>(dat.getContent().length()) + ";";
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
	std::map<std::string_view,std::string_view,cicomp>::iterator it;
	std::map<std::string_view,std::string_view,cicomp> reqparams = this->getRequestParams();
	std::map<std::string_view,std::string_view,cicomp> qryparams = this->getQueryParams();
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

void HttpRequest::setQueryParam(std::string_view name, std::string_view value)
{
	this->queryParams[name] = value;
}

std::string_view HttpRequest::getQueryParam(std::string_view key)
{
	if(this->queryParams.find(key)!=this->queryParams.end())
		return this->queryParams[key];
	return "";
}

bool HttpRequest::hasCookie()
{
	std::string_view ffeadid;
	if(cookieattrs.find("FFEADID")!=cookieattrs.end())
		ffeadid = cookieattrs["FFEADID"];
	return this->cookie && ffeadid!="";
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

std::string_view HttpRequest::getAuthOrderinfoAttribute(const int& key)
{
	if(authorderinf.find(key)!=authorderinf.end())
		return authorderinf[key];
	else return "";
}

std::string_view HttpRequest::getReqOrderinfoAttribute(const int& key)
{
	if(reqorderinf.find(key)!=reqorderinf.end())
		return reqorderinf[key];
	else return "";
}

std::string_view HttpRequest::getCookieInfoAttribute(std::string_view key)
{
	if(cookieattrs.find(key)!=cookieattrs.end())
		return cookieattrs[key];
	else return "";
}

std::string_view HttpRequest::getHeader(std::string_view key)
{
	if(this->headers.find(key)!=this->headers.end())
		return this->headers[key];
	return "";
}

bool HttpRequest::hasHeader(std::string_view key)
{
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
		if(strcasecmp(&method[0], "options")==0 && getHeader(AccessControlRequestMethod)!="")
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

void HttpRequest::addHeader(std::string_view header, std::string_view value)
{
	if(headers.find(header)!=headers.end()) {
		headers[header] += "," + value;
	} else {
		headers[header] = value;
	}
}

void HttpRequest::addHeaderValue(std::string_view header, std::string_view value)
{
	if(header!="")
	{
		if(VALID_REQUEST_HEADERS.find(","+header+",")!=std::string::npos)
		{
			if(headers.find(header)!=headers.end()) {
				headers[header] += "," + value;
			} else {
				headers[header] = value;
			}
		}
		else
		{
			//std::cout << ("Non standard Header std::string_view " + header) << std::endl;
			if(!RegexUtil::matches(header, "^[a-zA-Z]+[-|a-zA-Z0-9]*"))
			{
				//std::cout << ("Invalid Header std::string_view " + header) << std::endl;
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

strvVec HttpRequest::parseHeaderValue(std::string_view headerValue)
{
	std::string th = std::string(headerValue);
	RegexUtil::replace(th, "\\s*,\\s*|\\s+", ",");
	std::vector<std::string_view> tmp;
	StringUtil::split(tmp, th, ",");
	return tmp;
}

const std::string HttpRequest::VALID_METHODS = ",get,post,options,delete,head,put,post,trace,";
bool HttpRequest::isValidHttpMethod(std::string_view method)
{
	std::string lmeth = ","+method+",";
	return strcasestr(VALID_METHODS.c_str(), lmeth.c_str())!=NULL;
}

bool HttpRequest::isValidHttpMethod()
{
	return isValidHttpMethod(method);
}

bool HttpRequest::isAgentAcceptsCE()
{
	return strcasecmp(&getHeader(AcceptEncoding)[0], "gzip")==0 || strcasecmp(&getHeader(AcceptEncoding)[0], "deflate")==0;
}

bool HttpRequest::isHeaderValue(std::string_view header, std::string_view value, const bool& ignoreCase)
{
	return header!="" && headers.find(header)!=headers.end()
			&& (headers[header]==value || (ignoreCase && strcasecmp(&headers[header][0], &value[0])==0));
}

bool HttpRequest::hasHeaderValuePart(std::string_view header, std::string_view valuePart, const bool& ignoreCase)
{
	if(header!="" && headers.find(header)!=headers.end())
	{
		std::string_view hvalue = headers[header];
		if(hvalue==valuePart || (ignoreCase && strcasecmp(&hvalue[0], &valuePart[0])==0))
		{
			return true;
		}
		else if(ignoreCase)
		{
			//StringUtil::toLower(hvalue);
			//StringUtil::toLower(valuePart);
			std::vector<std::string_view> hvec;
			StringUtil::split(hvec, hvalue, ",");
			for(int yy=0;yy<(int)hvec.size();yy++)
			{
				if(strcasecmp(&hvec.at(yy)[0], &valuePart[0])==0) {
					return true;
				}
			}
		}
		else
		{
			std::vector<std::string_view> hvec;
			StringUtil::split(hvec, hvalue, ",");
			for(int yy=0;yy<(int)hvec.size();yy++)
			{
				//std::string_view vp = hvec.at(yy);
				//StringUtil::trim(vp);
				if(strcasecmp(&hvec.at(yy)[0], &valuePart[0])==0) {
					return true;
				}
			}
		}
	}
	return false;
}

std::vector<std::vector<int> > HttpRequest::getRanges(std::vector<std::string_view> &rangesVec)
{
	std::vector<std::vector<int> > rangeValuesLst;
	std::string_view ranges = getHeader(Range);
	if(ranges.find("bytes=")!=0)
	{
		return rangeValuesLst;
	}
	ranges = ranges.substr(ranges.find("bytes=")+6);
	//StringUtil::replaceFirst(ranges, "bytes=", "");
	StringUtil::split(rangesVec, ranges, (","));
	for (int var = 0; var <(int)rangesVec.size(); ++var) {
		std::string_view range = rangesVec.at(var);
		//StringUtil::trim(range);
		std::vector<std::string_view> rangeVals;
		StringUtil::split(rangeVals, range, ("-"));
		if(rangeVals.size()>2)
		{
			//error
		}
		else
		{
			if(rangeVals.at(0)!="" && rangeVals.at(1)!="")
			{
				try {
					int start = CastUtil::lexical_cast<int>(rangeVals.at(0));
					int end = CastUtil::lexical_cast<int>(rangeVals.at(1));

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
			else if(rangeVals.at(0)!="")
			{
				try {
					int start = CastUtil::lexical_cast<int>(rangeVals.at(0));
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
			else if(rangeVals.at(1)!="")
			{
				try {
					int end = CastUtil::lexical_cast<int>(rangeVals.at(1));
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

void HttpRequest::setContent_tfile(std::string_view tfile)
{
	content_tfile = tfile;
}

std::string_view HttpRequest::getContent_tfile()
{
	return content_tfile;
}

void HttpRequest::addMultipartFormContent(std::string_view key, const MultipartContent& content)
{
	requestParamsF[key] = content;
}

void HttpRequest::addContent(const MultipartContent& content)
{
	contentList.push_back(content);
}

bool HttpRequest::isNonBinary(std::string_view mimeType)
{
	return (strcasestr(&mimeType[0], "text")!=NULL || strcasestr(&mimeType[0], "css")!=NULL
			|| strcasestr(&mimeType[0], "x-javascript")!=NULL || strcasestr(&mimeType[0], "json")!=NULL
			|| strcasestr(&mimeType[0], "xml")!=NULL || strcasestr(&mimeType[0], "html")!=NULL);
}

std::string_view HttpRequest::getParamValue(std::string_view key)
{
	if(this->queryParams.find(key)!=this->queryParams.end())
		return this->queryParams[key];
	else if(this->requestParams.find(key)!=this->requestParams.end())
		return this->requestParams[key];
	else if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].getContent();
	else
		return "";
}

HTTPResponseStatus HttpRequest::getRequestParseStatus() const
{
	return status;
}

void HttpRequest::setSessionID(std::string_view sessionID)
{
	this->sessionID = sessionID;
}

std::string_view HttpRequest::getSessionID() const
{
	return sessionID;
}

std::vector<MultipartContent> HttpRequest::getMultiPartFileList(std::string_view name)
{
	std::vector<MultipartContent> filevec;
	FMap::iterator it;
	if(name!="")
	{
		for (it=this->requestParamsF.begin();it!=this->requestParamsF.end();++it) {
			if(it->second.getTempFileName()!="" && (it->second.getName()==name || it->second.getName().find(name+"[")==0))
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

std::string_view HttpRequest::getPassword() const {
	return password;
}

std::string_view HttpRequest::getUserName() const {
	return userName;
}

std::string_view HttpRequest::getAuthMethod() const {
	return authMethod;
}

void HttpRequest::setPassword(std::string_view v) {
	this->password = v;
}

void HttpRequest::setUserName(std::string_view v) {
	this->userName = v;
}

void HttpRequest::setAuthMethod(std::string_view v) {
	this->authMethod = v;
}

std::string_view HttpRequest::toPluginString() {
	std::string_view text = (this->cntxt_name + "\n");
	text += (this->host + "\n");
	text += (this->url + "\n");
	text += (this->file + "\n");
	text += (this->httpVersion + "\n");
	text += (this->method + "\n");
	text += (this->getDefaultLocale() + "\n");
	text += (this->sessionID + "\n");
	text += (this->actUrl + "\n");

	text += (CastUtil::lexical_cast<std::string>(this->content.length()) + "\n");
	text += (this->content);

	text += (CastUtil::lexical_cast<std::string>(this->preamble.length()) + "\n");
	text += (this->preamble);

	text += (CastUtil::lexical_cast<std::string>(this->epilogue.length()) + "\n");
	text += (this->epilogue);

	text += (CastUtil::lexical_cast<std::string>(this->requestParams.size()) + "\n");
	RMap::iterator it;
	for(it=this->requestParams.begin();it!=this->requestParams.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::lexical_cast<std::string>(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::lexical_cast<std::string>(this->queryParams.size()) + "\n");
	for(it=this->queryParams.begin();it!=this->queryParams.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::lexical_cast<std::string>(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::lexical_cast<std::string>(this->headers.size()) + "\n");
	for(it=this->headers.begin();it!=this->headers.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::lexical_cast<std::string>(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::lexical_cast<std::string>(this->requestParamsF.size()) + "\n");
	FMap::iterator fit;
	for(fit=this->requestParamsF.begin();fit!=this->requestParamsF.end();++fit)
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

void HttpRequest::setHttp2Headers(RMap headers)
{
	method = headers[":method"];
	authority = headers[":authority"];
	httpVers = 2.0;
	httpVersion = "http/2";
	scheme = headers[":scheme"];
	buildRequest("url", headers[":path"]);
	headers.erase(":method");
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

std::string_view HttpRequest::getContextHome()
{
	return cntxt_home;
}

void HttpRequest::setContextHome(std::string_view home)
{
	this->cntxt_home = home;
}

const std::string& HttpRequest::getExt() const
{
	return ext;
}
