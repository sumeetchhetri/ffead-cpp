/*
 * HttpClient.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef SRC_MODULES_CLIENT_UTIL_HTTP_HTTPCLIENT_H_
#define SRC_MODULES_CLIENT_UTIL_HTTP_HTTPCLIENT_H_

#include <curl/curl.h>
#include <string>
#include <map>
#include <cstdlib>
#include "HttpResponse.h"
#include "HttpRequest.h"


class HttpClient {
	std::string baseUrl;
	std::string _bd;
	std::vector<std::string> _hd;
	CURL* _h;
	static size_t onContent(void *res, size_t len, size_t mb, void *data);
	static size_t onHeaders(void *res, size_t len, size_t mb, void *data);
public:
	static void init();
	static void cleanup();
	HttpClient(std::string baseUrl);
	virtual ~HttpClient();

	void execute(HttpRequest* request, HttpResponse* response, propMap& props);
};

#endif /* SRC_MODULES_CLIENT_UTIL_HTTP_HTTPCLIENT_H_ */
