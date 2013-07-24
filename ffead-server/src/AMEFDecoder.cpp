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
AMEFObject* AMEFDecoder::decodeB(string buffer,bool considerLength,bool ignoreName)
{
	if(buffer.length()==0)return NULL;
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

void AMEFDecoder::decodeObjectName(string buffer, AMEFObject *jDBObject)
{
	while(buffer[position++]!=44){}
	int lenident = (int)buffer[position++];
	if(lenident>0)
	{
		int lengthm = AMEFObject::charArrayToInt(buffer,position++,lenident);
		jDBObject->setName(buffer.substr(position,  lengthm));
		position += lengthm;
	}
	while(buffer[position++]!=44){}
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
			decodeObjectName(buffer, jDBObject);
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
		int lengthm = AMEFObject::charArrayToInt(buffer,position,4);
		position += 4;
		string value = buffer.substr(position,lengthm);
		jDBObject->setValue(value);
		position += lengthm;
	}
	else if(type==AMEFObject::STRING_65536_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
		int lengthm = AMEFObject::charArrayToInt(buffer,position,2);
		position += 2;
		string value = buffer.substr(position,lengthm);
		jDBObject->setValue(value);
		position += lengthm;
	}
	else if(type==AMEFObject::STRING_16777216_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
		int lengthm = AMEFObject::charArrayToInt(buffer,position,3);
		position += 3;
		string value = buffer.substr(position,lengthm);
		jDBObject->setValue(value);
		position += lengthm;
	}
	else if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_256_TYPE || type==AMEFObject::DOUBLE_FLOAT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
		int lengthm = AMEFObject::charArrayToInt(buffer,position,1);
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
		jDBObject->pushChar(buffer[position]);
		position += 1;
	}
	else if(type==AMEFObject::SMALL_INT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
		jDBObject->pushChar(buffer[position]);
		position += 1;
	}
	else if(type==AMEFObject::VS_OBJECT_TYPE)
	{
		jDBObject = new AMEFObject();
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
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
			decodeObjectName(buffer, jDBObject);
		}
		else
			position++;
		position += 4;
		while(position<(int)buffer.length())
		{
			AMEFObject* obj = decodeSinglePacketB(buffer,ignoreName);
			jDBObject->addPacket(obj);
		}
	}
	return jDBObject;
}
