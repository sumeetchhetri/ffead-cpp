/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * Testing.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#include "Testing.h"

Testing::Testing() {
	// TODO Auto-generated constructor stub

}

Testing::~Testing() {
	// TODO Auto-generated destructor stub
}

void Testing::test1(std::string in)
{
	std::cout << "in Webservice Req for test1 --\n" << in << std::flush;
}
std::string Testing::test2()
{
	std::cout << "in Webservice Req for test2 --\n" <<  std::flush;
	return "success";
}
void Testing::test3(Test t)
{
	std::cout << "in Webservice Req for test3 --\n" << t.getName() << std::flush;
}
Test Testing::test4(std::string in)
{
	Test g;
	g.setId(1);
	g.setName("Sumeet");
	std::cout << "in Webservice Req for test4 --\n" << in << std::flush;
	return g;
}
