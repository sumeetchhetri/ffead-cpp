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
 * HttpClient.cpp
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#include "HttpClient.h"

void HttpClient::init() {
#ifdef HAVE_CURLLIB
	CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
	if (res != CURLE_OK) {
		throw std::runtime_error("Unable to initialize CURL");
	}
#endif
}

void HttpClient::cleanup() {
#ifdef HAVE_CURLLIB
	curl_global_cleanup();
#endif
}

HttpClient::HttpClient(std::string baseUrl) {
#ifdef HAVE_CURLLIB
	_h = curl_easy_init();
	if (!_h) {
		throw std::runtime_error("Unable to initialize curl handle");
	}
	this->baseUrl = baseUrl;
#endif
}

HttpClient::~HttpClient() {
#ifdef HAVE_CURLLIB
	if (_h) {
		curl_easy_cleanup(_h);
	}
#endif
}

void HttpClient::execute(HttpRequest* request, HttpResponse* response, propMap& props) {
#ifdef HAVE_CURLLIB
	curl_slist* headerList = NULL;

	std::string url = baseUrl + request->url;
	curl_easy_setopt(_h, CURLOPT_URL, url.c_str());

	curl_easy_setopt(_h, CURLOPT_WRITEFUNCTION, onContent);
	curl_easy_setopt(_h, CURLOPT_WRITEDATA, &_bd);

	curl_easy_setopt(_h, CURLOPT_HEADERFUNCTION, onHeaders);
	curl_easy_setopt(_h, CURLOPT_HEADERDATA, &_hd);

	std::string _t;
	for (RMap::const_iterator it = request->headers.begin(); it != request->headers.end(); ++it) {
		_t = it->first + ": " + it->second;
		headerList = curl_slist_append(headerList, _t.c_str());
	}
	curl_easy_setopt(_h, CURLOPT_HTTPHEADER, headerList);

	if(request->authMethod.length()>0 && request->userName.length() > 0) {
		StringUtil::toLower(request->authMethod);
		std::string _t = request->userName + ":" + request->password;
		if(request->authMethod=="none") {
			curl_easy_setopt(_h, CURLOPT_HTTPAUTH, CURLAUTH_NONE);
		} else if(request->authMethod=="basic") {
			curl_easy_setopt(_h, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		} else if(request->authMethod=="digest") {
			curl_easy_setopt(_h, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
		} else if(request->authMethod=="negotiate") {
#ifdef CURLAUTH_NEGOTIATE
			curl_easy_setopt(_h, CURLOPT_HTTPAUTH, CURLAUTH_NEGOTIATE);
#else
			curl_easy_setopt(_h, CURLOPT_HTTPAUTH, CURLAUTH_GSSNEGOTIATE);
#endif
		}
		curl_easy_setopt(_h, CURLOPT_USERPWD, _t.c_str());
	}

	if(request->headers.find(HttpRequest::UserAgent)!=request->headers.end()) {
		curl_easy_setopt(_h, CURLOPT_USERAGENT, request->headers[HttpRequest::UserAgent].c_str());
	} else {
		curl_easy_setopt(_h, CURLOPT_USERAGENT, "ffead-cpp client v2.0");
	}

	if (props["TIMEOUT"].length()>0) {
		try {
			curl_easy_setopt(_h, CURLOPT_TIMEOUT, CastUtil::toInt(props["TIMEOUT"]));
		} catch(const std::exception& e) {
		}
	}

	if (props["FOLLOWLOCATION"]=="true") {
		curl_easy_setopt(_h, CURLOPT_FOLLOWLOCATION, 1L);
		try {
			curl_easy_setopt(_h, CURLOPT_MAXREDIRS, CastUtil::toInt(props["MAXREDIRS"]));
		} catch(const std::exception& e) {
			curl_easy_setopt(_h, CURLOPT_MAXREDIRS, 2);
		}
	}

	if (props["CAINFO"].length()>0) {
		curl_easy_setopt(_h, CURLOPT_CAINFO, props["CAINFO"].c_str());
	}

	if (props["SSLCERT"].length()>0) {
		curl_easy_setopt(_h, CURLOPT_SSLCERT, props["SSLCERT"].c_str());
	}

	if (props["SSLCERTTYPE"].length()>0) {
		curl_easy_setopt(_h, CURLOPT_SSLCERTTYPE, props["SSLCERTTYPE"].c_str());
	}

	if (props["SSLKEY"].length()>0) {
		curl_easy_setopt(_h, CURLOPT_SSLKEY, props["SSLKEY"].c_str());
	}

	if (props["KEYPASSWD"].length()>0) {
		curl_easy_setopt(_h, CURLOPT_KEYPASSWD, props["KEYPASSWD"].c_str());
	}

	if (props["PROXY"].length()>0) {
		curl_easy_setopt(_h, CURLOPT_PROXY, props["PROXY"].c_str());
		curl_easy_setopt(_h, CURLOPT_HTTPPROXYTUNNEL, 1L);
	}

	curl_easy_setopt(_h, CURLOPT_NOSIGNAL, 1);

	CURLcode r = curl_easy_perform(_h);
	if (r != CURLE_OK) {
		switch (r) {
		case CURLE_OPERATION_TIMEDOUT:
			response->setHTTPResponseStatus(HTTPResponseStatus::GatewayTimeout);
			break;
		case CURLE_SSL_CERTPROBLEM:
			response->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			response->content = curl_easy_strerror(r);
			break;
		default:
			response->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
		}
	} else {
		int64_t http_code = 0;
		curl_easy_getinfo(_h, CURLINFO_RESPONSE_CODE, &http_code);
		response->setHTTPResponseStatus(HTTPResponseStatus::getStatusByCode(CastUtil::fromNumber(static_cast<int>(http_code))));
		response->content = _bd;
		if(_hd.size()!=0)
		{
			for(unsigned int i=0;i<_hd.size();i++)
			{
				if(_hd.at(i).find_first_of(":")!=std::string::npos)
				{
					response->addHeaderValue(_hd.at(i).substr(0, _hd.at(i).find_first_of(":")), _hd.at(i).substr(_hd.at(i).find_first_of(":")+1));
				}
			}
		}
	}

	double val;
	curl_easy_getinfo(_h, CURLINFO_TOTAL_TIME, &val);
	props["TOTAL_TIME"] = CastUtil::fromDouble(val);
	curl_easy_getinfo(_h, CURLINFO_NAMELOOKUP_TIME, &val);
	props["NAMELOOKUP_TIME"] = CastUtil::fromDouble(val);
	curl_easy_getinfo(_h, CURLINFO_CONNECT_TIME, &val);
	props["CONNECT_TIME"] = CastUtil::fromDouble(val);
	curl_easy_getinfo(_h, CURLINFO_APPCONNECT_TIME, &val);
	props["APPCONNECT_TIME"] = CastUtil::fromDouble(val);
	curl_easy_getinfo(_h, CURLINFO_PRETRANSFER_TIME, &val);
	props["PRETRANSFER_TIME"] = CastUtil::fromDouble(val);
	curl_easy_getinfo(_h, CURLINFO_STARTTRANSFER_TIME, &val);
	props["STARTTRANSFER_TIME"] = CastUtil::fromDouble(val);
	curl_easy_getinfo(_h, CURLINFO_REDIRECT_TIME, &val);
	props["REDIRECT_TIME"] = CastUtil::fromDouble(val);
	curl_easy_getinfo(_h, CURLINFO_REDIRECT_COUNT, &val);
	props["REDIRECT_COUNT"] = CastUtil::fromDouble(val);
	// free header list
	curl_slist_free_all(headerList);
	// reset curl handle
	curl_easy_reset(_h);
#endif
}

size_t HttpClient::onContent(void *res, size_t len, size_t mb, void *data) {
	std::string* _bd = (std::string*) data;
	_bd->append(reinterpret_cast<char*>(res), len * mb);
	return len * mb;
}

size_t HttpClient::onHeaders(void *res, size_t len, size_t mb, void *data) {
	std::string* _hd = (std::string*) data;
	_hd->append(reinterpret_cast<char*>(res), len * mb);
	return len * mb;
}

