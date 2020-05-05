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
 * MarkerTesting.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#include "MarkerTesting.h"

MarkerTesting::MarkerTesting() {
	// TODO Auto-generated constructor stub

}

MarkerTesting::~MarkerTesting() {
	// TODO Auto-generated destructor stub
}

void MarkerTesting::test1(std::string in)
{
	std::cout << "in Webservice Req for test1 --\n" << in << std::flush;
}
std::string MarkerTesting::test2()
{
	std::cout << "in Webservice Req for test2 --\n" <<  std::flush;
	return "success";
}
void MarkerTesting::test3(MarkerTest t)
{
	std::cout << "in Webservice Req for test3 --\n" << t.getName() << std::flush;
}
MarkerTest MarkerTesting::test4(std::string in)
{
	MarkerTest g;
	g.setId(1);
	g.setName("Sumeet");
	std::cout << "in Webservice Req for test4 --\n" << in << std::flush;
	return g;
}
