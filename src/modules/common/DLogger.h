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
 * DLogger.h
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#ifndef DLOGGER_H_
#define DLOGGER_H_
#include "PropFileReader.h"
#include "DateFormat.h"
#include "Mutex.h"
class DLogger {
public:
	static DLogger* getDLogger();
	static void info(std::string);
	static void debug(std::string);
	static void error(std::string);
	static void clear();
	static void init();
	static void init(std::string file);
	static void init(std::string level,std::string mode,std::string file);
private:
	virtual ~DLogger();
	DLogger();
	DLogger(std::string);
	DLogger(std::string,std::string,std::string);
	DateFormat datFormat;
	std::string level;
	std::string mode;
	std::string filepath;
	std::ofstream out;
	void write(std::string,std::string);
	Mutex p_mutex;
	static DLogger* m_pInstance;
};

#endif /* DLOGGER_H_ */
