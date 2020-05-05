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
 * CompressionUtil.h
 *
 *  Created on: 19-Apr-2013
 *      Author: sumeetc
 */

#ifndef COMPRESSIONUTIL_H_
#define COMPRESSIONUTIL_H_
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"
#include "string"
#include "math.h"
#include "fstream"

#define CHUNK 8192
#define COMPRESS_OK 1
#define COMPRESS_NOK 0

class CompressionUtil {
	static std::string _compress(char* datain, const size_t& insize, const bool& eostream, const bool& isGz, const int& chunkSize, const bool& retu= true, char* toFile= NULL);
	static std::string _uncompress(char* datain, const size_t& insize, const bool& isGz, const bool& retu= true, char* toFile= NULL);
	static std::string _compress(char* infile, const bool& isGz, const bool& retu, char* toFile);
	static std::string _uncompress(char* infile, const bool& isGz, const bool& retu, char* toFile);
	CompressionUtil();
public:
	static std::string zlibCompress(char* input, const size_t& siz, const bool& eostream= false, const int& chunkSize= 8192, const bool& retu= true, char* toFile= NULL);
	static std::string zlibCompress(const std::string& input, const bool& eostream= false, const int& chunkSize= 8192, const bool& retu= true, char* toFile= NULL);
	static std::string zlibCompressFile(char* infile, const bool& retu= true, char* toFile= NULL);
	static std::string gzipCompress(char* input, const size_t& siz, const bool& eostream= false, const int& chunkSize= 8192, const bool& retu= true, char* toFile= NULL);
	static std::string gzipCompress(const std::string& input, const bool& eostream= false, const int& chunkSize= 8192, const bool& retu= true, char* toFile= NULL);
	static std::string gzipCompressFile(char* infile, const bool& retu= true, char* toFile= NULL);
	static std::string zlibUnCompress(char* input, const size_t& siz, const bool& retu= true, char* toFile= NULL);
	static std::string zlibUnCompress(const std::string& input, const bool& retu= true, char* toFile= NULL);
	static std::string zlibUnCompressFile(char* infile, const bool& retu= true, char* toFile= NULL);
	static std::string gzipUnCompress(char* input, const size_t& siz, const bool& retu= true, char* toFile= NULL);
	static std::string gzipUnCompress(const std::string& input, const bool& retu= true, char* toFile= NULL);
	static std::string gzipUnCompressFile(char* infile, const bool& retu= true, char* toFile= NULL);
	virtual ~CompressionUtil();
};

#endif /* COMPRESSIONUTIL_H_ */
