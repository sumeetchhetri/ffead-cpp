/*
 * FlexAppController.h
 *
 *  Created on: Aug 20, 2009
 *      Author: sumeet
 */

#ifndef FLEXAPPCONTROLLER_H_
#define FLEXAPPCONTROLLER_H_
#include <iostream>
#include "Controller.h"

class FlexAppController : public Controller{
public:
	FlexAppController();
	virtual ~FlexAppController();
	HttpResponse service(HttpRequest);
};

#endif /* FLEXAPPCONTROLLER_H_ */
