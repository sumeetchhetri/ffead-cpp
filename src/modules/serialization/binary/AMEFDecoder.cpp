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
}

AMEFDecoder::~AMEFDecoder() {
}

AMEFObject* AMEFDecoder::decodeB(const string& buffer, const bool& considerLength)
{
	if(buffer.length()==0)return NULL;
	AMEFObject* amefObject = new AMEFObject;
	if(considerLength)
	{
		amefObject->position = 4;
	}
	amefObject->blength = buffer.length();
	decodeSinglePacketB(buffer, amefObject);
	return amefObject;
}

/*AMEFObject* AMEFDecoder::decodeBNew(const string& buffer, const bool& considerLength)
{
	if(buffer.length()==0)return NULL;
	AMEFObject* amefObject = new AMEFObject;
	if(considerLength)
	{
		amefObject->position = 4;
	}
	amefObject->blength = buffer.length();
	decodeSinglePacketBNew(buffer, true, amefObject);
	return amefObject;
}*/

void AMEFDecoder::decodeObjectName(const string& buffer, AMEFObject *jDBObject)
{
	++jDBObject->position;
	int lengthm = (int)buffer[jDBObject->position++];
	if(lengthm>0)
	{
		jDBObject->setName(buffer.substr(jDBObject->position, lengthm));
	}
	jDBObject->position += lengthm;
}

/*AMEFObject* AMEFDecoder::decodeSinglePacketB(const string& buffer, bool ignoreName, AMEFObject* jDBObject)
{
	char type = (char)buffer[jDBObject->position];
	if(type<AMEFObject::INT_TYPE) // All NULL Types
	{
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			++jDBObject->position;
	}
	else if(type<AMEFObject::BOOLEAN_TYPE) // Integer/Long Types
	{
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			++jDBObject->position;
		jDBObject->pushChar(buffer[jDBObject->position++]);
		jDBObject->pushChar(buffer[jDBObject->position++]);
		jDBObject->pushChar(buffer[jDBObject->position++]);
		jDBObject->pushChar(buffer[jDBObject->position++]);
		if(type==AMEFObject::LONG_INT_TYPE)
		{
			jDBObject->pushChar(buffer[jDBObject->position++]);
			jDBObject->pushChar(buffer[jDBObject->position++]);
			jDBObject->pushChar(buffer[jDBObject->position++]);
			jDBObject->pushChar(buffer[jDBObject->position++]);
		}
	}
	else if(type<AMEFObject::DATE_TYPE) //Boolean/Char Types
	{
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			++jDBObject->position;
		jDBObject->pushChar(buffer[jDBObject->position++]);
	}
	else if(type<AMEFObject::OBJECT_TYPE) // String/Double/Date Types
	{
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			++jDBObject->position;
		int lengthm = AMEFObject::charArrayToInt(buffer, jDBObject->position, 2);
		jDBObject->position += 2;
		string value = buffer.substr(jDBObject->position, lengthm);
		jDBObject->setValue(value);
		jDBObject->position += lengthm;
	}
	else if(type==AMEFObject::OBJECT_TYPE) // Object Types
	{
		jDBObject->setType(type);
		if(!ignoreName)
		{
			decodeObjectName(buffer, jDBObject);
		}
		else
			++jDBObject->position;
		jDBObject->blength = 4 + jDBObject->position + AMEFObject::charArrayToInt(buffer, jDBObject->position, 4);
		jDBObject->position += 4;
		while(jDBObject->position<jDBObject->blength)
		{
			AMEFObject* obj = new AMEFObject;
			obj->position = jDBObject->position;
			decodeSinglePacketB(buffer, false, obj);
			jDBObject->addPacket(obj);
			jDBObject->position = obj->position;
		}
	}
	return jDBObject;
}*/


AMEFObject* AMEFDecoder::decodeSinglePacketBInternal(const string& buffer, AMEFObject* jDBObject)
{
	char type = (char)buffer[jDBObject->position];
	if(type<AMEFObject::INT_TYPE) // All NULL Types
	{
		jDBObject->setType(type);
		decodeObjectName(buffer, jDBObject);
	}
	else if(type<AMEFObject::BOOLEAN_TYPE) // Integer/Long Types
	{
		jDBObject->setType(type);
		decodeObjectName(buffer, jDBObject);
		if(type==AMEFObject::LONG_INT_TYPE)
		{
			jDBObject->setValue(buffer.substr(jDBObject->position, 8));
			jDBObject->position += 8;
		}
		else
		{
			jDBObject->setValue(buffer.substr(jDBObject->position, 4));
			jDBObject->position += 4;
		}
	}
	else if(type<AMEFObject::DATE_TYPE) //Boolean/Char Types
	{
		jDBObject->setType(type);
		decodeObjectName(buffer, jDBObject);
		jDBObject->pushChar(buffer[jDBObject->position++]);
	}
	else if(type<AMEFObject::OBJECT_TYPE) // String/Double/Date Types
	{
		jDBObject->setType(type);
		decodeObjectName(buffer, jDBObject);
		int lengthm = AMEFObject::charArrayToInt(buffer, jDBObject->position, 3);
		jDBObject->position += 3;
		jDBObject->setValue(buffer.substr(jDBObject->position, lengthm));
		jDBObject->position += lengthm;
	}
	else if(type==AMEFObject::OBJECT_TYPE) // Object Types
	{
		jDBObject->setType(type);
		decodeObjectName(buffer, jDBObject);
		jDBObject->blength = 4 + jDBObject->position + AMEFObject::charArrayToInt(buffer, jDBObject->position, 4);
		jDBObject->position += 4;
		int poslen = AMEFObject::charArrayToInt(buffer, jDBObject->position, 4);
		jDBObject->position += poslen + 4;
		while(jDBObject->position<jDBObject->blength)
		{
			AMEFObject* obj = new AMEFObject;
			obj->position = jDBObject->position;
			decodeSinglePacketBInternal(buffer, obj);
			jDBObject->addPacket(obj);
			jDBObject->position = obj->position;
		}
	}
	return jDBObject;
}

AMEFObject* AMEFDecoder::decodeSinglePacketB(const string& buffer, AMEFObject* jDBObject)
{
	char type = (char)buffer[jDBObject->position];
	if(type==AMEFObject::OBJECT_TYPE) // Object Types
	{
		jDBObject->setType(type);
		++jDBObject->position;
		jDBObject->blength = 4 + jDBObject->position + AMEFObject::charArrayToInt(buffer, jDBObject->position, 4);
		jDBObject->position += 4;
		int poslen = AMEFObject::charArrayToInt(buffer, jDBObject->position, 4);
		jDBObject->position += poslen + 4;
		while(jDBObject->position<jDBObject->blength)
		{
			AMEFObject* obj = new AMEFObject;
			obj->position = jDBObject->position;
			decodeSinglePacketBInternal(buffer, obj);
			jDBObject->addPacket(obj);
			jDBObject->position = obj->position;
		}
	}
	return jDBObject;
}

string AMEFDecoder::updatePacket(const string& orig, AMEFObject* jDBObject, int index) {
	string np;
	vector<int> propLengths;
	int poslen = AMEFObject::charArrayToInt(orig, 5, 4);
	for (int var = 0; var < poslen/4; ++var) {
		propLengths.push_back(AMEFObject::charArrayToInt(orig, 9+(var*4), 4));
	}
	int start = 0, lindx = 0;
	for (int var = index; var < (int)jDBObject->getPackets().size(); var+=2) {
		int ind = jDBObject->getPackets().at(var)->getIntValue();
		string val = jDBObject->getPackets().at(var+1)->getValue();
		int tlen = propLengths.at(ind);
		int count = start==0?poslen+9:0;
		for (int v1 = lindx; v1 < ind; ++v1) {
			count += propLengths.at(v1);
		}
		np.append(orig.substr(start, count));
		lindx = ind+1;
		string nlength = AMEFObject::ulonglongTocharArray(val.length(), 4);
		np[9+ind*4] = nlength.at(0);
		np[10+ind*4] = nlength.at(1);
		np[11+ind*4] = nlength.at(2);
		np[12+ind*4] = nlength.at(3);
		np.append(val);
		start += count + tlen;
	}
	if(start!=(int)orig.length()) {
		np.append(orig.substr(start));
	}
	if(orig.length()!=np.length())
	{
		int clen = np.length() - 5;
		string nlength = AMEFObject::ulonglongTocharArray(clen, 4);
		np[1] = nlength.at(0);
		np[2] = nlength.at(1);
		np[3] = nlength.at(2);
		np[4] = nlength.at(3);
	}
	return np;
}
