/*
	Copyright 2009-2012, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
/*
 * Bigdecimal.h
 *
 *  Created on: 06-Mar-2013
 *      Author: sumeetc
 */

#ifndef BIGDECIMAL_H_
#define BIGDECIMAL_H_
#include "Bigint.h"

class Bigdecimal {
	vector<int> parts;
	bool isPositive;
	int decimalDigits;
	int decimalStartsAt;
	void create(string value);
	void checkAndSetIfZero();
public:
	Bigdecimal();
	Bigdecimal(string value);
	void add(Bigdecimal number);
	Bigdecimal operator+(Bigdecimal number);
	Bigdecimal operator-(Bigdecimal number);
	Bigdecimal operator*(Bigdecimal number);
	Bigdecimal operator/(Bigdecimal number);
	Bigdecimal& operator++();
	Bigdecimal& operator+=(Bigdecimal number);
	Bigdecimal& operator--();
	Bigdecimal& operator-=(Bigdecimal number);
	friend bool operator== (Bigdecimal &lhs, Bigdecimal &rhs);
	friend bool operator!= (Bigdecimal &lhs, Bigdecimal &rhs);
	friend bool operator< (Bigdecimal &lhs, Bigdecimal &rhs);
	friend bool operator<= (Bigdecimal &lhs, Bigdecimal &rhs);
	friend bool operator> (Bigdecimal &lhs, Bigdecimal &rhs);
	friend bool operator>= (Bigdecimal &lhs, Bigdecimal &rhs);
	void subtract(Bigdecimal number);
	void multiply(Bigdecimal number);
	void divide(Bigdecimal number, int precision=15);
	static int compare(Bigdecimal number1, Bigdecimal number2);
	int compare(Bigdecimal number);
	string toString();
	virtual ~Bigdecimal();
};

#endif /* BIGDECIMAL_H_ */
