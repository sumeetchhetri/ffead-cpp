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

#include "AMEFEncoder.h"

AMEFEncoder::AMEFEncoder() {
	// TODO Auto-generated constructor stub

}

AMEFEncoder::~AMEFEncoder() {
	// TODO Auto-generated destructor stub
}

char* AMEFEncoder::longTocharArray(long l,int ind)
{
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	return result;
}

string AMEFEncoder::longTocharArrayS(long l,int ind)
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

string AMEFEncoder::intTocharArray(int l,int ind)
{
	string result;
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result.push_back((char) ((l >> offset) & 0xFF));
	}
	return result;
}

string AMEFEncoder::intTocharArrayWI(int l)
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

int AMEFEncoder::charArrayToInt(char* l,int off,int ind)
{
	int t = 0;
	for (int i = 0; i < ind; i++)
	{
		int offset = (ind -1 - i) * 8;
		t += (l[off+i] & 0x000000FF) << offset;
	}
	return t;
}

long AMEFEncoder::charArrayToLong(char* l,int off,int ind)
{
	long t = 0;
	for (int i = 0; i < ind; i++)
	{
		int offset = (ind -1 - i) * 8;
		t += (l[off+i] & 0x000000FF) << offset;
	}
	return t;
}

long AMEFEncoder::charArrayToLong(char* l,int ind)
{
	long t = 0;
	for (int i = 0; i < ind; i++)
	{
		int offset = (ind -1 - i) * 8;
		t += (l[i] & 0x000000FF) << offset;
	}
	return t;
}

string AMEFEncoder::intTocharArrayS(int l, int ind)
{
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	string tem;
	for (int i = 0; i < ind; i++)
		tem.push_back(result[ind]);
	delete result;
	return tem;
}

string AMEFEncoder::encodeB(AMEFObject* packet,bool ignoreName)
{
	string enc = encodeSinglePacketB(packet, ignoreName);
	string len = intTocharArray(enc.length(), 4);
	string temp;
	temp.append(len);
	temp.append(enc);
	return temp;
}

string AMEFEncoder::encodeWL(AMEFObject* packet,bool ignoreName)
{
	string enc = encodeSinglePacketB(packet, ignoreName);
	return enc;
}

void AMEFEncoder::getValue(string value,char type,string buffer)
{
/*if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_65536_TYPE
			|| type==AMEFObject::STRING_256_TYPE || type==AMEFObject::DOUBLE_FLOAT_TYPE
			|| type==AMEFObject::STRING_16777216_TYPE || type==AMEFObject::STRING_TYPE
			|| type==AMEFObject::BOOLEAN_TYPE || type==AMEFObject::CHAR_TYPE)
{
	buffer.append(value);
}
else if(type==AMEFObject::VERY_SMALL_INT_TYPE)
{
	buffer.append(value);
}
else if(type==AMEFObject::SMALL_INT_TYPE)
{
	int intvalue = Integer.parseInt(value);
	buffer.append(intTocharArray(intvalue, 2));
}
else if(type==AMEFObject::BIG_INT_TYPE)
{
	int intvalue = Integer.parseInt(value);
	buffer.append(new string(intTocharArray(intvalue, 3)));
}
else if(type==AMEFObject::INT_TYPE)
{
	int intvalue = Integer.parseInt(value);
	buffer.append(new string(intTocharArray(intvalue, 4)));
}
else if(type==AMEFObject::VS_LONG_INT_TYPE)
{
	long l = Long.parseLong(value);
	buffer.append(new string(longTocharArray(l, 5)));
}
else if(type==AMEFObject::S_LONG_INT_TYPE)
{
	long l = Long.parseLong(value);
	buffer.append(new string(longTocharArray(l, 6)));
}
else if(type==AMEFObject::B_LONG_INT_TYPE)
{
	long l = Long.parseLong(value);
	buffer.append(new string(longTocharArray(l, 7)));
}
else if(type==AMEFObject::LONG_INT_TYPE)
{
	long l = Long.parseLong(value);
	buffer.append(new string(longTocharArray(l, 8)));
}*/
buffer.append(value);
}

string AMEFEncoder::getFinalVal(char type,string buffer,int length,string delim, string name)
{
	string retval = type + delim + name + delim;
	if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_256_TYPE
			|| type==AMEFObject::DOUBLE_FLOAT_TYPE)
	{
		retval += intTocharArrayS(length, 1) + buffer;
	}
	else if(type==AMEFObject::STRING_65536_TYPE)
	{
		retval += intTocharArrayS(length, 2) + buffer;
	}
	else if(type==AMEFObject::STRING_16777216_TYPE)
	{
		retval += intTocharArrayS(length, 3) + buffer;
	}
	else if(type==AMEFObject::STRING_TYPE)
	{
		retval += intTocharArrayS(length, 4) + buffer;
	}
	else if(type==AMEFObject::BOOLEAN_TYPE || type==AMEFObject::CHAR_TYPE
			|| type==AMEFObject::SMALL_INT_TYPE || type==AMEFObject::VERY_SMALL_INT_TYPE
			|| type==AMEFObject::BIG_INT_TYPE || type==AMEFObject::INT_TYPE
			|| type==AMEFObject::VS_LONG_INT_TYPE || type==AMEFObject::S_LONG_INT_TYPE
			|| type==AMEFObject::B_LONG_INT_TYPE || type==AMEFObject::LONG_INT_TYPE)
	{
		retval += buffer;
	}
	else if(type==AMEFObject::OBJECT_TYPE)
	{
		if(length<256)
		{
			retval = AMEFObject::VS_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 1) + buffer;
		}
		else if(length<65536)
		{
			retval = AMEFObject::S_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 2) + buffer;
		}
		else if(length<16777216)
		{
			retval = AMEFObject::B_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 3) + buffer;
		}
		else
		{
			retval = AMEFObject::OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 4) + buffer;
		}
	}
	else if(type==AMEFObject::VS_OBJECT_TYPE)
	{
		retval = AMEFObject::VS_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 1) + buffer;
	}
	else if(type==AMEFObject::S_OBJECT_TYPE)
	{
		retval = AMEFObject::S_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 2) + buffer;
	}
	else if(type==AMEFObject::B_OBJECT_TYPE)
	{
		retval = AMEFObject::B_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 3) + buffer;
	}
	return retval;
}


string AMEFEncoder::getPacketValue(string value)
{
	string arr;
	char type;
	if(value.length()<=256)
		type = AMEFObject::STRING_256_TYPE;
	else if(value.length()<=65536)
		type = AMEFObject::STRING_65536_TYPE;
	else if(value.length()<=16777216)
		type = AMEFObject::STRING_16777216_TYPE;
	else
		type = AMEFObject::STRING_TYPE;

	if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_256_TYPE
			|| type==AMEFObject::DOUBLE_FLOAT_TYPE)
	{
		arr.push_back((char)type);
		string len = intTocharArray(value.length(), 1);
		arr.append(len);
		arr.append(value);
	}
	else if(type==AMEFObject::STRING_65536_TYPE)
	{
		arr.push_back((char)type);
		string len = intTocharArray(value.length(), 2);
		arr.append(len);arr.append(value);
	}
	else if(type==AMEFObject::STRING_16777216_TYPE)
	{
		arr.push_back((char)type);
		string len = intTocharArray(value.length(), 3);
		arr.append(len);arr.append(value);
	}
	else if(type==AMEFObject::STRING_TYPE)
	{
		arr.push_back((char)type);
		string len = intTocharArray(value.length(), 4);
		arr.append(len);arr.append(value);
	}
	else if(type==AMEFObject::BOOLEAN_TYPE || type==AMEFObject::CHAR_TYPE
			|| type==AMEFObject::SMALL_INT_TYPE || type==AMEFObject::VERY_SMALL_INT_TYPE
			|| type==AMEFObject::BIG_INT_TYPE || type==AMEFObject::INT_TYPE
			|| type==AMEFObject::VS_LONG_INT_TYPE || type==AMEFObject::S_LONG_INT_TYPE
			|| type==AMEFObject::B_LONG_INT_TYPE || type==AMEFObject::LONG_INT_TYPE)
	{

	}
	else if(type==AMEFObject::OBJECT_TYPE)
	{
		int length = value.length();
		if(length<256)
		{
			arr[0] = (char)AMEFObject::VS_OBJECT_TYPE;
			string len = intTocharArray(value.length(), 1);
			arr.append(len);arr.append(value);
		}
		else if(length<65536)
		{
			arr[0] = (char)AMEFObject::S_OBJECT_TYPE;
			string len = intTocharArray(value.length(), 2);
			arr.append(len);
		}
		else if(length<16777216)
		{
			arr[0] = (char)AMEFObject::B_OBJECT_TYPE;
			string len = intTocharArray(value.length(),3);
			arr.append(len);
		}
		else
		{
			arr[0] = (char)AMEFObject::OBJECT_TYPE;
			string len = intTocharArray(value.length(),4);
			arr.append(len);
		}
	}
	return arr;
}

string AMEFEncoder::getPacketValue(int integer)
{
	char type;
	int ind = 1;
	if(integer<256)
		type = AMEFObject::VERY_SMALL_INT_TYPE;
	else if(integer<65536)
	{
		type = AMEFObject::SMALL_INT_TYPE;
		ind = 2;
	}
	else if(integer<16777216)
	{
		type = AMEFObject::BIG_INT_TYPE;
		ind = 3;
	}
	else
	{
		type = AMEFObject::INT_TYPE;
		ind = 4;
	}
	string arr;
	arr.push_back((char)type);
	string len = intTocharArray(integer, ind);
	arr.append(len);
	return arr;
}

#ifdef IS_64_BIT
string AMEFEncoder::getPacketValue(unsigned long long lon)
{
	char type;
	int ind = 1;
	if(lon<256)
		type = AMEFObject::VERY_SMALL_INT_TYPE;
	else if(lon<65536)
	{
		type = AMEFObject::SMALL_INT_TYPE;
		ind = 2;
	}
	else if(lon<16777216)
	{
		type = AMEFObject::BIG_INT_TYPE;
		ind = 3;
	}
	else if(lon<4294967296ULL)
	{
		type = AMEFObject::INT_TYPE;
		ind = 4;
	}
	else if(lon<1099511627776ULL)
	{
		type = AMEFObject::VS_LONG_INT_TYPE;
		ind = 5;
	}
	else if(lon<281474976710656ULL)
	{
		type = AMEFObject::S_LONG_INT_TYPE;
		ind = 6;
	}
	else if(lon<72057594037927936ULL)
	{
		type = AMEFObject::B_LONG_INT_TYPE;
		ind = 7;
	}
	else
	{
		type = AMEFObject::LONG_INT_TYPE;
		ind = 8;
	}
	string arr;
	arr.push_back((char)type);
	char* len = longTocharArray(lon, ind);
	arr.append(len);
	return arr;
}

string AMEFEncoder::getPacketValue(long lon)
{
	char type;
	int ind = 1;
	if(lon<256)
		type = AMEFObject::VERY_SMALL_INT_TYPE;
	else if(lon<65536)
	{
		type = AMEFObject::SMALL_INT_TYPE;
		ind = 2;
	}
	else if(lon<16777216)
	{
		type = AMEFObject::BIG_INT_TYPE;
		ind = 3;
	}
	else if(lon<(long)4294967296ULL)
	{
		type = AMEFObject::INT_TYPE;
		ind = 4;
	}
	else if(lon<(long)1099511627776ULL)
	{
		type = AMEFObject::VS_LONG_INT_TYPE;
		ind = 5;
	}
	else if(lon<(long)281474976710656ULL)
	{
		type = AMEFObject::S_LONG_INT_TYPE;
		ind = 6;
	}
	else if(lon<(long)72057594037927936ULL)
	{
		type = AMEFObject::B_LONG_INT_TYPE;
		ind = 7;
	}
	else
	{
		type = AMEFObject::LONG_INT_TYPE;
		ind = 8;
	}
	string arr;
	arr.push_back((char)type);
	char* len = longTocharArray(lon, ind);
	arr.append(len);
	return arr;
}
#else
string AMEFEncoder::getPacketValue(unsigned long long lon)
{
	char type;
	int ind = 1;
	if(lon<256)
		type = AMEFObject::VERY_SMALL_INT_TYPE;
	else if(lon<65536)
	{
		type = AMEFObject::SMALL_INT_TYPE;
		ind = 2;
	}
	else if(lon<16777216)
	{
		type = AMEFObject::BIG_INT_TYPE;
		ind = 3;
	}
	else if(lon<4294967296ULL)
	{
		type = AMEFObject::INT_TYPE;
		ind = 4;
	}
	else if(lon<1099511627776ULL)
	{
		type = AMEFObject::VS_LONG_INT_TYPE;
		ind = 5;
	}
	else if(lon<281474976710656ULL)
	{
		type = AMEFObject::S_LONG_INT_TYPE;
		ind = 6;
	}
	else if(lon<72057594037927936ULL)
	{
		type = AMEFObject::B_LONG_INT_TYPE;
		ind = 7;
	}
	else
	{
		type = AMEFObject::LONG_INT_TYPE;
		ind = 8;
	}
	string arr;
	arr.push_back((char)type);
	char* len = longTocharArray(lon, ind);
	arr.append(len);
	return arr;
}

string AMEFEncoder::getPacketValue(long lon)
{
	char type;
	int ind = 1;
	if(lon<256)
		type = AMEFObject::VERY_SMALL_INT_TYPE;
	else if(lon<65536)
	{
		type = AMEFObject::SMALL_INT_TYPE;
		ind = 2;
	}
	else if(lon<16777216)
	{
		type = AMEFObject::BIG_INT_TYPE;
		ind = 3;
	}
	else
	{
		type = AMEFObject::INT_TYPE;
		ind = 4;
	}
	string arr;
	arr.push_back((char)type);
	char* len = longTocharArray(lon, ind);
	arr.append(len);
	return arr;
}
#endif

string AMEFEncoder::getPacketValue(double lon)
{
	string buffer;
	char type = AMEFObject::DOUBLE_FLOAT_TYPE;
	//getValue(string.valueOf(lon),type, buffer);
	string retVal = getFinalVal(type, buffer, 1, "", "");
	return retVal;
}

string AMEFEncoder::getPacketValue(float lon)
{
	string buffer;
	char type = AMEFObject::DOUBLE_FLOAT_TYPE;
	//getValue(string.valueOf(lon),type, buffer);
	string retVal = getFinalVal(type, buffer, 1, "", "");
	return retVal;
}

string AMEFEncoder::getPacketValue(bool lon)
{
	return (AMEFObject::BOOLEAN_TYPE+(lon?"1":"0"));
}

string AMEFEncoder::getPacketValue(char lon)
{
	return (AMEFObject::CHAR_TYPE+""+lon);
}


//---------------------------------------------------------------------------------------//
string AMEFEncoder::encodeSinglePacketB(AMEFObject *packet,bool ignoreName)
{
	int bufferlength = packet->getNamedLength(ignoreName);
	string buffer;
	int pos = 0;
	if(!ignoreName)
	{
		if(packet->getType()=='o')
		{
			if(bufferlength+4<256)
			{
				buffer.push_back(AMEFObject::VS_OBJECT_TYPE);
			}
			else if(bufferlength+4<65536)
			{
				buffer.push_back(AMEFObject::S_OBJECT_TYPE);
			}
			else if(bufferlength+4<16777216)
			{
				buffer.push_back(AMEFObject::B_OBJECT_TYPE);
			}
		}
		else
			buffer.push_back((char)packet->getType());
		buffer.push_back((char)',');
		buffer.append(packet->getName());
		//System.arraycopy(packet->getName(), 0, buffer, 2, packet->getName().length);
		//pos = packet->getName().length() + 2;
		buffer.push_back((char)',');
		pos ++;
		if(packet->getType()!='n' && packet->getType()!='w' && packet->getType()!='r'
			&& packet->getType()!='i' && packet->getType()!='f' && packet->getType()!='x'
				&& packet->getType()!='e' && packet->getType()!='l' && packet->getType()!='b'
					&& packet->getType()!='c' && packet->getType()!='a' && packet->getType()!='g'
						&& packet->getType()!='j' && packet->getType()!='v' && packet->getType()!='z')
		{
			string lengthb = intTocharArrayWI(packet->getlength());
			buffer.append(lengthb);
			//System.arraycopy(lengthb, 0, buffer, pos, lengthb.length);
			//pos += lengthb.length;
		}
	}
	else
	{
		buffer.push_back((char)packet->getType());
		pos++;
		if(packet->getType()!='n' && packet->getType()!='w' && packet->getType()!='r'
			&& packet->getType()!='i' && packet->getType()!='f' && packet->getType()!='x'
				&& packet->getType()!='e' && packet->getType()!='l' && packet->getType()!='b'
					&& packet->getType()!='c' && packet->getType()!='a' && packet->getType()!='g'
						&& packet->getType()!='j' && packet->getType()!='v' && packet->getType()!='z')
		{
			string lengthb = intTocharArrayWI(packet->getlength());
			buffer.append(lengthb);
			//System.arraycopy(lengthb, 0, buffer, pos, lengthb.length);
			//pos += lengthb.length;
		}
	}

	for (int i=0;i<(int)packet->getPackets().size();i++)
	{
		AMEFObject *pack = packet->getPackets().at(i);
		string val = encodeSinglePacketB(pack,ignoreName);
		buffer.append(val);
		//System.arraycopy(val, 0, buffer, pos, val.length);
		//pos += val.length;
	}
	if(packet->getPackets().size()==0 && packet->getType()!='a' && packet->getType()!='g'
		&& packet->getType()!='j' && packet->getType()!='v' && packet->getType()!='z')
	{
		buffer.append(packet->getValue());
		//System.arraycopy(packet->getValue(), 0, buffer, pos, packet->getValue().length);
	}
	return buffer;
}
