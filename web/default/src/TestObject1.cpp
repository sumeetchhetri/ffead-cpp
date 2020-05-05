/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * TestObject1.cpp
 *
 *  Created on: 13-May-2013
 *      Author: sumeetc
 */

#include "TestObject1.h"

TestObject::TestObject() {
	a = 0;
	b = 0;
	d = 0;
	e = 0;
	f = 0;
	g = 0;
	h = 0;
	i = 0;
	j = 0;
	k = 0;
	l = 0;
}

int TestObject::getA() const {
	return a;
}

void TestObject::setA(int a) {
	this->a = a;
}

long TestObject::getB() const {
	return b;
}

void TestObject::setB(long b) {
	this->b = b;
}

std::string TestObject::getC() const {
	return c;
}

void TestObject::setC(std::string c) {
	this->c = c;
}

float TestObject::getD() const {
	return d;
}

void TestObject::setD(float d) {
	this->d = d;
}

double TestObject::getE() const {
	return e;
}

void TestObject::setE(double e) {
	this->e = e;
}

bool TestObject::isF() const {
	return f;
}

void TestObject::setF(bool f) {
	this->f = f;
}

long long TestObject::getG() const {
	return g;
}

void TestObject::setG(long long g) {
	this->g = g;
}

short TestObject::getH() const {
	return h;
}

void TestObject::setH(short h) {
	this->h = h;
}

unsigned short TestObject::getI() const {
	return i;
}

void TestObject::setI(unsigned short i) {
	this->i = i;
}

unsigned int TestObject::getJ() const {
	return j;
}

void TestObject::setJ(unsigned int j) {
	this->j = j;
}

unsigned long TestObject::getK() const {
	return k;
}

void TestObject::setK(unsigned long k) {
	this->k = k;
}

unsigned long long TestObject::getL() const {
	return l;
}

void TestObject::setL(unsigned long long l) {
	this->l = l;
}

TestObject::~TestObject() {
	// TODO Auto-generated destructor stub
}

std::string TestObject::toString() {
	std::string out;
	out.append(CastUtil::lexical_cast<std::string>(a) + " ");
	out.append(CastUtil::lexical_cast<std::string>(b) + " ");
	out.append(CastUtil::lexical_cast<std::string>(c) + " ");
	out.append(CastUtil::lexical_cast<std::string>(d) + " ");
	out.append(CastUtil::lexical_cast<std::string>(e) + " ");
	out.append(CastUtil::lexical_cast<std::string>(f) + " ");
	out.append(CastUtil::lexical_cast<std::string>(g) + " ");
	out.append(CastUtil::lexical_cast<std::string>(h) + " ");
	out.append(CastUtil::lexical_cast<std::string>(i) + " ");
	out.append(CastUtil::lexical_cast<std::string>(j) + " ");
	out.append(CastUtil::lexical_cast<std::string>(k) + " ");
	out.append(CastUtil::lexical_cast<std::string>(l) + " ");
	return out;
}


