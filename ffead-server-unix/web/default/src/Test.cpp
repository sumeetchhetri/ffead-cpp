/*
 * Test.cpp
 *
 *  Created on: Sep 13, 2009
 *      Author: sumeet
 */

#include "Test.h"

Test::Test() {
	// TODO Auto-generated constructor stub

}

Test::~Test() {
	// TODO Auto-generated destructor stub
}
int Test::getId() const
{
	return id;
}

void Test::setId(int id)
{
	this->id = id;
}

string Test::getName() const
{
	return name;
}

void Test::setName(string name)
{
	this->name = name;
}

bool Test::operator<(Test t) const
{
	return this->id<t.id;
}
