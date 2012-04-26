/*
 * DefaultIOFilter.h
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#ifndef DEFAULTRESTCONTROLLER_H_
#define DEFAULTRESTCONTROLLER_H_

#include "RestController.h"
#include <iostream>

class DefaultRestController: public RestController {
public:
	DefaultRestController();
	virtual ~DefaultRestController();
	void addNumbers(int,int);
	void power(int,int);
};

#endif /* DEFAULTRESTCONTROLLER_H_ */
