/*
 * Http2HPACKContext.cpp
 *
 *  Created on: 09-Dec-2014
 *      Author: sumeetc
 */

#include "Http2HPACKContext.h"

std::map<int, std::map<std::string_view, std::string_view> > Http2HPACKHeaderTable::HPACK_TABLE;
std::vector<Bits> Http2HPACKHeaderTable::HUFFMAN_TABLE;
std::map<std::string_view, std::map<int, int> > Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_BL_TABLE;
std::map<std::string_view, std::map<int, Bits> > Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_TABLE;
std::map<int, Bits> Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_TABLE;
std::map<int, int> Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_BL_TABLE;
std::map<std::string_view, Bits> Http2HPACKHeaderTable::cib;
std::map<uint32_t, std::bitset<38> > Http2HPACKHeaderTable::masks;

void Bits::print() {
	std::cout << "0x";
	for (int var = (int)bitv.size()-1; var >= 0; var--) {
		char hexVal[20];
		memset(hexVal, 0, sizeof(hexVal));
		sprintf(hexVal, "%x", (unsigned char)bitv.at(var).to_ulong());
		std::string_view hexstr(hexVal);
		if(hexstr.length()==1)
			hexstr = "0" + hexstr;
		std::cout << hexstr;
	}
	std::cout << std::endl;
}

bool Bits::matches(const unsigned char& v, const int& nbits) {
	std::bitset<8> bv(v);
	for (int var = nbits; var < 8; ++var) {
		bv.reset(var);
	}
	if(bv==bitv.at(bitv.size()-1))
		return true;
	return false;
}

void Bits::appendBits(std::string& out, const int& last) {
	for (int var = 0; var < (int)bitv.size(); ++var) {
		std::bitset<8> bb = bitv.at(var);
		unsigned char v = bb.to_ulong();
		if(last!=0) {
			unsigned long long lon = (v << last);
			std::bitset<16> bb1(lon);
			std::bitset<8> bb2;
			for (int s2 = 0; s2 < 8; ++s2) {
				bb2.set(s2, bb1.test(s2));
			}
			v = bb2.to_ulong();
			for (int si = 8; si < 8+last; ++si) {
				if(bb1.test(si))
				{
					out[out.length()-1] |= 0x01 << (si-8);
				}
			}
		}
		int lstbs = var==(int)bitv.size()-1?lastbssize:8;
		if(lastbssize==lstbs && lastbssize==0)
			lstbs = 8;
		if(last<lstbs)
		{
			out.push_back(v);
		}
	}
}

Bits::Bits() {
	numbits = 0;
	symbol = '\0';
	code = 0;
	lastbssize = 0;
}

Bits::Bits(const uint32_t& code, const uint32_t& rembits, const unsigned char& sym) {
	this->code = code;
	symbol = sym;
	numbits = rembits;
	lastbssize = rembits%8;
	int ind = rembits/8;
	/*if(rembits==8)
	{
		std::bitset<8> bits(code);
		bitv.push_back(bits);
	}
	else*/
	{
		unsigned long long lon = code;
		lon = lon << (8-lastbssize);
		int c = ind + (lastbssize>0?1:0);
		bits30v = std::bitset<30>(code << (30-numbits));
		for (int i = 0; i < c; ++i)
		{
			int offset = (ind - i) * 8;
			unsigned char v = ((lon >> offset) & 0xFF);
			std::bitset<8> bits(v);
			bitv.push_back(bits);
		}
	}
	/*if(lastbssize!=0) {
		int offset = 0;
		unsigned char v = ((lon >> offset) & 0xFF);
		std::bitset<8> bits(v);
		bitv.push_back(bits);
	}*/
}

Bits::~Bits(){
}


void Http2HPACKHeaderTable::init() {
	if(HPACK_TABLE.size()>0)return;
	HPACK_TABLE[1].insert(std::pair<std::string_view, std::string_view>(":authority", ""));
	HPACK_TABLE[2].insert(std::pair<std::string_view, std::string_view>(":method", "GET"));
	HPACK_TABLE[3].insert(std::pair<std::string_view, std::string_view>(":method", "POST"));
	HPACK_TABLE[4].insert(std::pair<std::string_view, std::string_view>(":path", "/"));
	HPACK_TABLE[5].insert(std::pair<std::string_view, std::string_view>(":path", "/index.html"));
	HPACK_TABLE[6].insert(std::pair<std::string_view, std::string_view>(":scheme", "http"));
	HPACK_TABLE[7].insert(std::pair<std::string_view, std::string_view>(":scheme", "https"));
	HPACK_TABLE[8].insert(std::pair<std::string_view, std::string_view>(":status", "200"));
	HPACK_TABLE[9].insert(std::pair<std::string_view, std::string_view>(":status", "204"));
	HPACK_TABLE[10].insert(std::pair<std::string_view, std::string_view>(":status", "206"));
	HPACK_TABLE[11].insert(std::pair<std::string_view, std::string_view>(":status", "304"));
	HPACK_TABLE[12].insert(std::pair<std::string_view, std::string_view>(":status", "400"));
	HPACK_TABLE[13].insert(std::pair<std::string_view, std::string_view>(":status", "404"));
	HPACK_TABLE[14].insert(std::pair<std::string_view, std::string_view>(":status", "500"));
	HPACK_TABLE[15].insert(std::pair<std::string_view, std::string_view>("accept-charset", ""));
	HPACK_TABLE[16].insert(std::pair<std::string_view, std::string_view>("accept-encoding", "gzip, deflate"));
	HPACK_TABLE[17].insert(std::pair<std::string_view, std::string_view>("accept-language", ""));
	HPACK_TABLE[18].insert(std::pair<std::string_view, std::string_view>("accept-ranges", ""));
	HPACK_TABLE[19].insert(std::pair<std::string_view, std::string_view>("accept", ""));
	HPACK_TABLE[20].insert(std::pair<std::string_view, std::string_view>("access-control-allow-origin", ""));
	HPACK_TABLE[21].insert(std::pair<std::string_view, std::string_view>("age", ""));
	HPACK_TABLE[22].insert(std::pair<std::string_view, std::string_view>("allow", ""));
	HPACK_TABLE[23].insert(std::pair<std::string_view, std::string_view>("authorization", ""));
	HPACK_TABLE[24].insert(std::pair<std::string_view, std::string_view>("cache-control", ""));
	HPACK_TABLE[25].insert(std::pair<std::string_view, std::string_view>("content-disposition", ""));
	HPACK_TABLE[26].insert(std::pair<std::string_view, std::string_view>("content-encoding", ""));
	HPACK_TABLE[27].insert(std::pair<std::string_view, std::string_view>("content-language", ""));
	HPACK_TABLE[28].insert(std::pair<std::string_view, std::string_view>("content-length", ""));
	HPACK_TABLE[29].insert(std::pair<std::string_view, std::string_view>("content-location", ""));
	HPACK_TABLE[30].insert(std::pair<std::string_view, std::string_view>("content-range", ""));
	HPACK_TABLE[31].insert(std::pair<std::string_view, std::string_view>("content-type", ""));
	HPACK_TABLE[32].insert(std::pair<std::string_view, std::string_view>("cookie", ""));
	HPACK_TABLE[33].insert(std::pair<std::string_view, std::string_view>("date", ""));
	HPACK_TABLE[34].insert(std::pair<std::string_view, std::string_view>("etag", ""));
	HPACK_TABLE[35].insert(std::pair<std::string_view, std::string_view>("expect", ""));
	HPACK_TABLE[36].insert(std::pair<std::string_view, std::string_view>("expires", ""));
	HPACK_TABLE[37].insert(std::pair<std::string_view, std::string_view>("from", ""));
	HPACK_TABLE[38].insert(std::pair<std::string_view, std::string_view>("host", ""));
	HPACK_TABLE[39].insert(std::pair<std::string_view, std::string_view>("if-match", ""));
	HPACK_TABLE[40].insert(std::pair<std::string_view, std::string_view>("if-modified-since", ""));
	HPACK_TABLE[41].insert(std::pair<std::string_view, std::string_view>("if-none-match", ""));
	HPACK_TABLE[42].insert(std::pair<std::string_view, std::string_view>("if-range", ""));
	HPACK_TABLE[43].insert(std::pair<std::string_view, std::string_view>("if-unmodified-since", ""));
	HPACK_TABLE[44].insert(std::pair<std::string_view, std::string_view>("last-modified", ""));
	HPACK_TABLE[45].insert(std::pair<std::string_view, std::string_view>("link", ""));
	HPACK_TABLE[46].insert(std::pair<std::string_view, std::string_view>("location", ""));
	HPACK_TABLE[47].insert(std::pair<std::string_view, std::string_view>("max-forwards", ""));
	HPACK_TABLE[48].insert(std::pair<std::string_view, std::string_view>("proxy-authenticate", ""));
	HPACK_TABLE[49].insert(std::pair<std::string_view, std::string_view>("proxy-authorization", ""));
	HPACK_TABLE[50].insert(std::pair<std::string_view, std::string_view>("range", ""));
	HPACK_TABLE[51].insert(std::pair<std::string_view, std::string_view>("referer", ""));
	HPACK_TABLE[52].insert(std::pair<std::string_view, std::string_view>("refresh", ""));
	HPACK_TABLE[53].insert(std::pair<std::string_view, std::string_view>("retry-after", ""));
	HPACK_TABLE[54].insert(std::pair<std::string_view, std::string_view>("server", ""));
	HPACK_TABLE[55].insert(std::pair<std::string_view, std::string_view>("set-cookie", ""));
	HPACK_TABLE[56].insert(std::pair<std::string_view, std::string_view>("strict-transport-security", ""));
	HPACK_TABLE[57].insert(std::pair<std::string_view, std::string_view>("transfer-encoding", ""));
	HPACK_TABLE[58].insert(std::pair<std::string_view, std::string_view>("user-agent", ""));
	HPACK_TABLE[59].insert(std::pair<std::string_view, std::string_view>("vary", ""));
	HPACK_TABLE[60].insert(std::pair<std::string_view, std::string_view>("via", ""));
	HPACK_TABLE[61].insert(std::pair<std::string_view, std::string_view>("www-authenticate", ""));

	HUFFMAN_TABLE.push_back(Bits(0x1ff8u, 13, 0));
	HUFFMAN_TABLE.push_back(Bits(0x7fffd8u, 23, 1));
	HUFFMAN_TABLE.push_back(Bits(0xfffffe2u, 28, 2));
	HUFFMAN_TABLE.push_back(Bits(0xfffffe3u, 28, 3));
	HUFFMAN_TABLE.push_back(Bits(0xfffffe4u, 28, 4));
	HUFFMAN_TABLE.push_back(Bits(0xfffffe5u, 28, 5));
	HUFFMAN_TABLE.push_back(Bits(0xfffffe6u, 28, 6));
	HUFFMAN_TABLE.push_back(Bits(0xfffffe7u, 28, 7));
	HUFFMAN_TABLE.push_back(Bits(0xfffffe8u, 28, 8));
	HUFFMAN_TABLE.push_back(Bits(0xffffeau, 24, 9));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffffcu, 30, 10));
	HUFFMAN_TABLE.push_back(Bits(0xfffffe9u, 28, 11));
	HUFFMAN_TABLE.push_back(Bits(0xfffffeau, 28, 12));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffffdu, 30, 13));
	HUFFMAN_TABLE.push_back(Bits(0xfffffebu, 28, 14));
	HUFFMAN_TABLE.push_back(Bits(0xfffffecu, 28, 15));
	HUFFMAN_TABLE.push_back(Bits(0xfffffedu, 28, 16));
	HUFFMAN_TABLE.push_back(Bits(0xfffffeeu, 28, 17));
	HUFFMAN_TABLE.push_back(Bits(0xfffffefu, 28, 18));
	HUFFMAN_TABLE.push_back(Bits(0xffffff0u, 28, 19));
	HUFFMAN_TABLE.push_back(Bits(0xffffff1u, 28, 20));
	HUFFMAN_TABLE.push_back(Bits(0xffffff2u, 28, 21));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffffeu, 30, 22));
	HUFFMAN_TABLE.push_back(Bits(0xffffff3u, 28, 23));
	HUFFMAN_TABLE.push_back(Bits(0xffffff4u, 28, 24));
	HUFFMAN_TABLE.push_back(Bits(0xffffff5u, 28, 25));
	HUFFMAN_TABLE.push_back(Bits(0xffffff6u, 28, 26));
	HUFFMAN_TABLE.push_back(Bits(0xffffff7u, 28, 27));
	HUFFMAN_TABLE.push_back(Bits(0xffffff8u, 28, 28));
	HUFFMAN_TABLE.push_back(Bits(0xffffff9u, 28, 29));
	HUFFMAN_TABLE.push_back(Bits(0xffffffau, 28, 30));
	HUFFMAN_TABLE.push_back(Bits(0xffffffbu, 28, 31));
	HUFFMAN_TABLE.push_back(Bits(0x14u, 6, 32));
	HUFFMAN_TABLE.push_back(Bits(0x3f8u, 10, 33));
	HUFFMAN_TABLE.push_back(Bits(0x3f9u, 10, 34));
	HUFFMAN_TABLE.push_back(Bits(0xffau, 12, 35));
	HUFFMAN_TABLE.push_back(Bits(0x1ff9u, 13, 36));
	HUFFMAN_TABLE.push_back(Bits(0x15u, 6, 37));
	HUFFMAN_TABLE.push_back(Bits(0xf8u, 8, 38));
	HUFFMAN_TABLE.push_back(Bits(0x7fau, 11, 39));
	HUFFMAN_TABLE.push_back(Bits(0x3fau, 10, 40));
	HUFFMAN_TABLE.push_back(Bits(0x3fbu, 10, 41));
	HUFFMAN_TABLE.push_back(Bits(0xf9u, 8, 42));
	HUFFMAN_TABLE.push_back(Bits(0x7fbu, 11, 43));
	HUFFMAN_TABLE.push_back(Bits(0xfau, 8, 44));
	HUFFMAN_TABLE.push_back(Bits(0x16u, 6, 45));
	HUFFMAN_TABLE.push_back(Bits(0x17u, 6, 46));
	HUFFMAN_TABLE.push_back(Bits(0x18u, 6, 47));
	HUFFMAN_TABLE.push_back(Bits(0x0u, 5, 48));
	HUFFMAN_TABLE.push_back(Bits(0x1u, 5, 49));
	HUFFMAN_TABLE.push_back(Bits(0x2u, 5, 50));
	HUFFMAN_TABLE.push_back(Bits(0x19u, 6, 51));
	HUFFMAN_TABLE.push_back(Bits(0x1au, 6, 52));
	HUFFMAN_TABLE.push_back(Bits(0x1bu, 6, 53));
	HUFFMAN_TABLE.push_back(Bits(0x1cu, 6, 54));
	HUFFMAN_TABLE.push_back(Bits(0x1du, 6, 55));
	HUFFMAN_TABLE.push_back(Bits(0x1eu, 6, 56));
	HUFFMAN_TABLE.push_back(Bits(0x1fu, 6, 57));
	HUFFMAN_TABLE.push_back(Bits(0x5cu, 7, 58));
	HUFFMAN_TABLE.push_back(Bits(0xfbu, 8, 59));
	HUFFMAN_TABLE.push_back(Bits(0x7ffcu, 15, 60));
	HUFFMAN_TABLE.push_back(Bits(0x20u, 6, 61));
	HUFFMAN_TABLE.push_back(Bits(0xffbu, 12, 62));
	HUFFMAN_TABLE.push_back(Bits(0x3fcu, 10, 63));
	HUFFMAN_TABLE.push_back(Bits(0x1ffau, 13, 64));
	HUFFMAN_TABLE.push_back(Bits(0x21u, 6, 65));
	HUFFMAN_TABLE.push_back(Bits(0x5du, 7, 66));
	HUFFMAN_TABLE.push_back(Bits(0x5eu, 7, 67));
	HUFFMAN_TABLE.push_back(Bits(0x5fu, 7, 68));
	HUFFMAN_TABLE.push_back(Bits(0x60u, 7, 69));
	HUFFMAN_TABLE.push_back(Bits(0x61u, 7, 70));
	HUFFMAN_TABLE.push_back(Bits(0x62u, 7, 71));
	HUFFMAN_TABLE.push_back(Bits(0x63u, 7, 72));
	HUFFMAN_TABLE.push_back(Bits(0x64u, 7, 73));
	HUFFMAN_TABLE.push_back(Bits(0x65u, 7, 74));
	HUFFMAN_TABLE.push_back(Bits(0x66u, 7, 75));
	HUFFMAN_TABLE.push_back(Bits(0x67u, 7, 76));
	HUFFMAN_TABLE.push_back(Bits(0x68u, 7, 77));
	HUFFMAN_TABLE.push_back(Bits(0x69u, 7, 78));
	HUFFMAN_TABLE.push_back(Bits(0x6au, 7, 79));
	HUFFMAN_TABLE.push_back(Bits(0x6bu, 7, 80));
	HUFFMAN_TABLE.push_back(Bits(0x6cu, 7, 81));
	HUFFMAN_TABLE.push_back(Bits(0x6du, 7, 82));
	HUFFMAN_TABLE.push_back(Bits(0x6eu, 7, 83));
	HUFFMAN_TABLE.push_back(Bits(0x6fu, 7, 84));
	HUFFMAN_TABLE.push_back(Bits(0x70u, 7, 85));
	HUFFMAN_TABLE.push_back(Bits(0x71u, 7, 86));
	HUFFMAN_TABLE.push_back(Bits(0x72u, 7, 87));
	HUFFMAN_TABLE.push_back(Bits(0xfcu, 8, 88));
	HUFFMAN_TABLE.push_back(Bits(0x73u, 7, 89));
	HUFFMAN_TABLE.push_back(Bits(0xfdu, 8, 90));
	HUFFMAN_TABLE.push_back(Bits(0x1ffbu, 13, 91));
	HUFFMAN_TABLE.push_back(Bits(0x7fff0u, 19, 92));
	HUFFMAN_TABLE.push_back(Bits(0x1ffcu, 13, 93));
	HUFFMAN_TABLE.push_back(Bits(0x3ffcu, 14, 94));
	HUFFMAN_TABLE.push_back(Bits(0x22u, 6, 95));
	HUFFMAN_TABLE.push_back(Bits(0x7ffdu, 15, 96));
	HUFFMAN_TABLE.push_back(Bits(0x3u, 5, 97));
	HUFFMAN_TABLE.push_back(Bits(0x23u, 6, 98));
	HUFFMAN_TABLE.push_back(Bits(0x4u, 5, 99));
	HUFFMAN_TABLE.push_back(Bits(0x24u, 6, 100));
	HUFFMAN_TABLE.push_back(Bits(0x5u, 5, 101));
	HUFFMAN_TABLE.push_back(Bits(0x25u, 6, 102));
	HUFFMAN_TABLE.push_back(Bits(0x26u, 6, 103));
	HUFFMAN_TABLE.push_back(Bits(0x27u, 6, 104));
	HUFFMAN_TABLE.push_back(Bits(0x6u, 5, 105));
	HUFFMAN_TABLE.push_back(Bits(0x74u, 7, 106));
	HUFFMAN_TABLE.push_back(Bits(0x75u, 7, 107));
	HUFFMAN_TABLE.push_back(Bits(0x28u, 6, 108));
	HUFFMAN_TABLE.push_back(Bits(0x29u, 6, 109));
	HUFFMAN_TABLE.push_back(Bits(0x2au, 6, 110));
	HUFFMAN_TABLE.push_back(Bits(0x7u, 5, 111));
	HUFFMAN_TABLE.push_back(Bits(0x2bu, 6, 112));
	HUFFMAN_TABLE.push_back(Bits(0x76u, 7, 113));
	HUFFMAN_TABLE.push_back(Bits(0x2cu, 6, 114));
	HUFFMAN_TABLE.push_back(Bits(0x8u, 5, 115));
	HUFFMAN_TABLE.push_back(Bits(0x9u, 5, 116));
	HUFFMAN_TABLE.push_back(Bits(0x2du, 6, 117));
	HUFFMAN_TABLE.push_back(Bits(0x77u, 7, 118));
	HUFFMAN_TABLE.push_back(Bits(0x78u, 7, 119));
	HUFFMAN_TABLE.push_back(Bits(0x79u, 7, 120));
	HUFFMAN_TABLE.push_back(Bits(0x7au, 7, 121));
	HUFFMAN_TABLE.push_back(Bits(0x7bu, 7, 122));
	HUFFMAN_TABLE.push_back(Bits(0x7ffeu, 15, 123));
	HUFFMAN_TABLE.push_back(Bits(0x7fcu, 11, 124));
	HUFFMAN_TABLE.push_back(Bits(0x3ffdu, 14, 125));
	HUFFMAN_TABLE.push_back(Bits(0x1ffdu, 13, 126));
	HUFFMAN_TABLE.push_back(Bits(0xffffffcu, 28, 127));
	HUFFMAN_TABLE.push_back(Bits(0xfffe6u, 20, 128));
	HUFFMAN_TABLE.push_back(Bits(0x3fffd2u, 22, 129));
	HUFFMAN_TABLE.push_back(Bits(0xfffe7u, 20, 130));
	HUFFMAN_TABLE.push_back(Bits(0xfffe8u, 20, 131));
	HUFFMAN_TABLE.push_back(Bits(0x3fffd3u, 22, 132));
	HUFFMAN_TABLE.push_back(Bits(0x3fffd4u, 22, 133));
	HUFFMAN_TABLE.push_back(Bits(0x3fffd5u, 22, 134));
	HUFFMAN_TABLE.push_back(Bits(0x7fffd9u, 23, 135));
	HUFFMAN_TABLE.push_back(Bits(0x3fffd6u, 22, 136));
	HUFFMAN_TABLE.push_back(Bits(0x7fffdau, 23, 137));
	HUFFMAN_TABLE.push_back(Bits(0x7fffdbu, 23, 138));
	HUFFMAN_TABLE.push_back(Bits(0x7fffdcu, 23, 139));
	HUFFMAN_TABLE.push_back(Bits(0x7fffddu, 23, 140));
	HUFFMAN_TABLE.push_back(Bits(0x7fffdeu, 23, 141));
	HUFFMAN_TABLE.push_back(Bits(0xffffebu, 24, 142));
	HUFFMAN_TABLE.push_back(Bits(0x7fffdfu, 23, 143));
	HUFFMAN_TABLE.push_back(Bits(0xffffecu, 24, 144));
	HUFFMAN_TABLE.push_back(Bits(0xffffedu, 24, 145));
	HUFFMAN_TABLE.push_back(Bits(0x3fffd7u, 22, 146));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe0u, 23, 147));
	HUFFMAN_TABLE.push_back(Bits(0xffffeeu, 24, 148));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe1u, 23, 149));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe2u, 23, 150));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe3u, 23, 151));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe4u, 23, 152));
	HUFFMAN_TABLE.push_back(Bits(0x1fffdcu, 21, 153));
	HUFFMAN_TABLE.push_back(Bits(0x3fffd8u, 22, 154));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe5u, 23, 155));
	HUFFMAN_TABLE.push_back(Bits(0x3fffd9u, 22, 156));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe6u, 23, 157));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe7u, 23, 158));
	HUFFMAN_TABLE.push_back(Bits(0xffffefu, 24, 159));
	HUFFMAN_TABLE.push_back(Bits(0x3fffdau, 22, 160));
	HUFFMAN_TABLE.push_back(Bits(0x1fffddu, 21, 161));
	HUFFMAN_TABLE.push_back(Bits(0xfffe9u, 20, 162));
	HUFFMAN_TABLE.push_back(Bits(0x3fffdbu, 22, 163));
	HUFFMAN_TABLE.push_back(Bits(0x3fffdcu, 22, 164));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe8u, 23, 165));
	HUFFMAN_TABLE.push_back(Bits(0x7fffe9u, 23, 166));
	HUFFMAN_TABLE.push_back(Bits(0x1fffdeu, 21, 167));
	HUFFMAN_TABLE.push_back(Bits(0x7fffeau, 23, 168));
	HUFFMAN_TABLE.push_back(Bits(0x3fffddu, 22, 169));
	HUFFMAN_TABLE.push_back(Bits(0x3fffdeu, 22, 170));
	HUFFMAN_TABLE.push_back(Bits(0xfffff0u, 24, 171));
	HUFFMAN_TABLE.push_back(Bits(0x1fffdfu, 21, 172));
	HUFFMAN_TABLE.push_back(Bits(0x3fffdfu, 22, 173));
	HUFFMAN_TABLE.push_back(Bits(0x7fffebu, 23, 174));
	HUFFMAN_TABLE.push_back(Bits(0x7fffecu, 23, 175));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe0u, 21, 176));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe1u, 21, 177));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe0u, 22, 178));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe2u, 21, 179));
	HUFFMAN_TABLE.push_back(Bits(0x7fffedu, 23, 180));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe1u, 22, 181));
	HUFFMAN_TABLE.push_back(Bits(0x7fffeeu, 23, 182));
	HUFFMAN_TABLE.push_back(Bits(0x7fffefu, 23, 183));
	HUFFMAN_TABLE.push_back(Bits(0xfffeau, 20, 184));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe2u, 22, 185));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe3u, 22, 186));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe4u, 22, 187));
	HUFFMAN_TABLE.push_back(Bits(0x7ffff0u, 23, 188));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe5u, 22, 189));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe6u, 22, 190));
	HUFFMAN_TABLE.push_back(Bits(0x7ffff1u, 23, 191));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe0u, 26, 192));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe1u, 26, 193));
	HUFFMAN_TABLE.push_back(Bits(0xfffebu, 20, 194));
	HUFFMAN_TABLE.push_back(Bits(0x7fff1u, 19, 195));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe7u, 22, 196));
	HUFFMAN_TABLE.push_back(Bits(0x7ffff2u, 23, 197));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe8u, 22, 198));
	HUFFMAN_TABLE.push_back(Bits(0x1ffffecu, 25, 199));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe2u, 26, 200));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe3u, 26, 201));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe4u, 26, 202));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffdeu, 27, 203));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffdfu, 27, 204));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe5u, 26, 205));
	HUFFMAN_TABLE.push_back(Bits(0xfffff1u, 24, 206));
	HUFFMAN_TABLE.push_back(Bits(0x1ffffedu, 25, 207));
	HUFFMAN_TABLE.push_back(Bits(0x7fff2u, 19, 208));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe3u, 21, 209));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe6u, 26, 210));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe0u, 27, 211));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe1u, 27, 212));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe7u, 26, 213));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe2u, 27, 214));
	HUFFMAN_TABLE.push_back(Bits(0xfffff2u, 24, 215));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe4u, 21, 216));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe5u, 21, 217));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe8u, 26, 218));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffe9u, 26, 219));
	HUFFMAN_TABLE.push_back(Bits(0xffffffdu, 28, 220));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe3u, 27, 221));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe4u, 27, 222));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe5u, 27, 223));
	HUFFMAN_TABLE.push_back(Bits(0xfffecu, 20, 224));
	HUFFMAN_TABLE.push_back(Bits(0xfffff3u, 24, 225));
	HUFFMAN_TABLE.push_back(Bits(0xfffedu, 20, 226));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe6u, 21, 227));
	HUFFMAN_TABLE.push_back(Bits(0x3fffe9u, 22, 228));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe7u, 21, 229));
	HUFFMAN_TABLE.push_back(Bits(0x1fffe8u, 21, 230));
	HUFFMAN_TABLE.push_back(Bits(0x7ffff3u, 23, 231));
	HUFFMAN_TABLE.push_back(Bits(0x3fffeau, 22, 232));
	HUFFMAN_TABLE.push_back(Bits(0x3fffebu, 22, 233));
	HUFFMAN_TABLE.push_back(Bits(0x1ffffeeu, 25, 234));
	HUFFMAN_TABLE.push_back(Bits(0x1ffffefu, 25, 235));
	HUFFMAN_TABLE.push_back(Bits(0xfffff4u, 24, 236));
	HUFFMAN_TABLE.push_back(Bits(0xfffff5u, 24, 237));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffeau, 26, 238));
	HUFFMAN_TABLE.push_back(Bits(0x7ffff4u, 23, 239));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffebu, 26, 240));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe6u, 27, 241));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffecu, 26, 242));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffedu, 26, 243));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe7u, 27, 244));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe8u, 27, 245));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffe9u, 27, 246));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffeau, 27, 247));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffebu, 27, 248));
	HUFFMAN_TABLE.push_back(Bits(0xffffffeu, 28, 249));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffecu, 27, 250));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffedu, 27, 251));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffeeu, 27, 252));
	HUFFMAN_TABLE.push_back(Bits(0x7ffffefu, 27, 253));
	HUFFMAN_TABLE.push_back(Bits(0x7fffff0u, 27, 254));
	HUFFMAN_TABLE.push_back(Bits(0x3ffffeeu, 26, 255));
	//HUFFMAN_TABLE.push_back(Bits(0x3fffffffu, 30, (unsigned char)256u));

	for(int i=0;i<(int)Http2HPACKHeaderTable::HUFFMAN_TABLE.size();i++) {
		Bits bts = Http2HPACKHeaderTable::HUFFMAN_TABLE.at(i);
		cib[bts.bits30v.to_string().substr(0, bts.numbits)] = bts;
		//cout << bts.bits30v.to_string().substr(0, bts.numbits) << " = " << (int)bts.symbol << std::endl;
		if(masks.find(bts.numbits)==masks.end()) {
			std::bitset<38> mask;
			for (int j = 0; j < (int)bts.numbits; j++)
			{
				mask.set(37-j, true);
			}
			masks[bts.numbits] = mask;
		}
	}

	for (int var = 0; var < (int)HUFFMAN_TABLE.size(); ++var) {
		std::string key;
		if(HUFFMAN_TABLE.at(var).bitv.size()>1)
		{
			for (int i = 0; i < (int)HUFFMAN_TABLE.at(var).bitv.size()-1 ; ++i) {
				key.push_back((unsigned char)HUFFMAN_TABLE.at(var).bitv.at(i).to_ulong());
			}
		}
		//if(HUFFMAN_TABLE.at(var).numbits==8) {
		//	key.push_back((unsigned char)HUFFMAN_TABLE.at(var).bitv.at(HUFFMAN_TABLE.at(var).bitv.size()-1).to_ulong());
		//}
		if(key.length()>0)
		{
			int siz = HUFFMAN_TABLE.at(var).bitv.size();
			HUFFMAN_LK_STRINDX_NUMINDX_BL_TABLE[key][(int)HUFFMAN_TABLE.at(var).bitv.at(siz-1).to_ulong()] = HUFFMAN_TABLE.at(var).lastbssize;
			HUFFMAN_LK_STRINDX_NUMINDX_TABLE[key][(int)HUFFMAN_TABLE.at(var).bitv.at(siz-1).to_ulong()] = HUFFMAN_TABLE.at(var);
		}
		else
		{
			HUFFMAN_LK_NUMINDX_BL_TABLE[(int)HUFFMAN_TABLE.at(var).bitv.at(0).to_ulong()] = HUFFMAN_TABLE.at(var).numbits;
			HUFFMAN_LK_NUMINDX_TABLE[(int)HUFFMAN_TABLE.at(var).bitv.at(0).to_ulong()] = HUFFMAN_TABLE.at(var);
		}
	}
}

Http2HPACKContext::Http2HPACKContext() {
	huffmanEncoding = true;
}

std::string_view Http2HPACKContext::encodeNumber(long number, const std::vector<bool>& prefixBits) {
	std::string ecval;
	int prefixSize = (int)prefixBits.size();
	long suffixMaxValue = pow(2, (8-prefixSize)) - 1;
	if(number<suffixMaxValue) {
		std::bitset<8> bits(number);
		for (int var = 7; var >= (8-prefixSize); var--) {
			bits.set(var, prefixBits.at(7-var));
		}
		ecval.push_back((unsigned char)bits.to_ulong());
	} else {
		std::bitset<8> bits(number);
		bits.set();
		for (int var = 7; var >= (8-prefixSize); var--) {
			bits.set(var, prefixBits.at(7-var));
		}
		ecval.push_back((unsigned char)bits.to_ulong());
		number -= suffixMaxValue;
		while(number>=128)
		{
			int np = (number %128) + 128;
			std::bitset<8> npbits(np);
			ecval.push_back((unsigned char)npbits.to_ulong());
			number = number/128;
		}
		std::bitset<8> rembits(number);
		ecval.push_back((unsigned char)rembits.to_ulong());
	}
	return ecval;
}

long Http2HPACKContext::decodeNumber(std::string_view data, const int& prefixSize, std::bitset<8> bits, size_t& index) {
	long suffixMaxValue = pow(2, (8-prefixSize)) - 1;
	for (int var = 7; var >= (8-prefixSize); var--) {
		bits.set(var, false);
	}
	index++;
	long num = bits.to_ulong();
	if(num<suffixMaxValue) {
		return num;
	} else {
		long m = 0;
		unsigned char b = data.at(index++);
		do {
			num += (b & 127) * pow(2, m);
			m += 7;
		} while((b & 128) == 128);
		return num;
	}
	return -1;
}

std::string_view Http2HPACKContext::encode(const std::map<std::string_view, std::string_view, cicomp>& headers) {
	std::map<std::string_view, std::string_view>::const_iterator it;
	std::string encoded;
	for (it=headers.begin();it!=headers.end();++it) {
		std::string_view name = it->first;
		std::string_view value = it->second;
		//TODO handle cookie header 8.1.2.5.  Compressing the Cookie Header Field(https://tools.ietf.org/html/draft-ietf-httpbis-http2-16#page-9)
		int index = table.getIndexByNameAndValue(name, value, false);
		if(index!=-1) {
			//Indexed header Name/Value pair
			std::vector<bool> pref;
			pref.push_back(true);
			encoded.append(encodeNumber(index, pref));
		} else {
			index = table.getIndexByName(name, false);
			if(index!=-1) {
				//Indexed header Name
				std::vector<bool> pref;
				pref.push_back(false);
				pref.push_back(true);
				encoded.append(encodeNumber(index, pref));
				encoded.append(encodeString(value));
			} else {
				//Custom header
				encoded.push_back((unsigned char)64);
				encoded.append(encodeString(name));
				encoded.append(encodeString(value));
			}
			table.addHeader(name, value, false);
		}
	}
	return encoded;
}

std::map<std::string_view, std::string_view, cicomp> Http2HPACKContext::decode(std::string_view data) {
	std::map<std::string_view, std::string_view, cicomp> headers;
	size_t indx = 0;
	while(data.length()>indx)
	{
		unsigned char st = data.at(indx);
		std::bitset<8> bits(st);
		int prefixSize = 1;
		//TODO handle cookie header 8.1.2.5.  Compressing the Cookie Header Field(https://tools.ietf.org/html/draft-ietf-httpbis-http2-16#page-9)
		if(bits.test(7)) {
			//Indexed header
			prefixSize = 1;
			long index = decodeNumber(data, prefixSize, bits, indx);
			std::map<std::string_view, std::string_view> hm = table.getNameAndValueByIndex(index, true);
			headers[hm.begin()->first] = hm.begin()->second;
		} else if(bits.test(6)) {
			//Literal indexed
			prefixSize = 2;
			if(st==64) {
				//New name
				indx++;
				std::string_view name = decodeString(data, indx);
				std::string_view value = decodeString(data, indx);
				headers[name] = value;
				table.addHeader(name, value, true);
			} else {
				//Indexed name
				long nmindx = decodeNumber(data, prefixSize, bits, indx);
				std::string_view name = table.getNameByIndex(nmindx, true);
				std::string_view value = decodeString(data, indx);
				headers[name] = value;
				table.addHeader(name, value, true);
			}
		} else if(st<=15) {
			//Literal without indexing
			prefixSize = 4;
			if(st==0) {
				//New name
				indx++;
				std::string_view name = decodeString(data, indx);
				std::string_view value = decodeString(data, indx);
				headers[name] = value;
			} else {
				//Indexed name
				long nmindx = decodeNumber(data, prefixSize, bits, indx);
				std::string_view name = table.getNameByIndex(nmindx, true);
				std::string_view value = decodeString(data, indx);
				headers[name] = value;
			}
		} else if(bits.test(3)) {
			//Literal never indexed
			prefixSize = 4;
			if(st==16) {
				//New name
				indx++;
				std::string_view name = decodeString(data, indx);
				std::string_view value = decodeString(data, indx);
				headers[name] = value;
			} else {
				//Indexed name
				long nmindx = decodeNumber(data, prefixSize, bits, indx);
				std::string_view name = table.getNameByIndex(nmindx, true);
				std::string_view value = decodeString(data, indx);
				headers[name] = value;
			}
		} else {
			//Invalid
		}
	}

	return headers;
}

std::string_view Http2HPACKContext::encodeHuffman(std::string_view value) {
	std::string out;
	int last = 0;
	int totbits = 0;
	for (int var = 0; var < (int)value.length(); ++var) {
		Bits b = Http2HPACKHeaderTable::HUFFMAN_TABLE.at((unsigned char)value.at(var));
		totbits += b.numbits;
		b.appendBits(out, last);
		last += (8 - b.lastbssize);
		last = last%8;
		//CommonUtils::printHEX(out);
		if(var==(int)value.length()-1) {
			totbits = totbits%8;
			if(totbits!=0)
			{
				std::bitset<8> bb(out.at(out.length()-1));
				for (int var = 0; var < 8-totbits; ++var) {
					bb.set(var);
				}
				out[out.length()-1] = (unsigned char)bb.to_ulong();
			}
		}
	}
	return out;
}

std::string_view Http2HPACKContext::encodeString(std::string_view value) {
	std::string encoded;
	std::vector<bool> vallen;
	if(huffmanEncoding)
	{
		value = encodeHuffman(value);
		vallen.push_back(true);
	}
	else
	{
		vallen.push_back(false);
	}
	encoded.append(encodeNumber(value.length(), vallen));
	encoded.append(value);
	return encoded;
}

std::string_view Http2HPACKContext::decodeHuffman(std::string_view value) {
	std::map<uint32_t, std::bitset<38> >::reverse_iterator mit;
	std::string dval;
	int indx = 0;
	std::bitset<38> k;
	int stbind = 37;
	int totbits = (int)value.length()*8;
	int currbits = 0;
	unsigned char c = 0;
	if(totbits>8)
	{
		c = (unsigned char)value.at(indx);
		std::bitset<8> b(c);
		for (int i = 7; i >= 0; i--) {
			k.set(stbind--, b.test(i));
		}
	}
	bool last = false;
	while(indx<=(int)value.length()) {
		for(mit=Http2HPACKHeaderTable::masks.rbegin();mit!=Http2HPACKHeaderTable::masks.rend();++mit) {
			if((38-stbind)>=(int)mit->first) {
				std::bitset<38> kn = k & mit->second;
				std::string_view kt = kn.to_string().substr(0, mit->first);
				if(Http2HPACKHeaderTable::cib.find(kt)!=Http2HPACKHeaderTable::cib.end())
				{
					dval.push_back(Http2HPACKHeaderTable::cib[kt].symbol);
					k <<= Http2HPACKHeaderTable::cib[kt].numbits;
					stbind = stbind+Http2HPACKHeaderTable::cib[kt].numbits;
					currbits += Http2HPACKHeaderTable::cib[kt].numbits;
					break;
				}
			}
		}
		if(last)
		{
			break;
		}
		if(totbits-currbits<=7)
		{
			bool allset = true;
			for(int i = 37; i > 37-(totbits-currbits); i--) {
				allset &= k.test(i);
			}
			if(allset)
			{
				break;
			}
			last = true;
		}
		if(indx<(int)value.length() && (stbind>=8)) {
			indx++;
			if(indx<=(int)value.length()-1)
			{
				c = (unsigned long)value.at(indx);
				std::bitset<8> b((unsigned long)c);
				for (int i = 7; i >= 0; i--) {
					if(stbind>=0)k.set(stbind--, b.test(i));
				}
			}
		}
	}
	return dval;
}

std::string_view Http2HPACKContext::decodeHuffmanOld(std::string_view value) {
	std::string out;
	uint32_t indx = 0;
	std::bitset<8> prev;
	int last = 0;
	std::string key;
	bool useKey = false;
	bool lastsymflow = false;
	int totbits = 0;
	int ttb = (int)value.length()*8;
	int cub = 0;
	while(indx<value.length() || last>0) {
		totbits %= 8;
		if(lastsymflow)
			break;
		unsigned char vm = 0;
		if(indx>=value.length() && last>0)
		{
			if(ttb-cub<=7)
			{
				bool allset = true;
				for(int i = 7; i > 7-(ttb-cub); i--) {
					allset &= prev.test(i);
				}
				if(allset)
				{
					break;
				}
				last = true;
			}
			if((last+totbits)%8!=0)
				break;
			vm = (unsigned char)prev.to_ulong();
			prev.reset();
			last = 0;
			lastsymflow = true;
		}
		else
		{
			vm = (unsigned char)value.at(indx);
		}
		std::bitset<8> bvm(vm);
		std::bitset<8> obvm = bvm;
		std::bitset<8> tp;
		if(last>0)
		{
			for (int var = 0; var < last; ++var) {
				tp.set(7-var, bvm.test(last-1-var));
			}
			bvm.operator >>=(last);
			for (int var = 0; var < last; ++var) {
				bvm.set(7-var, prev.test(7-var));
			}
			vm = (unsigned char)bvm.to_ulong();
			prev.reset();
		}

		if(!useKey)
		{
			if(vm==255) {
				key.push_back(vm);
			} else if(vm==254 && key.length()==0) {
				key.push_back(vm);
				prev = tp;
				tp.reset();
				indx++;
				useKey = true;
				if(indx<value.length())
					continue;
			} else if(key.length()>0) {
				std::string tkey = key;
				tkey.push_back(vm);
				useKey = true;
				if(Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_TABLE.find(tkey)
						!=Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_TABLE.end())
				{
					key = tkey;
					indx++;
					prev = tp;
					tp.reset();
					if(indx<value.length())
						continue;
				}
			}

			if(key.length()>0 && !useKey)
			{
				prev = tp;
				tp.reset();
				indx++;
				if(indx<value.length())
					continue;
			}
		}

		if(key.length()>0) {
			if(indx>=value.length())
			{
				bvm = prev;
			}
		}
		indx++;

		int bitnum = -1;
		while(bitnum<8) {
			if(decipherHuffmanValue(bitnum++, obvm, bvm, out, prev, last, indx, key, value, totbits, cub))
				break;
		}

		if(key.length()>0) {
			key.clear();
			useKey = false;
		}
	}
	return out;
}

bool Http2HPACKContext::decipherHuffmanValue(const int& bitnum, std::bitset<8> obvm, std::bitset<8>& bvm, std::string &out, std::bitset<8>& prev, int& last, uint32_t& indx, std::string& key, std::string_view& value, int& totbits, int& cub) {
	if(key.length()==0) {
		if(bitnum!=-1)bvm.reset(bitnum);
		if(Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_TABLE.find((int)bvm.to_ulong())
				!=Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_TABLE.end())
		{
			int ss = Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_BL_TABLE[(int)bvm.to_ulong()];
			int g = 0;
			if(ss>0)last += (8 - ss);
			if(last>=8)
			{
				indx--;
				obvm = std::bitset<8>((unsigned char)value.at(indx-1));
			}
			last %= 8;
			for (g = 0; g < last; ++g) {
				prev.set(7-g, obvm.test(last-1-g));
			}
			totbits += Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_TABLE[(int)bvm.to_ulong()].numbits;
			cub += Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_TABLE[(int)bvm.to_ulong()].numbits;
			out.push_back(Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_TABLE[(int)bvm.to_ulong()].symbol);
			return true;
		}
	} else {
		if(Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_TABLE.find(key)
				!=Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_TABLE.end())
		{
			if(bitnum!=-1)bvm.reset(bitnum);
			if(Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_TABLE[key].find((int)bvm.to_ulong())
					!=Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_TABLE[key].end())
			{
				int ss = Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_BL_TABLE[key][(int)bvm.to_ulong()];
				int g = 0;
				if(ss>0)last += (8 - ss);
				if(last>=8)
				{
					indx--;
					obvm = std::bitset<8>((unsigned char)value.at(indx-1));
				}
				last %= 8;
				for (g = 0; g < last; ++g) {
					prev.set(7-g, obvm.test(last-1-g));
				}
				totbits += Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_TABLE[(int)bvm.to_ulong()].numbits;
				cub += Http2HPACKHeaderTable::HUFFMAN_LK_NUMINDX_TABLE[(int)bvm.to_ulong()].numbits;
				out.push_back(Http2HPACKHeaderTable::HUFFMAN_LK_STRINDX_NUMINDX_TABLE[key][(int)bvm.to_ulong()].symbol);
				return true;
			}
		}
	}
	return false;
}

std::string_view Http2HPACKContext::decodeString(std::string_view data, size_t& indx) {
	unsigned char st = data.at(indx);
	std::bitset<8> bits(st);
	bool isHuffmanEncoded = bits.test(7);
	long len = decodeNumber(data, 1, bits, indx);
	std::string_view value = data.substr(indx, len);
	//CommonUtils::printHEX(value);
	if(isHuffmanEncoded) {
		value = decodeHuffman(value);
	}
	//cout << value << std::endl;
	indx += len;
	return value;
}

Http2HPACKContext::~Http2HPACKContext() {
	// TODO Auto-generated destructor stub
}

Http2HPACKHeaderTable::Http2HPACKHeaderTable() {
	init();
	this->reqContextTable = HPACK_TABLE;
	this->reqcurrentSize = reqContextTable.size();
	std::map<int, std::map<std::string_view, std::string_view> >::iterator it;
	for (it=reqContextTable.begin();it!=reqContextTable.end();++it) {
		if(it->second.begin()->second!="")
		{
			reqhnvIndexTable[it->second.begin()->first + it->second.begin()->second] = it->first;
		}
		if(reqhnIndexTable.find(it->second.begin()->first)==reqhnIndexTable.end())
		{
			reqhnIndexTable[it->second.begin()->first] = it->first;
		}
	}
	this->resContextTable = HPACK_TABLE;
	this->rescurrentSize = resContextTable.size();
	std::map<int, std::map<std::string_view, std::string_view> >::iterator itr;
	for (itr=resContextTable.begin();itr!=resContextTable.end();++itr) {
		if(itr->second.begin()->second!="")
		{
			reshnvIndexTable[itr->second.begin()->first + itr->second.begin()->second] = itr->first;
		}
		if(reshnIndexTable.find(itr->second.begin()->first)==reshnIndexTable.end())
		{
			reshnIndexTable[itr->second.begin()->first] = itr->first;
		}
	}
}

Http2HPACKHeaderTable::~Http2HPACKHeaderTable() {
}

int Http2HPACKHeaderTable::getIndexByNameAndValue(std::string_view name, std::string_view value, const bool& isRequest) {
	if(isRequest)
	{
		if(reqhnvIndexTable.find(name+value)!=reqhnvIndexTable.end()) {
			return reqhnvIndexTable[name+value];
		}
	}
	else
	{
		if(reshnvIndexTable.find(name+value)!=reshnvIndexTable.end()) {
			return reshnvIndexTable[name+value];
		}
	}
	return -1;
}

int Http2HPACKHeaderTable::getIndexByName(std::string_view name, const bool& isRequest) {
	if(isRequest)
	{
		if(reqhnIndexTable.find(name)!=reqhnIndexTable.end()) {
			return reqhnIndexTable[name];
		}
	}
	else
	{
		if(reshnIndexTable.find(name)!=reshnIndexTable.end()) {
			return reshnIndexTable[name];
		}
	}
	return -1;
}

std::map<std::string_view, std::string_view> Http2HPACKHeaderTable::getNameAndValueByIndex(const int& index, const bool& isRequest) {
	if(isRequest)
	{
		if(reqContextTable.find(index)!=reqContextTable.end()) {
			return reqContextTable[index];
		}
	}
	else
	{
		if(resContextTable.find(index)!=resContextTable.end()) {
			return resContextTable[index];
		}
	}
	std::map<std::string_view, std::string_view> mp;
	return mp;
}

std::string_view Http2HPACKHeaderTable::getNameByIndex(const int& index, const bool& isRequest) {
	if(isRequest)
	{
		if(reqContextTable.find(index)!=reqContextTable.end()) {
			return reqContextTable[index].begin()->first;
		}
	}
	else
	{
		if(resContextTable.find(index)!=resContextTable.end()) {
			return resContextTable[index].begin()->first;
		}
	}
	return "";
}

void Http2HPACKHeaderTable::addHeader(std::string_view name, std::string_view value, const bool& isRequest) {
	if(isRequest)
	{
		if(getIndexByNameAndValue(name, value, isRequest)==-1)
		{
			reqContextTable[++reqcurrentSize].insert(std::pair<std::string_view, std::string_view>(name, value));
			if(value!="")
			{
				reqhnvIndexTable[name+value] = reqcurrentSize;
			}
		}
	}
	else
	{
		if(getIndexByNameAndValue(name, value, isRequest)==-1)
		{
			resContextTable[++rescurrentSize].insert(std::pair<std::string_view, std::string_view>(name, value));
			if(value!="")
			{
				reshnvIndexTable[name+value] = rescurrentSize;
			}
		}
	}
}
