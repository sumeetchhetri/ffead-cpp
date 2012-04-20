/*
 * Service2.h
 *
 *  Created on: Mar 24, 2010
 *      Author: sumeet
 */

#ifndef SERVICE2_H_
#define SERVICE2_H_
#include "string"
#include "ServiceInt.h"
using namespace std;
class Service2 : public ServiceInt{
public:
	Service2();
	virtual ~Service2();
	string service2(string);
};

#endif /* SERVICE2_H_ */
