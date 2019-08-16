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
	static std::map<int, std::map<std::string_view, std::string_view> > HPACK_TABLE;
	static std::vector<Bits> HUFFMAN_TABLE;

	static std::map<std::string_view, std::map<int, Bits> > HUFFMAN_LK_STRINDX_NUMINDX_TABLE;
	static std::map<std::string_view, std::map<int, int> > HUFFMAN_LK_STRINDX_NUMINDX_BL_TABLE;

	static std::map<int, Bits> HUFFMAN_LK_NUMINDX_TABLE;
	static std::map<int, int> HUFFMAN_LK_NUMINDX_BL_TABLE;

	static std::map<std::string_view, Bits> cib;
	static std::map<uint32_t, std::bitset<38> > masks;

	static void init();

	std::map<int, std::map<std::string_view, std::string_view> > reqContextTable;
	std::map<int, std::map<std::string_view, std::string_view> > resContextTable;
	std::map<std::string_view, int> reqhnIndexTable;
	std::map<std::string_view, int> reqhnvIndexTable;
	int reqcurrentSize;
	std::map<std::string_view, int> reshnIndexTable;
	std::map<std::string_view, int> reshnvIndexTable;
	int rescurrentSize;
	Http2HPACKHeaderTable();
	friend class Http2HPACKContext;
public:
	virtual ~Http2HPACKHeaderTable();
	int getIndexByNameAndValue(std::string_view name, std::string_view value, const bool& isRequest);
	int getIndexByName(std::string_view name, const bool& isRequest);
	std::map<std::string_view, std::string_view> getNameAndValueByIndex(const int& index, const bool& isRequest);
	std::string_view getNameByIndex(const int& index, const bool& isRequest);
	void addHeader(std::string_view name, std::string_view value, const bool& isRequest);
};

class Http2HPACKContext {
public:
	Http2HPACKHeaderTable table;
	bool huffmanEncoding;
	bool decipherHuffmanValue(const int& bitnum, std::bitset<8> obvm, std::bitset<8>& bvm, std::string &out, std::bitset<8>& prev, int& last, uint32_t& indx, std::string& key, std::string_view& value, int& totbits, int& cub);
	long decodeNumber(std::string_view data, const int& prefixSize, std::bitset<8> bits, size_t& index);
	std::string_view encodeNumber(long number, const std::vector<bool>& prefixBits);
	std::string_view encodeString(std::string_view value);
	std::string_view decodeString(std::string_view data, size_t& indx);
	std::string_view encodeHuffman(std::string_view value);
	std::string_view decodeHuffman(std::string_view value);
	std::string_view decodeHuffmanOld(std::string_view value);
	Http2HPACKContext();
	std::string_view encode(const std::map<std::string_view, std::string_view, cicomp>& headers);
	std::map<std::string_view, std::string_view, cicomp> decode(std::string_view data);
	virtual ~Http2HPACKContext();
};

#endif /* HTTP2HPACKCONTEXT_H_ */
