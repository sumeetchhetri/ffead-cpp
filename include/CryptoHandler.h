/*
 * CryptoHandler.h
 *
 *  Created on: Nov 24, 2010
 *      Author: sumeet
 */

#ifndef CRYPTOHANDLER_H_
#define CRYPTOHANDLER_H_
#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
using namespace std;
class CryptoHandler {
	CryptoHandler();
	virtual ~CryptoHandler();
public:
	static char* base64encode(const unsigned char *input, int length);
	static char* base64decode(unsigned char *input, int length);
	static char* hmac_sha1(char*,char*,bool);
	static char* hmac_sha256(char*,char*,bool);
	static char* hmac_sha384(char*,char*,bool);
	static char* hmac_sha512(char*,char*,bool);
	static string urlEncode(string str);
	static string urlDecode(string str);
};

#endif /* CRYPTOHANDLER_H_ */
