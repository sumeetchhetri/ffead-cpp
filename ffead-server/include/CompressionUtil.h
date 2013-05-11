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
using namespace std;
#define CHUNK 8192
#define COMPRESS_OK 1
#define COMPRESS_NOK 0

class CompressionUtil {
	static string _compress(char* datain, size_t insize, bool eostream, bool isGz, int chunkSize, bool retu = true, char* toFile = NULL);
	static string _uncompress(char* datain, size_t insize, bool isGz, bool retu = true, char* toFile = NULL);
	static string _compress(char* infile, bool isGz, bool retu, char* toFile);
	static string _uncompress(char* infile, bool isGz, bool retu, char* toFile);
	CompressionUtil();
public:
	static string zlibCompress(char* input, size_t siz, bool eostream = false, int chunkSize = 8192,  bool retu = true, char* toFile = NULL);
	static string zlibCompress(const string& input, bool eostream = false, int chunkSize = 8192,  bool retu = true, char* toFile = NULL);
	static string zlibCompressFile(char* infile, bool retu = true, char* toFile = NULL);
	static string gzipCompress(char* input, size_t siz, bool eostream = false, int chunkSize = 8192,  bool retu = true, char* toFile = NULL);
	static string gzipCompress(const string& input, bool eostream = false, int chunkSize = 8192,  bool retu = true, char* toFile = NULL);
	static string gzipCompressFile(char* infile, bool retu = true, char* toFile = NULL);
	static string zlibUnCompress(char* input, size_t siz, bool retu = true, char* toFile = NULL);
	static string zlibUnCompress(const string& input, bool retu = true, char* toFile = NULL);
	static string zlibUnCompressFile(char* infile, bool retu = true, char* toFile = NULL);
	static string gzipUnCompress(char* input, size_t siz, bool retu = true, char* toFile = NULL);
	static string gzipUnCompress(const string& input, bool retu = true, char* toFile = NULL);
	static string gzipUnCompressFile(char* infile, bool retu = true, char* toFile = NULL);
	virtual ~CompressionUtil();
};

#endif /* COMPRESSIONUTIL_H_ */
