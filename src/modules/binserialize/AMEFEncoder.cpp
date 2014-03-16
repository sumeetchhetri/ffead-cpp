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

string AMEFEncoder::encodeB(AMEFObject* packet,bool ignoreName)
{
	string enc = encodeSinglePacketB(packet, ignoreName);
	string len = AMEFObject::intTocharArray(enc.length(), 4);
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

string AMEFEncoder::encodeSinglePacketB(AMEFObject *packet,bool ignoreName)
{
	string buffer, vbuffer;
	if(!ignoreName)
	{
		buffer.push_back((char)',');
		buffer.append(AMEFObject::ulonglongTocharArrayWithLI(packet->getNameStr().length()));
		buffer.append(packet->getName());
		buffer.push_back((char)',');
	}
	for (int i=0;i<(int)packet->getPackets().size();i++)
	{
		AMEFObject *pack = packet->getPackets().at(i);
		string val = encodeSinglePacketB(pack,ignoreName);
		vbuffer.append(val);
	}
	if(packet->getPackets().size()==0 && packet->getType()!='a' && packet->getType()!='g'
		&& packet->getType()!='j' && packet->getType()!='v' && packet->getType()!='z')
	{
		vbuffer.append(packet->getValue());
	}

	if(packet->getType()!='n' && packet->getType()!='w' && packet->getType()!='r'
		&& packet->getType()!='i' && packet->getType()!='f' && packet->getType()!='x'
			&& packet->getType()!='e' && packet->getType()!='l' && packet->getType()!='b'
				&& packet->getType()!='c' && packet->getType()!='a' && packet->getType()!='g'
					&& packet->getType()!='j' && packet->getType()!='v' && packet->getType()!='z')
	{
		buffer.append(AMEFObject::ulonglongTocharArray(vbuffer.length()));
	}
	buffer.append(vbuffer);
	if(packet->getType()=='o' || packet->getType()=='m' || packet->getType()=='q' || packet->getType()=='p')
	{
		if(vbuffer.length()<256)
		{
			buffer.insert(buffer.begin(), AMEFObject::VS_OBJECT_TYPE);
		}
		else if(vbuffer.length()<65536)
		{
			buffer.insert(buffer.begin(), AMEFObject::S_OBJECT_TYPE);
		}
		else if(vbuffer.length()<16777216)
		{
			buffer.insert(buffer.begin(), AMEFObject::B_OBJECT_TYPE);
		}
	}
	else
		buffer.insert(buffer.begin(), packet->getType());
	return buffer;
}
