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
 * DLogger.h
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#ifndef DLogger_H_
#define DLogger_H_
#include "PropFileReader.h"
#include "DateFormat.h"
#include "queue.h"
#include <boost/thread/thread.hpp>
class DLogger {
public:
	static DLogger* getDLogger();
	void info(string);
	void debug(string);
	void error(string);
	static void init();
	static void init(string file);
	static void init(string level,string mode,string file);
private:
	virtual ~DLogger();
	DLogger();
	DLogger(string);
	DLogger(string,string,string);
	string className;
	static DateFormat datFormat;
	static string level;
	static string mode;
	static string filepath;
	static ofstream out;
	void write(string,string);
	static boost::mutex* _theLogmutex;
};

#endif /* DLogger_H_ */
