/*
 * Testing.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#include "Testing.h"

Testing::Testing() {
	// TODO Auto-generated constructor stub

}

Testing::~Testing() {
	// TODO Auto-generated destructor stub
}

void Testing::test1(string in)
{
	cout << "in Webservice Req for test1 --\n" << in << flush;
}
string Testing::test2()
{
	cout << "in Webservice Req for test2 --\n" <<  flush;
	return "success";
}
void Testing::test3(Test t)
{
	cout << "in Webservice Req for test3 --\n" << t.getName() << flush;
}
Test Testing::test4(string in)
{
	Test g;
	g.setId(1);
	g.setName("Sumeet");
	cout << "in Webservice Req for test4 --\n" << in << flush;
	return g;
}
