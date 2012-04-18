/*
 * Test3.h
 *
 *  Created on: Apr 17, 2010
 *      Author: sumeet
 */

#ifndef TEST3_H_
#define TEST3_H_
#include "Test.h"
#include "vector"

class Test3 {
	int id;
	int test_id;
	vector<Test> tests;
public:
	Test3();
	virtual ~Test3();
    int getId() const;
    void setId(int id);
    vector<Test> getTests() const;
    void setTests(vector<Test> tests);
    int getTest_id() const;
    void setTest_id(int test_id);
};

#endif /* TEST3_H_ */
