/*
 * Test.h
 *
 *  Created on: Sep 13, 2009
 *      Author: sumeet
 */

#ifndef TEST_H_
#define TEST_H_
#include "string"
using namespace std;

class Test {
	int id;
	string name;
public:
	Test();
	virtual ~Test();
    int getId() const;
    void setId(int);
    string getName() const;
    void setName(string);
    bool operator<(Test t) const;
};

#endif /* TEST_H_ */
