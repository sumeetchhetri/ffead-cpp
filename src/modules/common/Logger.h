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
#include "RegexUtil.h"

class LoggerConfig
{
	std::string name, mode, level, file, logdirtype, pattern;
	Mutex* lock;
	std::ostream* out;
	DateFormat datFormat;
	int vhostNumber;
	friend class LoggerFactory;
	friend class Logger;
public:
	LoggerConfig();
	~LoggerConfig();
};

class Logger {
public:
	static std::string LEVEL_OFF;
	static std::string LEVEL_FATAL;
	static std::string LEVEL_ERROR;
	static std::string LEVEL_WARN;
	static std::string LEVEL_INFO;
	static std::string LEVEL_DEBUG;
	static std::string LEVEL_TRACE;
	void fatal(const std::string&);
	void error(const std::string&);
	void warn(const std::string&);
	void info(const std::string&);
	void debug(const std::string&);
	void trace(const std::string&);
	Logger();
	virtual ~Logger();
	Logger& f();
	Logger& e();
	Logger& w();
	Logger& i();
	Logger& d();
	Logger& t();
	Logger& o();
	template <typename T>
	friend Logger& operator<< (Logger& logger, const T& msg)
	{
		if(logger.config==NULL) return logger;
		logger.writeTemplate(msg, logger.level, false);
		return logger;
	}
	friend Logger& operator<< (Logger& logger, std::ostream& (*pf) (std::ostream&));
	//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/8098080
	template<typename ... Args>
	void write(const std::string& format, Args ... args)
	{
		if(config==NULL) return;
		int size_s = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
		if( size_s <= 0 ) {
			writeInternal(format, level, false);
			return;
		}
		char buf[size_s];
		snprintf( buf, size_s, format.c_str(), args ... );
		writeInternal(std::string( &buf[0], size_s - 1 ), level, false);
	}
private:
	static std::map<std::string, int> levelMap;
	void setClassName(const std::string& className);
	friend class CHServer;
	friend class LoggerFactory;
	Logger(LoggerConfig *config, const std::string& className);
	Logger(LoggerConfig *config, const std::string& className, const std::string& level);
	std::string className, level, oldLevel;
	LoggerConfig *config;
	void writeInternal(const std::string& msg, const std::string& mod, const bool& newline);
	template <typename T>
	void writeTemplate(const T& tmsg, const std::string& mod, const bool& newline)
	{
		if(config==NULL)return;
		Date dat;
		std::string te = config->datFormat.format(dat);
		std::string vhnclsn = this->className + (config->vhostNumber>0?("-"+CastUtil::fromNumber(config->vhostNumber)):"");
		std::string msg = "[" + te + "] ("+vhnclsn + ") <"+mod+"> :";
		config->lock->lock();
		*config->out << msg << tmsg;
		if(newline)
		{
			*config->out << std::endl;
		}
		else
		{
			*config->out << std::flush;
		}
		config->lock->unlock();
	}
	void writeToStream(std::ostream& (*pf) (std::ostream&), const std::string& mod);
};
#endif /* LOGGER_H_ */
