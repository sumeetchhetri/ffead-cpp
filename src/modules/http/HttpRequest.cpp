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

string HttpRequest::VALID_REQUEST_HEADERS = ",accept,accept-charset,accept-encoding,accept-language,accept-datetime,access-control-request-headers,access-control-request-method,authorization,cache-control,connection,cookie,content-length,content-md5,content-type,date,expect,from,host,if-match,if-modified-since,if-none-match,if-range,if-unmodified-since,max-forwards,origin,pragma,proxy-authorization,range,referer,te,upgrade,user-agent,via,warning,transfer-encoding,sec-websocket-key,sec-websocket-version,sec-websocket-accept,sec-websocket-protocol,sec-websocket-extensions,alt-used,http2-settings,";

string HttpRequest::Accept =			 "Accept";
string HttpRequest::AcceptCharset = 		 "Accept-Charset";
string HttpRequest::AcceptEncoding = 		 "Accept-Encoding";
string HttpRequest::AcceptLanguage = 		 "Accept-Language";
string HttpRequest::AcceptDatetime = 		 "Accept-Datetime";
string HttpRequest::AccessControlRequestHeaders = "Access-Control-Request-Headers";
string HttpRequest::AccessControlRequestMethod =  "Access-Control-Request-Method";
string HttpRequest::Authorization = 		 "Authorization";
string HttpRequest::CacheControl = 		 "Cache-Control";
string HttpRequest::Connection = 			 "Connection";
string HttpRequest::Cookie = 			 "Cookie";
string HttpRequest::ContentLength = 		 "Content-Length";
string HttpRequest::ContentMD5 = 			 "Content-MD5";
string HttpRequest::ContentType = 			 "Content-Type";
string HttpRequest::Date = 				 "Date";
string HttpRequest::Expect = 			 "Expect";
string HttpRequest::From = 				 "From";
string HttpRequest::Host = 				 "Host";
string HttpRequest::IfMatch = 			 "If-Match";
string HttpRequest::IfModifiedSince = 		 "If-Modified-Since";
string HttpRequest::IfNoneMatch = 		 "If-None-Match";
string HttpRequest::IfRange = 			 "If-Range";
string HttpRequest::TransferEncoding = "Transfer-Encoding";
string HttpRequest::IfUnmodifiedSince = 		 "If-Unmodified-Since";
string HttpRequest::MaxForwards = 			 "Max-Forwards";
string HttpRequest::Origin = 			 "Origin";
string HttpRequest::Pragma = 			 "Pragma";
string HttpRequest::ProxyAuthorization = 		 "Proxy-Authorization";
string HttpRequest::Range = 			 "Range";
string HttpRequest::Referer = 			 "Referer";
string HttpRequest::TE = 				 "TE";
string HttpRequest::Upgrade = 			 "Upgrade";
string HttpRequest::UserAgent = 			 "User-Agent";
string HttpRequest::Via = 				 "Via";
string HttpRequest::Warning = 			 "Warning";
string HttpRequest::SecWebSocketKey = "Sec-WebSocket-Key";
string HttpRequest::SecWebSocketVersion = "Sec-WebSocket-Version";
string HttpRequest::SecWebSocketAccept = "Sec-WebSocket-Accept";
string HttpRequest::SecWebSocketProtocol = "Sec-WebSocket-Protocol";
string HttpRequest::SecWebSocketExtensions = "Sec-WebSocket-Extensions";
string HttpRequest::AltUsed = "Alt-Used";
string HttpRequest::Http2Settings = "HTTP2-Settings";

void HttpRequest::getAuthParams(string str)
{
	authMethod = (str.substr(0,str.find(" ")));
	StringUtil::toLower(authMethod);
	str = str.substr(str.find(" ")+1);
	if(authMethod=="basic")
	{
		unsigned char *input = (unsigned char *)str.c_str();
		int length = str.length();
		string temp = CryptoHandler::base64decode(input,length);
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
			string temr = tempvv.at(1);
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

void HttpRequest::getOauthParams(string str)
{
	authinfo["Method"] = str.substr(0,str.find(" "));
	str = str.substr(str.find(" ")+1);

	strVec tempv;
	StringUtil::split(tempv, str, (","));
	for(unsigned int i=0;i<tempv.size();i++)
	{
		strVec tempvv;
		StringUtil::split(tempvv, tempv.at(i), ("="));
		StringUtil::replaceFirst(tempvv.at(0),"\r","");
		StringUtil::replaceFirst(tempvv.at(0),"\n","");
		string temr = tempvv.at(1);
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
}

HttpRequest::HttpRequest(const string& path)
{
	this->webpath = path;
	cookie = false;
	httpVers = 0;
}

HttpRequest::HttpRequest(const strVec& vec, const string& path)
{
	this->webpath = path;
	if(vec.size()!=0)
	{
		this->setContent("");
		string conten;
		bool contStarts = false;
		this->cookie = false;
		for(unsigned int i=0;i<vec.size();i++)
		{
			strVec temp,vemp,memp;
			if((vec.at(i)=="\r" || vec.at(i)==""|| vec.at(i)=="\r\n") && !contStarts)
			{
				contStarts = true;
				continue;
			}
			//if(!contStarts && vec.at(i)=="\r")
			//	contStarts = true;
			StringUtil::split(temp, vec.at(i), (": "));
			if(!contStarts && temp.size()>1)
			{
				StringUtil::replaceFirst(temp.at(1),"\r","");
				StringUtil::toLower(temp.at(0));
				if(temp.at(0)==HttpRequest::Authorization)
				{
					this->getAuthParams(temp.at(1));
					addHeaderValue(temp.at(0), temp.at(1));
				}
				else if(temp.at(0)==HttpRequest::AcceptLanguage)
				{
					strVec lemp;
					StringUtil::split(lemp, temp.at(1), (","));
					for(unsigned int li=0;li<lemp.size();li++)
					{
						if(lemp.at(li).find(";")==string::npos && lemp.at(li)!="")
						{
							string t = lemp.at(li);
							size_t s = t.find_first_not_of(" ");
							size_t e = t.find_last_not_of(" ")+1;
							t = t.substr(s,e-s);
							this->localeInfo.push_back(t);
						}
						else if(lemp.at(li)!="")
						{
							string t = lemp.at(li);
							size_t s = t.find_first_not_of(" ");
							size_t e = t.find(";");
							t = t.substr(s,e-s);
							e = t.find_last_not_of(" ")+1;
							t = t.substr(0,e);
							this->localeInfo.push_back(t);
						}
					}
					addHeaderValue(temp.at(0), temp.at(1));
				}
				else if(temp.at(0)==HttpRequest::ContentType)
				{
					string tempi(temp.at(1));
					size_t s = tempi.find("boundary");
					if(s!=string::npos)
					{
						addHeaderValue(temp.at(0), tempi.substr(0,s));
						tempi = tempi.substr(s);
						strVec results;
						StringUtil::split(results, tempi, ("="));
						if(results.size()==2)
						{
							string bound = "--" + results.at(1).substr(0,results.at(1).length());
							this->setContent_boundary(bound);
						}
					}
					else
					{
						addHeaderValue(temp.at(0), temp.at(1));
					}
				}
				else if(temp.at(0)==HttpRequest::Cookie)
				{
					this->cookie = true;
					strVec results;
					StringUtil::split(results, temp.at(1), ("; "));
					for(unsigned j=0;j<(int)results.size();j++)
					{
						strVec results1;
						StringUtil::split(results1, results.at(j), ("="));
						if(results1.size()==2)
							cookieattrs[results1.at(0)] = results1.at(1);
						else
							cookieattrs[results1.at(0)] = "true";
					}
					addHeaderValue(temp.at(0), temp.at(1));
				}
				else
				{
					addHeaderValue(temp.at(0), temp.at(1));
				}
			}
			else
			{
				string tem = temp.at(0);
				if(!contStarts)
				{
					StringUtil::split(vemp, tem, (" "));
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
						string versionStr = StringUtil::replaceFirstCopy(StringUtil::toLowerCopy(vemp.at(2)), "http/", "");
						StringUtil::trim(versionStr);
						float version = -1;
						try {
							version = CastUtil::lexical_cast<float>(versionStr);
							this->httpVers = version;
						} catch(...) {
							status = HTTPResponseStatus::HttpVersionNotSupported;
							return;
						}
						if(version<1.0 && version>1.1)
						{
							status = HTTPResponseStatus::HttpVersionNotSupported;
							return;
						}
						if(version<1.1 && StringUtil::toLowerCopy(vemp.at(0))=="options")
						{
							status = HTTPResponseStatus::InvalidMethod;
							return;
						}
					}
				}
				if(!contStarts && StringUtil::toLowerCopy(vemp.at(0))=="get")
				{
					//StringUtil::replaceFirst(tem,"GET ","");
					vemp.erase(vemp.begin());
					this->setMethod("GET");
					//StringUtil::split(vemp, tem, (" "));
					StringUtil::replaceFirst(vemp.at(1),"\r","");
					this->httpVersion = vemp.at(1);
					StringUtil::replaceFirst(vemp.at(0)," ","");
					if(vemp.at(0).find("?")!=string ::npos)
					{
						strVec params;
						string valu(vemp.at(0));
						vemp[0] = valu.substr(0,vemp.at(0).find("?"));
						valu = valu.substr(valu.find("?")+1);
						//valu = CryptoHandler::urlDecode(valu);
						StringUtil::split(params,valu , ("&"));
						map<string ,int> indices;
						map<string,string>::iterator it;
						for(unsigned j=0;j<params.size();j++)
						{
							strVec param;
							StringUtil::split(param, params.at(j), ("="));
							if(param.size()==2)
							{
								string att = param.at(0);
								StringUtil::replaceFirst(att,"\r","");
								StringUtil::replaceFirst(att,"\t","");
								StringUtil::replaceFirst(att," ","");
								//this->setRequestParam(att,CryptoHandler::urlDecode(param.at(1)));
								string attN = CryptoHandler::urlDecode(att);
								if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]"] = CryptoHandler::urlDecode(param.at(1));
									cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
													  + CastUtil::lexical_cast<string>(indices[attN])
													  + "]"
													  + CryptoHandler::urlDecode(param.at(1))) << endl;
								}
								else
								{
									this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
								}
								reqorderinf[reqorderinf.size()+1] = att;
							}
						}
					}
					this->setActUrl(vemp.at(0));
					memp = StringUtil::splitAndReturn<vector<string> >(vemp.at(0), ("/"));
					int fs = vemp.at(0).find_first_of("/");
					int es = vemp.at(0).find_last_of("/");
					if(fs==es)
					{
						this->setCntxt_root(path+"default");
						this->setCntxt_name("default");
						this->setFile(vemp.at(0).substr(es+1));
						this->setUrl(path+"default/"+vemp.at(0));
					}
					else
					{
						int ss = vemp.at(0).substr(fs+1).find("/");
						if(ss>fs)
						{
							this->setCntxt_name(vemp.at(0).substr(fs+1,ss-fs));
							this->setCntxt_root(path+this->getCntxt_name());
							this->setFile(vemp.at(0).substr(es+1));
							this->setUrl(path+vemp.at(0));
						}
					}
				}
				else if(!contStarts && StringUtil::toLowerCopy(vemp.at(0))=="head")
				{
					//StringUtil::replaceFirst(tem,"HEAD ","");
					vemp.erase(vemp.begin());
					this->setMethod("HEAD");
					//StringUtil::split(vemp, tem, (" "));
					StringUtil::replaceFirst(vemp.at(1),"\r","");
					this->httpVersion = vemp.at(1);
					StringUtil::replaceFirst(vemp.at(0)," ","");
					if(vemp.at(0).find("?")!=string ::npos)
					{
						strVec params;
						string valu(vemp.at(0));
						vemp[0] = valu.substr(0,vemp.at(0).find("?"));
						valu = valu.substr(valu.find("?")+1);
						//valu = CryptoHandler::urlDecode(valu);
						StringUtil::split(params,valu , ("&"));
						map<string ,int> indices;
						map<string,string>::iterator it;
						for(unsigned j=0;j<params.size();j++)
						{
							strVec param;
							StringUtil::split(param, params.at(j), ("="));
							if(param.size()==2)
							{
								string att = param.at(0);
								StringUtil::replaceFirst(att,"\r","");
								StringUtil::replaceFirst(att,"\t","");
								StringUtil::replaceFirst(att," ","");
								//this->setRequestParam(att,CryptoHandler::urlDecode(param.at(1)));
								string attN = CryptoHandler::urlDecode(att);
								if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]"] = CryptoHandler::urlDecode(param.at(1));
									cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
													  + CastUtil::lexical_cast<string>(indices[attN])
													  + "]"
													  + CryptoHandler::urlDecode(param.at(1))) << endl;
								}
								else
								{
									this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
								}
								reqorderinf[reqorderinf.size()+1] = att;
							}
						}
					}
					this->setActUrl(vemp.at(0));
					memp = StringUtil::splitAndReturn<vector<string> >(vemp.at(0), ("/"));
					int fs = vemp.at(0).find_first_of("/");
					int es = vemp.at(0).find_last_of("/");
					if(fs==es)
					{
						this->setCntxt_root(path+"default");
						this->setCntxt_name("default");
						this->setFile(vemp.at(0).substr(es+1));
						this->setUrl(path+"default/"+vemp.at(0));
					}
					else
					{
						int ss = vemp.at(0).substr(fs+1).find("/");
						if(ss>fs)
						{
							this->setCntxt_name(vemp.at(0).substr(fs+1,ss-fs));
							this->setCntxt_root(path+this->getCntxt_name());
							this->setFile(vemp.at(0).substr(es+1));
							this->setUrl(path+vemp.at(0));
						}
					}
				}
				else if(!contStarts && StringUtil::toLowerCopy(vemp.at(0))=="trace")
				{
					//StringUtil::replaceFirst(tem,"TRACE ","");
					vemp.erase(vemp.begin());
					this->setMethod("TRACE");
					//StringUtil::split(vemp, tem, (" "));
					StringUtil::replaceFirst(vemp.at(1),"\r","");
					this->httpVersion = vemp.at(1);
					StringUtil::replaceFirst(vemp.at(0)," ","");
					if(vemp.at(0).find("?")!=string ::npos)
					{
						strVec params;
						string valu(vemp.at(0));
						vemp[0] = valu.substr(0,vemp.at(0).find("?"));
						valu = valu.substr(valu.find("?")+1);
						//valu = CryptoHandler::urlDecode(valu);
						StringUtil::split(params,valu , ("&"));
						map<string ,int> indices;
						map<string,string>::iterator it;
						for(unsigned j=0;j<params.size();j++)
						{
							strVec param;
							StringUtil::split(param, params.at(j), ("="));
							if(param.size()==2)
							{
								string att = param.at(0);
								StringUtil::replaceFirst(att,"\r","");
								StringUtil::replaceFirst(att,"\t","");
								StringUtil::replaceFirst(att," ","");
								//this->setRequestParam(att,CryptoHandler::urlDecode(param.at(1)));
								string attN = CryptoHandler::urlDecode(att);
								if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]"] = CryptoHandler::urlDecode(param.at(1));
									cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
													  + CastUtil::lexical_cast<string>(indices[attN])
													  + "]"
													  + CryptoHandler::urlDecode(param.at(1))) << endl;
								}
								else
								{
									this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
								}
								reqorderinf[reqorderinf.size()+1] = att;
							}
						}
					}
					this->setActUrl(vemp.at(0));
					memp = StringUtil::splitAndReturn<vector<string> >(vemp.at(0), ("/"));
					int fs = vemp.at(0).find_first_of("/");
					int es = vemp.at(0).find_last_of("/");
					if(fs==es)
					{
						this->setCntxt_root(path+"default");
						this->setCntxt_name("default");
						this->setFile(vemp.at(0).substr(es+1));
						this->setUrl(path+"default/"+vemp.at(0));
					}
					else
					{
						int ss = vemp.at(0).substr(fs+1).find("/");
						if(ss>fs)
						{
							this->setCntxt_name(vemp.at(0).substr(fs+1,ss-fs));
							this->setCntxt_root(path+this->getCntxt_name());
							this->setFile(vemp.at(0).substr(es+1));
							this->setUrl(path+vemp.at(0));
						}
					}
				}
				else if(!contStarts && StringUtil::toLowerCopy(vemp.at(0))=="options")
				{
					//StringUtil::replaceFirst(tem,"OPTIONS ","");
					this->setMethod("OPTIONS");
					vemp.erase(vemp.begin());
					//StringUtil::split(vemp, tem, (" "));
					StringUtil::replaceFirst(vemp.at(1),"\r","");
					this->httpVersion = vemp.at(1);
					StringUtil::replaceFirst(vemp.at(0)," ","");
					this->setActUrl(vemp.at(0));
					memp = StringUtil::splitAndReturn<vector<string> >(vemp.at(0), ("/"));
					int fs = vemp.at(0).find_first_of("/");
					int es = vemp.at(0).find_last_of("/");
					if(fs==es)
					{
						this->setCntxt_root(path+"default");
						this->setCntxt_name("default");
						this->setFile(vemp.at(0).substr(es+1));
						this->setUrl(path+"default/"+vemp.at(0));
					}
					else
					{
						int ss = vemp.at(0).substr(fs+1).find("/");
						if(ss>fs)
						{
							this->setCntxt_name(vemp.at(0).substr(fs+1,ss-fs));
							this->setCntxt_root(path+this->getCntxt_name());
							this->setFile(vemp.at(0).substr(es+1));
							this->setUrl(path+vemp.at(0));
						}
					}
				}
				else if(!contStarts && StringUtil::toLowerCopy(vemp.at(0))=="delete")
				{
					//StringUtil::replaceFirst(tem,"DELETE ","");
					this->setMethod("DELETE");
					vemp.erase(vemp.begin());
					//StringUtil::split(vemp, tem, (" "));
					StringUtil::replaceFirst(vemp.at(1),"\r","");
					this->httpVersion = vemp.at(1);
					StringUtil::replaceFirst(vemp.at(0)," ","");
					if(vemp.at(0).find("?")!=string ::npos)
					{
						strVec params;
						string valu(vemp.at(0));
						vemp[0] = valu.substr(0,vemp.at(0).find("?"));
						valu = valu.substr(valu.find("?")+1);
						valu = CryptoHandler::urlDecode(valu);
						StringUtil::split(params,valu, ("&"));
						map<string ,int> indices;
						map<string,string>::iterator it;
						for(unsigned j=0;j<params.size();j++)
						{
							strVec param;
							StringUtil::split(param, params.at(j), ("="));
							if(param.size()==2)
							{
								string att = param.at(0);
								StringUtil::replaceFirst(att,"\r","");
								StringUtil::replaceFirst(att,"\t","");
								StringUtil::replaceFirst(att," ","");
								string attN = CryptoHandler::urlDecode(att);
								if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]"] = CryptoHandler::urlDecode(param.at(1));
									cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
													  + CastUtil::lexical_cast<string>(indices[attN])
													  + "]"
													  + CryptoHandler::urlDecode(param.at(1))) << endl;
								}
								else
								{
									this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
								}
								reqorderinf[reqorderinf.size()+1] = att;
							}
						}
					}
					this->setActUrl(vemp.at(0));
					memp = StringUtil::splitAndReturn<vector<string> >(vemp.at(0), ("/"));
					int fs = vemp.at(0).find_first_of("/");
					int es = vemp.at(0).find_last_of("/");
					if(fs==es)
					{
						this->setCntxt_root(path+"default");
						this->setCntxt_name("default");
						this->setFile(vemp.at(0).substr(es+1));
						this->setUrl(path+"default/"+vemp.at(0));
					}
					else
					{
						int ss = vemp.at(0).substr(fs+1).find("/");
						if(ss>fs)
						{
							this->setCntxt_name(vemp.at(0).substr(fs+1,ss-fs));
							this->setCntxt_root(path+this->getCntxt_name());
							this->setFile(vemp.at(0).substr(es+1));
							this->setUrl(path+vemp.at(0));
						}
					}
				}
				else if(!contStarts && StringUtil::toLowerCopy(vemp.at(0))=="put")
				{
					//StringUtil::replaceFirst(tem,"PUT ","");
					this->setMethod("PUT");
					vemp.erase(vemp.begin());
					//StringUtil::split(vemp, tem, (" "));
					StringUtil::replaceFirst(vemp.at(1),"\r","");
					this->httpVersion = vemp.at(1);
					StringUtil::replaceFirst(vemp.at(0)," ","");
					if(vemp.at(0).find("?")!=string ::npos)
					{
						strVec params;
						string valu(vemp.at(0));
						vemp[0] = valu.substr(0,vemp.at(0).find("?"));
						valu = valu.substr(valu.find("?")+1);
						valu = CryptoHandler::urlDecode(valu);
						StringUtil::split(params,valu, ("&"));
						map<string ,int> indices;
						map<string,string>::iterator it;
						for(unsigned j=0;j<params.size();j++)
						{
							strVec param;
							StringUtil::split(param, params.at(j), ("="));
							if(param.size()==2)
							{
								string att = param.at(0);
								StringUtil::replaceFirst(att,"\r","");
								StringUtil::replaceFirst(att,"\t","");
								StringUtil::replaceFirst(att," ","");
								string attN = CryptoHandler::urlDecode(att);
								if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]"] = CryptoHandler::urlDecode(param.at(1));
									cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
													  + CastUtil::lexical_cast<string>(indices[attN])
													  + "]"
													  + CryptoHandler::urlDecode(param.at(1))) << endl;
								}
								else
								{
									this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
								}
								reqorderinf[reqorderinf.size()+1] = att;
							}
						}
					}
					this->setActUrl(vemp.at(0));
					memp = StringUtil::splitAndReturn<vector<string> >(vemp.at(0), ("/"));
					int fs = vemp.at(0).find_first_of("/");
					int es = vemp.at(0).find_last_of("/");
					if(fs==es)
					{
						this->setCntxt_root(path+"default");
						this->setCntxt_name("default");
						this->setFile(vemp.at(0).substr(es+1));
						this->setUrl(path+"default/"+vemp.at(0));
					}
					else
					{
						int ss = vemp.at(0).substr(fs+1).find("/");
						if(ss>fs)
						{
							this->setCntxt_name(vemp.at(0).substr(fs+1,ss-fs));
							this->setCntxt_root(path+this->getCntxt_name());
							this->setFile(vemp.at(0).substr(es+1));
							this->setUrl(path+vemp.at(0));
						}
					}
				}
				else if(!contStarts && StringUtil::toLowerCopy(vemp.at(0))=="post")
				{
					//StringUtil::replaceFirst(tem,"POST ","");
					this->setMethod("POST");
					vemp.erase(vemp.begin());
					//StringUtil::split(vemp, tem, (" "));
					//this->setUrl(vemp.at(0));
					//string pat(vemp.at(0));
					StringUtil::replaceFirst(vemp.at(1),"\r","");
					this->httpVersion = vemp.at(1);
					StringUtil::replaceFirst(vemp.at(0)," ","");
					if(vemp.at(0).find("?")!=string ::npos)
					{
						strVec params;
						string valu(vemp.at(0));
						vemp[0] = valu.substr(0,vemp.at(0).find("?"));
						valu = valu.substr(valu.find("?")+1);
						valu = CryptoHandler::urlDecode(valu);
						StringUtil::split(params,valu, ("&"));
						map<string ,int> indices;
						map<string,string>::iterator it;
						for(unsigned j=0;j<params.size();j++)
						{
							strVec param;
							StringUtil::split(param, params.at(j), ("="));
							if(param.size()==2)
							{
								string att = param.at(0);
								StringUtil::replaceFirst(att,"\r","");
								StringUtil::replaceFirst(att,"\t","");
								StringUtil::replaceFirst(att," ","");
								string attN = CryptoHandler::urlDecode(att);
								if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]"] = CryptoHandler::urlDecode(param.at(1));
									cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
													  + CastUtil::lexical_cast<string>(indices[attN])
													  + "]"
													  + CryptoHandler::urlDecode(param.at(1))) << endl;
								}
								else
								{
									this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
								}
								reqorderinf[reqorderinf.size()+1] = att;
							}
						}
					}
					this->setActUrl(vemp.at(0));
					memp = StringUtil::splitAndReturn<vector<string> >(vemp.at(0), ("/"));
					int fs = vemp.at(0).find_first_of("/");
					int es = vemp.at(0).find_last_of("/");
					if(fs==es)
					{
						this->setCntxt_root(path+"default");
						this->setCntxt_name("default");
						this->setFile(vemp.at(0).substr(es+1));
						this->setUrl(path+"default/"+vemp.at(0));
					}
					else
					{
						int ss = vemp.at(0).substr(fs+1).find("/");
						if(ss>fs)
						{
							this->setCntxt_name(vemp.at(0).substr(fs+1,ss-fs));
							this->setCntxt_root(path+this->getCntxt_name());
							this->setFile(vemp.at(0).substr(es+1));
							this->setUrl(path+vemp.at(0));
						}
					}
				}
				else if(contStarts)
				{
					conten.append(vec.at(i));
					if(i!=vec.size()-1)
					{
						conten.append("\n");
					}
				}
				else
				{
					cout << ("Bad Request line - " + vec.at(i)) << endl;
				}
			}
		}
		this->setContent(conten);
	}
}

void HttpRequest::updateContent()
{
	if(this->content!="")
	{
		updateFromContentStr();
	}
	else
	{
		updateFromContentFile();
	}
}

void HttpRequest::updateFromContentStr_Old()
{
	//cout << this->getContent() << flush;
	if(this->getHeader(ContentType).find("application/x-www-form-urlencoded")!=string::npos)
	{
		strVec params;
		string valu(this->getContent());
		StringUtil::split(params,valu , ("&"));
		map<string ,int> indices;
		map<string,string>::iterator it;
		for(unsigned j=0;j<params.size();j++)
		{
			strVec param;
			StringUtil::split(param, params.at(j), ("="));
			if(param.size()==2)
			{
				string att = param.at(0);
				StringUtil::replaceFirst(att,"\r","");
				StringUtil::replaceFirst(att,"\t","");
				StringUtil::replaceFirst(att," ","");
				string attN = CryptoHandler::urlDecode(att);
				if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
							  + CastUtil::lexical_cast<string>(indices[attN])
							  + "]"] = CryptoHandler::urlDecode(param.at(1));
					cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
									  + CastUtil::lexical_cast<string>(indices[attN])
									  + "]"
									  + CryptoHandler::urlDecode(param.at(1))) << endl;
				}
				else
				{
					this->setRequestParam(attN,CryptoHandler::urlDecode(param.at(1)));
				}
				reqorderinf[reqorderinf.size()+1] = attN;
			}
		}
	}
	else if(this->getContent()!="" && this->getContent_boundary()!="")
	{
		string delb = this->getContent_boundary();
		string delend = this->getContent_boundary()+"--";
		size_t stb = this->getContent().find(delb)+delb.length()+1;
		//size_t enb = this->getContent().find_last_not_of(delend);
		string param_conts = this->getContent().substr(stb);
		StringUtil::replaceFirst(param_conts,delend,"");
		param_conts = param_conts.substr(0,param_conts.length()-1);
		strVec parameters;
		StringUtil::split(parameters, param_conts, (delb));
		//cout << "Boundary: " << this->getContent_boundary() << flush;
		//cout << "\nLength: " << this->getContent().length() << flush;
		//cout << "\nStart End: " << stb << " " << enb << "\n" << flush;
		//cout << "\nContent: " << param_conts << "\n" << flush;
		map<string ,int> indices;
		map<string,string>::iterator it;
		for(unsigned j=0;j<parameters.size();j++)
		{
			if(parameters.at(j)=="" || parameters.at(j).find_first_not_of(" ")==string::npos
					|| parameters.at(j).find_first_not_of("\r")==string::npos)
				continue;
			MultipartContent datf;
			string parm = parameters.at(j);
			//cout << parm << "\nparm" << flush;
			size_t dis = parm.find("Content-Disposition: ");
			if(dis==string::npos)
				dis = parm.find("Content-disposition: ");
			string cont_disp,cont_type;
			if(dis!=string::npos)
			{
				size_t dist = parm.find("Content-Type: ");
				if(dist==string::npos)
					dist = parm.find("Content-type: ");
				size_t dise;
				if(dist==string::npos)
				{
					dist = parm.find("\r\r");
					dise = dist + 2;
					//cout << "\ndist = npos" << flush;
				}
				else
				{
					//parm = parm.substr(dist+14);
					cont_type = parm.substr(dist+14,parm.find("\r\r")-(dist+14));
					dise = parm.find("\r\r") + 2;
					//cout << "\nctype = " << cont_type << flush;
					//dist = dist-12;
				}
				cont_disp = parm.substr(dis+21,dist-(dis+21));
				StringUtil::replaceFirst(cont_disp,"\r","");
				//cout << "\ncdisp = " << cont_disp << flush;
				//cout << "\ndise = " << dise << flush;
				parm = parm.substr(dise);
			}
			strVec parmdef;
			StringUtil::split(parmdef, cont_disp, (";"));
			string key;
			for(unsigned k=0;k<parmdef.size();k++)
			{
				if(parmdef.at(k)!="" && parmdef.at(k).find("=")!=string::npos)
				{
					size_t stpd = parmdef.at(k).find_first_not_of(" ");
					size_t enpd = parmdef.at(k).find_last_not_of(" ");
					//cout << "\nparmdef = " << parmdef.at(k) << flush;
					//cout << "\nst en = " << stpd  << " " << enpd << flush;
					string propert = parmdef.at(k).substr(stpd,enpd-stpd+1);
					strVec proplr;
					StringUtil::split(proplr, propert, ("="));
					if(proplr.size()==2)
					{
						if(proplr.at(0)=="name" && proplr.at(1)!="\"\"")
						{
							key = proplr.at(1);
							key = key.substr(key.find_first_not_of("\""),key.find_last_not_of("\"")-key.find_first_not_of("\"")+1);
							key = CryptoHandler::urlDecode(key);
							StringUtil::replaceFirst(cont_type,"\r","");
							datf.addHeaderValue(MultipartContent::ContentType, cont_type);
							datf.setContent(parm);
						}
						else if(proplr.at(0)=="filename" && proplr.at(1)!="\"\"")
						{
							string fna = proplr.at(1);
							fna = fna.substr(fna.find_first_not_of("\""),fna.find_last_not_of("\"")-fna.find_first_not_of("\"")+1);
							fna = CryptoHandler::urlDecode(fna);
							datf.setFileName(fna);
						}
					}
				}
			}
			if(key!="")
			{
				string attN = CryptoHandler::urlDecode(key);
				if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
				{
					if(indices.find(attN)==indices.end())
					{
						indices[attN] = 0;
					}
					else
					{
						indices[attN] = indices[attN] + 1;
					}
					this->requestParamsF[attN.substr(0, attN.find("[")+1)
							  + CastUtil::lexical_cast<string>(indices[attN])
							  + "]"] = datf;
					cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
									  + CastUtil::lexical_cast<string>(indices[attN])
									  + "]"
									  + datf.getFileName()) << endl;
				}
				else
				{
					this->addMultipartFormContent(attN, datf);
				}
				reqorderinf[reqorderinf.size()+1] = attN;
				if(datf.getFileName()!="")
				{
					string tmpfile = this->getContextHome() + "/temp/"+ this->getContent_boundary() + datf.getContent();
					ofstream os;
					os.open(tmpfile.c_str(), ios::binary);
					os.write(datf.getContent().c_str(), datf.getContent().length());
					os.close();
					datf.setTempFileName(tmpfile);
					//datf.length = datf.value.length();
				}
				string hr = (key + " " + datf.getHeader("Content-Type") + " "+ datf.getFileName() +" "+ datf.getContent());
				//cout << hr << flush;
			}
		}
	}
}

//@TODO -- need to change this only for one pass of reading request body
void HttpRequest::updateFromContentStr()
{
	//cout << this->getContent() << flush;
	if(this->getHeader(ContentType).find("application/x-www-form-urlencoded")!=string::npos)
	{
		strVec params;
		string valu = CryptoHandler::urlDecode(this->getContent());
		StringUtil::split(params,valu , ("&"));
		map<string ,int> indices;
		map<string,string>::iterator it;
		for(unsigned j=0;j<params.size();j++)
		{
			strVec param;
			StringUtil::split(param, params.at(j), ("="));
			if(param.size()==2)
			{
				string attN = param.at(0);
				StringUtil::replaceFirst(attN,"\r","");
				StringUtil::replaceFirst(attN,"\t","");
				StringUtil::replaceFirst(attN," ","");
				if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
							  + CastUtil::lexical_cast<string>(indices[attN])
							  + "]"] = CryptoHandler::urlDecode(param.at(1));
					cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
									  + CastUtil::lexical_cast<string>(indices[attN])
									  + "]"
									  + CryptoHandler::urlDecode(param.at(1))) << endl;
				}
				else
				{
					this->setRequestParam(attN,CryptoHandler::urlDecode(param.at(1)));
				}
				reqorderinf[reqorderinf.size()+1] = attN;
			}
		}
	}
	else if(this->getContent()!="" && this->getContent_boundary()!="")
	{
		string contemp = this->getContent();

		bool bcontstarts = false, bhdrstarts = false, bcontends = false;
		string filen;
		ofstream ofile;
		string temp;
		string delb = this->getContent_boundary();
		string delend = this->getContent_boundary()+"--";
		string cont;
		vector<string> hdrs;
		map<string ,int> indices;
		cout << delb << endl;
		cout << delend << endl;
		while(contemp!="")
		{
			if(contemp.find("\n")!=string::npos)
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
					string attN = CryptoHandler::urlDecode(content.getName());
					if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
								  + CastUtil::lexical_cast<string>(indices[attN])
								  + "]", content);
						cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
										  + CastUtil::lexical_cast<string>(indices[attN])
										  + "]") << endl;
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
									  + CastUtil::lexical_cast<string>(indices[attN])
									  + "]", content);
							cout << ("for existing ---- creating array from similar params" + attN+"["
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]") << endl;
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
								  + CastUtil::lexical_cast<string>(indices[attN])
								  + "]", content);
						cout << ("creating array from similar params" + attN+"["
										  + CastUtil::lexical_cast<string>(indices[attN])
										  + "]") << endl;

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
					if(StringUtil::toLowerCopy(temp).find("content-disposition: ")!=string::npos
							&& StringUtil::toLowerCopy(temp).find("filename")!=string::npos)
					{
						filen = this->getContent_boundary();
						StringUtil::replaceAll(filen, "-", "");
						filen = this->getContextHome() + "/temp/"+ filen + CastUtil::lexical_cast<string>(Timer::getCurrentTime());
						ofile.open(filen.c_str(), ios::binary | ios::app);
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
	ifstream infile(this->content_tfile.c_str(), ios::binary);
	if(infile.is_open())
	{
		if(this->getContent_boundary()!="")
		{
			bool bcontstarts = false, bhdrstarts = false, bcontends = false;
			string filen;
			ofstream ofile;
			string temp;
			string delb = this->getContent_boundary();
			string delend = this->getContent_boundary()+"--";
			string cont;
			vector<string> hdrs;
			map<string ,int> indices;
			cout << delb << endl;
			cout << delend << endl;
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
						string attN = CryptoHandler::urlDecode(content.getName());
						if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
									  + CastUtil::lexical_cast<string>(indices[attN])
									  + "]", content);
							cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]") << endl;
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
										  + CastUtil::lexical_cast<string>(indices[attN])
										  + "]", content);
								cout << ("for existing ---- creating array from similar params" + attN+"["
												  + CastUtil::lexical_cast<string>(indices[attN])
												  + "]") << endl;
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
									  + CastUtil::lexical_cast<string>(indices[attN])
									  + "]", content);
							cout << ("creating array from similar params" + attN+"["
											  + CastUtil::lexical_cast<string>(indices[attN])
											  + "]") << endl;

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
						if(StringUtil::toLowerCopy(temp).find("content-disposition: ")!=string::npos
								&& StringUtil::toLowerCopy(temp).find("filename")!=string::npos)
						{
							filen = this->getContent_boundary();
							StringUtil::replaceAll(filen, "-", "");
							filen = this->getContextHome() + "/temp/"+ filen + CastUtil::lexical_cast<string>(Timer::getCurrentTime());
							ofile.open(filen.c_str(), ios::binary | ios::app);
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
			string temp;
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
	buildRequest(string(keyc), string(valuec));
}

void HttpRequest::buildRequest(string key, string value)
{
	string retval;
	stringstream ss;
	StringUtil::toLower(key);
	if(key=="accept-language")
	{
		StringUtil::trim(value);
		strVec lemp;
		StringUtil::split(lemp, value, (","));
		for(unsigned int li=0;li<lemp.size();li++)
		{
			if(lemp.at(li).find(";")==string::npos && lemp.at(li)!="")
			{
				string t = lemp.at(li);
				size_t s = t.find_first_not_of(" ");
				size_t e = t.find_last_not_of(" ")+1;
				t = t.substr(s,e-s);
				this->localeInfo.push_back(t);
			}
			else if(lemp.at(li)!="")
			{
				string t = lemp.at(li);
				size_t s = t.find_first_not_of(" ");
				size_t e = t.find(";");
				t = t.substr(s,e-s);
				e = t.find_last_not_of(" ")+1;
				t = t.substr(0,e);
				this->localeInfo.push_back(t);
			}
		}
		addHeaderValue(key, value);
	}
	else if(key=="authorization")
	{
		StringUtil::trim(value);
		this->getAuthParams(value);
		addHeaderValue(key, value);
	}
	else if(key=="cookie")
	{
		StringUtil::trim(value);
		this->cookie = true;
		strVec results;
		StringUtil::split(results, value, ("; "));
		for(unsigned j=0;j<(int)results.size();j++)
		{
			strVec results1;
			StringUtil::split(results1, results.at(j), ("="));
			if(results1.size()==2)
				cookieattrs[results1.at(0)] = results1.at(1);
			else
				cookieattrs[results1.at(0)] = "true";
		}
		addHeaderValue(key, value);
	}
	else if(key=="content-type")
	{
		StringUtil::trim(value);
		string tempi(value);
		size_t s = tempi.find("boundary");
		if(s!=string::npos)
		{
			addHeaderValue(key, tempi.substr(0,s));
			tempi = tempi.substr(s);
			strVec results;
			StringUtil::split(results, tempi, ("="));
			if(results.size()==2)
			{
				string bound = "--" + results.at(1).substr(0,results.at(1).length());
				this->setContent_boundary(bound);
			}
		}
		else
		{
			addHeaderValue(key, value);
		}
	}
	else if(key=="content" && value!="")
	{
		content.append(value);
	}
	else if(key=="method")
	{
		this->setMethod(value);
	}
	else if(key=="httpversion")
	{
		this->httpVersion = value;
		string versionStr = StringUtil::replaceFirstCopy(StringUtil::toLowerCopy(value), "http/", "");
		StringUtil::trim(versionStr);
		float version = -1;
		try {
			version = CastUtil::lexical_cast<float>(versionStr);
			this->httpVers = version;
		} catch(...) {
		}
	}
	else if(key=="getarguments")
	{
		strVec params;
		map<string ,int> indices;
		value = CryptoHandler::urlDecode(value);
		StringUtil::split(params, value, ("&"));
		for(unsigned j=0;j<params.size();j++)
		{
			strVec param;
			StringUtil::split(param, params.at(j), ("="));
			if(param.size()==2)
			{
				string attN = param.at(0);
				StringUtil::replaceFirst(attN,"\r","");
				StringUtil::replaceFirst(attN,"\t","");
				StringUtil::replaceFirst(attN," ","");
				if(attN.find("[")!=string::npos && attN.find("]")!=string::npos)
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
							  + CastUtil::lexical_cast<string>(indices[attN])
							  + "]"] = CryptoHandler::urlDecode(param.at(1));
					cout << ("creating array from similar params" + attN.substr(0, attN.find("[")+1)
									  + CastUtil::lexical_cast<string>(indices[attN])
									  + "]"
									  + CryptoHandler::urlDecode(param.at(1))) << endl;
				}
				else
				{
					this->setQueryParam(attN,param.at(1));
				}
				reqorderinf[reqorderinf.size()+1] = attN;
			}
		}
	}
	else if(key.find("url")!=string::npos)
	{
		strVec memp;
		this->setActUrl(value);
		StringUtil::split(memp, value, ("/"));
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
	else if(key=="httpline")
	{
		strVec vemp;
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
			string versionStr = StringUtil::replaceFirstCopy(StringUtil::toLowerCopy(vemp.at(2)), "http/", "");
			StringUtil::trim(versionStr);
			float version = -1;
			try {
				version = CastUtil::lexical_cast<float>(versionStr);
				this->httpVers = version;
			} catch(...) {
				status = HTTPResponseStatus::HttpVersionNotSupported;
				return;
			}
			if(version<1.0 && version>1.1)
			{
				status = HTTPResponseStatus::HttpVersionNotSupported;
				return;
			}
			if(version<1.1 && StringUtil::toLowerCopy(vemp.at(0))=="options")
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
		this->setMethod(StringUtil::toUpperCopy(vemp.at(0)));
		vemp.erase(vemp.begin());
		StringUtil::replaceFirst(vemp.at(1),"\r","");
		this->httpVersion = vemp.at(1);
		StringUtil::replaceFirst(vemp.at(0)," ","");
		if(vemp.at(0).find("?")!=string ::npos)
		{
			string valu(vemp.at(0));
			vemp[0] = valu.substr(0,vemp.at(0).find("?"));
			valu = valu.substr(valu.find("?")+1);
			buildRequest("getarguments", valu);
		}
		buildRequest("url", vemp.at(0));
	}
	else
	{
		StringUtil::trim(value);
		addHeaderValue(key, value);
	}
}

string HttpRequest::toString()
{
	string ret;
	map<string,string>::iterator it;
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
	string vals;
	cout << "logging request data " << endl;
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			vals+= ("\nKey: "+iter->first + " Value: "+iter->second);
		}
	}
	cout << "logging multipart data " << endl;
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();++iter)
		{
			MultipartContent dat = iter->second;
			vals+= ("\nKey: "+iter->first);
			cout << dat.getHeaders().size() << endl;
			for(it=dat.headers.begin();it!=dat.headers.end();++it)
			{
				vals += "\n\t" + it->first + ": " + it->second;
			}
			vals+= ("\n\tFileName: "+dat.fileName);
			vals+= ("\n\tTempFileName: "+dat.tempFileName);
		}
	}
	ret += "\nRequest Parameters "+vals;//CastUtil::lexical_cast<string>(this->getRequestParams().size());
	return ret;
}


HttpRequest::~HttpRequest()
{
//	/delete this;
}
string HttpRequest::getMethod() const
{
	return method;
}

void HttpRequest::setMethod(const string& method)
{
	this->method = method;
}

HttpSession* HttpRequest::getSession()
{
	return &(this->session);
}

void HttpRequest::setUrl(string url)
{
	StringUtil::replaceFirst(url,"//","/");
	this->url = url;
	this->ext = getFileExtension(url);
}

string HttpRequest::getUrl() const
{
	return this->url;
}

float HttpRequest::getHttpVers() const
{
	return this->httpVers;
}

string HttpRequest::getHttpVersion() const
{
	return this->httpVersion;
}

string HttpRequest::getContent_boundary() const
{
	return content_boundary;
}

void HttpRequest::setContent_boundary(const string& content_boundary)
{
	this->content_boundary = content_boundary;
}

string HttpRequest::getContent() const
{
	return content;
}

void HttpRequest::setContent(const string& content)
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

string HttpRequest::getRequestParam(const string& key)
{
	if(this->requestParams.find(key)!=this->requestParams.end())
		return this->requestParams[key];
	else if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].getContent();
	else
		return "";
}

MultipartContent HttpRequest::getMultipartContent(const string& key)
{
	MultipartContent cont;
	if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key];
	else
		return cont;
}


string HttpRequest::getRequestParamType(const string& key)
{
	if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].getHeader("Content-Type");
	else
		return "";
}

void HttpRequest::setRequestParam(const string& key, const string& value)
{
	this->requestParams[key] = value;
}

string HttpRequest::getCntxt_root() const
{
	return cntxt_root;
}

void HttpRequest::setCntxt_root(const string& cntxt_root)
{
	this->cntxt_root = cntxt_root;
	this->cntxt_home = webpath + "/" + cntxt_name;
}

string HttpRequest::getDefaultLocale() const
{
	if(this->localeInfo.size()>0)
		return this->localeInfo.at(0);
	else
		return "en";
}

string HttpRequest::getCntxt_name() const
{
	return cntxt_name;
}

void HttpRequest::setCntxt_name(const string& cntxt_name)
{
	this->cntxt_name = cntxt_name;
}

string HttpRequest::getFile() const
{
	return file;
}

void HttpRequest::setFile(const string& file)
{
	//cout << "file is " << this->file << endl;
	if(this->file!="" && this->url.find(this->file)!=string::npos
			&& this->url.find("/")!=string::npos)
	{
		//int fst = this->url.find_last_of(this->file) - this->file.length() + 1;
		this->url = this->url.substr(0, this->url.find_last_of("/")+1) +  file;
	}
	else
	{
		this->url +=  "/" + file;
	}
	this->file = file;
	this->ext = getFileExtension(file);
}

string HttpRequest::getActUrl() const
{
	return actUrl;
}

void HttpRequest::setActUrl(const string& actUrl)
{
	string au = actUrl;
	if(au.at(0)=='/') {
		au = au.substr(1);
	}
	StringUtil::split(actUrlParts, au, ("/"));
	if(actUrlParts.size()>1 || (actUrlParts.size()==1 && actUrlParts[0].find(".")==string::npos)) {
		cntxt_name = actUrlParts[0];
	}
	if(actUrlParts.size()>0 && actUrlParts[actUrlParts.size()-1].find(".")!=string::npos) {
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
		actUrl.insert(0, "/"+cntxt_name);
	}
}

const vector<string>& HttpRequest::getActUrlParts() const
{
	return actUrlParts;
}

map<string,string> HttpRequest::getAuthinfo() const
{
	return authinfo;
}

void HttpRequest::setAuthinfo(const map<string,string>& authinfo)
{
	this->authinfo = authinfo;
}

#ifdef INC_SCRH
string HttpRequest::toPHPVariablesString(const string& def)
{
	string ret;
	ret = "<?php";
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		string header = StringUtil::toUpperCopy(it->first);
		StringUtil::replaceAll(header, "-", "_");
		ret += "$_SERVER['HTTP_"+header+"'] = '"+it->second + "';\n";
	}
	ret += "$HTTP_RAW_POST_DATA = '"+this->getContent();
	ret += "';\n$_SERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "';\n$_SERVER['REQUEST_METHOD'] = '"+this->getMethod();
	string requri = this->getActUrl();
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				ret += "\nif(!isset($_FILES['"+iter->first.substr(0, iter->first.find("["))+"']))\n{\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']=array()\n}\n";
				ret += "\nif(!isset($_FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"))\n"
						+ "{\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"=array();\n}\n";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "';";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "';";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length()) + ";";
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
				ret += "\n$_FILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length()) + ";";
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

string HttpRequest::toPerlVariablesString()
{
	string ret;
	ret += "$_SERVER = {};";
	ret += "\n$_SERVER_EX = {};";
	ret += "\n$_GET = {};";
	ret += "\n$_POST = {};";
	ret += "\n$_FILES = {};\n";
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		string header = StringUtil::toUpperCopy(it->first);
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first;
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first;
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first;
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
						key.substr(key.find("{")) + "{'size'} = "+ CastUtil::lexical_cast<string>(dat.getContent().length()) + ";";
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
				ret += "\n$_FILES{'"+iter->first+"'}{'size'} = "+ CastUtil::lexical_cast<string>(dat.getContent().length()) + ";";
				ret += "\n$_FILES{'"+iter->first+"'}{'tmp_name'} = '"+ dat.getTempFileName() + "';";
				ret += "\n$_FILES{'"+iter->first+"'}{'error'} = 0;";
			}
		}
	}
	ret += "\n\n";
	return ret;
}

string HttpRequest::toRubyVariablesString()
{
	string ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		string header = StringUtil::toUpperCopy(it->first);
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(0, iter->first.find("["));
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(0, iter->first.find("["));
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(iter->first.find("["));
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
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length());
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
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\nrequire ('"+this->getUrl()+"')\n";
	return ret;
}

string HttpRequest::toPythonVariablesString()
{
	string ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		string header = StringUtil::toUpperCopy(it->first);
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(0, iter->first.find("["));
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(0, iter->first.find("["));
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(iter->first.find("["));
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
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length());
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
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\n\n";
	return ret;
}

string HttpRequest::toLuaVariablesString()
{
	string ret;
	ret += "SERVER = {}";
	ret += "\nSERVER_EX = {}";
	ret += "\nGET = {}";
	ret += "\nPOST = {}";
	ret += "\nFILES = {}\n";
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		string header = StringUtil::toUpperCopy(it->first);
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(0, iter->first.find("["));
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(0, iter->first.find("["));
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first.substr(iter->first.find("["));
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
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length());
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
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length());
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.getTempFileName() + "'";
				ret += "\nFILES['"+iter->first+"']['error'] = 0";
			}
		}
	}
	ret += "\ndofile(\""+this->getUrl()+"\")\n";
	return ret;
}

string HttpRequest::toNodejsVariablesString()
{
	string ret;
	ret += "SERVER = {};";
	ret += "\nSERVER_EX = {};";
	ret += "\nGET = {};";
	ret += "\nPOST = {};";
	ret += "\nFILES = {};\n";
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		string header = StringUtil::toUpperCopy(it->first);
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
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
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				ret += "\nif((FILES['"+iter->first.substr(0, iter->first.find("["))+"'])==undefined)\n{\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']={};\n}\n";
				ret += "\nif((FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+")==undefined)\n"
						+ "{\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"={};\n}\n";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.getFileName() + "';";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.getHeader("Content-Type") + "';";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length()) + ";";
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
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.getContent().length()) + ";";
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
	map<string,string>::iterator it;
	map<string,string> reqparams = this->getRequestParams();
	map<string,string> qryparams = this->getQueryParams();
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

void HttpRequest::setQueryParam(const string& name, const string& value)
{
	this->queryParams[name] = value;
}

string HttpRequest::getQueryParam(const string& key)
{
	if(this->queryParams.find(key)!=this->queryParams.end())
		return this->queryParams[key];
	return "";
}

bool HttpRequest::hasCookie()
{
	string ffeadid;
	if(cookieattrs.find("FFEADID")!=cookieattrs.end())
		ffeadid = cookieattrs["FFEADID"];
	StringUtil::trim(ffeadid);
	return this->cookie && ffeadid!="";
}

map<int,string> HttpRequest::getAuthOrderinfo() const
{
	return authorderinf;
}

map<int,string> HttpRequest::getReqOrderinfo() const
{
	return reqorderinf;
}

map<string,string> HttpRequest::getCookieInfo() const
{
	return cookieattrs;
}

string HttpRequest::getAuthOrderinfoAttribute(const int& key)
{
	if(authorderinf.find(key)!=authorderinf.end())
		return authorderinf[key];
	else return "";
}

string HttpRequest::getReqOrderinfoAttribute(const int& key)
{
	if(reqorderinf.find(key)!=reqorderinf.end())
		return reqorderinf[key];
	else return "";
}

string HttpRequest::getCookieInfoAttribute(const string& key)
{
	if(cookieattrs.find(key)!=cookieattrs.end())
		return cookieattrs[key];
	else return "";
}

string HttpRequest::getHeader(string key)
{
	StringUtil::toLower(key);
	if(this->headers.find(key)!=this->headers.end())
		return this->headers[key];
	return "";
}

bool HttpRequest::hasHeader(string key)
{
	StringUtil::toLower(key);
	if(this->headers.find(key)!=this->headers.end())
		return true;
	return false;
}

map<string,string> HttpRequest::getHeaders()
{
	return headers;
}

int HttpRequest::getCORSRequestType()
{
	if(getHeader(Origin)=="" || getHeader(Origin)==("http://"+getHeader(Host)) || getHeader(Origin)==("https://"+getHeader(Host)))
	{
		//Not a CORS request
		cout << ("Not a CORS request") << endl;
		return OTHER;
	}
	else if(getHeader(AccessControlRequestMethod)!="" && StringUtil::toLowerCopy(method)=="options")
	{
		//CORS Preflight request
		cout << ("CORS Preflight request") << endl;
		return PREFLIGHT;
	}
	else
	{
		//Actual CORS request
		cout << ("Actual CORS request") << endl;
		return CORS;
	}
}

void HttpRequest::addHeaderValue(string header, const string& value)
{
	StringUtil::toLower(header);
	if(header!="")
	{
		if(VALID_REQUEST_HEADERS.find(","+header+",")!=string::npos)
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
			if(!RegexUtil::matches(header, "^[a-zA-Z]+[-|a-zA-Z0-9]*"))
			{
				cout << ("Invalid Header string " + header) << endl;
				return;
			}
			/*vector<string> matres = RegexUtil::search(header, "^[a-zA-Z]+[-|a-zA-Z]+[a-zA-Z]*[a-zA-Z]$");
			if(matres.size()==0)
			{
				matres = RegexUtil::search(header, "^[a-zA-Z]+[a-zA-Z0-9]*");
				if(matres.size()==0)
				{
					cout << ("Invalid Header string " + header) << endl;
					return;
				}
			}*/
			if(headers.find(header)!=headers.end()) {
				headers[header] += "," + value;
			} else {
				headers[header] = value;
			}
		}
	}
}

vector<string> HttpRequest::parseHeaderValue(string headerValue)
{
	RegexUtil::replace(headerValue, "\\s*,\\s*|\\s+", ",");
	return StringUtil::splitAndReturn<vector<string> >(headerValue, ",");
}

bool HttpRequest::isValidHttpMethod(const string& method)
{
	string lmeth = StringUtil::toLowerCopy(method);
	return lmeth=="get" || lmeth=="post" || lmeth=="options" || lmeth=="head" || lmeth=="put" || lmeth=="delete" || lmeth=="trace";
}

bool HttpRequest::isValidHttpMethod()
{
	return isValidHttpMethod(method);
}

bool HttpRequest::isAgentAcceptsCE()
{
	string lmeth = StringUtil::toLowerCopy(getHeader(AcceptEncoding));
	return lmeth.find("gzip")!=string::npos || lmeth.find("deflate")!=string::npos;
}

bool HttpRequest::isHeaderValue(string header, const string& value, const bool& ignoreCase)
{
	StringUtil::toLower(header);
	return header!="" && headers.find(header)!=headers.end()
			&& (headers[header]==value ||
					(ignoreCase && StringUtil::toLowerCopy(headers[header])==StringUtil::toLowerCopy(value)));
}

bool HttpRequest::hasHeaderValuePart(string header, string valuePart, const bool& ignoreCase)
{
	StringUtil::toLower(header);
	if(header!="" && headers.find(header)!=headers.end())
	{
		string hvalue = headers[header];
		if(hvalue==valuePart || (ignoreCase && StringUtil::toLowerCopy(hvalue)==StringUtil::toLowerCopy(valuePart)))
		{
			return true;
		}
		else if(ignoreCase)
		{
			StringUtil::toLower(hvalue);
			StringUtil::toLower(valuePart);
			vector<string> hvec = StringUtil::splitAndReturn<vector<string> >(hvalue, ",");
			for(int yy=0;yy<(int)hvec.size();yy++)
			{
				string vp = StringUtil::toLowerCopy(hvec.at(yy));
				StringUtil::trim(vp);
				if(vp==valuePart) {
					return true;
				}
			}
		}
		else
		{
			vector<string> hvec = StringUtil::splitAndReturn<vector<string> >(hvalue, ",");
			for(int yy=0;yy<(int)hvec.size();yy++)
			{
				string vp = hvec.at(yy);
				StringUtil::trim(vp);
				if(vp==valuePart) {
					return true;
				}
			}
		}
	}
	return false;
}

vector<vector<int> > HttpRequest::getRanges(vector<string> &rangesVec)
{
	vector<vector<int> > rangeValuesLst;
	string ranges = getHeader(Range);
	if(ranges.find("bytes=")!=0)
	{
		return rangeValuesLst;
	}
	StringUtil::replaceFirst(ranges, "bytes=", "");
	StringUtil::split(rangesVec, ranges, (","));
	for (int var = 0; var <(int)rangesVec.size(); ++var) {
		string range = rangesVec.at(var);
		StringUtil::trim(range);
		vector<string> rangeVals;
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
						throw 1;
					vector<int> values;
					values.push_back(start);
					values.push_back(end - start);
					rangeValuesLst.push_back(values);
				} catch(...) {
					//error
				}
			}
			else if(rangeVals.at(0)!="")
			{
				try {
					int start = CastUtil::lexical_cast<int>(rangeVals.at(0));
					if(start<0)
						throw 1;
					vector<int> values;
					values.push_back(start);
					values.push_back(-1);
					rangeValuesLst.push_back(values);
				} catch(...) {
					//error
				}
			}
			else if(rangeVals.at(1)!="")
			{
				try {
					int end = CastUtil::lexical_cast<int>(rangeVals.at(1));
					if(end<0)
						throw 1;
					vector<int> values;
					values.push_back(-1);
					values.push_back(end);
					rangeValuesLst.push_back(values);
				} catch(...) {
					//error
				}
			}
		}
	}
	return rangeValuesLst;
}

void HttpRequest::setContent_tfile(const string& tfile)
{
	content_tfile = tfile;
}

string HttpRequest::getContent_tfile()
{
	return content_tfile;
}

void HttpRequest::addMultipartFormContent(const string& key, const MultipartContent& content)
{
	requestParamsF[key] = content;
}

void HttpRequest::addContent(const MultipartContent& content)
{
	contentList.push_back(content);
}

bool HttpRequest::isNonBinary(const string& mimeType)
{
	string contType = StringUtil::toLowerCopy(mimeType);
	return (contType.find("text")!=string::npos || contType.find("css")!=string::npos
			|| contType.find("x-javascript")!=string::npos || contType.find("json")!=string::npos
			|| contType.find("xml")!=string::npos || contType.find("html")!=string::npos);
}

string HttpRequest::getParamValue(const string& key)
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

void HttpRequest::setSessionID(const string& sessionID)
{
	this->sessionID = sessionID;
}

string HttpRequest::getSessionID() const
{
	return sessionID;
}

vector<MultipartContent> HttpRequest::getMultiPartFileList(const string& name)
{
	vector<MultipartContent> filevec;
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

string HttpRequest::getPassword() const {
	return password;
}

string HttpRequest::getUserName() const {
	return userName;
}

string HttpRequest::getAuthMethod() const {
	return authMethod;
}

string HttpRequest::toPluginString() {
	string text = (this->cntxt_name + "\n");
	text += (this->host + "\n");
	text += (this->url + "\n");
	text += (this->file + "\n");
	text += (CastUtil::lexical_cast<string>(this->httpVersion) + "\n");
	text += (this->method + "\n");
	text += (this->getDefaultLocale() + "\n");
	text += (this->sessionID + "\n");
	text += (this->actUrl + "\n");

	text += (CastUtil::lexical_cast<string>(this->content.length()) + "\n");
	text += (this->content);

	text += (CastUtil::lexical_cast<string>(this->preamble.length()) + "\n");
	text += (this->preamble);

	text += (CastUtil::lexical_cast<string>(this->epilogue.length()) + "\n");
	text += (this->epilogue);

	text += (CastUtil::lexical_cast<string>(this->requestParams.size()) + "\n");
	RMap::iterator it;
	for(it=this->requestParams.begin();it!=this->requestParams.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::lexical_cast<string>(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::lexical_cast<string>(this->queryParams.size()) + "\n");
	for(it=this->queryParams.begin();it!=this->queryParams.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::lexical_cast<string>(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::lexical_cast<string>(this->headers.size()) + "\n");
	for(it=this->headers.begin();it!=this->headers.end();++it)
	{
		text += it->first + "\n";
		text += CastUtil::lexical_cast<string>(it->second.length()) + "\n";
		text += it->second;
	}

	text += (CastUtil::lexical_cast<string>(this->requestParamsF.size()) + "\n");
	FMap::iterator fit;
	for(fit=this->requestParamsF.begin();fit!=this->requestParamsF.end();++fit)
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

void HttpRequest::setHttp2Headers(map<string,string> headers)
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
	map<string,string>::iterator it;
	for(it=headers.begin();it!=headers.end();++it)
	{
		buildRequest(it->first, it->second);
	}
}

string HttpRequest::getFileExtension(const string& file)
{
	if(file.find_last_of(".")!=string::npos)return file.substr(file.find_last_of("."));
	return file;
}

string HttpRequest::getContextHome()
{
	return cntxt_home;
}

void HttpRequest::setContextHome(const string& home)
{
	this->cntxt_home = home;
}

string HttpRequest::getExt() const
{
	return ext;
}
