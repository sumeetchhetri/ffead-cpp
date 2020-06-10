/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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
