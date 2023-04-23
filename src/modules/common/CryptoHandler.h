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
 * CryptoHandler.h
 *
 *  Created on: Nov 24, 2010
 *      Author: sumeet
 */

#ifndef CRYPTOHANDLER_H_
#define CRYPTOHANDLER_H_
#include "AppDefines.h"
#include "Compatibility.h"  // WinSock subsystem
#ifdef HAVE_SSLINC
#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#endif
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "string"
#include "cstring"
#include <stdlib.h>
#include "StringUtil.h"
#include <iomanip>
#include "hescape.h"

class CryptoHandler {
#ifndef HAVE_SSLINC
	static const char base46_map[];
#endif
public:
	static char* hmac_sha1(char*, char*, const bool&);
	static char* hmac_sha256(char*, char*, const bool&);
	static char* hmac_sha384(char*, char*, const bool&);
	static char* hmac_sha512(char*, char*, const bool&);
	static std::string sha1(const std::string&);
	static std::string urlEncode(const std::string& str);
	static std::string urlDecode(const std::string& str);
	static std::string_view sanitizeHtmlFast(const uint8_t *buf, size_t size, std::string& data, bool& allocd);
	static void sanitizeHtml(std::string& data);
	static void deSanitizeHtml(std::string& strret);
	static std::string base64encodeStr(const std::string& input);
	static std::string base64decodeStr(const std::string& input);
	static char* base64encode(const unsigned char *input, const int& length);
	static char* base64decode(unsigned char *input, const int& length);
	static std::string md5(unsigned char* buf, unsigned int buf_size);
};

#endif /* CRYPTOHANDLER_H_ */
