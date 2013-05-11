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

#include "AMEFDecoder.h"

AMEFDecoder::AMEFDecoder() {
	position = 0;
}

AMEFDecoder::~AMEFDecoder() {
	// TODO Auto-generated destructor stub
}

char* AMEFDecoder::longTocharArray(long l,int ind)
{
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	return result;
}

string AMEFDecoder::longTocharArrayS(long l,int ind)
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

char* AMEFDecoder::intTocharArray(int l,int ind)
{
	char* result = new char[ind];
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result[i] = (char) ((l >> offset) & 0xFF);
	}
	return result;
}

char* AMEFDecoder::intTocharArrayWI(int l)
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

int AMEFDecoder::charArrayToInt(string l,int off,int ind)
{
	int t = 0;
	for (int i = 0; i < ind; i++)
	{
		int offset = (ind -1 - i) * 8;
		t += (l[off+i] & 0x000000FF) << offset;
	}
	return t;
}

long AMEFDecoder::charArrayToLong(char* l,int off,int ind)
{
	long t = 0;
	for (int i = 0; i < ind; i++)
	{
		int offset = (ind -1 - i) * 8;
		t += (l[off+i] & 0x000000FF) << offset;
	}
	return t;
}
long AMEFDecoder::charArrayToLong(char* l,int ind)
{
	long t = 0;
	for (int i = 0; i < ind; i++)
	{
		int offset = (ind -1 - i) * 8;
		t += (l[i] & 0x000000FF) << offset;
	}
	return t;
}

string AMEFDecoder::intTocharArrayS(int l, int ind)
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

AMEFObject* AMEFDecoder::decodeB(string buffer,bool considerLength,bool ignoreName)
{
	position = 0;
	string strdata;
	if(considerLength)
	{
		strdata = buffer.substr(4);
	}
	else
	{
		strdata = buffer;
	}
	AMEFObject* AMEFObject = decodeSinglePacketB(strdata,ignoreName);
	return AMEFObject;
}

AMEFObject* AMEFDecoder::decodeSinglePacketB(string buffer,bool ignoreName)
{
	char type = (char)buffer[position];
	AMEFObject *jDBObject = NULL;
	int st, en;
	if(type==AMEFObject::NULL_STRING || type==AMEFObject::NULL_DATE || type==AMEFObject::NULL_NUMBER
			|| type==AMEFObject::NULL_BOOL || type==AMEFObject::NULL_CHAR)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
	}
	else if(type==AMEFObject::STRING_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		int lengthm = charArrayToInt(buffer,position,4);
		jDBObject->setLength(lengthm);
		position += 4;
		string value = buffer.substr(position,lengthm);
		jDBObject->setValue(value);
		position += 5+lengthm;
	}
	else if(type==AMEFObject::STRING_65536_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		int lengthm = charArrayToInt(buffer,position,2);
		jDBObject->setLength(lengthm);
		position += 2;
		string value = buffer.substr(position,lengthm);
		jDBObject->setValue(value);
		position += 3+lengthm;
	}
	else if(type==AMEFObject::STRING_16777216_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		int lengthm = charArrayToInt(buffer,position,3);
		jDBObject->setLength(lengthm);
		position += 3;
		string value = buffer.substr(position,lengthm);
		jDBObject->setValue(value);
		position += 4+lengthm;
	}
	else if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_256_TYPE || type==AMEFObject::DOUBLE_FLOAT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		int lengthm = charArrayToInt(buffer,position,1);
		jDBObject->setLength(lengthm);
		position++;
		string value = buffer.substr(position,lengthm);
		jDBObject->setValue(value);
		position += lengthm;
	}
	else if(type==AMEFObject::VERY_SMALL_INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(1);
		jDBObject->pushChar(buffer[position]);
		position += 1;
	}
	else if(type==AMEFObject::SMALL_INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(2);
		jDBObject->pushChar(buffer[position]);
		jDBObject->pushChar(buffer[position+1]);
		position += 2;
	}
	else if(type==AMEFObject::BIG_INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(3);
		jDBObject->pushChar(buffer[position]);
		jDBObject->pushChar(buffer[position+1]);
		jDBObject->pushChar(buffer[position+2]);
		position += 3;
	}
	else if(type==AMEFObject::INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(4);
		jDBObject->pushChar(buffer[position]);
		jDBObject->pushChar(buffer[position+1]);
		jDBObject->pushChar(buffer[position+2]);
		jDBObject->pushChar(buffer[position+3]);
		position += 4;
	}
	else if(type==AMEFObject::VS_LONG_INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(5);
		jDBObject->pushChar(buffer[position]);
		jDBObject->pushChar(buffer[position+1]);
		jDBObject->pushChar(buffer[position+2]);
		jDBObject->pushChar(buffer[position+3]);
		jDBObject->pushChar(buffer[position+4]);
		position += 5;
	}
	else if(type==AMEFObject::S_LONG_INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(6);
		jDBObject->pushChar(buffer[position]);
		jDBObject->pushChar(buffer[position+1]);
		jDBObject->pushChar(buffer[position+2]);
		jDBObject->pushChar(buffer[position+3]);
		jDBObject->pushChar(buffer[position+4]);
		jDBObject->pushChar(buffer[position+5]);
		position += 6;
	}
	else if(type==AMEFObject::B_LONG_INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(7);
		jDBObject->pushChar(buffer[position]);
		jDBObject->pushChar(buffer[position+1]);
		jDBObject->pushChar(buffer[position+2]);
		jDBObject->pushChar(buffer[position+3]);
		jDBObject->pushChar(buffer[position+4]);
		jDBObject->pushChar(buffer[position+5]);
		jDBObject->pushChar(buffer[position+6]);
		position += 7;
	}
	else if(type==AMEFObject::LONG_INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(8);
		jDBObject->pushChar(buffer[position]);
		jDBObject->pushChar(buffer[position+1]);
		jDBObject->pushChar(buffer[position+2]);
		jDBObject->pushChar(buffer[position+3]);
		jDBObject->pushChar(buffer[position+4]);
		jDBObject->pushChar(buffer[position+5]);
		jDBObject->pushChar(buffer[position+6]);
		jDBObject->pushChar(buffer[position+7]);
		position += 8;
	}
	else if(type==AMEFObject::BOOLEAN_TYPE || type==AMEFObject::CHAR_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		jDBObject->setLength(1);
		jDBObject->pushChar(buffer[position]);
		position += 1;
	}
	else if(type==AMEFObject::VS_OBJECT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		int lengthm = charArrayToInt(buffer,position,1);
		jDBObject->setLength(lengthm);
		position++;
		while(position<(int)buffer.length())
		{
			AMEFObject *obj = decodeSinglePacketB(buffer,ignoreName);
			jDBObject->addPacket(obj);
		}
	}
	else if(type==AMEFObject::S_OBJECT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		int lengthm = charArrayToInt(buffer,position,2);
		jDBObject->setLength(lengthm);
		position += 2;
		while(position<(int)buffer.length())
		{
			AMEFObject* obj = decodeSinglePacketB(buffer,ignoreName);
			jDBObject->addPacket(obj);
		}
	}
	else if(type==AMEFObject::B_OBJECT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		int lengthm = charArrayToInt(buffer,position,3);
		jDBObject->setLength(lengthm);
		position += 3;
		while(position<(int)buffer.length())
		{
			AMEFObject* obj = decodeSinglePacketB(buffer,ignoreName);
			jDBObject->addPacket(obj);
		}
	}
	else if(type==AMEFObject::OBJECT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			while(buffer[position++]!=44){}
			st = position;
			while(buffer[position++]!=44){}
			en = position - 1;
			if(en>st){

			jDBObject->setName(buffer.substr(st,en-st));}
		}
		else
			position++;
		int lengthm = charArrayToInt(buffer,position,4);
		jDBObject->setLength(lengthm);
		position += 4;
		while(position<(int)buffer.length())
		{
			AMEFObject* obj = decodeSinglePacketB(buffer,ignoreName);
			jDBObject->addPacket(obj);
		}
	}
	return jDBObject;
}
