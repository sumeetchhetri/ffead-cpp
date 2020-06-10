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

#ifndef AMEFDECODER_H_
#define AMEFDECODER_H_

#include "AMEFObject.h"
#include "iostream"
#include "Timer.h"
class AMEFDecoder
{
	void decodeObjectName(const std::string& buffer, AMEFObject *jDBObject);
public:
	~AMEFDecoder();
	AMEFDecoder();
	AMEFObject* decodeB(const std::string& buffer, const bool& considerLength);
	//AMEFObject* decodeBNew(const std::string& buffer, const bool& considerLength);
	AMEFObject* decodeSinglePacketBInternal(const std::string& buffer, AMEFObject* jDBObject);
	AMEFObject* decodeSinglePacketB(const std::string& buffer, AMEFObject* jDBObject);
	//AMEFObject* decodeSinglePacketBNew(const std::string& buffer, bool ignoreName, AMEFObject* jDBObject);
	std::string updatePacket(const std::string& orig, AMEFObject* jDBObject, int index);
};

#endif /* AMEFDECODER_H_ */
