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
 * TestObject.h
 *
 *  Created on: 13-May-2013
 *      Author: sumeetc
 */

#ifndef TESTOBJECT_H_
#define TESTOBJECT_H_
#include "string"
#include "vector"
#include "CastUtil.h"


namespace com {
	namespace obj {
		class TestObject {
			std::vector<short> a;
			std::vector<int> b;
			std::vector<long> c;
			std::vector<long long> d;
			std::vector<unsigned short> e;
			std::vector<unsigned int> f;
			std::vector<unsigned long> g;
			std::vector<unsigned long long> h;
			std::vector<float> i;
			std::vector<double> j;
			std::vector<bool> k;
			std::vector<std::string> l;
		public:
			TestObject();
			virtual ~TestObject();
			std::vector<short> getA() const;
			void setA(std::vector<short> a);
			std::vector<int> getB() const;
			void setB(std::vector<int> b);
			std::vector<long> getC() const;
			void setC(std::vector<long> c);
			std::vector<long long> getD() const;
			void setD(std::vector<long long> d);
			std::vector<unsigned short> getE() const;
			void setE(std::vector<unsigned short> e);
			std::vector<unsigned int> getF() const;
			void setF(std::vector<unsigned int> f);
			std::vector<unsigned long> getG() const;
			void setG(std::vector<unsigned long> g);
			std::vector<unsigned long long> getH() const;
			void setH(std::vector<unsigned long long> h);
			std::vector<float> getI() const;
			void setI(std::vector<float> i);
			std::vector<double> getJ() const;
			void setJ(std::vector<double> j);
			std::vector<bool> getK() const;
			void setK(std::vector<bool> k);
			std::vector<std::string> getL() const;
			void setL(std::vector<std::string> l);
			std::string toString();
		};
	}
} /* namespace com */
#endif /* TESTOBJECT_H_ */
