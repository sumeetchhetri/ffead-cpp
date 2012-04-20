/*
 * DefaultIOFilter.h
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#ifndef DEFAULTIOFILTER_H_
#define DEFAULTIOFILTER_H_

#include "Filter.h"
#include <iostream>

class DefaultIOFilter: public Filter {
public:
	DefaultIOFilter();
	virtual ~DefaultIOFilter();
	void doInputFilter(HttpRequest *req);
	void doOutputFilter(HttpResponse *res);
};

#endif /* DEFAULTIOFILTER_H_ */
