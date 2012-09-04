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
/*
 * Expose.cpp
 *
 *  Created on: Aug 28, 2009
 *      Author: sumeet
 */

#include "Expose.h"

Expose::Expose() {
	// TODO Auto-generated constructor stub

}

Expose::~Expose() {
	// TODO Auto-generated destructor stub
}

YObject Expose::sayHello(string i,int b,float d)
{
	YObject y;
	return y;
}

string Expose::sayHello1(string i,int b,float d)
{
	return "Hello World";
}

YObject Expose::sayHello2(YObject i,int b,float d)
{
	YObject y;
	return y;
}
