/*
 * DefaultRestController.cpp
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#include "DefaultRestController.h"

DefaultRestController::DefaultRestController() {
	// TODO Auto-generated constructor stub

}

DefaultRestController::~DefaultRestController() {
	// TODO Auto-generated destructor stub
}

void DefaultRestController::addNumbers(int a, int b)
{
	int c = a + b;
	response->setStatusCode("200");
	response->setStatusMsg("OK");
	response->setContent_type("text/plain");
	response->setContent_str(boost::lexical_cast<string>(a) + " + " + boost::lexical_cast<string>(b) + " = " +
			boost::lexical_cast<string>(c));
	cout << "Processed input request inside DefaultRestController..." << endl;
}

void DefaultRestController::power(int base, int exponent)
{
	int c = pow((double)base, (double)exponent);
	response->setStatusCode("200");
	response->setStatusMsg("OK");
	response->setContent_type("text/plain");
	response->setContent_str(boost::lexical_cast<string>(base) + " ^ " + boost::lexical_cast<string>(exponent) + " = " +
			boost::lexical_cast<string>(c));
	cout << "Processed input request inside DefaultRestController..." << endl;
}
