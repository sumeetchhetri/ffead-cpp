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
#include "Logger.h"
using namespace std;

class MultipartContent {
	Logger logger;
	static string VALID_HEADERS;
	map<string,string> headers;
	string content;
	string tempFileName;
	string fileName;
	string name;
	friend class HttpResponse;
	friend class HttpRequest;
public:
	MultipartContent();
	MultipartContent(vector<string> headers);
	virtual ~MultipartContent();
	void addHeaderValue(string header, string value);
	string getContent() const;
	void setContent(string content);
	string getFileName() const;
	void setFileName(string fileName);
	map<string, string> getHeaders() const;
	string getTempFileName() const;
	void setTempFileName(string tempFileName);
	bool isHeaderValue(string header, string value, bool ignoreCase = true);
	string getHeader(string);
	string getName() const;
	void setName(string name);
};

#endif /* MULTIPARTCONTENT_H_ */
