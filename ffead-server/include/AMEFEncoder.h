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

#ifndef AMEFENCODER_H_
#define AMEFENCODER_H_

#include "AMEFObject.h"
#include "iostream"

class AMEFEncoder
{
	/*The default delimiter for single object representation*/
	//string delim = ",";

public:
	AMEFEncoder();
	~AMEFEncoder();
	static char* longTocharArray(long l,int ind);
	static string longTocharArrayS(long l,int ind);
	static string intTocharArray(int l,int ind);
	static string intTocharArrayWI(int l);
	static int charArrayToInt(char* l,int off,int ind);
	static long charArrayToLong(char* l,int off,int ind);
	static long charArrayToLong(char* l,int ind);
	static string intTocharArrayS(int l, int ind);
	string encodeB(AMEFObject* packet,bool ignoreName);
	string encodeWL(AMEFObject* packet,bool ignoreName);
	void getValue(string value,char type,string buffer);
	string getFinalVal(char type,string buffer,int length,string delim, string name);
	string getPacketValue(string value);
	string getPacketValue(int integer);

	#ifdef IS_64_BIT
		string getPacketValue(unsigned long long lon);
		string getPacketValue(long lon);
	#else
		string getPacketValue(unsigned long long lon);
		string getPacketValue(long lon);
	#endif

	string getPacketValue(double lon);
	string getPacketValue(float lon);
	string getPacketValue(bool lon);
	string getPacketValue(char lon);
	string encodeSinglePacketB(AMEFObject *packet,bool ignoreName);
}
;

#endif /* AMEFENCODER_H_ */
