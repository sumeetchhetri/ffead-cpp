/*
	Copyright 2010, Sumeet Chhetri

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
 * TestObject.cpp
 *
 *  Created on: 13-May-2013
 *      Author: sumeetc
 */

#include "TestObject.h"

namespace com {
	namespace obj {
		TestObject::TestObject() {
			// TODO Auto-generated constructor stub

		}

		std::vector<short> TestObject::getA() const {
			return a;
		}

		void TestObject::setA(std::vector<short> a) {
			this->a = a;
		}

		std::vector<int> TestObject::getB() const {
			return b;
		}

		void TestObject::setB(std::vector<int> b) {
			this->b = b;
		}

		std::vector<long> TestObject::getC() const {
			return c;
		}

		void TestObject::setC(std::vector<long> c) {
			this->c = c;
		}

		std::vector<long long> TestObject::getD() const {
			return d;
		}

		void TestObject::setD(std::vector<long long> d) {
			this->d = d;
		}

		std::vector<unsigned short> TestObject::getE() const {
			return e;
		}

		void TestObject::setE(std::vector<unsigned short> e) {
			this->e = e;
		}

		std::vector<unsigned int> TestObject::getF() const {
			return f;
		}

		void TestObject::setF(std::vector<unsigned int> f) {
			this->f = f;
		}

		std::vector<unsigned long> TestObject::getG() const {
			return g;
		}

		void TestObject::setG(std::vector<unsigned long> g) {
			this->g = g;
		}

		std::vector<unsigned long long> TestObject::getH() const {
			return h;
		}

		void TestObject::setH(std::vector<unsigned long long> h) {
			this->h = h;
		}

		std::vector<float> TestObject::getI() const {
			return i;
		}

		void TestObject::setI(std::vector<float> i) {
			this->i = i;
		}

		std::vector<double> TestObject::getJ() const {
			return j;
		}

		void TestObject::setJ(std::vector<double> j) {
			this->j = j;
		}

		std::vector<bool> TestObject::getK() const {
			return k;
		}

		void TestObject::setK(std::vector<bool> k) {
			this->k = k;
		}

		std::vector<std::string> TestObject::getL() const {
			return l;
		}

		void TestObject::setL(std::vector<std::string> l) {
			this->l = l;
		}

		TestObject::~TestObject() {
			// TODO Auto-generated destructor stub
		}

		std::string TestObject::toString() {
			std::string out;
			for (int var = 0; var < (int)a.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(a.at(var)) + " ");
			}
			for (int var = 0; var < (int)b.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(b.at(var)) + " ");
			}
			for (int var = 0; var < (int)c.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(c.at(var)) + " ");
			}
			for (int var = 0; var < (int)d.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(d.at(var)) + " ");
			}
			for (int var = 0; var < (int)e.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(e.at(var)) + " ");
			}
			for (int var = 0; var < (int)f.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(f.at(var)) + " ");
			}
			for (int var = 0; var < (int)g.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(g.at(var)) + " ");
			}
			for (int var = 0; var < (int)h.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(h.at(var)) + " ");
			}
			for (int var = 0; var < (int)i.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(i.at(var)) + " ");
			}
			for (int var = 0; var < (int)j.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(j.at(var)) + " ");
			}
			for (int var = 0; var < (int)k.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(k.at(var)) + " ");
			}
			for (int var = 0; var < (int)l.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(l.at(var)) + " ");
			}
			return out;
		}
	}
} /* namespace com */
