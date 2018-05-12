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
 * DefTemp.cpp
 *
 *  Created on: Sep 12, 2009
 *      Author: sumeet
 */

#include "DefTemp.h"

DefTemp::DefTemp() {
	// TODO Auto-generated constructor stub

}

DefTemp::~DefTemp() {
	// TODO Auto-generated destructor stub
}

void DefTemp::getContext(HttpRequest* request, Context* context)
{
	std::string dat = "1.js";
	GenericObject& dato = (*cnt)["dat"];
	dato << dat;

	Test t;
	t.setId(1);
	t.setName("name");
	GenericObject& to = (*cnt)["test"];
	to << t;

	std::vector<std::string> vect;
	vect.push_back("vec1");
	vect.push_back("vec2");
	GenericObject& vecto = (*cnt)["vect"];
	vecto << vect;

	int num = 5;
	GenericObject& numo = (*cnt)["number"];
	numo << num;
}
