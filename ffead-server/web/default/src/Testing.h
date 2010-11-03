/*
 * Testing.h
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#ifndef TESTING_H_
#define TESTING_H_
#include "Test.h"
#include <iostream>

class Testing {
public:
	Testing();
	virtual ~Testing();
	void test1(string);
	string test2();
	void test3(Test);
	Test test4(string);
};

#endif /* TESTING_H_ */
