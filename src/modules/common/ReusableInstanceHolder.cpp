/*
 * ReusableInstanceHolder.cpp
 *
 *  Created on: 03-Aug-2019
 *      Author: sumeetc
 */

#include "ReusableInstanceHolder.h"

ReusableInstanceHolder::ReusableInstanceHolder(Create crt, Init ini, Destroy des, int limit) {
	this->crt = crt;
	this->ini = ini;
	this->des = des;
	this->limit = limit;
}

void ReusableInstanceHolder::push(void *item) {
	if(limit<=(int)repo.size_approx()) {
		des(item);
	} else {
		repo.enqueue(item);
	}
}

void* ReusableInstanceHolder::pull(void* args) {
	void* item;
	if(!repo.try_dequeue(item)) {
		item = crt(args);
	} else {
		ini(item, args);
	}
	return item;
}

ReusableInstanceHolder::~ReusableInstanceHolder() {
	void* item;
	while(repo.try_dequeue(item)) {
		des(item);
	}
}

