/*
	Copyright 2010, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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
 * MultipartContent.h
 *
 *  Created on: 28-Apr-2013
 *      Author: sumeetc
 */

#ifndef MULTIPARTCONTENT_H_
#define MULTIPARTCONTENT_H_
#include "map"
#include "string"
#include "CryptoHandler.h"
#include "RegexUtil.h"
#include <fstream>
#include "ContentTypes.h"
#include "HTTPResponseStatus.h"
#include "string_view"


class MultipartContent {
	static std::string_view VALID_HEADERS;
	std::map<std::string_view, std::string_view,cicomp> headers;
	std::string_view content;
	std::string_view tempFileName;
	std::string_view fileName;
	std::string_view name;
	void setContent(const std::string_view& content);
	void setName(const std::string_view& name);
	void setTempFileName(const std::string_view& tempFileName);
	std::string_view getTempFileName() const;
	void setFileName(const std::string_view& fileName);
	friend class HttpResponse;
	friend class HttpRequest;
	friend class ControllerHandler;
	friend class ServiceTask;
	void addHeader(std::string_view header, const std::string_view& value);
public:
	static std::string_view ContentId,ContentDisposition,ContentTransferEncoding,
				  ContentLocation,ContentBase,ContentLength,ContentMD5,ContentType;
	MultipartContent();
	MultipartContent(const std::string_view& content);
	MultipartContent(const std::vector<std::string_view>& headers, const std::string_view& content= "");
	virtual ~MultipartContent();
	void addHeaderValue(std::string_view header, const std::string_view& value);
	std::string_view getContent() const;
	std::string_view getFileName() const;
	std::map<std::string_view, std::string_view,cicomp> getHeaders();
	bool isHeaderValue(std::string_view header, const std::string_view& value, const bool& ignoreCase= true);
	std::string_view getHeader(const std::string_view&);
	std::string_view getName() const;
	bool isAFile();
	bool isValid();
};

#endif /* MULTIPARTCONTENT_H_ */
