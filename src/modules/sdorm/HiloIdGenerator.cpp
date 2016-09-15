/*
 * HiloIdGenerator.cpp
 *
 *  Created on: 24-Sep-2014
 *      Author: sumeetc
 */

#include "HiloIdGenerator.h"

std::map<std::string, long long> HiloIdGenerator::idsInSession;

std::map<std::string, long long> HiloIdGenerator::hiloIdMaxValuesInSession;

std::map<std::string, Mutex> HiloIdGenerator::locks;

std::string HiloIdGenerator::ALL = "__ALL__";

void HiloIdGenerator::init(const std::string& name, const long long& id, const int& lowValue, const bool& forceReinit) {
	locks[name].lock();
	if(forceReinit || (!forceReinit && idsInSession.find(name)==idsInSession.end()))
	{
		idsInSession[name] = id*lowValue;
		hiloIdMaxValuesInSession[name] = ((id+1)*lowValue) - 1;
	}
	locks[name].unlock();
}

void HiloIdGenerator::init(const long long& id, const int& lowValue, const bool& forceReinit) {
	locks[ALL].lock();
	if(forceReinit || (!forceReinit && idsInSession.find(ALL)==idsInSession.end()))
	{
		idsInSession[ALL] = id*lowValue;
		hiloIdMaxValuesInSession[ALL] = ((id+1)*lowValue) - 1;
	}
	locks[ALL].unlock();
}

long long HiloIdGenerator::next() {
	if(locks.find(ALL)==locks.end())return -1;
	long long val = -2;
	locks[ALL].lock();
	if((idsInSession[ALL]+1)<hiloIdMaxValuesInSession[ALL])
	{
		val = idsInSession[ALL];
		idsInSession[ALL] = val + 1;
	}
	locks[ALL].unlock();
	return val;
}

long long HiloIdGenerator::next(const std::string& name) {
	if(locks.find(name)==locks.end())return -1;
	long long val = -2;
	locks[name].lock();
	if((idsInSession[name]+1)<hiloIdMaxValuesInSession[name])
	{
		val = idsInSession[name];
		idsInSession[name] = val + 1;
	}
	locks[name].unlock();
	return val;
}

bool HiloIdGenerator::isInitialized(const std::string& name) {
	locks[name].lock();
	bool fl = idsInSession.find(name)!=idsInSession.end();
	locks[name].unlock();
	return fl;
}

bool HiloIdGenerator::isInitialized() {
	locks[ALL].lock();
	bool fl = idsInSession.find(ALL)!=idsInSession.end();
	locks[ALL].unlock();
	return fl;
}
