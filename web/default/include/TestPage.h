/*
 * TestPage.h
 *
 *  Created on: Jul 13, 2011
 *      Author: sumeet
 */

#ifndef TESTPAGE_H_
#define TESTPAGE_H_
#include "string"
using namespace std;
class TestPage {
public:
	TestPage();
	virtual ~TestPage();
	string textonclick(int,string,string);
	int linkonclick();
};

#endif /* TESTPAGE_H_ */
