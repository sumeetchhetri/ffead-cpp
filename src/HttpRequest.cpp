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
 * HttpRequest.cpp
 *
 *  Created on: Aug 10, 2009
 *      Author: sumeet
 */

#include "HttpRequest.h"
using namespace std;

void HttpRequest::unbase64(string str)
{
	logger << "before " << str << endl;
	authinfo["Method"] = (str.substr(0,str.find(" ")));
	str = str.substr(str.find(" ")+1);
	logger << "bafter " << str << endl;

	/*unsigned char *input = (unsigned char *)str.c_str();
	int length = str.length();

	BIO *b64, *bmem;

	char *buffer = (char *)malloc(length);
	memset(buffer, 0, length);

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new_mem_buf(input, length);
	bmem = BIO_push(b64, bmem);

	BIO_read(bmem, buffer, length);

	BIO_free_all(bmem);

	string temp(buffer);*/
	unsigned char *input = (unsigned char *)str.c_str();
	int length = str.length();
	string temp = CryptoHandler::base64decode(input,length);
	logger << "after " << temp << endl;

	authinfo["Username"] = (temp.substr(0,temp.find(":")));
	temp = temp.substr(temp.find(":")+1);
	authinfo["Password"] = (temp);
}

void HttpRequest::getOauthParams(string str)
{
	authinfo["Method"] = str.substr(0,str.find(" "));
	str = str.substr(str.find(" ")+1);
	logger << str << endl;

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
		//logger << tempvv.at(0) << " = " << temr << endl;
	}
}

HttpRequest::HttpRequest()
{
	logger = Logger::getLogger("HttpRequest");
}

HttpRequest::HttpRequest(strVec vec,string path)
{
	if(vec.size()!=0){
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
				if(temp.at(0)=="Host")
					this->setHost(temp.at(1));
				else if(temp.at(0)=="User-Agent" || temp.at(0)=="User-agent")
					this->setUser_agent(temp.at(1));
				else if(temp.at(0)=="Accept")
					this->setAccept(temp.at(1));
				else if(temp.at(0)=="Authorization")
				{
					logger << "found auth" <<endl;
					if(temp.at(1).find("oauth_")!=string::npos)
					{
						this->getOauthParams(temp.at(1));
					}
					else
						this->unbase64(temp.at(1));
				}
				else if(temp.at(0)=="Accept-Language" || temp.at(0)=="Accept-language")
				{
					strVec lemp;
					this->setAccept_lang(temp.at(1));
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
					//logger << temp.at(1) << flush;
				}
				else if(temp.at(0)=="Accept-Encoding" || temp.at(0)=="Accept-encoding")
					this->setAccept_encod(temp.at(1));
				else if(temp.at(0)=="Accept-Charset" || temp.at(0)=="Accept-charset")
					this->setAccept_lang(temp.at(1));
				else if(temp.at(0)=="Keep-Alive" || temp.at(0)=="Keep-alive")
					this->setKeep_alive(temp.at(1));
				else if(temp.at(0)=="Connection")
					this->setConnection(temp.at(1));
				else if(temp.at(0)=="Cache-Control" || temp.at(0)=="Cache-control")
					this->setCache_ctrl(temp.at(1));
				else if(temp.at(0)=="Content-Type" || temp.at(0)=="Content-type")
				{
					this->setContent_type(temp.at(1));
					string tempi(temp.at(1));
					size_t s = tempi.find("boundary");
					if(s!=string::npos)
					{
						this->setContent_type(tempi.substr(0,s));
						tempi = tempi.substr(s);
						strVec results;
						StringUtil::split(results, tempi, ("="));
						if(results.size()==2)
						{
							string bound = "--" + results.at(1).substr(0,results.at(1).length());
							this->setContent_boundary(bound);
						}
					}
				}
				else if(temp.at(0)=="Cookie")
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
				}
				else if(temp.at(0)=="Content-Length" || temp.at(0)=="Content-length")
					this->setContent_len(temp.at(1));
				else if(temp.at(0)=="Referer")
					this->setReferer(temp.at(1));
				else if(temp.at(0)=="Pragma")
					this->setPragma(temp.at(1));
				//logger << temp.at(0) <<  "---" << temp.at(1) << flush;
			}
			else
			{
				string tem = temp.at(0);
				if(!contStarts && tem.find("GET")!=string::npos)
				{
					StringUtil::replaceFirst(tem,"GET ","");
					this->setMethod("GET");
					StringUtil::split(vemp, tem, (" "));
					if(vemp.size()==2)
					{
						StringUtil::replaceFirst(vemp.at(1),"\r","");
						this->setHttpVersion(vemp.at(1));
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
										this->requestParams[attN.substr(0, attN.find("[")+1)
												  + CastUtil::lexical_cast<string>(indices[attN])
												  + "]"] = CryptoHandler::urlDecode(param.at(1));
										logger << "creating array from similar params" << attN.substr(0, attN.find("[")+1)
														  + CastUtil::lexical_cast<string>(indices[attN])
														  + "]"
														  << CryptoHandler::urlDecode(param.at(1)) << endl;
									}
									else
										this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
									//logger << att << " = " << param.at(1) << endl;
									reqorderinf[reqorderinf.size()+1] = att;
								}
							}
						}
						this->setActUrl(vemp.at(0));
						memp = StringUtil::split(vemp.at(0), ("/"));
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
				}
				else if(!contStarts && tem.find("HEAD")!=string::npos)
				{
					StringUtil::replaceFirst(tem,"HEAD ","");
					this->setMethod("HEAD");
					StringUtil::split(vemp, tem, (" "));
					if(vemp.size()==2)
					{
						StringUtil::replaceFirst(vemp.at(1),"\r","");
						this->setHttpVersion(vemp.at(1));
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
										this->requestParams[attN.substr(0, attN.find("[")+1)
												  + CastUtil::lexical_cast<string>(indices[attN])
												  + "]"] = CryptoHandler::urlDecode(param.at(1));
										logger << "creating array from similar params" << attN.substr(0, attN.find("[")+1)
														  + CastUtil::lexical_cast<string>(indices[attN])
														  + "]"
														  << CryptoHandler::urlDecode(param.at(1)) << endl;
									}
									else
										this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
									//logger << att << " = " << param.at(1) << endl;
									reqorderinf[reqorderinf.size()+1] = att;
								}
							}
						}
						this->setActUrl(vemp.at(0));
						memp = StringUtil::split(vemp.at(0), ("/"));
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
				}
				else if(!contStarts && tem.find("TRACE")!=string::npos)
				{
					StringUtil::replaceFirst(tem,"TRACE ","");
					this->setMethod("TRACE");
					StringUtil::split(vemp, tem, (" "));
					if(vemp.size()==2)
					{
						StringUtil::replaceFirst(vemp.at(1),"\r","");
						this->setHttpVersion(vemp.at(1));
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
										this->requestParams[attN.substr(0, attN.find("[")+1)
												  + CastUtil::lexical_cast<string>(indices[attN])
												  + "]"] = CryptoHandler::urlDecode(param.at(1));
										logger << "creating array from similar params" << attN.substr(0, attN.find("[")+1)
														  + CastUtil::lexical_cast<string>(indices[attN])
														  + "]"
														  << CryptoHandler::urlDecode(param.at(1)) << endl;
									}
									else
										this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
									//logger << att << " = " << param.at(1) << endl;
									reqorderinf[reqorderinf.size()+1] = att;
								}
							}
						}
						this->setActUrl(vemp.at(0));
						memp = StringUtil::split(vemp.at(0), ("/"));
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
				}
				else if(!contStarts && tem.find("OPTIONS")!=string::npos)
				{
					StringUtil::replaceFirst(tem,"OPTIONS ","");
					this->setMethod("OPTIONS");
					StringUtil::split(vemp, tem, (" "));
					if(vemp.size()==2)
					{
						StringUtil::replaceFirst(vemp.at(1),"\r","");
						this->setHttpVersion(vemp.at(1));
						StringUtil::replaceFirst(vemp.at(0)," ","");
						this->setActUrl(vemp.at(0));
						memp = StringUtil::split(vemp.at(0), ("/"));
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
				}
				else if(!contStarts && tem.find("DELETE")!=string::npos)
				{
					StringUtil::replaceFirst(tem,"DELETE ","");
					this->setMethod("DELETE");
					StringUtil::split(vemp, tem, (" "));
					if(vemp.size()==2)
					{
						StringUtil::replaceFirst(vemp.at(1),"\r","");
						this->setHttpVersion(vemp.at(1));
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
										this->requestParams[attN.substr(0, attN.find("[")+1)
												  + CastUtil::lexical_cast<string>(indices[attN])
												  + "]"] = CryptoHandler::urlDecode(param.at(1));
										logger << "creating array from similar params" << attN.substr(0, attN.find("[")+1)
														  + CastUtil::lexical_cast<string>(indices[attN])
														  + "]"
														  << CryptoHandler::urlDecode(param.at(1)) << endl;
									}
									else
										this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
									reqorderinf[reqorderinf.size()+1] = att;
								}
							}
						}
						this->setActUrl(vemp.at(0));
						memp = StringUtil::split(vemp.at(0), ("/"));
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
				}
				else if(!contStarts && tem.find("PUT")!=string::npos)
				{
					StringUtil::replaceFirst(tem,"PUT ","");
					this->setMethod("PUT");
					StringUtil::split(vemp, tem, (" "));
					if(vemp.size()==2)
					{
						StringUtil::replaceFirst(vemp.at(1),"\r","");
						this->setHttpVersion(vemp.at(1));
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
										this->requestParams[attN.substr(0, attN.find("[")+1)
												  + CastUtil::lexical_cast<string>(indices[attN])
												  + "]"] = CryptoHandler::urlDecode(param.at(1));
										logger << "creating array from similar params" << attN.substr(0, attN.find("[")+1)
														  + CastUtil::lexical_cast<string>(indices[attN])
														  + "]"
														  << CryptoHandler::urlDecode(param.at(1)) << endl;
									}
									else
										this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
									reqorderinf[reqorderinf.size()+1] = att;
								}
							}
						}
						this->setActUrl(vemp.at(0));
						memp = StringUtil::split(vemp.at(0), ("/"));
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
				}
				else if(!contStarts && tem.find("POST")!=string::npos)
				{
					StringUtil::replaceFirst(tem,"POST ","");
					this->setMethod("POST");
					StringUtil::split(vemp, tem, (" "));
					if(vemp.size()==2)
					{
						//this->setUrl(vemp.at(0));
						//string pat(vemp.at(0));
						StringUtil::replaceFirst(vemp.at(1),"\r","");
						this->setHttpVersion(vemp.at(1));
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
										this->requestParams[attN.substr(0, attN.find("[")+1)
												  + CastUtil::lexical_cast<string>(indices[attN])
												  + "]"] = CryptoHandler::urlDecode(param.at(1));
										logger << "creating array from similar params" << attN.substr(0, attN.find("[")+1)
														  + CastUtil::lexical_cast<string>(indices[attN])
														  + "]"
														  << CryptoHandler::urlDecode(param.at(1)) << endl;
									}
									else
										this->setQueryParam(attN,CryptoHandler::urlDecode(param.at(1)));
									reqorderinf[reqorderinf.size()+1] = att;
								}
							}
						}
						this->setActUrl(vemp.at(0));
						memp = StringUtil::split(vemp.at(0), ("/"));
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
				}
				else if(contStarts)
				{
					/*string temp;
					if(vec.at(i).find("<?")!=string::npos && vec.at(i).find("?>")!=string::npos)
					{
						temp = vec.at(i).substr(vec.at(i).find("?>")+2);
						conten.append(temp);
					}
					else*/
					conten.append(vec.at(i));
					if(i!=vec.size()-1)
						conten.append("\n");
					//this->content=con;
				}
			}
		}
		this->setContent(conten);
		/*if(this->getContent()!="")
		{
			strVec params;
			string content = this->getContent();
			//logger << content << flush;
			StringUtil::split(params, content, ("&"));
			//logger << "\n\n\nsize: " << params.size() << flush;
			for(unsigned j=0;j<params.size();j++)
			{
				strVec param;
				//logger << params.at(j) << flush;
				StringUtil::split(param, params.at(j), ("="));
				//logger << param.size() << flush;
				if(param.size()==2)
				{
					string att = param.at(0);
					StringUtil::replaceFirst(att,"\r","");
					StringUtil::replaceFirst(att,"\t","");
					StringUtil::replaceFirst(att," ","");
					//logger << "attribute:  " << param.at(0) << "\n"<< flush;
					//logger << "value: " << param.at(1) << "\n" << flush;
					this->setRequestParam(att,param.at(1));
				}
			}
		}*/
		if(this->getContent()!="")
		{
			logger << this->getContent() << flush;
			if(this->getContent_type().find("application/x-www-form-urlencoded")!=string::npos)
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
							logger << "creating array from similar params" << attN.substr(0, attN.find("[")+1)
											  << CastUtil::lexical_cast<string>(indices[attN])
											  << "]"
											  << CryptoHandler::urlDecode(param.at(1)) << endl;
						}
						else
							this->setRequestParam(attN,CryptoHandler::urlDecode(param.at(1)));
						reqorderinf[reqorderinf.size()+1] = att;
					}
				}
			}
			else if(this->getContent()!="" && this->getContent_boundary()!="")
			{
				string delb = "\r"+this->getContent_boundary();
				string delend = "\r"+this->getContent_boundary()+"--";
				size_t stb = this->getContent().find_first_of(delb)+delb.length()+1;
				size_t enb = this->getContent().find_last_not_of(delend);
				string param_conts = this->getContent().substr(stb);
				StringUtil::replaceFirst(param_conts,delend,"");
				param_conts = param_conts.substr(0,param_conts.length()-1);
				strVec parameters;
				StringUtil::split(parameters, param_conts, (delb));
				//logger << "Boundary: " << this->getContent_boundary() << flush;
				//logger << "\nLength: " << this->getContent().length() << flush;
				//logger << "\nStart End: " << stb << " " << enb << "\n" << flush;
				//logger << "\nContent: " << param_conts << "\n" << flush;
				map<string ,int> indices;
				map<string,string>::iterator it;
				for(unsigned j=0;j<parameters.size();j++)
				{
					if(parameters.at(j)=="" || parameters.at(j).find_first_not_of(" ")==string::npos
							|| parameters.at(j).find_first_not_of("\r")==string::npos)
						continue;
					FormData datf;
					string parm = parameters.at(j);
					//logger << parm << "\nparm" << flush;
					size_t dis = parm.find("Content-Disposition: ");
					if(dis==string::npos)
						dis = parm.find("Content-disposition: ");
					string cont_disp,cont_type;
					if(dis!=string::npos)
					{
						size_t dist = parm.find("Content-Type: ");
						if(dist==string::npos)
							dist = parm.find("Content-yype: ");
						size_t dise;
						if(dist==string::npos)
						{
							dist = parm.find("\r\r");
							dise = dist + 2;
							//logger << "\ndist = npos" << flush;
						}
						else
						{
							//parm = parm.substr(dist+14);
							cont_type = parm.substr(dist+14,parm.find("\r\r")-(dist+14));
							dise = parm.find("\r\r") + 2;
							//logger << "\nctype = " << cont_type << flush;
							//dist = dist-12;
						}
						cont_disp = parm.substr(dis+21,dist-(dis+21));
						StringUtil::replaceFirst(cont_disp,"\r","");
						//logger << "\ncdisp = " << cont_disp << flush;
						//logger << "\ndise = " << dise << flush;
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
							//logger << "\nparmdef = " << parmdef.at(k) << flush;
							//logger << "\nst en = " << stpd  << " " << enpd << flush;
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
									datf.type = cont_type;
									datf.value = parm;
								}
								else if(proplr.at(0)=="filename" && proplr.at(1)!="\"\"")
								{
									string fna = proplr.at(1);
									fna = fna.substr(fna.find_first_not_of("\""),fna.find_last_not_of("\"")-fna.find_first_not_of("\"")+1);
									fna = CryptoHandler::urlDecode(fna);
									datf.fileName = fna;
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
							logger << "creating array from similar params" << attN.substr(0, attN.find("[")+1)
											  << CastUtil::lexical_cast<string>(indices[attN])
											  << "]"
											  << datf.fileName << endl;
						}
						this->setRequestParamF(attN,datf);
						if(datf.fileName!="")
						{
							string tmpfile = this->getCntxt_root() + "/temp/"+ this->getContent_boundary() +datf.value;
							ofstream os;
							os.open(tmpfile.c_str());
							os.write(datf.value.c_str(), datf.value.length());
							os.close();
							datf.tmpFileName = tmpfile;
							datf.length = datf.value.length();
						}
						string hr = (key + " " + datf.type + " "+ datf.fileName+" "+ datf.value);
						//logger << hr << flush;
					}
				}
			}

		}
	}
	//logger << this->toString() << flush;
}

string HttpRequest::buildRequest(const char *keyc,const char *valuec)
{
	string retval;
	stringstream ss;
	string key,value;
	key = keyc;
	value = valuec;
	if(key=="Host")
		this->setHost(value);
	else if(key=="User-Agent" || key=="User-agent")
		this->setUser_agent(value);
	else if(key=="Accept")
		this->setAccept(value);
	else if(key=="Accept-Language" || key=="Accept-language")
	{
		strVec lemp;
		this->setAccept_lang(value);
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
		//logger << temp.at(1) << flush;
	}
	else if(key=="Accept-Encoding" || key=="Accept-encoding")
		this->setAccept_encod(value);
	else if(key=="Accept-Charset" || key=="Accept-charset")
		this->setAccept_lang(value);
	else if(key=="Keep-Alive" || key=="Keep-alive")
		this->setKeep_alive(value);
	else if(key=="Connection")
		this->setConnection(value);
	else if(key=="Cache-Control" || key=="Cache-control")
		this->setCache_ctrl(value);
	else if(key=="Content-Type" || key=="Content-type")
	{
		this->setContent_type(value);
		string tempi(value);
		size_t s = tempi.find("boundary");
		if(s!=string::npos)
		{
			this->setContent_type(tempi.substr(0,s));
			tempi = tempi.substr(s);
			strVec results;
			StringUtil::split(results, tempi, ("="));
			if(results.size()==2)
			{
				string bound = "--" + results.at(1).substr(0,results.at(1).length());
				this->setContent_boundary(bound);
			}
		}
	}
	else if(key=="Content" && value!="")
	{
		if((this->getContent_type().find("application/soap+xml")!=string::npos || this->getContent_type().find("text/xml")!=string::npos))
		{
			value = value.substr(0,value.find_last_of(">")+1);
			if(value.find("<?")!=string::npos && value.find("?>")!=string::npos)
			{
				value = value.substr(value.find("?>")+2);
			}
			this->setContent(value);
		}
		else if(this->getContent_type().find("application/x-www-form-urlencoded")!=string::npos)
		{
			strVec params;
			StringUtil::split(params,value , ("&"));
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
					this->setRequestParam(att,param.at(1));
				}
			}
		}
		else
		{
			size_t rn = value.find_first_of("\r\n");
			string h = CastUtil::lexical_cast<string>(rn);

			string boundary = this->getContent_boundary();
			fprintf(stderr,boundary.c_str());
			fprintf(stderr,value.c_str());
			fflush(stderr);

			string retval;
			string delb = boundary+"\r\n";
			string delend = boundary+"--\r\n";
			size_t stb = value.find_first_of(delb)+delb.length();
			size_t enb = value.find_last_not_of(delend);
			h = CastUtil::lexical_cast<string>(stb)+" "+CastUtil::lexical_cast<string>(enb);

			string param_conts = value.substr(stb,enb-stb-2);
			strVec parameters;
			StringUtil::replaceFirst(value,delb,"");
			StringUtil::replaceFirst(value,delend,"");
			delb = "\r\n"+delb;
			StringUtil::split(parameters, value, (delb));
			retval =  "Boundary: "+ boundary + "\nLength: " ;
			retval += CastUtil::lexical_cast<string>(value.length()) +"\nStart End: "
			+ CastUtil::lexical_cast<string>(stb) + " " + CastUtil::lexical_cast<string>(enb) +"\n";

			for(unsigned j=0;j<parameters.size();j++)
			{
				if(parameters.at(j)=="" || parameters.at(j).find_first_not_of(" ")==string::npos
						|| parameters.at(j).find_first_not_of("\r\n")==string::npos)
					continue;
				fprintf(stderr,parameters.at(j).c_str());
				fflush(stderr);
				FormData datf;
				string parm = parameters.at(j);
				retval+= parm + "\nparm";
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
						dist = parm.find("\r\n\r\n");
						dise = dist + 4;
					}
					else
					{
						cont_type = parm.substr(dist+14,parm.find("\r\n\r\n")-(dist+14));
						dise = parm.find("\r\n\r\n") + 4;
					}
					cont_disp = parm.substr(dis+21,dist-(dis+21));
					StringUtil::replaceFirst(cont_disp,"\r\n","");
					retval+=  "\ncdisp = " + cont_disp;
					retval+= "\ndise = " + CastUtil::lexical_cast<string>(dise);
					parm = parm.substr(dise);
					parm = parm.substr(0,parm.length()-1);
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
						retval+=  "\nparmdef = " + parmdef.at(k) ;
						retval+=  "\nst en = " + CastUtil::lexical_cast<string>(stpd)  + " " + CastUtil::lexical_cast<string>(enpd);
						string propert = parmdef.at(k).substr(stpd,enpd-stpd+1);
						strVec proplr;
						StringUtil::split(proplr, propert, ("="));
						if(proplr.size()==2)
						{
							if(proplr.at(0)=="name" && proplr.at(1)!="\"\"")
							{
								key = proplr.at(1);
								key = key.substr(key.find_first_not_of("\""),key.find_last_not_of("\"")-key.find_first_not_of("\"")+1);
								datf.type = cont_type;
								datf.value = parm;
							}
							else if(proplr.at(0)=="filename" && proplr.at(1)!="\"\"")
							{
								string fna = proplr.at(1);
								fna = fna.substr(fna.find_first_not_of("\""),fna.find_last_not_of("\"")-fna.find_first_not_of("\"")+1);
								datf.fileName = fna;
							}
						}
					}
				}
				if(key!="")
				{
					this->setRequestParamF(key,datf);
					retval+= (key + " " + datf.type + " "+ datf.fileName+" "+ datf.value);
				}
			}
		}

	}
	else if(key=="Content-Length" || key=="Content-length")
		this->setContent_len(value);
	else if(key=="Referer")
		this->setReferer(value);
	else if(key=="Pragma")
		this->setPragma(value);
	else if(key=="Method")
		this->setMethod(value);
	else if(key=="HttpVersion")
		this->setHttpVersion(value);
	else if(key=="GetArguments")
	{
		strVec params;
		StringUtil::split(params, value, ("&"));
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
				this->setRequestParam(att,param.at(1));
			}
		}
	}
	/*else if(key=="PostArguments")
	{
		strVec params;
		StringUtil::split(params, value, ("&"));
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
				this->setRequestParam(att,param.at(1));
			}
		}
	}*/
	else if(key.find("URL")!=string::npos)
	{
		StringUtil::replaceFirst(key,"URL","");
		strVec memp;
		this->setActUrl(value);
		StringUtil::split(memp, value, ("/"));
		int fs = value.find_first_of("/");
		int es = value.find_last_of("/");
		if(fs==es)
		{
			this->setCntxt_root(key+"default");
			this->setCntxt_name("default");
			this->setFile(value.substr(es+1));
			this->setUrl(key+"default/"+value);
		}
		else
		{
			int ss = value.substr(fs+1).find("/");
			if(ss>fs)
			{
				this->setCntxt_name(value.substr(fs+1,ss-fs));
				this->setCntxt_root(key+this->getCntxt_name());
				this->setFile(value.substr(es+1));
				this->setUrl(key+value);
			}
		}
	}
	return retval;
}

string HttpRequest::toString()
{
	string ret;
	ret = "\nHost: "+this->getHost();
	ret += "\nAccept: "+this->getAccept();
	ret += "\nAccept Chars: "+this->getAccept_chars();
	ret += "\nAccept Encoding: "+this->getAccept_encod();
	ret += "\nAccept Language: "+this->getAccept_lang();
	ret += "\nCache Ctrl: "+this->getCache_ctrl();
	ret += "\nConnection: "+this->getConnection();
	ret += "\nContent: "+this->getContent();
	ret += "\nContent Length: "+this->getContent_len();
	ret += "\nContent Type: "+this->getContent_type();
	ret += "\nHttp Version: "+this->getHttpVersion();
	ret += "\nMethod: "+this->getMethod();
	ret += "\nUser agent: "+this->getUser_agent();
	ret += "\nUrl: "+this->getUrl();
	ret += "\nFile: "+this->getFile();
	ret += "\nContext Name: "+this->getCntxt_name();
	ret += "\nContext Root: "+this->getCntxt_root();
	ret += "\nDefault Locale: "+this->getDefaultLocale();
	ret += "\nContent Boundary: "+this->getContent_boundary();
	string vals;
	if(this->requestParams.size()>0)
	{
		RMap::iterator iter;
		for (iter=this->requestParams.begin();iter!=this->requestParams.end();iter++)
		{
			vals+= ("\nKey: "+iter->first + " Value: "+iter->second);
		}
	}
	if(this->requestParamsF.size()>0)
	{
		FMap::iterator iter;
		for (iter=this->requestParamsF.begin();iter!=this->requestParamsF.end();iter++)
		{
			FormData dat = iter->second;
			vals+= ("\nKey: "+iter->first + " Type: "+dat.type + " FileName: "+dat.fileName);
			vals+= ("\nValue: "+dat.value);
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

void HttpRequest::setMethod(string method)
{
	this->method = method;
}

string HttpRequest::getUser_agent() const
{
	return user_agent;
}

void HttpRequest::setUser_agent(string user_agent)
{
	this->user_agent = user_agent;
}

string HttpRequest::getAccept() const
{
	return accept;
}

void HttpRequest::setAccept(string accept)
{
	this->accept = accept;
}

string HttpRequest::getAccept_lang() const
{
	return accept_lang;
}

void HttpRequest::setAccept_lang(string accept_lang)
{
	this->accept_lang = accept_lang;
}

string HttpRequest::getAccept_encod() const
{
	return accept_encod;
}

void HttpRequest::setAccept_encod(string accept_encod)
{
	this->accept_encod = accept_encod;
}

string HttpRequest::getAccept_chars() const
{
	return accept_chars;
}

void HttpRequest::setAccept_chars(string accept_chars)
{
	this->accept_chars = accept_chars;
}

string HttpRequest::getKeep_alive() const
{
	return keep_alive;
}

void HttpRequest::setKeep_alive(string keep_alive)
{
	this->keep_alive = keep_alive;
}

string HttpRequest::getConnection() const
{
	return connection;
}

void HttpRequest::setConnection(string connection)
{
	this->connection = connection;
}

string HttpRequest::getCache_ctrl() const
{
	return cache_ctrl;
}

void HttpRequest::setCache_ctrl(string cache_ctrl)
{
	this->cache_ctrl = cache_ctrl;
}

Map HttpRequest::getAttributes() const
{
	return attributes;
}

void HttpRequest::setAttributes(Map attributes)
{
	this->attributes = attributes;
}

string HttpRequest::getAttribute(string key)
{
	return attributes[key];
}

void HttpRequest::setAttribute(string key,string value)
{
	this->attributes[key] = value;
}

HttpSession* HttpRequest::getSession()
{
	return &(this->session);
}
void HttpRequest::setSession(HttpSession session)
{
	this->session = session;
}

string HttpRequest::getHost() const
{
	return this->host;
}
void HttpRequest::setHost(string host)
{
	this->host = host;
}

void HttpRequest::setUrl(string url)
{
	this->url = url;
}

string HttpRequest::getUrl()
{
	return this->url;
}

void HttpRequest::setHttpVersion(string httpVersion)
{
	this->httpVersion = httpVersion;
}
string HttpRequest::getHttpVersion()
{
	return this->httpVersion;
}

string HttpRequest::getContent_type() const
{
	return content_type;
}

void HttpRequest::setContent_type(string content_type)
{
	this->content_type = content_type;
}

string HttpRequest::getContent_boundary() const
{
	return content_boundary;
}

void HttpRequest::setContent_boundary(string content_boundary)
{
	this->content_boundary = content_boundary;
}

string HttpRequest::getContent_len() const
{
	return content_len;
}

void HttpRequest::setContent_len(string content_len)
{
	this->content_len = content_len;
}

string HttpRequest::getContent() const
{
	return content;
}

void HttpRequest::setContent(string content)
{
	this->content = content;
}

string HttpRequest::getReferer() const
{
	return referer;
}

void HttpRequest::setReferer(string referer)
{
	this->referer = referer;
}

string HttpRequest::getPragma() const
{
	return pragma;
}

void HttpRequest::setPragma(string pragma)
{
	this->pragma = pragma;
}

RMap HttpRequest::getRequestParams() const
{
	return requestParams;
}

void HttpRequest::setRequestParams(RMap requestParams)
{
	this->requestParams = requestParams;
}

string HttpRequest::getRequestParam(string key)
{
	if(this->requestParams.find(key)!=this->requestParams.end())
		return this->requestParams[key];
	else if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].value;
	else
		return "";
}

string HttpRequest::getRequestParamType(string key)
{
	if(this->requestParamsF.find(key)!=this->requestParamsF.end())
		return this->requestParamsF[key].type;
	else
		return "";
}

void HttpRequest::setRequestParam(string key,string value)
{
	this->requestParams[key] = value;
}

void HttpRequest::setRequestParamF(string key,FormData value)
{
	this->requestParamsF[key] = value;
}

string HttpRequest::getCntxt_root() const
{
	return cntxt_root;
}

void HttpRequest::setCntxt_root(string cntxt_root)
{
	this->cntxt_root = cntxt_root;
}
string HttpRequest::getDefaultLocale()
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

void HttpRequest::setCntxt_name(string cntxt_name)
{
	this->cntxt_name = cntxt_name;
}

string HttpRequest::getFile() const
{
	return file;
}

void HttpRequest::setFile(string file)
{
	logger << "file is " << this->file << endl;
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
}

string HttpRequest::getActUrl() const
{
	return actUrl;
}

void HttpRequest::setActUrl(string actUrl)
{
	this->actUrl = actUrl;
}

map<string,string> HttpRequest::getAuthinfo() const
{
	return authinfo;
}

void HttpRequest::setAuthinfo(map<string,string> authinfo)
{
	this->authinfo = authinfo;
}

string HttpRequest::toPHPVariablesString(string def)
{
	string ret;
	ret = "<?php\n$_SERVER['HTTP_HOST'] = '"+this->getHost();
	ret += "';\n$_SERVER['HTTP_ACCEPT'] = '"+this->getAccept();
	ret += "';\n$_SERVER['HTTP_ACCEPT_CHARSET'] = '"+this->getAccept_chars();
	ret += "';\n$_SERVER['HTTP_ACCEPT_ENCODING'] = '"+this->getAccept_encod();
	ret += "';\n$_SERVER['HTTP_ACCEPT_LANGUAGE'] = '"+this->getAccept_lang();
	ret += "';\n$_SERVER_EX['CACHE_CNTRL'] = '"+this->getCache_ctrl();
	ret += "';\n$_SERVER['HTTP_CONNECTION'] = '"+this->getConnection();
	ret += "';\n$_SERVER_EX['HTTP_CONTENT'] = '"+this->getContent();
	ret += "';\n$_SERVER_EX['HTTP_CNTENT_LENGTH'] = '"+this->getContent_len();
	ret += "';\n$_SERVER_EX['HTTP_CNTENT_TYPE'] = '"+this->getContent_type();
	ret += "';\n$_SERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "';\n$_SERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "';\n$_SERVER['HTTP_USER_AGENT'] = '"+this->getUser_agent();
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
	ret += "';\n$_SERVER['HTTP_HOST'] = '"+this->getContent_boundary() + "';\n";
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
			FormData dat = iter->second;
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				ret += "\nif(!isset($_FILES['"+iter->first.substr(0, iter->first.find("["))+"']))\n{\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']=array()\n}\n";
				ret += "\nif(!isset($_FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"))\n"
						+ "{\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"=array();\n}\n";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.fileName + "';";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.type + "';";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.length) + ";";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.tmpFileName + "';";
				ret += "\n$_FILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nif(!isset($_FILES['"+iter->first+"']))\n{\n$_FILES['"+iter->first+"']=array();\n}\n";
				ret += "\n$_FILES['"+iter->first+"']['name'] = '"+ dat.fileName + "';";
				ret += "\n$_FILES['"+iter->first+"']['type'] = '"+ dat.type + "';";
				ret += "\n$_FILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.length) + ";";
				ret += "\n$_FILES['"+iter->first+"']['tmp_name'] = '"+ dat.tmpFileName + "';";
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
	ret += "\n$_FILES = {};";
	ret += "\n$_SERVER{'HTTP_HOST'} = '"+this->getHost();
	ret += "';\n$_SERVER{'HTTP_ACCEPT'} = '"+this->getAccept();
	ret += "';\n$_SERVER{'HTTP_ACCEPT_CHARSET'} = '"+this->getAccept_chars();
	ret += "';\n$_SERVER{'HTTP_ACCEPT_ENCODING'} = '"+this->getAccept_encod();
	ret += "';\n$_SERVER{'HTTP_ACCEPT_LANGUAGE'} = '"+this->getAccept_lang();
	ret += "';\n$_SERVER_EX{'CACHE_CNTRL'} = '"+this->getCache_ctrl();
	ret += "';\n$_SERVER{'HTTP_CONNECTION'} = '"+this->getConnection();
	ret += "';\n$_SERVER_EX{'HTTP_CONTENT'} = '"+this->getContent();
	ret += "';\n$_SERVER_EX{'HTTP_CNTENT_LENGTH'} = '"+this->getContent_len();
	ret += "';\n$_SERVER_EX{'HTTP_CNTENT_TYPE'} = '"+this->getContent_type();
	ret += "';\n$_SERVER_EX{'HTTP_VERSION'} = '"+this->getHttpVersion();
	ret += "';\n$_SERVER{'REQUEST_METHOD'} = '"+this->getMethod();
	ret += "';\n$_SERVER{'HTTP_USER_AGENT'} = '"+this->getUser_agent();
	ret += "';\n$_SERVER{'REQUEST_URI'} = '"+this->getUrl();
	ret += "';\n$_SERVER_EX{'HTTP_REQ_FILE'} = '"+this->getFile();
	ret += "';\n$_SERVER_EX{'CONTEXT_NAME'} = '"+this->getCntxt_name();
	ret += "';\n$_SERVER{'DOCUMENT_ROOT'} = '"+this->getCntxt_root();
	ret += "';\n$_SERVER_EX{'DEFAULT_LOCALE'} = '"+this->getDefaultLocale();
	ret += "';\n$_SERVER{'HTTP_HOST'} = '"+this->getContent_boundary() + "';\n";
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
			FormData dat = iter->second;
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				string key = iter->first;
				StringUtil::replaceFirst(key,"[","{");
				StringUtil::replaceFirst(key,"]","}");
				ret += "\nif(!exists $_FILES{'"+key.substr(0, key.find("{"))+"'})\n{\n$_FILES{'"+key.substr(0, key.find("{"))+"'}={}\n}\n";
				ret += "\nif(!exists $_FILES{'"+key.substr(0, key.find("{"))+"'}{'"+key.substr(key.find("{"))+"'})\n"
						+ "{\n$_FILES{'"+key.substr(0, key.find("{"))+"'}{'"+key.substr(key.find("{"))+"'}={}\n}\n";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'name'} = '"+ dat.fileName + "';";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'type'} = '"+ dat.type + "';";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'size'} = "+ CastUtil::lexical_cast<string>(dat.length) + ";";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'tmp_name'} = '"+ dat.tmpFileName + "';";
				ret += "\n$_FILES{'"+key.substr(0, key.find("{"))+"'}" +
						key.substr(key.find("{")) + "{'error'} = 0;";
			}
			else
			{
				ret += "\nif(!exists $_FILES{'"+iter->first+"'})\n{\n$_FILES{'"+iter->first+"'}={}\n}\n";
				ret += "\n$_FILES{'"+iter->first+"'}{'name'} = '"+ dat.fileName + "';";
				ret += "\n$_FILES{'"+iter->first+"'}{'type'} = '"+ dat.type + "';";
				ret += "\n$_FILES{'"+iter->first+"'}{'size'} = "+ CastUtil::lexical_cast<string>(dat.length) + ";";
				ret += "\n$_FILES{'"+iter->first+"'}{'tmp_name'} = '"+ dat.tmpFileName + "';";
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
	ret += "\nFILES = {}";
	ret += "\nSERVER['HTTP_HOST'] = '"+this->getHost();
	ret += "'\nSERVER['HTTP_ACCEPT'] = '"+this->getAccept();
	ret += "'\nSERVER['HTTP_ACCEPT_CHARSET'] = '"+this->getAccept_chars();
	ret += "'\nSERVER['HTTP_ACCEPT_ENCODING'] = '"+this->getAccept_encod();
	ret += "'\nSERVER['HTTP_ACCEPT_LANGUAGE'] = '"+this->getAccept_lang();
	ret += "'\nSERVER_EX['CACHE_CNTRL'] = '"+this->getCache_ctrl();
	ret += "'\nSERVER['HTTP_CONNECTION'] = '"+this->getConnection();
	ret += "'\nSERVER_EX['HTTP_CONTENT'] = '"+this->getContent();
	ret += "'\nSERVER_EX['HTTP_CNTENT_LENGTH'] = '"+this->getContent_len();
	ret += "'\nSERVER_EX['HTTP_CNTENT_TYPE'] = '"+this->getContent_type();
	ret += "'\nSERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "'\nSERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "'\nSERVER['HTTP_USER_AGENT'] = '"+this->getUser_agent();
	ret += "'\nSERVER['REQUEST_URI'] = '"+this->getUrl();
	ret += "'\nSERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "'\nSERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	ret += "'\nSERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	ret += "'\nSERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "'\nSERVER['HTTP_HOST'] = '"+this->getContent_boundary() + "'\n";
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
			FormData dat = iter->second;
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
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.fileName + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.type + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.length);
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.tmpFileName + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nFILES['"+iter->first+"'] = {}";
				ret += "\nFILES['"+iter->first+"']['name'] = '"+ dat.fileName + "'";
				ret += "\nFILES['"+iter->first+"']['type'] = '"+ dat.type + "'";
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.length);
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.tmpFileName + "'";
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
	ret += "\nFILES = {}";
	ret += "\nSERVER['HTTP_HOST'] = '"+this->getHost();
	ret += "'\nSERVER['HTTP_ACCEPT'] = '"+this->getAccept();
	ret += "'\nSERVER['HTTP_ACCEPT_CHARSET'] = '"+this->getAccept_chars();
	ret += "'\nSERVER['HTTP_ACCEPT_ENCODING'] = '"+this->getAccept_encod();
	ret += "'\nSERVER['HTTP_ACCEPT_LANGUAGE'] = '"+this->getAccept_lang();
	ret += "'\nSERVER_EX['CACHE_CNTRL'] = '"+this->getCache_ctrl();
	ret += "'\nSERVER['HTTP_CONNECTION'] = '"+this->getConnection();
	ret += "'\nSERVER_EX['HTTP_CONTENT'] = '"+this->getContent();
	ret += "'\nSERVER_EX['HTTP_CNTENT_LENGTH'] = '"+this->getContent_len();
	ret += "'\nSERVER_EX['HTTP_CNTENT_TYPE'] = '"+this->getContent_type();
	ret += "'\nSERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "'\nSERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "'\nSERVER['HTTP_USER_AGENT'] = '"+this->getUser_agent();
	ret += "'\nSERVER['REQUEST_URI'] = '"+this->getUrl();
	ret += "'\nSERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "'\nSERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	ret += "'\nSERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	ret += "'\nSERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "'\nSERVER['HTTP_HOST'] = '"+this->getContent_boundary() + "'\n";
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
			FormData dat = iter->second;
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
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.fileName + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.type + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.length);
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.tmpFileName + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nFILES['"+iter->first+"'] = {}";
				ret += "\nFILES['"+iter->first+"']['name'] = '"+ dat.fileName + "'";
				ret += "\nFILES['"+iter->first+"']['type'] = '"+ dat.type + "'";
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.length);
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.tmpFileName + "'";
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
	ret += "\nFILES = {}";
	ret += "\nSERVER['HTTP_HOST'] = '"+this->getHost();
	ret += "'\nSERVER['HTTP_ACCEPT'] = '"+this->getAccept();
	ret += "'\nSERVER['HTTP_ACCEPT_CHARSET'] = '"+this->getAccept_chars();
	ret += "'\nSERVER['HTTP_ACCEPT_ENCODING'] = '"+this->getAccept_encod();
	ret += "'\nSERVER['HTTP_ACCEPT_LANGUAGE'] = '"+this->getAccept_lang();
	ret += "'\nSERVER_EX['CACHE_CNTRL'] = '"+this->getCache_ctrl();
	ret += "'\nSERVER['HTTP_CONNECTION'] = '"+this->getConnection();
	ret += "'\nSERVER_EX['HTTP_CONTENT'] = '"+this->getContent();
	ret += "'\nSERVER_EX['HTTP_CNTENT_LENGTH'] = '"+this->getContent_len();
	ret += "'\nSERVER_EX['HTTP_CNTENT_TYPE'] = '"+this->getContent_type();
	ret += "'\nSERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "'\nSERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "'\nSERVER['HTTP_USER_AGENT'] = '"+this->getUser_agent();
	ret += "'\nSERVER['REQUEST_URI'] = '"+this->getUrl();
	ret += "'\nSERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "'\nSERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	ret += "'\nSERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	ret += "'\nSERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "'\nSERVER['HTTP_HOST'] = '"+this->getContent_boundary() + "'\n";
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
			FormData dat = iter->second;
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
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.fileName + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.type + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.length);
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.tmpFileName + "'";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nFILES['"+iter->first+"'] = {}";
				ret += "\nFILES['"+iter->first+"']['name'] = '"+ dat.fileName + "'";
				ret += "\nFILES['"+iter->first+"']['type'] = '"+ dat.type + "'";
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.length);
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.tmpFileName + "'";
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
	ret += "\nFILES = {};";
	ret += "\nSERVER['HTTP_HOST'] = '"+this->getHost();
	ret += "';\nSERVER['HTTP_ACCEPT'] = '"+this->getAccept();
	ret += "';\nSERVER['HTTP_ACCEPT_CHARSET'] = '"+this->getAccept_chars();
	ret += "';\nSERVER['HTTP_ACCEPT_ENCODING'] = '"+this->getAccept_encod();
	ret += "';\nSERVER['HTTP_ACCEPT_LANGUAGE'] = '"+this->getAccept_lang();
	ret += "';\nSERVER_EX['CACHE_CNTRL'] = '"+this->getCache_ctrl();
	ret += "';\nSERVER['HTTP_CONNECTION'] = '"+this->getConnection();
	ret += "';\nSERVER_EX['HTTP_CONTENT'] = '"+this->getContent();
	ret += "';\nSERVER_EX['HTTP_CNTENT_LENGTH'] = '"+this->getContent_len();
	ret += "';\nSERVER_EX['HTTP_CNTENT_TYPE'] = '"+this->getContent_type();
	ret += "';\nSERVER_EX['HTTP_VERSION'] = '"+this->getHttpVersion();
	ret += "';\nSERVER['REQUEST_METHOD'] = '"+this->getMethod();
	ret += "';\nSERVER['HTTP_USER_AGENT'] = '"+this->getUser_agent();
	ret += "';\nSERVER['REQUEST_URI'] = '"+this->getUrl();
	ret += "';\nSERVER_EX['HTTP_REQ_FILE'] = '"+this->getFile();
	ret += "';\nSERVER_EX['CONTEXT_NAME'] = '"+this->getCntxt_name();
	ret += "';\nSERVER['DOCUMENT_ROOT'] = '"+this->getCntxt_root();
	ret += "';\nSERVER_EX['DEFAULT_LOCALE'] = '"+this->getDefaultLocale();
	ret += "';\nSERVER['HTTP_HOST'] = '"+this->getContent_boundary() + "';\n";
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
			FormData dat = iter->second;
			if(iter->first.find("[")!=string::npos && iter->first.find("]")!=string::npos)
			{
				ret += "\nif((FILES['"+iter->first.substr(0, iter->first.find("["))+"'])==undefined)\n{\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']={};\n}\n";
				ret += "\nif((FILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+")==undefined)\n"
						+ "{\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']"+iter->first.substr(iter->first.find("["))+"={};\n}\n";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['name'] = '"+ dat.fileName + "';";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['type'] = '"+ dat.type + "';";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['size'] = "+ CastUtil::lexical_cast<string>(dat.length) + ";";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['tmp_name'] = '"+ dat.tmpFileName + "';";
				ret += "\nFILES['"+iter->first.substr(0, iter->first.find("["))+"']" +
						iter->first.substr(iter->first.find("[")) + "['error'] = 0;";
			}
			else
			{
				ret += "\nif(!isset(FILES['"+iter->first+"']))\n{\nFILES['"+iter->first+"']={};\n}\n";
				ret += "\nFILES['"+iter->first+"']['name'] = '"+ dat.fileName + "';";
				ret += "\nFILES['"+iter->first+"']['type'] = '"+ dat.type + "';";
				ret += "\nFILES['"+iter->first+"']['size'] = "+ CastUtil::lexical_cast<string>(dat.length) + ";";
				ret += "\nFILES['"+iter->first+"']['tmp_name'] = '"+ dat.tmpFileName + "';";
				ret += "\nFILES['"+iter->first+"']['error'] = 0;";
			}
		}
	}
	ret += "\nrequire('"+this->getUrl()+"');";
	return ret;
}

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
