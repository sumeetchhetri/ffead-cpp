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
 * MarkerExpose.cpp
 *
 *  Created on: Aug 28, 2009
 *      Author: sumeet
 */

#include "MarkerExpose.h"

MarkerExpose::MarkerExpose() {
	// TODO Auto-generated constructor stub

}

MarkerExpose::~MarkerExpose() {
	// TODO Auto-generated destructor stub
}

MarkerYObject MarkerExpose::sayHello(std::string i,int b,float d)
{
	MarkerYObject y;
	return y;
}

std::string MarkerExpose::sayHello1(std::string i,int b,float d)
{
	return "Hello World";
}

MarkerYObject MarkerExpose::sayHello2(MarkerYObject i,int b,float d)
{
	MarkerYObject y;
	return y;
}
