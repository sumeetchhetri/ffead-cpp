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
 * CryptoHandler.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: sumeet
 */

#include "CryptoHandler.h"
#include "iostream"
CryptoHandler::CryptoHandler() {
	// TODO Auto-generated constructor stub

}

CryptoHandler::~CryptoHandler() {
	// TODO Auto-generated destructor stub
}

char* CryptoHandler::base64decode(unsigned char *input, int length)
{
	BIO *b64, *bmem;

	char *buffer = (char *)malloc(length);
	memset(buffer, 0, length);

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new_mem_buf(input, length);
	bmem = BIO_push(b64, bmem);

	BIO_read(bmem, buffer, length);

	BIO_free_all(bmem);

	return buffer;
}

char* CryptoHandler::base64encode(const unsigned char *input, int length)
{
	BIO *bmem, *b64;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, input, length);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	char *buff = (char *)malloc(bptr->length);
	memcpy(buff, bptr->data, bptr->length-1);
	buff[bptr->length-1] = 0;

	BIO_free_all(b64);

	return buff;
}

char* CryptoHandler::hmac_sha1(char* datain,char* keyin,bool base64)
{
	unsigned char* key = (unsigned char*) keyin;
	unsigned char* data = (unsigned char*) datain;
	unsigned char* result,*result1;
	unsigned int result_len = 20,result1_len;

	HMAC_CTX ctx;
	result = (unsigned char*) malloc(sizeof(char) * result_len);

	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(keyin), EVP_sha1(), NULL);
	HMAC_Update(&ctx, data, strlen(datain));
	HMAC_Final(&ctx, result, &result_len);
	HMAC_CTX_cleanup(&ctx);

	//HMAC(EVP_sha1(),key,strlen(keyin),data,strlen(datain),result,&result_len);
	if(base64)
		return base64encode(result,result_len);
	return (char*)result;
}

char* CryptoHandler::hmac_sha256(char* datain,char* keyin,bool base64)
{
	unsigned char* key = (unsigned char*) keyin;
	unsigned char* data = (unsigned char*) datain;
	unsigned char* result;
	unsigned int result_len = 32;

	HMAC_CTX ctx;
	result = (unsigned char*) malloc(sizeof(char) * result_len);

	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(keyin), EVP_sha256(), NULL);
	HMAC_Update(&ctx, data, strlen(datain));
	HMAC_Final(&ctx, result, &result_len);
	HMAC_CTX_cleanup(&ctx);

	if(base64)
		return base64encode(result,result_len);
	return (char*)result;
}

char* CryptoHandler::hmac_sha384(char* datain,char* keyin,bool base64)
{
	unsigned char* key = (unsigned char*) keyin;
	unsigned char* data = (unsigned char*) datain;
	unsigned char* result;
	unsigned int result_len = 48;

	HMAC_CTX ctx;
	result = (unsigned char*) malloc(sizeof(char) * result_len);

	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(keyin), EVP_sha384(), NULL);
	HMAC_Update(&ctx, data, strlen(datain));
	HMAC_Final(&ctx, result, &result_len);
	HMAC_CTX_cleanup(&ctx);

	if(base64)
		return base64encode(result,result_len);
	return (char*)result;
}

char* CryptoHandler::hmac_sha512(char* datain,char* keyin,bool base64)
{
	unsigned char* key = (unsigned char*) keyin;
	unsigned char* data = (unsigned char*) datain;
	unsigned char* result;
	unsigned int result_len = 64;

	HMAC_CTX ctx;
	result = (unsigned char*) malloc(sizeof(char) * result_len);

	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(keyin), EVP_sha512(), NULL);
	HMAC_Update(&ctx, data, strlen(datain));
	HMAC_Final(&ctx, result, &result_len);
	HMAC_CTX_cleanup(&ctx);

	if(base64)
		return base64encode(result,result_len);
	return (char*)result;
}


string CryptoHandler::urlDecode(string str)
{
	string strret = str;
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

string CryptoHandler::urlEncode(string str)
{
	string strret;
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
