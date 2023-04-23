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
 * CryptoHandler.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: sumeet
 */

#include "CryptoHandler.h"

#ifndef HAVE_SSLINC
const char CryptoHandler::base46_map[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
#endif

std::string CryptoHandler::base64encodeStr(const std::string& input)
{
	return std::string(base64encode((const unsigned char *)input.c_str(), input.length()));
}

std::string CryptoHandler::base64decodeStr(const std::string& input)
{
	return std::string(base64decode((unsigned char *)input.c_str(), input.length()));
}

char* CryptoHandler::base64decode(unsigned char *input, const int& length)
{
#ifdef HAVE_SSLINC
	BIO *b64, *bmem;

	char *buffer = (char *)malloc(length);
	memset(buffer, 0, length);

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new_mem_buf(input, length);
	bmem = BIO_push(b64, bmem);

	int dlen = BIO_read(bmem, buffer, length);
	buffer[dlen] = '\0';

	BIO_free_all(bmem);

	return buffer;
#else
	char counts = 0;
	char buffer[4];
	char* plain = (char*)malloc(length * 3 / 4);
	int i = 0, p = 0;

	for(i = 0; input[i] != '\0'; i++) {
		char k;
		for(k = 0 ; k < 64 && base46_map[k] != input[i]; k++);
		buffer[counts++] = k;
		if(counts == 4) {
			plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
			if(buffer[2] != 64)
				plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
			if(buffer[3] != 64)
				plain[p++] = (buffer[2] << 6) + buffer[3];
			counts = 0;
		}
	}

	plain[p] = '\0';    /* string padding character */
	return plain;
#endif
}

char* CryptoHandler::base64encode(const unsigned char *input, const int& length)
{
#ifdef HAVE_SSLINC
	BIO *bmem, *b64;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, input, length);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	char *buff = (char *)malloc(bptr->length+1);
	memcpy(buff, bptr->data, bptr->length);
	buff[bptr->length] = '\0';

	BIO_free_all(b64);

	return buff;
#else
	char counts = 0;
	char buffer[3];
	char* cipher = (char*)malloc(length * 4 / 3 + 4);
	int i = 0, c = 0;

	for(i = 0; input[i] != '\0'; i++) {
		buffer[counts++] = input[i];
		if(counts == 3) {
			cipher[c++] = base46_map[buffer[0] >> 2];
			cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
			cipher[c++] = base46_map[((buffer[1] & 0x0f) << 2) + (buffer[2] >> 6)];
			cipher[c++] = base46_map[buffer[2] & 0x3f];
			counts = 0;
		}
	}

	if(counts > 0) {
		cipher[c++] = base46_map[buffer[0] >> 2];
		if(counts == 1) {
			cipher[c++] = base46_map[(buffer[0] & 0x03) << 4];
			cipher[c++] = '=';
		} else {                      // if counts == 2
			cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
			cipher[c++] = base46_map[(buffer[1] & 0x0f) << 2];
		}
		cipher[c++] = '=';
	}

	cipher[c] = '\0';   /* string padding character */
	return cipher;
#endif
}

char* CryptoHandler::hmac_sha1(char* datain, char* keyin, const bool& base64)
{
#ifdef HAVE_SSLINC
	unsigned char* key = (unsigned char*) keyin;
	unsigned char* data = (unsigned char*) datain;
	unsigned char* result;
	unsigned int result_len = 20;

	result = (unsigned char*) malloc(sizeof(char) * result_len);

	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

#if OPENSSL_VERSION_NUMBER < 0x10100000L
	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(keyin), EVP_sha1(), NULL);
	HMAC_Update(&ctx, data, strlen(datain));
	HMAC_Final(&ctx, result, &result_len);
	HMAC_CTX_cleanup(&ctx);
#else
	HMAC_CTX *ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, strlen(keyin), EVP_sha1(), NULL);
	HMAC_Update(ctx, data, strlen(datain));
	HMAC_Final(ctx, result, &result_len);
	HMAC_CTX_free(ctx) ;
	ctx = NULL;
#endif
	//HMAC(EVP_sha1(),key,strlen(keyin),data,strlen(datain),result,&result_len);
	if(base64)
		return base64encode(result,result_len);
	return (char*)result;
#else
	return NULL;
#endif
}

char* CryptoHandler::hmac_sha256(char* datain, char* keyin, const bool& base64)
{
#ifdef HAVE_SSLINC
	unsigned char* key = (unsigned char*) keyin;
	unsigned char* data = (unsigned char*) datain;
	unsigned char* result;
	unsigned int result_len = 32;

	result = (unsigned char*) malloc(sizeof(char) * result_len);

	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

#if OPENSSL_VERSION_NUMBER < 0x10100000L
	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(keyin), EVP_sha256(), NULL);
	HMAC_Update(&ctx, data, strlen(datain));
	HMAC_Final(&ctx, result, &result_len);
	HMAC_CTX_cleanup(&ctx);
#else
	HMAC_CTX *ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, strlen(keyin), EVP_sha256(), NULL);
	HMAC_Update(ctx, data, strlen(datain));
	HMAC_Final(ctx, result, &result_len);
	HMAC_CTX_free(ctx) ;
	ctx = NULL;
#endif

	if(base64)
		return base64encode(result,result_len);
	return (char*)result;
#else
	return NULL;
#endif
}

char* CryptoHandler::hmac_sha384(char* datain, char* keyin, const bool& base64)
{
#ifdef HAVE_SSLINC
	unsigned char* key = (unsigned char*) keyin;
	unsigned char* data = (unsigned char*) datain;
	unsigned char* result;
	unsigned int result_len = 48;

	result = (unsigned char*) malloc(sizeof(char) * result_len);

	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

#if OPENSSL_VERSION_NUMBER < 0x10100000L
	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(keyin), EVP_sha384(), NULL);
	HMAC_Update(&ctx, data, strlen(datain));
	HMAC_Final(&ctx, result, &result_len);
	HMAC_CTX_cleanup(&ctx);
#else
	HMAC_CTX *ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, strlen(keyin), EVP_sha384(), NULL);
	HMAC_Update(ctx, data, strlen(datain));
	HMAC_Final(ctx, result, &result_len);
	HMAC_CTX_free(ctx) ;
	ctx = NULL;
#endif

	if(base64)
		return base64encode(result,result_len);
	return (char*)result;
#else
	return NULL;
#endif
}

char* CryptoHandler::hmac_sha512(char* datain, char* keyin, const bool& base64)
{
#ifdef HAVE_SSLINC
	unsigned char* key = (unsigned char*) keyin;
	unsigned char* data = (unsigned char*) datain;
	unsigned char* result;
	unsigned int result_len = 64;

	result = (unsigned char*) malloc(sizeof(char) * result_len);

	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

#if OPENSSL_VERSION_NUMBER < 0x10100000L
	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(keyin), EVP_sha512(), NULL);
	HMAC_Update(&ctx, data, strlen(datain));
	HMAC_Final(&ctx, result, &result_len);
	HMAC_CTX_cleanup(&ctx);
#else
	HMAC_CTX *ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, strlen(keyin), EVP_sha512(), NULL);
	HMAC_Update(ctx, data, strlen(datain));
	HMAC_Final(ctx, result, &result_len);
	HMAC_CTX_free(ctx) ;
	ctx = NULL;
#endif

	if(base64)
		return base64encode(result,result_len);
	return (char*)result;
#else
	return NULL;
#endif
}


std::string CryptoHandler::urlDecode(const std::string& str)
{
	std::string strret = str;
	StringUtil::replaceAll(strret,"%25","%");
	StringUtil::replaceAll(strret,"%3D","=");
	StringUtil::replaceAll(strret,"%3A",":");
	StringUtil::replaceAll(strret,"%20"," ");
	StringUtil::replaceAll(strret,"%5E","^");
	StringUtil::replaceAll(strret,"%26","&");
	StringUtil::replaceAll(strret,"%60","`");
	StringUtil::replaceAll(strret,"%7B","{");
	StringUtil::replaceAll(strret,"%7D","}");
	StringUtil::replaceAll(strret,"%7C","|");
	StringUtil::replaceAll(strret,"%5D","]");
	StringUtil::replaceAll(strret,"%5B","[");
	StringUtil::replaceAll(strret,"%22","\"");
	StringUtil::replaceAll(strret,"%3C","<");
	StringUtil::replaceAll(strret,"%3E",">");
	StringUtil::replaceAll(strret,"%5C","\\");
	StringUtil::replaceAll(strret,"%23","#");
	StringUtil::replaceAll(strret,"%3F","?");
	StringUtil::replaceAll(strret,"%2F","/");
	StringUtil::replaceAll(strret,"%2B","+");
	return strret;
}

std::string CryptoHandler::urlEncode(const std::string& str)
{
	std::string strret;
	int len = str.length();
	for(int i=0; i<len; i++)
	{
		switch(str[i])
		{
		case '%':
			strret.append("%25");
			break;
		case '=':
			strret.append("%3D");
			break;
		case ':':
			strret.append("%3A");
			break;
		case ' ':
			strret.append("%20");
			break;
		case '^':
			strret.append("%5E");
			break;
		case '&':
			strret.append("%26");
			break;
		case '`':
			strret.append("%60");
			break;
		case '{':
			strret.append("%7B");
			break;
		case '}':
			strret.append("%7D");
			break;
		case '|':
			strret.append("%7C");
			break;
		case ']':
			strret.append("%5D");
			break;
		case '[':
			strret.append("%5B");
			break;
		case '"':
			strret.append("%22");
			break;
		case '<':
			strret.append("%3C");
			break;
		case '>':
			strret.append("%3E");
			break;
		case '\\':
			strret.append("%5C");
			break;
		case '#':
			strret.append("%23");
			break;
		case '?':
			strret.append("%3F");
			break;
		case '/':
			strret.append("%2F");
			break;
		case '+':
			strret.append("%2B");
			break;
		default:
			strret.push_back(str[i]);
			break;
		}
	}
	return strret;
}

std::string CryptoHandler::sha1(const std::string& data)
{
#ifdef HAVE_SSLINC
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*)data.c_str(), data.length(), hash);
	// Transform byte-array to string
	/*std::stringstream shastr;
	shastr << std::hex << std::setfill('0');
	for (int i=0;i<SHA_DIGEST_LENGTH;i++)
	{
		shastr << std::setw(2) << (int)hash[i];
	}
	return shastr.str();*/
	return std::string((const char*)hash, SHA_DIGEST_LENGTH);
#else
	return data;
#endif
}


std::string_view CryptoHandler::sanitizeHtmlFast(const uint8_t *buf, size_t size, std::string& data, bool& allocd) {
	return hesc_escape_html(buf, size, data, allocd);
}

void CryptoHandler::sanitizeHtml(std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    data.swap(buffer);
}

void CryptoHandler::deSanitizeHtml(std::string& strret) {
	StringUtil::replaceAll(strret,"&amp;","&");
	StringUtil::replaceAll(strret,"&quot;","\"");
	StringUtil::replaceAll(strret,"&apos;","'");
	StringUtil::replaceAll(strret,"&lt;","<");
	StringUtil::replaceAll(strret,"&gt;",">");
}

std::string CryptoHandler::md5(unsigned char* buf, unsigned int buf_size)
{
	std::string result;
#ifdef HAVE_SSLINC
    EVP_MD_CTX *mdctx;
    unsigned char *md5_digest;
    unsigned int md5_digest_len = EVP_MD_size(EVP_md5());
    
    // MD5_Init
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

    // MD5_Update
    EVP_DigestUpdate(mdctx, buf, buf_size);

    // MD5_Final
    md5_digest = (unsigned char *)OPENSSL_malloc(md5_digest_len);
    EVP_DigestFinal_ex(mdctx, md5_digest, &md5_digest_len);
    EVP_MD_CTX_free(mdctx);
    
    result.reserve(32);  // C++11 only, otherwise ignore
    for (std::size_t i = 0; i != md5_digest_len; ++i)
    {
        result += "0123456789abcdef"[md5_digest[i] / 16];
        result += "0123456789abcdef"[md5_digest[i] % 16];
    }
#endif   
    return result;
}
