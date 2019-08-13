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
 * MultipartContent.cpp
 *
 *  Created on: 28-Apr-2013
 *      Author: sumeetc
 */

#include "MultipartContent.h"

std::string MultipartContent::VALID_HEADERS = ",content-id,content-type,content-disposition,content-transfer-encoding,content-location,content-base,";

std::string MultipartContent::ContentId = "Content-Id";
std::string MultipartContent::ContentDisposition = "Content-Disposition";
std::string MultipartContent::ContentTransferEncoding = "Content-Transfer-Encoding";
std::string MultipartContent::ContentLocation = "Content-Location";
std::string MultipartContent::ContentBase = "Content-Base";
std::string MultipartContent::ContentLength = "Content-Length";
std::string MultipartContent::ContentMD5 = "Content-MD5";
std::string MultipartContent::ContentType =	"Content-Type";

MultipartContent::MultipartContent() {
}

MultipartContent::MultipartContent(const std::string& content)
{
	setContent(content);
}

MultipartContent::MultipartContent(const std::vector<std::string>& headers, const std::string& content)
{
	setContent(content);
	if(headers.size()!=0)
	{
		for(unsigned int i=0;i<headers.size();i++)
		{
			std::vector<std::string> temp;
			if(headers.at(i)=="\r" || headers.at(i)==""|| headers.at(i)=="\r\n" || headers.at(i).find(": ")==std::string::npos)
			{
				continue;
			}

			temp.push_back(headers.at(i).substr(0, headers.at(i).find(": ")));
			temp.push_back(headers.at(i).substr(headers.at(i).find(": ")+2));
			if(temp.size()>1)
			{
				StringUtil::replaceFirst(temp.at(1), "\r", "");
				if(temp.at(0).find("Content-Disposition")!=std::string::npos)
				{
					std::vector<std::string> parmdef;
					StringUtil::split(parmdef, temp.at(1), (";"));
					for(unsigned k=0;k<parmdef.size();k++)
					{
						if(parmdef.at(k)!="" && parmdef.at(k).find("=")!=std::string::npos)
						{
							size_t stpd = parmdef.at(k).find_first_not_of(" ");
							size_t enpd = parmdef.at(k).find_last_not_of(" ");
							std::string propert = parmdef.at(k).substr(stpd,enpd-stpd+1);
							std::vector<std::string> proplr;
							StringUtil::split(proplr, propert, ("="));
							if(proplr.size()==2)
							{
								if(proplr.at(0)=="name" && proplr.at(1)!="\"\"")
								{
									std::string key = proplr.at(1);
									key = key.substr(key.find_first_not_of("\""),key.find_last_not_of("\"")-key.find_first_not_of("\"")+1);
									key = CryptoHandler::urlDecode(key);
									name = key;
								}
								else if(proplr.at(0)=="filename" && proplr.at(1)!="\"\"")
								{
									std::string fna = proplr.at(1);
									fna = fna.substr(fna.find_first_not_of("\""),fna.find_last_not_of("\"")-fna.find_first_not_of("\"")+1);
									fna = CryptoHandler::urlDecode(fna);
									fileName = fna;
								}
							}
						}
					}
				}
				addHeaderValue(temp.at(0), temp.at(1));
			}
		}
		if(this->headers.find(MultipartContent::ContentType)==this->headers.end()) {
			addHeaderValue(MultipartContent::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
		}
	}
}

MultipartContent::~MultipartContent() {
}

std::string MultipartContent::getContent() const {
	return content;
}

void MultipartContent::setContent(const std::string& content) {
	this->content = content;
}

std::string MultipartContent::getFileName() const {
	return fileName;
}

void MultipartContent::setFileName(const std::string& fileName) {
	this->fileName = fileName;
}

std::map<std::string, std::string> MultipartContent::getHeaders() {
	std::map<std::string, std::string> htemp = headers;
	return htemp;
}

std::string MultipartContent::getTempFileName() const {
	return tempFileName;
}

void MultipartContent::setTempFileName(const std::string& tempFileName) {
	this->tempFileName = tempFileName;
}

void MultipartContent::addHeader(std::string header, const std::string& value)
{
	if(headers.find(header)!=headers.end()) {
		headers[header] += "," + value;
	} else {
		headers[header] = value;
	}
}

void MultipartContent::addHeaderValue(std::string header, const std::string& value)
{
	header = StringUtil::camelCasedCopy(header, "-");
	if(header!="")
	{
		if(VALID_HEADERS.find(","+StringUtil::toLowerCopy(header)+",")!=std::string::npos)
		{
			if(headers.find(header)!=headers.end()) {
				headers[header] += "," + value;
			} else {
				headers[header] = value;
			}
		}
		else
		{
			std::vector<std::string> matres = RegexUtil::search(header, "^[a-zA-Z]+[-|a-zA-Z]+[a-zA-Z]*[a-zA-Z]$");
			if(matres.size()==0)
			{
				//std::cout << ("Invalid Header std::string " + header) << std::endl;
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

bool MultipartContent::isHeaderValue(std::string header, const std::string& value, const bool& ignoreCase)
{
	header = StringUtil::camelCasedCopy(header, "-");
	return header!="" && headers.find(header)!=headers.end()
			&& (headers[header]==value ||
					(ignoreCase && StringUtil::toLowerCopy(headers[header])==StringUtil::toLowerCopy(value)));
}

std::string MultipartContent::getName() const {
	return name;
}

void MultipartContent::setName(const std::string& name) {
	this->name = name;
}

std::string MultipartContent::getHeader(const std::string& header)
{
	if(headers.find(header)!=headers.end())
	{
		return headers[header];
	}
	return "";
}

bool MultipartContent::isAFile()
{
	return tempFileName=="" && fileName==""?false:true;
}

bool MultipartContent::isValid()
{
	return headers.size()>0 || content!="" || tempFileName!="" || name!="";
}
