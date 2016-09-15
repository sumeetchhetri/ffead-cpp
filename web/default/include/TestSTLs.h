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
class TestSTLs {
	std::list<int> vpli;
	std::vector<int> vpvi;
	std::set<int> vpls;
	std::multiset<int> vpmsi;
	std::queue<int> vpqi;
	std::deque<int> vpdi;

	std::list<int>* vppli;
	std::vector<int>* vppvi;
	std::set<int>* vppls;
	std::multiset<int>* vppmsi;
	std::queue<int>* vppqi;
	std::deque<int>* vppdi;
public:
	int mi;

	std::list<int> vli;
	std::list<std::string> vls;
	std::list<double> vld;
	std::list<long> vll;
	std::list<bool> vlb;
	std::list<short> vlsh;
	std::list<YObject> vlyo;

	std::vector<int> vvi;
	std::vector<std::string> vvs;
	std::vector<double> vvd;
	std::vector<long> vvl;
	std::vector<bool> vvb;
	std::vector<short> vvsh;
	std::vector<YObject> vvyo;

	std::deque<int> vdi;
	std::deque<std::string> vds;
	std::deque<double> vdd;
	std::deque<long> vdl;
	std::deque<bool> vdb;
	std::deque<short> vdsh;
	std::deque<YObject> vdyo;

	std::set<int> vsi;
	std::set<std::string> vss;
	std::set<double> vsd;
	std::set<long> vsl;
	std::set<short> vssh;
	std::set<Test> vsyo;

	std::multiset<int> vmsi;
	std::multiset<std::string> vmss;
	std::multiset<double> vmsd;
	std::multiset<long> vmsl;
	std::multiset<short> vmssh;
	std::multiset<Test> vmsyo;

	std::queue<int> vqi;
	std::queue<std::string> vqs;
	std::queue<double> vqd;
	std::queue<long> vql;
	std::queue<bool> vqb;
	std::queue<short> vqsh;
	std::queue<YObject> vqyo;

	std::list<int>* vpppli;
	std::list<std::string>* vpppls;
	std::list<double>* vpppld;
	std::list<long>* vpppll;
	std::list<bool>* vppplb;
	std::list<short>* vppplsh;
	std::list<YObject>* vppplyo;

	std::vector<int>* vpppvi;
	std::vector<std::string>* vpppvs;
	std::vector<double>* vpppvd;
	std::vector<long>* vpppvl;
	std::vector<bool>* vpppvb;
	std::vector<short>* vpppvsh;
	std::vector<YObject>* vpppvyo;

	std::deque<int>* vpppdi;
	std::deque<std::string>* vpppds;
	std::deque<double>* vpppdd;
	std::deque<long>* vpppdl;
	std::deque<bool>* vpppdb;
	std::deque<short>* vpppdsh;
	std::deque<YObject>* vpppdyo;

	std::set<int>* vpppsi;
	std::set<std::string>* vpppss;
	std::set<double>* vpppsd;
	std::set<long>* vpppsl;
	std::set<short>* vpppssh;
	std::set<Test>* vpppsyo;

	std::multiset<int>* vpppmsi;
	std::multiset<std::string>* vpppmss;
	std::multiset<double>* vpppmsd;
	std::multiset<long>* vpppmsl;
	std::multiset<short>* vpppmssh;
	std::multiset<Test>* vpppmsyo;

	std::queue<int>* vpppqi;
	std::queue<std::string>* vpppqs;
	std::queue<double>* vpppqd;
	std::queue<long>* vpppql;
	std::queue<bool>* vpppqb;
	std::queue<short>* vpppqsh;
	std::queue<YObject>* vpppqyo;

	YObject* yobjectp;

	YObject yobject;
public:
	std::deque<int> getVpdi() const
	{
		return vpdi;
	}

	void setVpdi(std::deque<int> vpdi)
	{
		this->vpdi = vpdi;
	}

	std::list<int> getVpli() const
	{
		return vpli;
	}

	void setVpli(std::list<int> vpli)
	{
		this->vpli = vpli;
	}

	std::set<int> getVpls() const
	{
		return vpls;
	}

	void setVpls(std::set<int> vpls)
	{
		this->vpls = vpls;
	}

	std::multiset<int> getVpmsi() const
	{
		return vpmsi;
	}

	void setVpmsi(std::multiset<int> vpmsi)
	{
		this->vpmsi = vpmsi;
	}

	std::queue<int> getVpqi() const
	{
		return vpqi;
	}

	void setVpqi(std::queue<int> vpqi)
	{
		this->vpqi = vpqi;
	}

	std::vector<int> getVpvi() const
	{
		return vpvi;
	}

	void setVpvi(std::vector<int> vpvi)
	{
		this->vpvi = vpvi;
	}

	std::deque<int>* getVppdi() const {
		return vppdi;
	}

	void setVppdi(std::deque<int>* vppdi) {
		this->vppdi = vppdi;
	}

	std::list<int>* getVppli() const {
		return vppli;
	}

	void setVppli(std::list<int>* vppli) {
		this->vppli = vppli;
	}

	std::set<int>* getVppls() const {
		return vppls;
	}

	void setVppls(std::set<int>* vppls) {
		this->vppls = vppls;
	}

	std::multiset<int>* getVppmsi() const {
		return vppmsi;
	}

	void setVppmsi(std::multiset<int>* vppmsi) {
		this->vppmsi = vppmsi;
	}

	std::queue<int>* getVppqi() const {
		return vppqi;
	}

	void setVppqi(std::queue<int>* vppqi) {
		this->vppqi = vppqi;
	}

	std::vector<int>* getVppvi() const {
		return vppvi;
	}

	void setVppvi(std::vector<int>* vppvi) {
		this->vppvi = vppvi;
	}

	bool operator <(TestSTLs t) const
	{
		return this->mi < t.mi;
	}

	TestSTLs()
	{
	}

	virtual ~TestSTLs()
	{
	}

	std::string toString()
	{
		std::string str;
		for (int var = 0; var < (int)vvi.size(); ++var) {
			str += CastUtil::lexical_cast<std::string>(vvi.at(var));
		}
		for (int var = 0; var < (int)vvsh.size(); ++var) {
			str += CastUtil::lexical_cast<std::string>(vvsh.at(var));
		}
		for (int var = 0; var < (int)vvl.size(); ++var) {
			str += CastUtil::lexical_cast<std::string>(vvl.at(var));
		}
		for (int var = 0; var < (int)vvd.size(); ++var) {
			str += CastUtil::lexical_cast<std::string>(vvd.at(var));
		}
		for (int var = 0; var < (int)vvb.size(); ++var) {
			str += CastUtil::lexical_cast<std::string>(vvb.at(var));
		}
		for (int var = 0; var < (int)vvs.size(); ++var) {
			str += CastUtil::lexical_cast<std::string>(vvs.at(var));
		}

		for (std::list<int>::iterator it=vli.begin();it!=vli.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::list<short>::iterator it=vlsh.begin();it!=vlsh.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::list<long>::iterator it=vll.begin();it!=vll.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::list<double>::iterator it=vld.begin();it!=vld.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::list<bool>::iterator it=vlb.begin();it!=vlb.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::list<std::string>::iterator it=vls.begin();it!=vls.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}

		for (std::set<int>::iterator it=vsi.begin();it!=vsi.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::set<short>::iterator it=vssh.begin();it!=vssh.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::set<long>::iterator it=vsl.begin();it!=vsl.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::set<double>::iterator it=vsd.begin();it!=vsd.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::set<std::string>::iterator it=vss.begin();it!=vss.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}

		for (std::multiset<int>::iterator it=vmsi.begin();it!=vmsi.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::multiset<short>::iterator it=vmssh.begin();it!=vmssh.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::multiset<long>::iterator it=vmsl.begin();it!=vmsl.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::multiset<double>::iterator it=vmsd.begin();it!=vmsd.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::multiset<std::string>::iterator it=vmss.begin();it!=vmss.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}

		for (std::deque<int>::iterator it=vdi.begin();it!=vdi.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::deque<short>::iterator it=vdsh.begin();it!=vdsh.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::deque<long>::iterator it=vdl.begin();it!=vdl.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::deque<double>::iterator it=vdd.begin();it!=vdd.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::deque<bool>::iterator it=vdb.begin();it!=vdb.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}
		for (std::deque<std::string>::iterator it=vds.begin();it!=vds.end();it++) {
			str += CastUtil::lexical_cast<std::string>(*it);
		}

		std::queue<int> temp = vqi;
		while (!temp.empty()) {
			str += CastUtil::lexical_cast<std::string>(temp.front());
			temp.pop();
		}
		std::queue<short> tempsh = vqsh;
		while (!tempsh.empty()) {
			str += CastUtil::lexical_cast<std::string>(tempsh.front());
			tempsh.pop();
		}
		std::queue<long> templ = vql;
		while (!templ.empty()) {
			str += CastUtil::lexical_cast<std::string>(templ.front());
			templ.pop();
		}
		std::queue<double> tempd = vqd;
		while (!tempd.empty()) {
			str += CastUtil::lexical_cast<std::string>(tempd.front());
			tempd.pop();
		}
		std::queue<bool> tempb = vqb;
		while (!tempb.empty()) {
			str += CastUtil::lexical_cast<std::string>(tempb.front());
			tempb.pop();
		}
		std::queue<std::string> temps = vqs;
		while (!temps.empty()) {
			str += CastUtil::lexical_cast<std::string>(temps.front());
			temps.pop();
		}
		return str;
	}
};

#endif /* TESTSTLS_H_ */
