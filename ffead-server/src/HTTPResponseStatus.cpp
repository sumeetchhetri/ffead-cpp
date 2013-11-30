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
 * HTTPResponseStatus.cpp
 *
 *  Created on: 25-Jan-2013
 *      Author: sumeetc
 */

#include "HTTPResponseStatus.h"

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
HTTPResponseStatus HTTPResponseStatus::NotFound(404, "Not found");
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
}

HTTPResponseStatus::HTTPResponseStatus(int code, string msg) {
	this->code = code;
	this->msg = msg;
}

HTTPResponseStatus::HTTPResponseStatus(const HTTPResponseStatus& status) {
	this->code = status.code;
	this->msg = status.msg;
}

HTTPResponseStatus::HTTPResponseStatus(const HTTPResponseStatus& status, string msg) {
	this->code = status.code;
	this->msg = msg;
}

HTTPResponseStatus::~HTTPResponseStatus() {
	// TODO Auto-generated destructor stub
}

int HTTPResponseStatus::getCode()
{
	return code;
}
string HTTPResponseStatus::getMsg()
{
	return msg;
}
