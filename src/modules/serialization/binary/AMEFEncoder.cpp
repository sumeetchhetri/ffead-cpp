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
}

AMEFEncoder::~AMEFEncoder() {
}

string AMEFEncoder::encodeB(AMEFObject* packet)
{
	string enc = encodeSinglePacketBNew(packet, true);
	string len = AMEFObject::intTocharArray(enc.length(), 4);
	string temp;
	temp.append(len);
	temp.append(enc);
	return temp;
}

string AMEFEncoder::encodeWL(AMEFObject* packet)
{
	string enc = encodeSinglePacketBNew(packet, true);
	return enc;
}

string AMEFEncoder::encodeSinglePacketB(AMEFObject *packet, const bool& ignoreName)
{
	string buffer, vbuffer;
	if(!ignoreName)
	{
		buffer.append(AMEFObject::ulonglongTocharArray(packet->getNameStr().length()));
		buffer.append(packet->getNameStr());
	}

	for (int i=0;i<(int)packet->getPackets().size();i++)
	{
		AMEFObject *pack = packet->getPackets().at(i);
		string val = encodeSinglePacketB(pack, false);
		vbuffer.append(val);
	}

	if(packet->getType()>AMEFObject::NULL_OBJECT)
	{
		vbuffer.append(packet->getValue());
	}

	if(packet->getType()>=AMEFObject::DATE_TYPE && packet->getType()<AMEFObject::OBJECT_TYPE)
	{
		buffer.append(AMEFObject::ulonglongTocharArray(vbuffer.length(), 2));
	}
	else if(packet->getType()==AMEFObject::OBJECT_TYPE)
	{
		buffer.append(AMEFObject::ulonglongTocharArray(vbuffer.length(), 4));
	}
	buffer.append(vbuffer);

	if(packet->getType()==AMEFObject::OBJECT_TYPE)
	{
		buffer.insert(buffer.begin(), AMEFObject::OBJECT_TYPE);
	}
	else
	{
		buffer.insert(buffer.begin(), packet->getType());
	}
	return buffer;
}
/*
string AMEFEncoder::encodeBNew(AMEFObject* packet)
{
	string enc = encodeSinglePacketBNew(packet, true);
	string len = AMEFObject::intTocharArray(enc.length(), 4);
	string temp;
	temp.append(len);
	temp.append(enc);
	return temp;
}
*/

string AMEFEncoder::encodeSinglePacketBNew(AMEFObject *packet, const bool& ignoreName)
{
	string buffer, vbuffer, pbuffer;
	if(!ignoreName)
	{
		buffer.append(AMEFObject::ulonglongTocharArray(packet->getNameStr().length(), 1));
		buffer.append(packet->getNameStr());
	}

	for (int i=0;i<(int)packet->getPackets().size();i++)
	{
		AMEFObject *pack = packet->getPackets().at(i);
		string val = encodeSinglePacketBNew(pack, false);
		if(packet->getType()==AMEFObject::OBJECT_TYPE) {
			pbuffer.append(AMEFObject::ulonglongTocharArray(val.length(), 4));
		}
		vbuffer.append(val);
	}

	if(packet->getType()>AMEFObject::NULL_OBJECT)
	{
		vbuffer.append(packet->getValue());
	}

	if(packet->getType()>=AMEFObject::DATE_TYPE && packet->getType()<AMEFObject::OBJECT_TYPE)
	{
		buffer.append(AMEFObject::ulonglongTocharArray(vbuffer.length(), 3));
	}
	else if(packet->getType()==AMEFObject::OBJECT_TYPE)
	{
		buffer.append(AMEFObject::ulonglongTocharArray(vbuffer.length()+pbuffer.length()+4, 4));
	}
	buffer.append(vbuffer);

	buffer.insert(buffer.begin(), packet->getType());
	if(packet->getType()==AMEFObject::OBJECT_TYPE)
	{
		buffer.insert(5, AMEFObject::ulonglongTocharArray(pbuffer.length(), 4));
		buffer.insert(9, pbuffer);
	}

	return buffer;
}
