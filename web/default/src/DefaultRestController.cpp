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

int DefaultRestController::addNumbers(int a, int b)
{
	cout << "Processed input request inside DefaultRestController for addNumbers..." << endl;
	return a + b;
}

double DefaultRestController::power(int base, int exponent)
{
	cout << "Processed input request inside DefaultRestController for power..." << endl;
	return pow((double)base, (double)exponent);
}

vector<int> DefaultRestController::testVector(vector<int> param)
{
	cout << "Processed input request inside DefaultRestController for testVector..." << endl;
	return param;
}

TestMany DefaultRestController::testObject(TestMany testMany)
{
	cout << "Processed input request inside DefaultRestController for testObject..." << endl;
	return testMany;
}


vector<TestMany> DefaultRestController::testVectorObject(vector<TestMany> param)
{
	cout << "Processed input request inside DefaultRestController for testVectorObject..." << endl;
	return param;
}

string DefaultRestController::testUploadFile(ifstream* ifs, string param)
{
	string vals;
	unsigned int siz = 0;
	if (ifs!=NULL && ifs->is_open())
	{
		ifs->seekg(0, ios::end);
		siz = ifs->tellg();
	}
	vals = "Uploaded File Size = " + CastUtil::lexical_cast<string>(siz);
	vals += "\nField value passed = " + param;
	cout << "Processed input request inside DefaultRestController for testUploadFile..." << endl;
	return vals;
}

string DefaultRestController::testUploadFileMulti1(ifstream* ifs1, ifstream* ifs2, ifstream* ifs3, string param)
{
	string vals;
	unsigned int siz = 0;
	if (ifs1!=NULL && ifs1->is_open())
	{
		ifs1->seekg(0, ios::end);
		siz = ifs1->tellg();
	}
	vals = "Uploaded File1 Size = " + CastUtil::lexical_cast<string>(siz);
	siz = 0;
	if (ifs2!=NULL && ifs2->is_open())
	{
		ifs2->seekg(0, ios::end);
		siz = ifs2->tellg();
	}
	vals += "\nUploaded File2 Size = " + CastUtil::lexical_cast<string>(siz);
	siz = 0;
	if (ifs3!=NULL && ifs3->is_open())
	{
		ifs3->seekg(0, ios::end);
		siz = ifs3->tellg();
	}
	vals += "\nUploaded File3 Size = " + CastUtil::lexical_cast<string>(siz);
	vals += "\nField value passed = " + param;
	cout << "Processed input request inside DefaultRestController for testUploadFileMulti1..." << endl;
	return vals;
}

string DefaultRestController::testUploadFileMulti2(vector<ifstream*> vifs, string param)
{
	string vals;
	for(int i=0;i<(int)vifs.size();++i) {
		ifstream* ifs = vifs.at(i);
		unsigned int siz = 0;
		if (ifs!=NULL && ifs->is_open())
		{
			ifs->seekg(0, ios::end);
			siz = ifs->tellg();
		}
		vals += "Uploaded File" + CastUtil::lexical_cast<string>(i) + " Size = " + CastUtil::lexical_cast<string>(siz) + "\n";
	}
	vals += "Field value passed = " + param;
	cout << "Processed input request inside DefaultRestController for testUploadFileMulti2..." << endl;
	return vals;
}
