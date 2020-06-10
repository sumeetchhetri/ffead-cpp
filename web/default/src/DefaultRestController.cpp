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
	std::cout << "Processed input request inside DefaultRestController for addNumbers..." << std::endl;
	return a + b;
}

double DefaultRestController::power(int base, int exponent)
{
	std::cout << "Processed input request inside DefaultRestController for power..." << std::endl;
	return pow((double)base, (double)exponent);
}

std::vector<int> DefaultRestController::testVector(std::vector<int> param)
{
	std::cout << "Processed input request inside DefaultRestController for testVector..." << std::endl;
	return param;
}

TestMany DefaultRestController::testObject(TestMany testMany)
{
	std::cout << "Processed input request inside DefaultRestController for testObject..." << std::endl;
	return testMany;
}


std::vector<TestMany> DefaultRestController::testVectorObject(std::vector<TestMany> param)
{
	std::cout << "Processed input request inside DefaultRestController for testVectorObject..." << std::endl;
	return param;
}

std::string DefaultRestController::testUploadFile(std::ifstream* ifs, std::string param)
{
	std::string vals;
	unsigned int siz = 0;
	if (ifs!=NULL && ifs->is_open())
	{
		ifs->seekg(0, std::ios::end);
		siz = ifs->tellg();
	}
	vals = "Uploaded File Size = " + CastUtil::lexical_cast<std::string>(siz);
	vals += "\nField value passed = " + param;
	std::cout << "Processed input request inside DefaultRestController for testUploadFile..." << std::endl;
	return vals;
}

std::string DefaultRestController::testUploadFileMulti1(std::ifstream* ifs1, std::ifstream* ifs2, std::ifstream* ifs3, std::string param)
{
	std::string vals;
	unsigned int siz = 0;
	if (ifs1!=NULL && ifs1->is_open())
	{
		ifs1->seekg(0, std::ios::end);
		siz = ifs1->tellg();
	}
	vals = "Uploaded File1 Size = " + CastUtil::lexical_cast<std::string>(siz);
	siz = 0;
	if (ifs2!=NULL && ifs2->is_open())
	{
		ifs2->seekg(0, std::ios::end);
		siz = ifs2->tellg();
	}
	vals += "\nUploaded File2 Size = " + CastUtil::lexical_cast<std::string>(siz);
	siz = 0;
	if (ifs3!=NULL && ifs3->is_open())
	{
		ifs3->seekg(0, std::ios::end);
		siz = ifs3->tellg();
	}
	vals += "\nUploaded File3 Size = " + CastUtil::lexical_cast<std::string>(siz);
	vals += "\nField value passed = " + param;
	std::cout << "Processed input request inside DefaultRestController for testUploadFileMulti1..." << std::endl;
	return vals;
}

std::string DefaultRestController::testUploadFileMulti2(std::vector<std::ifstream*> vifs, std::string param)
{
	std::string vals;
	for(int i=0;i<(int)vifs.size();++i) {
		std::ifstream* ifs = vifs.at(i);
		unsigned int siz = 0;
		if (ifs!=NULL && ifs->is_open())
		{
			ifs->seekg(0, std::ios::end);
			siz = ifs->tellg();
		}
		vals += "Uploaded File" + CastUtil::lexical_cast<std::string>(i) + " Size = " + CastUtil::lexical_cast<std::string>(siz) + "\n";
	}
	vals += "Field value passed = " + param;
	std::cout << "Processed input request inside DefaultRestController for testUploadFileMulti2..." << std::endl;
	return vals;
}

TestSTLs DefaultRestController::sertest(TestSTLs ts) {
	return ts;
}

/*int DefaultRestController::alloccounter() {
	return ConfigurationData::counter;
}*/
