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

#ifndef AMEFRESOURCES_H_
#define AMEFRESOURCES_H_

#include "AMEFEncoder.h"
#include "AMEFDecoder.h"

class AMEFResources
{
	AMEFResources(){}
	~AMEFResources();
public:

	static char* longTocharArray(long l,int ind);
	static string longTocharArrayS(long l,int ind);
	static char* intTocharArray(int l,int ind);
	static char* intTocharArrayWI(int l);
	static int charArrayToInt(char* l,int off,int ind);
	static long charArrayToLong(char* l,int off,int ind);
	static long charArrayToLong(char* l,int ind);
	static string intTocharArrayS(int l, int ind);
	#ifdef IS_64_BIT
		static string longTocharArrayWI(unsigned long long l);
		static string longTocharArrayWI(long l);
	#else
		static string longTocharArrayWI(unsigned long long l);
		static string longTocharArrayWI(long l);
	#endif
};

#endif /* AMEFRESOURCES_H_ */
