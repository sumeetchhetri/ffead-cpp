/*
 * ReusableInstanceHolder.h
 *
 *  Created on: 03-Aug-2019
 *      Author: sumeetc
 */

#ifndef SRC_MODULES_COMMON_REUSABLEINSTANCEHOLDER_H_
#define SRC_MODULES_COMMON_REUSABLEINSTANCEHOLDER_H_

#include "concurrentqueue.h"

typedef void* (*Create)(void* args);
typedef void (*Init)(void*, void*);
typedef void (*Destroy)(void*);

class ReusableInstanceHolder {
	Create crt;
	Init ini;
	Destroy des;
	int limit;
	moodycamel::ConcurrentQueue<void*> repo;
public:
	void push(void* in);
	void* pull(void* args);
	ReusableInstanceHolder(Create crt, Init ini, Destroy des, int limit);
	virtual ~ReusableInstanceHolder();
};

#endif /* SRC_MODULES_COMMON_REUSABLEINSTANCEHOLDER_H_ */
