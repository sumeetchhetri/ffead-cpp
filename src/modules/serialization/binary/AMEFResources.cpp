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

#include "AMEFResources.h"


AMEFResources::~AMEFResources() {
	// TODO Auto-generated destructor stub
}

char* AMEFResources::longTocharArray(const long& l, const int& ind)
{
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	return result;
}

string AMEFResources::longTocharArrayS(const long& l, const int& ind)
{
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	string tem(result);
	return tem;
}

char* AMEFResources::intTocharArray(const int& l, const int& ind)
{
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	return result;
}

char* AMEFResources::intTocharArrayWI(const int& l)
{
	int ind = 1;
	if(l<256)
		ind =1;
	else if(l<65536)
		ind = 2;
	else if(l<16777216)
		ind =3;
	else
		ind =4;
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	return result;
}

int AMEFResources::charArrayToInt(char* l, const int& off, const int& ind)
{
	int t = 0;
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + ((unsigned char)l[off+i] & 0xff);
	}
	return t;
}

long AMEFResources::charArrayToLong(char* l, const int& off, const int& ind)
{
	long t = 0;
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + ((unsigned char)l[off+i] & 0xff);
	}
	return t;
}

long AMEFResources::charArrayToLong(char* l, const int& ind)
{
	long t = 0;
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + ((unsigned char)l[i] & 0xff);
	}
	return t;
}

string AMEFResources::intTocharArrayS(const int& l, const int& ind)
{
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	string tem(result);
	return tem;
}
#ifdef IS_64_BIT
	string AMEFResources::longTocharArrayWI(const unsigned long long& l)
	{
		int ind = 1;
		if(l<256)
			ind =1;
		else if(l<65536)
			ind = 2;
		else if(l<16777216)
			ind =3;
		else if(l<4294967296ULL)
			ind =4;
		else if(l<1099511627776ULL)
			ind =5;
		else if(l<281474976710656ULL)
			ind =6;
		else if(l<72057594037927936ULL)
			ind =7;
		else
			ind =8;
		string result;
		for (int i = 0; i<ind; i++)
		{
			int offset = (ind - 1 - i) * 8;
			result.push_back((char) ((l >> offset) & 0xFF));
		}
		return result;
	}
	string AMEFResources::longTocharArrayWI(const long& l)
	{
		int ind = 1;
		if(l<256)
			ind =1;
		else if(l<65536)
			ind = 2;
		else if(l<16777216)
			ind =3;
		else if(l<(long)4294967296ULL)
			ind =4;
		else if(l<(long)1099511627776ULL)
			ind =5;
		else if(l<(long)281474976710656ULL)
			ind =6;
		else if(l<(long)72057594037927936ULL)
			ind =7;
		else
			ind =8;
		string result;
		for (int i = 0; i<ind; i++)
		{
			int offset = (ind - 1 - i) * 8;
			result.push_back((char) ((l >> offset) & 0xFF));
		}
		return result;
	}
#else
	string AMEFResources::longTocharArrayWI(const unsigned long long& l)
	{
		int ind = 1;
		if(l<256)
			ind =1;
		else if(l<65536)
			ind = 2;
		else if(l<16777216)
			ind =3;
		else if(l<4294967296ULL)
			ind =4;
		else if(l<1099511627776ULL)
			ind =5;
		else if(l<281474976710656ULL)
			ind =6;
		else if(l<72057594037927936ULL)
			ind =7;
		else
			ind =8;
		string result;
		for (int i = 0; i<ind; i++)
		{
			int offset = (ind - 1 - i) * 8;
			result.push_back((char) ((l >> offset) & 0xFF));
		}
		return result;
	}
	string AMEFResources::longTocharArrayWI(const long& l)
	{
		int ind = 1;
		if(l<256)
			ind =1;
		else if(l<65536)
			ind = 2;
		else if(l<16777216)
			ind =3;
		else
			ind =4;
		string result;
		for (int i = 0; i<ind; i++)
		{
			int offset = (ind - 1 - i) * 8;
			result.push_back((char) ((l >> offset) & 0xFF));
		}
		return result;
	}
#endif
