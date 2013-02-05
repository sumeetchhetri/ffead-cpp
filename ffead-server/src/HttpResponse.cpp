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

HttpResponse::HttpResponse() {
	// TODO Auto-generated constructor stub
	this->server = "FFEAD 1.1";
}

HttpResponse::~HttpResponse() {
	// TODO Auto-generated destructor stub
}

string HttpResponse::generateResponse()
{
	string resp;
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	if(this->server!="")resp += "Server: " + this->server + "\r\n";
	if(this->date!="")resp += "Date: " + this->date + "\r\n";
	if(this->connection!="")resp += "Connection: " + this->connection + "\r\n";
	if(this->location!="")resp += "Location: " + this->location + "\r\n";
	if(this->accept_ranges!="")resp += "Accept-Ranges: " + this->accept_ranges + "\r\n";
	if(this->content_type!="")resp += "Content-Type: " + this->content_type + "\r\n";
	if(this->content_str!="")resp += "Content-Length: " + CastUtil::lexical_cast<string>((int)content_str.length()) + "\r\n";
	if(this->last_modified!="")resp += "Last-Modified: " + this->last_modified + "\r\n";
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp += "Set-Cookie: " + this->cookies.at(var) + "\r\n";
	}
	resp += "\r\n";
	resp += this->content_str;
	return resp;
}

string HttpResponse::generateHeadResponse()
{
	string resp;
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	if(this->server!="")resp += "Server: " + this->server + "\r\n";
	if(this->date!="")resp += "Date: " + this->date + "\r\n";
	if(this->connection!="")resp += "Connection: " + this->connection + "\r\n";
	if(this->location!="")resp += "Location: " + this->location + "\r\n";
	if(this->accept_ranges!="")resp += "Accept-Ranges: " + this->accept_ranges + "\r\n";
	if(this->content_type!="")resp += "Content-Type: " + this->content_type + "\r\n";
	if(this->content_str!="")resp += "Content-Length: " + CastUtil::lexical_cast<string>((int)content_str.length()) + "\r\n";
	if(this->last_modified!="")resp += "Last-Modified: " + this->last_modified + "\r\n";
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp += "Set-Cookie: " + this->cookies.at(var) + "\r\n";
	}
	resp += "\r\n";
	return resp;
}

string HttpResponse::generateOptionsResponse()
{
	string resp;
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	if(this->server!="")resp += "Server: " + this->server + "\r\n";
	if(this->date!="")resp += "Date: " + this->date + "\r\n";
	if(this->connection!="")resp += "Connection: " + this->connection + "\r\n";
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
	string resp;
	resp = (httpVersion + " " + statusCode + " " + statusMsg + "\r\n");
	if(this->server!="")resp += "Server: " + this->server + "\r\n";
	if(this->date!="")resp += "Date: " + this->date + "\r\n";
	if(this->connection!="")resp += "Connection: " + this->connection + "\r\n";
	if(this->location!="")resp += "Location: " + this->location + "\r\n";
	if(this->accept_ranges!="")resp += "Accept-Ranges: " + this->accept_ranges + "\r\n";
	if(this->content_type!="")resp += "Content-Type: " + this->content_type + "\r\n";
	if(this->content_str!="")resp += "Content-Length: " + CastUtil::lexical_cast<string>((int)content_str.length()) + "\r\n";
	if(this->last_modified!="")resp += "Last-Modified: " + this->last_modified + "\r\n";
	for (int var = 0; var < (int)this->cookies.size(); var++)
	{
		resp += "Set-Cookie: " + this->cookies.at(var) + "\r\n";
	}
	resp += "\r\n";
	if(req!=NULL)
	{
		if(req->getHost()!="")resp += "Host: " + req->getHost() + "\r\n";
		if(req->getUser_agent()!="")resp += "User-Agent: " + req->getUser_agent() + "\r\n";
		if(req->getAccept()!="")resp += "Accept: " + req->getAccept() + "\r\n";
		if(req->getAccept_lang()!="")resp += "Accept-Language: " + req->getAccept_lang() + "\r\n";
		if(req->getAccept_encod()!="")resp += "Accept-Encoding: " + req->getAccept_encod() + "\r\n";
		if(req->getAccept_chars()!="")resp += "Accept-Charset: " + req->getAccept_chars() + "\r\n";
		if(req->getKeep_alive()!="")resp += "Keep-Alive: " + req->getKeep_alive() + "\r\n";
		if(req->getConnection()!="")resp += "Connection: " + req->getConnection() + "\r\n";
		if(req->getCache_ctrl()!="")resp += "Cache-Control: " + req->getCache_ctrl() + "\r\n";
		if(req->getContent_type()!="")resp += "Content-Type: " + req->getContent_type() + "\r\n";
		if(req->getContent_len()!="")resp += "Content-Length: " + req->getContent_len() + "\r\n";
		if(req->getReferer()!="")resp += "Referer: " + req->getReferer() + "\r\n";
		if(req->getPragma()!="")resp += "Pragma: " + req->getPragma() + "\r\n";
	}
	return resp;
}
