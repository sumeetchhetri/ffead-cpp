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
 * HTTPResponseStatus.cpp
 *
 *  Created on: 25-Jan-2013
 *      Author: sumeetc
 */

#include "HTTPResponseStatus.h"

std::map<int, HTTPResponseStatus> HTTPResponseStatus::statuses;
std::map<std::string, HTTPResponseStatus> HTTPResponseStatus::sstatuses;

void HTTPResponseStatus::init() {
	if(statuses.size()==0) {
		statuses[100] = HTTPResponseStatus::Continue;
		statuses[101] = HTTPResponseStatus::Switching;
		statuses[200] = HTTPResponseStatus::Ok;
		statuses[201] = HTTPResponseStatus::Created;
		statuses[202] = HTTPResponseStatus::Accepted;
		statuses[203] = HTTPResponseStatus::NonAuthInfo;
		statuses[204] = HTTPResponseStatus::NoContent;
		statuses[205] = HTTPResponseStatus::ResetContent;
		statuses[206] = HTTPResponseStatus::PartialContent;
		statuses[302] = HTTPResponseStatus::ObjectMoved;
		statuses[303] = HTTPResponseStatus::MovedPermanently;
		statuses[304] = HTTPResponseStatus::NotModified;
		statuses[307] = HTTPResponseStatus::TempRedirect;
		statuses[400] = HTTPResponseStatus::BadRequest;
		statuses[401] = HTTPResponseStatus::AccessDenied;
		statuses[401] = HTTPResponseStatus::Unauthorized;
		statuses[403] = HTTPResponseStatus::Forbidden;
		statuses[404] = HTTPResponseStatus::NotFound;
		statuses[405] = HTTPResponseStatus::InvalidMethod;
		statuses[406] = HTTPResponseStatus::InvalidMime;
		statuses[407] = HTTPResponseStatus::ProxyAuthRequired;
		statuses[412] = HTTPResponseStatus::PreconditionFailed;
		statuses[413] = HTTPResponseStatus::ReqEntityLarge;
		statuses[414] = HTTPResponseStatus::ReqUrlLarge;
		statuses[415] = HTTPResponseStatus::UnsupportedMedia;
		statuses[416] = HTTPResponseStatus::InvalidReqRange;
		statuses[417] = HTTPResponseStatus::ExecutionFailed;
		statuses[423] = HTTPResponseStatus::LockedError;
		statuses[500] = HTTPResponseStatus::InternalServerError;
		statuses[501] = HTTPResponseStatus::InvalidHeaderConf;
		statuses[502] = HTTPResponseStatus::BadGateway;
		statuses[503] = HTTPResponseStatus::ServiceUnavailable;
		statuses[504] = HTTPResponseStatus::GatewayTimeout;
		statuses[505] = HTTPResponseStatus::HttpVersionNotSupported;
		std::map<int, HTTPResponseStatus>::iterator i;
		for(i=statuses.begin();i!=statuses.end();++i) {
			sstatuses[CastUtil::fromNumber(i->first)] = i->second;
		}
	}
}

HTTPResponseStatus& HTTPResponseStatus::getStatusByCode(const int& code)
{
	if(statuses.find(code)!=statuses.end())
	{
		return statuses[code];
	}
	return HTTPResponseStatus::Ok;
}

HTTPResponseStatus& HTTPResponseStatus::getStatusByCode(const std::string& code)
{
	if(sstatuses.find(code)!=sstatuses.end())
	{
		return sstatuses[code];
	}
	return HTTPResponseStatus::Ok;
}

HTTPResponseStatus HTTPResponseStatus::Continue(100, "Continue");
HTTPResponseStatus HTTPResponseStatus::Switching(101, "Switching protocols");
HTTPResponseStatus HTTPResponseStatus::Ok(200, "OK");
HTTPResponseStatus HTTPResponseStatus::Created(201, "Created");
HTTPResponseStatus HTTPResponseStatus::Accepted(202, "Accepted");
HTTPResponseStatus HTTPResponseStatus::NonAuthInfo(203, "Non-authoritative information");
HTTPResponseStatus HTTPResponseStatus::NoContent(204, "No content");
HTTPResponseStatus HTTPResponseStatus::ResetContent(205, "Reset content");
HTTPResponseStatus HTTPResponseStatus::PartialContent(206, "Partial content");
HTTPResponseStatus HTTPResponseStatus::ObjectMoved(302, "Object moved");
HTTPResponseStatus HTTPResponseStatus::MovedPermanently(303, "Moved Permanently");
HTTPResponseStatus HTTPResponseStatus::NotModified(304, "Not modified");
HTTPResponseStatus HTTPResponseStatus::TempRedirect(307, "Temporary redirect");
HTTPResponseStatus HTTPResponseStatus::BadRequest(400, "Bad request");
HTTPResponseStatus HTTPResponseStatus::AccessDenied(401, "Access denied");
HTTPResponseStatus HTTPResponseStatus::Unauthorized(401, "Unauthorized\r\nWWW-Authenticate: Invalid authentication details");
HTTPResponseStatus HTTPResponseStatus::Forbidden(403, "Forbidden");
HTTPResponseStatus HTTPResponseStatus::NotFound(404, "Not Found");
HTTPResponseStatus HTTPResponseStatus::InvalidMethod(405, "HTTP verb used to access this page is not allowed (method not allowed)");
HTTPResponseStatus HTTPResponseStatus::InvalidMime(406, "Client browser does not accept the MIME type of the requested page");
HTTPResponseStatus HTTPResponseStatus::ProxyAuthRequired(407, "Proxy authentication required");
HTTPResponseStatus HTTPResponseStatus::PreconditionFailed(412, "Precondition failed");
HTTPResponseStatus HTTPResponseStatus::ReqEntityLarge(413, "Request entity too large");
HTTPResponseStatus HTTPResponseStatus::ReqUrlLarge(414, "Request-URL too long");
HTTPResponseStatus HTTPResponseStatus::UnsupportedMedia(415, "Unsupported media type");
HTTPResponseStatus HTTPResponseStatus::InvalidReqRange(416, "Requested range not satisfiable");
HTTPResponseStatus HTTPResponseStatus::ExecutionFailed(417, "Execution failed");
HTTPResponseStatus HTTPResponseStatus::LockedError(423, "Locked error");
HTTPResponseStatus HTTPResponseStatus::InternalServerError(500, "Internal server error");
HTTPResponseStatus HTTPResponseStatus::InvalidHeaderConf(501, "Header values specify a configuration that is not implemented");
HTTPResponseStatus HTTPResponseStatus::BadGateway(502, "Bad Gateway");
HTTPResponseStatus HTTPResponseStatus::ServiceUnavailable(503, "Service unavailable");
HTTPResponseStatus HTTPResponseStatus::GatewayTimeout(504, "Gateway timeout");
HTTPResponseStatus HTTPResponseStatus::HttpVersionNotSupported(505, "HTTP version not supported");

HTTPResponseStatus::HTTPResponseStatus() {
	this->code = 0;
	this->scode = "";
}

HTTPResponseStatus::HTTPResponseStatus(const int& code, const std::string& msg) {
	this->code = code;
	this->msg = msg;
	this->scode = CastUtil::fromNumber(code);
}

HTTPResponseStatus::HTTPResponseStatus(const HTTPResponseStatus& status) {
	this->code = status.code;
	this->msg = status.msg;
	this->scode = CastUtil::fromNumber(code);
}

HTTPResponseStatus::HTTPResponseStatus(const HTTPResponseStatus& status, const std::string& msg) {
	this->code = status.code;
	this->msg = msg;
	this->scode = CastUtil::fromNumber(status.code);
}

HTTPResponseStatus::~HTTPResponseStatus() {
}

int HTTPResponseStatus::getCode() const
{
	return code;
}

std::string HTTPResponseStatus::getSCode() const
{
	return scode;
}

const std::string& HTTPResponseStatus::getMsg()
{
	return msg;
}
