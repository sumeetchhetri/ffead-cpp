/*
 * Test4.cpp
 *
 *  Created on: Jun 9, 2010
 *      Author: sumeet
 */

#include "Test4.h"

Test4::Test4() {
	// TODO Auto-generated constructor stub

}

Test4::~Test4() {
	// TODO Auto-generated destructor stub
}

Date Test4::getDate() const
{
	return this->date;
}

void Test4::setDate(Date date)
{
	this->date = date;
}


Date Test4::getDatt() const
{
	return datt;
}

void Test4::setDatt(Date datt)
{
	this->datt = datt;
}

Date Test4::getDattm() const
{
	return dattm;
}

void Test4::setDattm(Date dattm)
{
	this->dattm = dattm;
}

BinaryData Test4::getBinar() const
{
	return binar;
}

void Test4::setBinar(BinaryData binar)
{
	this->binar = binar;
}
