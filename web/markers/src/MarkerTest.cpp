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
 * MarkerTest.cpp
 *
 *  Created on: Sep 13, 2009
 *      Author: sumeet
 */

#include "MarkerTest.h"

MarkerTest::MarkerTest() {
	// TODO Auto-generated constructor stub

}

MarkerTest::~MarkerTest() {
	// TODO Auto-generated destructor stub
}
int MarkerTest::getId() const
{
	return id;
}

void MarkerTest::setId(int id)
{
	this->id = id;
}

std::string MarkerTest::getName() const
{
	return name;
}

void MarkerTest::setName(std::string name)
{
	this->name = name;
}

bool MarkerTest::operator<(MarkerTest t) const
{
	return this->id<t.id;
}
