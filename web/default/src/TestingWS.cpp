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
 * TestingWS.cpp
 *
 *  Created on: 13-May-2013
 *      Author: sumeetc
 */

#include "TestingWS.h"

namespace ws {
	namespace test {
		TestingWS::TestingWS() {
			// TODO Auto-generated constructor stub

		}

		TestingWS::~TestingWS() {
			// TODO Auto-generated destructor stub
		}

		void TestingWS::wsmeth1(int a, std::string b, long c) {
			std::cout << ("WS Method wsmeth1 called with args - " + CastUtil::lexical_cast<std::string>(a) + " " + b + " " + CastUtil::lexical_cast<std::string>(c)) << std::endl;
		}

		std::string TestingWS::wsmeth2(std::string b, std::vector<int> c) {
			std::string out(b + " ");
			for (int var = 0; var < (int)c.size(); ++var) {
				out.append(CastUtil::lexical_cast<std::string>(c.at(var)) + " ");
			}
			std::cout << ("WS Method wsmeth2 called with args - " + out) << std::endl;
			return out;
		}

		TestObject TestingWS::wsmeth3(std::string a) {
			TestObject obj;
			obj.setA(3);
			obj.setB(4);
			obj.setC(a);
			obj.setD(5.0);
			obj.setE(6.0);
			obj.setF(true);
			obj.setG(13);
			obj.setH(2);
			obj.setI(-2);
			obj.setJ(-3);
			obj.setK(-4);
			obj.setL(-13);
			std::cout << ("WS Method wsmeth3 called with args - " + obj.toString()) << std::endl;
			return obj;
		}

		com::obj::TestObject TestingWS::wsmeth4(bool bol) {
			com::obj::TestObject obj;
			std::vector<short> a;
			a.push_back(2);
			obj.setA(a);
			std::vector<int> b;
			b.push_back(3);
			obj.setB(b);
			std::vector<long> c;
			c.push_back(4);
			obj.setC(c);
			std::vector<long long> d;
			d.push_back(13);
			obj.setD(d);
			std::vector<unsigned short> e;
			e.push_back(-2);
			obj.setE(e);
			std::vector<unsigned int> f;
			f.push_back(-3);
			obj.setF(f);
			std::vector<unsigned long> g;
			g.push_back(-4);
			obj.setG(g);
			std::vector<unsigned long long> h;
			h.push_back(-13);
			obj.setH(h);
			std::vector<float> i;
			i.push_back(5.0);
			obj.setI(i);
			std::vector<double> j;
			j.push_back(6.0);
			obj.setJ(j);
			std::vector<bool> k;
			k.push_back(2);
			obj.setK(k);
			std::vector<std::string> l;
			l.push_back("string");
			obj.setL(l);
			std::cout << ("WS Method wsmeth4 called with args - " + obj.toString()) << std::endl;
			return obj;
		}

		std::string TestingWS::wsmeth5(TestObject obj) {
			std::cout << ("WS Method wsmeth5 called with args - " + obj.toString()) << std::endl;
			return obj.toString();
		}

		long TestingWS::wsmeth6(com::obj::TestObject obj) {
			std::cout << ("WS Method wsmeth6 called with args - " + obj.toString()) << std::endl;
			return 13;
		}
	}
} /* namespace ws */
