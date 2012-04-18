/*
 * Expose.h
 *
 *  Created on: Aug 28, 2009
 *      Author: sumeet
 */

#ifndef EXPOSE_H_
#define EXPOSE_H_
#include "PropFileReader.h"
#include "YObject.h"

class Expose {
public:
	Expose();
	virtual ~Expose();
	YObject sayHello(string,int,float);
	string sayHello1(string,int,float);
	YObject sayHello2(YObject,int,float);
};

#endif /* EXPOSE_H_ */
