/*
 * Test2.h
 *
 *  Created on: Apr 13, 2010
 *      Author: sumeet
 */

#ifndef TEST2_H_
#define TEST2_H_
#include "Test.h"

class Test2 {
	int id;
	int test_id;
	Test test;
public:
	Test2();
	virtual ~Test2();
    int getId() const;
    void setId(int id);;
    int getTest_id() const;
    void setTest_id(int test_id);
    Test getTest() const;
    void setTest(Test test);
};

#endif /* TEST2_H_ */
