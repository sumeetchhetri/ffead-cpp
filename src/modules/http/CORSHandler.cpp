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
 * CORSHandler.cpp
 *
 *  Created on: 16-Apr-2013
 *      Author: sumeetc
 */

#include "CORSHandler.h"

CORSHandler::~CORSHandler() {
}

bool CORSHandler::handle(CorsConfig& corsConfig, HttpRequest *req, HttpResponse *res) {
	if(!req->isCorsRequest())return false;
	if(req->getCORSRequestType() == PREFLIGHT)
	{
		if(corsConfig.isOriginAllowed(req->getHeader(HttpRequest::Origin)))
		{
			if(req->getHeader(HttpResponse::AccessControlAllowMethods)=="")
			{
				HTTPResponseStatus status(HTTPResponseStatus::BadRequest, "Invalid preflight CORS request: Missing Access-Control-Request-Method header");
				throw status;
			}
			else
			{
				if(!HttpRequest::isValidHttpMethod(req->getHeader(HttpResponse::AccessControlAllowMethods)))
				{
					HTTPResponseStatus status(HTTPResponseStatus::InvalidMethod, "Unsupported HTTP method: " + req->getHeader(HttpResponse::AccessControlAllowMethods));
					throw status;
				}

				strVec reqHdrLst = req->parseHeaderValue(req->getHeader(HttpResponse::AccessControlAllowHeaders));

				if(!corsConfig.isMethodAllowed(req->getHeader(HttpResponse::AccessControlAllowMethods)))
				{
					HTTPResponseStatus status(HTTPResponseStatus::InvalidMethod, "Unsupported HTTP method: " + req->getHeader(HttpResponse::AccessControlAllowMethods));
					throw status;
				}

				std::string erheadr;
				if(!corsConfig.isHeaderAllowed(reqHdrLst, erheadr))
				{
					HTTPResponseStatus status(HTTPResponseStatus::Forbidden, "Unsupported HTTP request header: " + erheadr);
					throw status;
				}

				if(corsConfig.allwdCredentials)
				{
					res->addHeaderValue(HttpResponse::AccessControlAllowCredentials, "true");
				}
				if(corsConfig.allwdOrigins=="*")
				{
					res->addHeaderValue(HttpResponse::AccessControlAllowOrigin, "*");
				}
				else
				{
					res->addHeaderValue(HttpResponse::AccessControlAllowOrigin, req->getHeader(HttpRequest::Origin));
				}

				if(corsConfig.maxAge>0)
				{
					res->addHeaderValue(HttpResponse::AccessControlAllowMethods, CastUtil::lexical_cast<std::string>(corsConfig.maxAge));
				}

				res->addHeaderValue(HttpResponse::AccessControlAllowMethods, corsConfig.allwdMethods);
				res->addHeaderValue(HttpResponse::AccessControlMaxAge, corsConfig.allwdHeaders);

			}
		}
		else
		{
			HTTPResponseStatus status(HTTPResponseStatus::Forbidden, "CORS origin denied");
			throw status;
		}

		return true;
	}
	else if(req->getCORSRequestType() == CORS)
	{
		if(corsConfig.isOriginAllowed(req->getHeader(HttpRequest::Origin)))
		{
			if(!corsConfig.isMethodAllowed(req->getMethod()))
			{
				HTTPResponseStatus status(HTTPResponseStatus::InvalidMethod, "Unsupported HTTP method: " + req->getMethod());
				throw status;
			}

			if(corsConfig.allwdCredentials)
			{
				res->addHeaderValue(HttpResponse::AccessControlAllowCredentials, "true");
			}

			res->addHeaderValue(HttpResponse::AccessControlAllowOrigin, req->getHeader(HttpRequest::Origin));
			res->addHeaderValue(HttpResponse::AccessControlAllowHeaders, corsConfig.exposedHeaders);
		}
		else
		{
			HTTPResponseStatus status(HTTPResponseStatus::Forbidden, "CORS origin denied");
			throw status;
		}
	}
	return false;
}

CorsConfig::CorsConfig() {
	allwdCredentials = false;
	maxAge = -1;
}

CorsConfig::CorsConfig(const std::string& allwdOrigins, const std::string& allwdMethods, const std::string& allwdHeaders, const std::string& exposedHeaders, const bool& allwdCredentials, const long& maxAge)
{
	this->allwdOrigins = allwdOrigins;
	this->allwdMethods = allwdMethods;
	this->allwdHeaders = allwdHeaders;
	this->exposedHeaders = exposedHeaders;
	this->allwdCredentials = allwdCredentials;
	this->maxAge = maxAge;
	init();
}

CorsConfig::~CorsConfig()
{

}

void CorsConfig::init()
{
	if(allwdOriginsv.size()>0)return;
	StringUtil::trim(allwdOrigins);
	StringUtil::split(allwdOriginsv, allwdOrigins, (","));
	for (int var = 0; var < (int)allwdOriginsv.size(); ++var) {
		StringUtil::trim(allwdOriginsv.at(var));
		StringUtil::toLower(allwdOriginsv.at(var));
	}
	StringUtil::trim(allwdMethods);
	StringUtil::split(allwdMethodsv, allwdMethods, (","));
	for (int var = 0; var < (int)allwdMethodsv.size(); ++var) {
		StringUtil::trim(allwdMethodsv.at(var));
		StringUtil::toLower(allwdMethodsv.at(var));
	}
	StringUtil::trim(allwdHeaders);
	StringUtil::split(allwdHeadersv, allwdHeaders, (","));
	for (int var = 0; var < (int)allwdHeadersv.size(); ++var) {
		StringUtil::trim(allwdHeadersv.at(var));
		StringUtil::toLower(allwdHeadersv.at(var));
	}
	StringUtil::trim(exposedHeaders);
	StringUtil::split(exposedHeadersv, exposedHeaders, (","));
	for (int var = 0; var < (int)exposedHeadersv.size(); ++var) {
		StringUtil::trim(exposedHeadersv.at(var));
		StringUtil::toLower(exposedHeadersv.at(var));
	}
}

bool CorsConfig::isOriginAllowed(const std::string& reqOrgLst)
{
	if(allwdOrigins=="*")
	{
		return true;
	}
	for (int var1 = 0; var1 < (int)allwdOriginsv.size(); ++var1) {
		if(allwdOriginsv.at(var1)==StringUtil::toLowerCopy(reqOrgLst))
		{
			return true;
		}
	}
	return false;
}
bool CorsConfig::isMethodAllowed(const std::string& method)
{
	if(method=="")
	{
		return false;
	}
	for (int var = 0; var < (int)allwdMethodsv.size(); ++var) {
		if(StringUtil::toLowerCopy(method)==allwdMethodsv.at(var))
		{
			return true;
		}
	}
	return false;
}
bool CorsConfig::isHeaderAllowed(const strVec& reqHdrLst, std::string& erheadr)
{
	if(allwdHeaders=="*")
	{
		return true;
	}
	for (int var = 0; var < (int)reqHdrLst.size(); ++var) {
		for (int var1 = 0; var1 < (int)allwdHeadersv.size(); ++var1) {
			if(allwdHeadersv.at(var1)==StringUtil::toLowerCopy(reqHdrLst.at(var)))
			{
				return true;
			}
		}
		erheadr = StringUtil::toUpperCopy(reqHdrLst.at(var));
		break;
	}
	return false;
}
