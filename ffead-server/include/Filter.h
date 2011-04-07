/*
 * Filter.h
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#ifndef FILTER_H_
#define FILTER_H_
#include "HttpResponse.h"
#include "HttpRequest.h"

class Filter {
public:
	Filter();
	virtual ~Filter();
	virtual void doInputFilter(HttpRequest *req){}
	virtual void doOutputFilter(HttpResponse *res){}
};

#endif /* FILTER_H_ */
