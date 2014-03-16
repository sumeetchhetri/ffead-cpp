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

Context DefTemp::getContext()
{
	Context cnt;
	string* dat = new string("1.js");
	Object dato;
	dato << dat;
	cnt["dat"] = dato;

	Test* t = new Test;
	t->setId(1);
	t->setName("name");
	Object to;
	to << t;
	cnt["test"] = to;

	vector<string>* vect = new vector<string>;
	vect->push_back("vec1");
	vect->push_back("vec2");
	Object vecto;
	vecto << vect;
	cnt["vect"] = vecto;

	int* num = new int(5);
	Object numo;
	numo << num;
	cnt["number"] = numo;
	return cnt;
}
