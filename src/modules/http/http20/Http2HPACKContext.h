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
 * Http2HPACKContext.h
 *
 *  Created on: 09-Dec-2014
 *      Author: sumeetc
 */

#ifndef HTTP2HPACKCONTEXT_H_
#define HTTP2HPACKCONTEXT_H_
#include "map"
#include "string"
#include "bitset"
#include "vector"
#include <math.h>
#include <stdio.h>
#include "cstring"
#include "CommonUtils.h"
#include "HTTPResponseStatus.h"

class Bits {
	std::vector<std::bitset<8> > bitv;
	std::bitset<30> bits30v;
	int lastbssize;
	uint32_t numbits;
	uint32_t code;
	unsigned char symbol;
	friend class Http2HPACKContext;
	friend class Http2HPACKHeaderTable;
public:
	void print();
	bool matches(const unsigned char& v, const int& nbits);
	void appendBits(std::string& out, const int& last);
	Bits();
	Bits(const uint32_t& code, const uint32_t& rembits, const unsigned char& sym);
	virtual ~Bits();
};


class Http2HPACKHeaderTable {
	static std::map<int, std::map<std::string, std::string> > HPACK_TABLE;
	static std::vector<Bits> HUFFMAN_TABLE;

	static std::map<std::string, std::map<int, Bits> > HUFFMAN_LK_STRINDX_NUMINDX_TABLE;
	static std::map<std::string, std::map<int, int> > HUFFMAN_LK_STRINDX_NUMINDX_BL_TABLE;

	static std::map<int, Bits> HUFFMAN_LK_NUMINDX_TABLE;
	static std::map<int, int> HUFFMAN_LK_NUMINDX_BL_TABLE;

	static std::map<std::string, Bits> cib;
	static std::map<uint32_t, std::bitset<38> > masks;

	static void init();

	std::map<int, std::map<std::string, std::string> > reqContextTable;
	std::map<int, std::map<std::string, std::string> > resContextTable;
	std::map<std::string, int> reqhnIndexTable;
	std::map<std::string, int> reqhnvIndexTable;
	int reqcurrentSize;
	std::map<std::string, int> reshnIndexTable;
	std::map<std::string, int> reshnvIndexTable;
	int rescurrentSize;
	Http2HPACKHeaderTable();
	friend class Http2HPACKContext;
public:
	virtual ~Http2HPACKHeaderTable();
	int getIndexByNameAndValue(const std::string& name, const std::string& value, const bool& isRequest);
	int getIndexByName(const std::string& name, const bool& isRequest);
	std::map<std::string, std::string> getNameAndValueByIndex(const int& index, const bool& isRequest);
	std::string getNameByIndex(const int& index, const bool& isRequest);
	void addHeader(const std::string& name, const std::string& value, const bool& isRequest);
};

class Http2HPACKContext {
public:
	Http2HPACKHeaderTable table;
	bool huffmanEncoding;
	bool decipherHuffmanValue(const int& bitnum, std::bitset<8> obvm, std::bitset<8>& bvm, std::string &out, std::bitset<8>& prev, int& last, uint32_t& indx, std::string& key, std::string& value, int& totbits, int& cub);
	long decodeNumber(const std::string& data, const int& prefixSize, std::bitset<8> bits, size_t& index);
	std::string encodeNumber(long number, const std::vector<bool>& prefixBits);
	std::string encodeString(std::string value);
	std::string decodeString(const std::string& data, size_t& indx);
	std::string encodeHuffman(const std::string& value);
	std::string decodeHuffman(const std::string& value);
	std::string decodeHuffmanOld(std::string value);
	Http2HPACKContext();
	std::string encode(const std::map<std::string, std::string, std::less<>>& headers);
	std::map<std::string, std::string, std::less<>> decode(const std::string& data);
	virtual ~Http2HPACKContext();
};

#endif /* HTTP2HPACKCONTEXT_H_ */
