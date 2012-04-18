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
