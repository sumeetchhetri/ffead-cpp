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
 * DefaultRestController.cpp
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#include "DefaultRestController.h"

DefaultRestController::DefaultRestController() {
	// TODO Auto-generated constructor stub

}

DefaultRestController::~DefaultRestController() {
	// TODO Auto-generated destructor stub
}

void DefaultRestController::addNumbers(int a, int b)
{
	int c = a + b;
	response->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	response->setContent_type(ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent_str(CastUtil::lexical_cast<string>(a) + " + " + CastUtil::lexical_cast<string>(b) + " = " +
			CastUtil::lexical_cast<string>(c));
	cout << "Processed input request inside DefaultRestController..." << endl;
}

void DefaultRestController::power(int base, int exponent)
{
	int c = pow((double)base, (double)exponent);
	response->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	response->setContent_type(ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent_str(CastUtil::lexical_cast<string>(base) + " ^ " + CastUtil::lexical_cast<string>(exponent) + " = " +
			CastUtil::lexical_cast<string>(c));
	cout << "Processed input request inside DefaultRestController..." << endl;
}

void DefaultRestController::testVector(vector<int> param)
{
	string temvec = "vector[";
	for (int var = 0; var < param.size(); ++var) {
		temvec += CastUtil::lexical_cast<string>(param.at(var));
		if(var!=param.size()-1)
			temvec += ",";
	}
	temvec += "]";
	response->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	response->setContent_type(ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent_str(temvec);
	cout << "Processed input request inside DefaultRestController..." << endl;
}

void DefaultRestController::testObject(TestMany testMany)
{
	buildResponse(HTTPResponseStatus::Ok, "TestMany", &testMany);
	cout << "Processed input request inside DefaultRestController..." << endl;
}


void DefaultRestController::testVectorObject(vector<TestMany> param)
{
	buildResponseVector(HTTPResponseStatus::Ok, "TestMany", &param);
	cout << "Processed input request inside DefaultRestController..." << endl;
}
