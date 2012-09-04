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
