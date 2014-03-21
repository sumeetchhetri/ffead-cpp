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

class LoggerConfig
{
	string name, mode, level, file, logdirtype;
	Mutex lock;
	ostream* out;
	DateFormat datFormat;
	friend class LoggerFactory;
	friend class Logger;
};

class Logger {
public:
	static string LEVEL_ERROR;
	static string LEVEL_DEBUG;
	static string LEVEL_INFO;
	void info(string);
	void debug(string);
	void error(string);
	Logger();
	virtual ~Logger();
	template <typename T>
	friend Logger& operator<< (Logger& logger, T msg)
	{
		logger.write(msg,"info",false);
		return logger;
	}
	friend Logger& operator<< (Logger& logger, ostream& (*pf) (ostream&));
private:
	friend class LoggerFactory;
	Logger(LoggerConfig *config, string className);
	Logger(LoggerConfig *config, string className, string level);
	string className, level;
	LoggerConfig *config;
	void write(string msg,string mod,bool newline);
	template <typename T>
	void write(T tmsg, string mod,bool newline)
	{
		Date dat;
		string te = config->datFormat.format(dat);
		string msg = "[" + te + "] ("+this->className + ") <"+mod+"> :";
		//if(mode=="FILE")
		{
			config->lock.lock();
			*config->out << msg << tmsg;
			if(newline)
				*config->out << endl;
			else
				*config->out << flush;
			config->lock.unlock();
		}
		/*else
		{
			config->lock.lock();
			*config->out << msg << tmsg;
			if(newline)
				*config->out << endl;
			else
				*config->out << flush;
			config->lock.unlock();
		}*/
	}
	void write(ostream& (*pf) (ostream&), string mod);
};
#endif /* LOGGER_H_ */
