/*
 * TestFormController.h
 *
 *  Created on: Jul 14, 2011
 *      Author: sumeet
 */

#ifndef TESTFORMCONTROLLER_H_
#define TESTFORMCONTROLLER_H_
#include "HttpResponse.h"
#include "TestForm.h"
#include "FormController.h"
#include "iostream"

class TestFormController: public FormController {
public:
	TestFormController();
	virtual ~TestFormController();
	void onSubmit(void*,HttpResponse*);
};

#endif /* TESTFORMCONTROLLER_H_ */
