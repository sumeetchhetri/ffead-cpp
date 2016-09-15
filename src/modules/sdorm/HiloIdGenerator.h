/*
 * HiloIdGenerator.h
 *
 *  Created on: 24-Sep-2014
 *      Author: sumeetc
 */

#ifndef HILOIDGENERATOR_H_
#define HILOIDGENERATOR_H_
#include "string"
#include "map"
#include "Mutex.h"


class HiloIdGenerator {
	static std::map<std::string, long long> idsInSession;
	static std::map<std::string, long long> hiloIdMaxValuesInSession;
	static std::map<std::string, Mutex> locks;
	static std::string ALL;
public:
	static void init(const std::string& name, const long long& id, const int& lowValue, const bool& forceReinit= false);
	static void init(const long long& id, const int& lowValue, const bool& forceReinit= false);
	static bool isInitialized(const std::string& name);
	static bool isInitialized();

	static long long next();
	static long long next(const std::string& name);
};

#endif /* HILOIDGENERATOR_H_ */
