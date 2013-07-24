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
	response->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent(CastUtil::lexical_cast<string>(a) + " + " + CastUtil::lexical_cast<string>(b) + " = " +
			CastUtil::lexical_cast<string>(c));
	cout << "Processed input request inside DefaultRestController for addNumbers..." << endl;
}

void DefaultRestController::power(int base, int exponent)
{
	int c = pow((double)base, (double)exponent);
	response->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	response->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent(CastUtil::lexical_cast<string>(base) + " ^ " + CastUtil::lexical_cast<string>(exponent) + " = " +
			CastUtil::lexical_cast<string>(c));
	cout << "Processed input request inside DefaultRestController for power..." << endl;
}

void DefaultRestController::testVector(vector<int> param)
{
	string temvec = "vector[";
	for (int var = 0; var < (int)param.size(); ++var) {
		temvec += CastUtil::lexical_cast<string>(param.at(var));
		if(var!=(int)param.size()-1)
			temvec += ",";
	}
	temvec += "]";
	response->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	response->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent(temvec);
	cout << "Processed input request inside DefaultRestController for testVector..." << endl;
}

void DefaultRestController::testObject(TestMany testMany)
{
	buildResponse(HTTPResponseStatus::Ok, "TestMany", &testMany);
	cout << "Processed input request inside DefaultRestController for testObject..." << endl;
}


void DefaultRestController::testVectorObject(vector<TestMany> param)
{
	buildResponseVector(HTTPResponseStatus::Ok, "TestMany", &param);
	cout << "Processed input request inside DefaultRestController for testVectorObject..." << endl;
}

void DefaultRestController::testUploadFile(ifstream* ifs, string param)
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

	response->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	response->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent(vals);
	cout << "Processed input request inside DefaultRestController for testUploadFile..." + response->generateResponse() << endl;
}

void DefaultRestController::testUploadFileMulti1(ifstream* ifs1, ifstream* ifs2, ifstream* ifs3, string param)
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

	response->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	response->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent(vals);
	cout << "Processed input request inside DefaultRestController for testUploadFileMulti1..." + response->generateResponse() << endl;
}

void DefaultRestController::testUploadFileMulti2(vector<ifstream*> vifs, string param)
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

	response->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	response->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
	response->setContent(vals);
	cout << "Processed input request inside DefaultRestController for testUploadFileMulti2..." + response->generateResponse() << endl;
}
