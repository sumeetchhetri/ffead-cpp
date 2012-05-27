/*
 * TestForm.h
 *
 *  Created on: Jul 14, 2011
 *      Author: sumeet
 */

#ifndef TESTFORM_H_
#define TESTFORM_H_
#include "string"
using namespace std;

class TestForm {
	int num;
	string txt;
	string che;
public:
	TestForm();
	virtual ~TestForm();
	string getChe() const;
    int getNum() const;
    string getTxt() const;
    void setChe(string che);
    void setNum(int num);
    void setTxt(string txt);
};

#endif /* TESTFORM_H_ */
