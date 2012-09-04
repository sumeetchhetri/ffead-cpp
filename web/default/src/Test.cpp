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
 * Test.cpp
 *
 *  Created on: Sep 13, 2009
 *      Author: sumeet
 */

#include "Test.h"

Test::Test() {
	// TODO Auto-generated constructor stub

}

Test::~Test() {
	// TODO Auto-generated destructor stub
}
int Test::getId() const
{
	return id;
}

void Test::setId(int id)
{
	this->id = id;
}

string Test::getName() const
{
	return name;
}

void Test::setName(string name)
{
	this->name = name;
}

bool Test::operator<(Test t) const
{
	return this->id<t.id;
}
