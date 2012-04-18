/*
 * TestForm.cpp
 *
 *  Created on: Jul 14, 2011
 *      Author: sumeet
 */

#include "TestForm.h"

TestForm::TestForm() {
	// TODO Auto-generated constructor stub

}

string TestForm::getChe() const
{
    return che;
}

int TestForm::getNum() const
{
    return num;
}

string TestForm::getTxt() const
{
    return txt;
}

void TestForm::setChe(string che)
{
    this->che = che;
}

void TestForm::setNum(int num)
{
    this->num = num;
}

void TestForm::setTxt(string txt)
{
    this->txt = txt;
}

TestForm::~TestForm() {
	// TODO Auto-generated destructor stub
}
