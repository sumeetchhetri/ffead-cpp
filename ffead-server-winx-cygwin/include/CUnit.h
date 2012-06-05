/*
	Copyright 2010, Sumeet Chhetri

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
 * Block.h
 *
 *  Created on: Nov 21, 2009
 *      Author: sumeet
 */

#ifndef CUNIT_H_
#define CUNIT_H_
#include "string"
#include "Object.h"
using namespace std;

class CUnit {
public:
	virtual void setUp()=0;
	virtual void beforeTest()=0;
	virtual void test()=0;
	virtual void afterTest()=0;
	template <typename T> bool isEquals(T lhs, T rhs)
	{
		return lhs==rhs;
	}
	template <typename T> bool isNotEquals(T lhs, T rhs)
	{
		return lhs!=rhs;
	}
	template <typename T> bool isTrue(bool value)
	{
		return value==true;
	}
	template <typename T> bool isFalse(bool value)
	{
		return value!=true;
	}
	template <typename T> bool isOfType(T lhs, string className)
	{
		return Object::instanceOf(lhs, className);
	}
};

#endif /* CUNIT_H_ */
