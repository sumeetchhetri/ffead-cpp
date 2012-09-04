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
 * Test3.cpp
 *
 *  Created on: Apr 17, 2010
 *      Author: sumeet
 */

#include "Test3.h"

Test3::Test3() {
	// TODO Auto-generated constructor stub

}

Test3::~Test3() {
	// TODO Auto-generated destructor stub
}
int Test3::getId() const
{
	return id;
}

void Test3::setId(int id)
{
	this->id = id;
}

vector<Test> Test3::getTests() const
{
	return tests;
}

void Test3::setTests(vector<Test> tests)
{
	this->tests = tests;
}
int Test3::getTest_id() const
{
	return test_id;
}

void Test3::setTest_id(int test_id)
{
	this->test_id = test_id;
}
