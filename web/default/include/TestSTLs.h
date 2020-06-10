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
 * TestSTLs.h
 *
 *  Created on: Jan 31, 2013
 *      Author: sumeet
 */

#ifndef TESTSTLS_H_
#define TESTSTLS_H_
#include "Test.h"
#include "CastUtil.h"
#include "vector"
#include "YObject.h"
#include "queue"
#include "deque"
#include "set"
#include "list"
#include "Date.h"

class TestSTLs {
public:
	bool _1;
	char _2;
	unsigned char _3;
	short _4;
	unsigned short _5;
	int _6;
	unsigned int _7;
	long _8;
	unsigned long _9;
	long long _10;
	unsigned long long _11;
	float _12;
	double _13;
	long double _14;
	std::string _15;
	Date _16;
	Test _17;
	std::set<bool> _c_1;
	std::multiset<bool> _c_2;
	std::list<bool> _c_3;
	std::vector<bool> _c_4;
	std::queue<bool> _c_5;
	std::deque<bool> _c_6;
	std::set<char> _c_7;
	std::multiset<char> _c_8;
	std::list<char> _c_9;
	std::vector<char> _c_10;
	std::queue<char> _c_11;
	std::deque<char> _c_12;
	std::set<unsigned char> _c_13;
	std::multiset<unsigned char> _c_14;
	std::list<unsigned char> _c_15;
	std::vector<unsigned char> _c_16;
	std::queue<unsigned char> _c_17;
	std::deque<unsigned char> _c_18;
	std::set<short> _c_19;
	std::multiset<short> _c_20;
	std::list<short> _c_21;
	std::vector<short> _c_22;
	std::queue<short> _c_23;
	std::deque<short> _c_24;
	std::set<unsigned short> _c_25;
	std::multiset<unsigned short> _c_26;
	std::list<unsigned short> _c_27;
	std::vector<unsigned short> _c_28;
	std::queue<unsigned short> _c_29;
	std::deque<unsigned short> _c_30;
	std::set<int> _c_31;
	std::multiset<int> _c_32;
	std::list<int> _c_33;
	std::vector<int> _c_34;
	std::queue<int> _c_35;
	std::deque<int> _c_36;
	std::set<unsigned int> _c_37;
	std::multiset<unsigned int> _c_38;
	std::list<unsigned int> _c_39;
	std::vector<unsigned int> _c_40;
	std::queue<unsigned int> _c_41;
	std::deque<unsigned int> _c_42;
	std::set<long> _c_43;
	std::multiset<long> _c_44;
	std::list<long> _c_45;
	std::vector<long> _c_46;
	std::queue<long> _c_47;
	std::deque<long> _c_48;
	std::set<unsigned long> _c_49;
	std::multiset<unsigned long> _c_50;
	std::list<unsigned long> _c_51;
	std::vector<unsigned long> _c_52;
	std::queue<unsigned long> _c_53;
	std::deque<unsigned long> _c_54;
	std::set<long long> _c_55;
	std::multiset<long long> _c_56;
	std::list<long long> _c_57;
	std::vector<long long> _c_58;
	std::queue<long long> _c_59;
	std::deque<long long> _c_60;
	std::set<unsigned long long> _c_61;
	std::multiset<unsigned long long> _c_62;
	std::list<unsigned long long> _c_63;
	std::vector<unsigned long long> _c_64;
	std::queue<unsigned long long> _c_65;
	std::deque<unsigned long long> _c_66;
	std::set<float> _c_67;
	std::multiset<float> _c_68;
	std::list<float> _c_69;
	std::vector<float> _c_70;
	std::queue<float> _c_71;
	std::deque<float> _c_72;
	std::set<double> _c_73;
	std::multiset<double> _c_74;
	std::list<double> _c_75;
	std::vector<double> _c_76;
	std::queue<double> _c_77;
	std::deque<double> _c_78;
	std::set<long double> _c_79;
	std::multiset<long double> _c_80;
	std::list<long double> _c_81;
	std::vector<long double> _c_82;
	std::queue<long double> _c_83;
	std::deque<long double> _c_84;
	std::set<std::string> _c_85;
	std::multiset<std::string> _c_86;
	std::list<std::string> _c_87;
	std::vector<std::string> _c_88;
	std::queue<std::string> _c_89;
	std::deque<std::string> _c_90;
	std::set<Date> _c_91;
	std::multiset<Date> _c_92;
	std::list<Date> _c_93;
	std::vector<Date> _c_94;
	std::queue<Date> _c_95;
	std::deque<Date> _c_96;
	std::set<Test> _c_97;
	std::multiset<Test> _c_98;
	std::list<Test> _c_99;
	std::vector<Test> _c_100;
	std::queue<Test> _c_101;
	std::deque<Test> _c_102;

	TestSTLs() {
		_1 = true;
		_2 = 'a';
		_3 = 129;
		_4 = 2;
		_5 = 130;
		_6 = 222;
		_7 = 32999;
		_8 = 222;
		_9 = 32999;
		_10 = 23456789;
		_11 = 23456899665;
		_12 = 1.67;
		_13 = 2.34;
		_14 = 2.34444544;
		_15 = "String";
		_16 = Date();
		_17 = Test(2,"Name");
		_c_1.insert(true);
		_c_1.insert(true);
		_c_2.insert(true);
		_c_2.insert(true);
		_c_3.push_back(true);
		_c_3.push_back(true);
		_c_4.push_back(true);
		_c_4.push_back(true);
		_c_5.push(true);
		_c_5.push(true);
		_c_6.push_back(true);
		_c_6.push_back(true);
		_c_7.insert('a');
		_c_7.insert('a');
		_c_8.insert('a');
		_c_8.insert('a');
		_c_9.push_back('a');
		_c_9.push_back('a');
		_c_10.push_back('a');
		_c_10.push_back('a');
		_c_11.push('a');
		_c_11.push('a');
		_c_12.push_back('a');
		_c_12.push_back('a');
		_c_13.insert(129);
		_c_13.insert(129);
		_c_14.insert(129);
		_c_14.insert(129);
		_c_15.push_back(129);
		_c_15.push_back(129);
		_c_16.push_back(129);
		_c_16.push_back(129);
		_c_17.push(129);
		_c_17.push(129);
		_c_18.push_back(129);
		_c_18.push_back(129);
		_c_19.insert(2);
		_c_19.insert(2);
		_c_20.insert(2);
		_c_20.insert(2);
		_c_21.push_back(2);
		_c_21.push_back(2);
		_c_22.push_back(2);
		_c_22.push_back(2);
		_c_23.push(2);
		_c_23.push(2);
		_c_24.push_back(2);
		_c_24.push_back(2);
		_c_25.insert(130);
		_c_25.insert(130);
		_c_26.insert(130);
		_c_26.insert(130);
		_c_27.push_back(130);
		_c_27.push_back(130);
		_c_28.push_back(130);
		_c_28.push_back(130);
		_c_29.push(130);
		_c_29.push(130);
		_c_30.push_back(130);
		_c_30.push_back(130);
		_c_31.insert(222);
		_c_31.insert(222);
		_c_32.insert(222);
		_c_32.insert(222);
		_c_33.push_back(222);
		_c_33.push_back(222);
		_c_34.push_back(222);
		_c_34.push_back(222);
		_c_35.push(222);
		_c_35.push(222);
		_c_36.push_back(222);
		_c_36.push_back(222);
		_c_37.insert(32999);
		_c_37.insert(32999);
		_c_38.insert(32999);
		_c_38.insert(32999);
		_c_39.push_back(32999);
		_c_39.push_back(32999);
		_c_40.push_back(32999);
		_c_40.push_back(32999);
		_c_41.push(32999);
		_c_41.push(32999);
		_c_42.push_back(32999);
		_c_42.push_back(32999);
		_c_43.insert(222);
		_c_43.insert(222);
		_c_44.insert(222);
		_c_44.insert(222);
		_c_45.push_back(222);
		_c_45.push_back(222);
		_c_46.push_back(222);
		_c_46.push_back(222);
		_c_47.push(222);
		_c_47.push(222);
		_c_48.push_back(222);
		_c_48.push_back(222);
		_c_49.insert(32999);
		_c_49.insert(32999);
		_c_50.insert(32999);
		_c_50.insert(32999);
		_c_51.push_back(32999);
		_c_51.push_back(32999);
		_c_52.push_back(32999);
		_c_52.push_back(32999);
		_c_53.push(32999);
		_c_53.push(32999);
		_c_54.push_back(32999);
		_c_54.push_back(32999);
		_c_55.insert(23456789);
		_c_55.insert(23456789);
		_c_56.insert(23456789);
		_c_56.insert(23456789);
		_c_57.push_back(23456789);
		_c_57.push_back(23456789);
		_c_58.push_back(23456789);
		_c_58.push_back(23456789);
		_c_59.push(23456789);
		_c_59.push(23456789);
		_c_60.push_back(23456789);
		_c_60.push_back(23456789);
		_c_61.insert(23456899665);
		_c_61.insert(23456899665);
		_c_62.insert(23456899665);
		_c_62.insert(23456899665);
		_c_63.push_back(23456899665);
		_c_63.push_back(23456899665);
		_c_64.push_back(23456899665);
		_c_64.push_back(23456899665);
		_c_65.push(23456899665);
		_c_65.push(23456899665);
		_c_66.push_back(23456899665);
		_c_66.push_back(23456899665);
		_c_67.insert(1.67);
		_c_67.insert(1.67);
		_c_68.insert(1.67);
		_c_68.insert(1.67);
		_c_69.push_back(1.67);
		_c_69.push_back(1.67);
		_c_70.push_back(1.67);
		_c_70.push_back(1.67);
		_c_71.push(1.67);
		_c_71.push(1.67);
		_c_72.push_back(1.67);
		_c_72.push_back(1.67);
		_c_73.insert(2.34);
		_c_73.insert(2.34);
		_c_74.insert(2.34);
		_c_74.insert(2.34);
		_c_75.push_back(2.34);
		_c_75.push_back(2.34);
		_c_76.push_back(2.34);
		_c_76.push_back(2.34);
		_c_77.push(2.34);
		_c_77.push(2.34);
		_c_78.push_back(2.34);
		_c_78.push_back(2.34);
		_c_79.insert(2.34444544);
		_c_79.insert(2.34444544);
		_c_80.insert(2.34444544);
		_c_80.insert(2.34444544);
		_c_81.push_back(2.34444544);
		_c_81.push_back(2.34444544);
		_c_82.push_back(2.34444544);
		_c_82.push_back(2.34444544);
		_c_83.push(2.34444544);
		_c_83.push(2.34444544);
		_c_84.push_back(2.34444544);
		_c_84.push_back(2.34444544);
		_c_85.insert("String");
		_c_85.insert("String");
		_c_86.insert("String");
		_c_86.insert("String");
		_c_87.push_back("String");
		_c_87.push_back("String");
		_c_88.push_back("String");
		_c_88.push_back("String");
		_c_89.push("String");
		_c_89.push("String");
		_c_90.push_back("String");
		_c_90.push_back("String");
		_c_91.insert(Date());
		_c_91.insert(Date());
		_c_92.insert(Date());
		_c_92.insert(Date());
		_c_93.push_back(Date());
		_c_93.push_back(Date());
		_c_94.push_back(Date());
		_c_94.push_back(Date());
		_c_95.push(Date());
		_c_95.push(Date());
		_c_96.push_back(Date());
		_c_96.push_back(Date());
		_c_97.insert(Test(2,"Name"));
		_c_97.insert(Test(2,"Name"));
		_c_98.insert(Test(2,"Name"));
		_c_98.insert(Test(2,"Name"));
		_c_99.push_back(Test(2,"Name"));
		_c_99.push_back(Test(2,"Name"));
		_c_100.push_back(Test(2,"Name"));
		_c_100.push_back(Test(2,"Name"));
		_c_101.push(Test(2,"Name"));
		_c_101.push(Test(2,"Name"));
		_c_102.push_back(Test(2,"Name"));
		_c_102.push_back(Test(2,"Name"));
	}
	~TestSTLs() {}
};

#endif /* TESTSTLS_H_ */
