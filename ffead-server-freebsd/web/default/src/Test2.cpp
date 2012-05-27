/*
 * Test2.cpp
 *
 *  Created on: Apr 13, 2010
 *      Author: sumeet
 */

#include "Test2.h"

Test2::Test2() {
	// TODO Auto-generated constructor stub

}

Test2::~Test2() {
	// TODO Auto-generated destructor stub
}
int Test2::getId() const
{
	return id;
}

void Test2::setId(int id)
{
	this->id = id;
}

int Test2::getTest_id() const
{
	return test_id;
}

void Test2::setTest_id(int test_id)
{
	this->test_id = test_id;
}

Test Test2::getTest() const
{
	return test;
}

void Test2::setTest(Test test)
{
	this->test = test;
}
