/*
 * TeBkFortune.cpp
 *
 *  Created on: 11-Mar-2015
 *      Author: sumeetc
 */

#include "TeBkFortune.h"

int TeBkFortune::getId() const {
	return id;
}

void TeBkFortune::setId(int id) {
	this->id = id;
}

const string& TeBkFortune::getMessage() const {
	return message;
}

void TeBkFortune::setMessage(const string& message) {
	this->message = message;
}

TeBkFortune::~TeBkFortune() {
	// TODO Auto-generated destructor stub
}

bool TeBkFortune::operator < (const TeBkFortune& other) const {
	return message.compare(other.message)<0;
}
