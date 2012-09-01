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
 * Logger.h
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#ifndef LOGGER_H_
#define LOGGER_H_
#include "PropFileReader.h"
#include "DateFormat.h"
#include "Mutex.h"
class Logger {
public:
	static string LEVEL_ERROR;
	static string LEVEL_DEBUG;
	static string LEVEL_INFO;
	static Logger getLogger(string className);
	void info(string);
	void debug(string);
	void error(string);
	static void init();
	static void destroy();
	static void init(string file);
	static void init(string level,string mode,string file);
	Logger();
	virtual ~Logger();
	template <typename T>
	friend Logger& operator<< (Logger& logger, T msg)
	{
		logger.write(msg,"info");
		return logger;
	}
	friend Logger& operator<< (Logger& logger, ostream& (*pf) (ostream&));
private:
	Logger(string);
	Logger(string,string,string);
	string className;
	static DateFormat* datFormat;
	static string* level;
	static string* mode;
	static string* filepath;
	static ofstream* out;
	static Mutex* _theLogmutex;
	void write(string,string);
	template <typename T>
	void write(T tmsg, string mod)
	{
		Date dat;
		string te = datFormat->format(dat);
		string msg = "[" + te + "] ("+this->className + ") <"+mod+"> :";
		if(*mode=="FILE")
		{
			_theLogmutex->lock();
			*out << msg << tmsg << endl;
			_theLogmutex->unlock();
		}
		else
		{
			_theLogmutex->lock();
			cout << msg << tmsg << endl;
			_theLogmutex->unlock();
		}
	}
	void write(ostream& (*pf) (ostream&), string mod);
};
#endif /* LOGGER_H_ */
