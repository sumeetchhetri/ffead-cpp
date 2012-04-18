/*
 * DefaultController.h
 *
 *  Created on: Aug 20, 2009
 *      Author: sumeet
 */

#ifndef DEFAULTCONTROLLER_H_
#define DEFAULTCONTROLLER_H_
#include <iostream>
#include "Controller.h"

class DefaultController : public Controller{
public:
	DefaultController();
	virtual ~DefaultController();
	HttpResponse service(HttpRequest);
};

#endif /* DEFAULTCONTROLLER_H_ */
