/*
 * Service1.h
 *
 *  Created on: Mar 24, 2010
 *      Author: sumeet
 */

#ifndef SERVICE1_H_
#define SERVICE1_H_
#include "string"
#include "ServiceInt.h"
using namespace std;

class Service1 :public ServiceInt{
public:
	Service1();
	virtual ~Service1();
	string service1();
};

#endif /* SERVICE1_H_ */
