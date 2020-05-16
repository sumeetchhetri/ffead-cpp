/*
	Copyright 2009-2020, Sumeet Chhetri

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


class MultipartContent {
	static std::map<std::string, int, std::less<>> HDRS_SW_CODES;
	static std::string VALID_HEADERS;
	std::map<std::string,std::string, std::less<>> headers;
	std::string content;
	std::string tempFileName;
	std::string fileName;
	std::string name;
	void setContent(const std::string& content);
	void setName(const std::string& name);
	void setTempFileName(const std::string& tempFileName);
	std::string getTempFileName() const;
	void setFileName(const std::string& fileName);
	friend class HttpResponse;
	friend class HttpRequest;
	friend class ControllerHandler;
	friend class ServiceTask;
	void addHeader(std::string header, const std::string& value);
public:
	static void init();
	static std::string ContentId,ContentDisposition,ContentTransferEncoding,
				  ContentLocation,ContentBase,ContentLength,ContentMD5,ContentType;
	MultipartContent();
	MultipartContent(const std::string& content);
	MultipartContent(const std::vector<std::string>& headers, const std::string& content= "");
	virtual ~MultipartContent();
	void addHeaderValue(std::string header, const std::string& value);
	std::string getContent() const;
	std::string getFileName() const;
	std::map<std::string, std::string,std::less<>> getHeaders();
	bool isHeaderValue(std::string header, const std::string& value, const bool& ignoreCase= true);
	std::string getHeader(const std::string&);
	std::string getName() const;
	bool isAFile();
	bool isValid();
};

#endif /* MULTIPARTCONTENT_H_ */
