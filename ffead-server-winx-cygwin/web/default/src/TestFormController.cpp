/*
 * TestFormController.cpp
 *
 *  Created on: Jul 14, 2011
 *      Author: sumeet
 */

#include "TestFormController.h"


TestFormController::TestFormController() {
	// TODO Auto-generated constructor stub

}

TestFormController::~TestFormController() {
	// TODO Auto-generated destructor stub
}

void TestFormController::onSubmit(void* vform,HttpResponse* res)
{
	TestForm* form = (TestForm*)vform;
	res->setStatusCode("200");
	res->setStatusMsg("OK");
	res->setContent_type("text/plain");
	res->setContent_str(form->getTxt()+form->getChe());
	cout << form->getTxt()+form->getChe() << "inside TestFormController" << endl;
}
