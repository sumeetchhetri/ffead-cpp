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
 * Logger.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#include "LoggerFactory.h"

string Logger::LEVEL_OFF = "OFF";
string Logger::LEVEL_FATAL = "FATAL";
string Logger::LEVEL_ERROR = "ERROR";
string Logger::LEVEL_WARN = "WARN";
string Logger::LEVEL_INFO = "INFO";
string Logger::LEVEL_DEBUG = "DEBUG";
string Logger::LEVEL_TRACE = "TRACE";

map<string, int> Logger::levelMap;

Logger::Logger(LoggerConfig *config, const string& className)
{
	this->className = className;
	this->config = NULL;
	this->level = LEVEL_INFO;
	if(config->level!=LEVEL_OFF) {
		this->config = config;
	}
}

void Logger::setClassName(const string& className)
{
	this->className = className;
}

Logger::Logger(LoggerConfig *config, const string& className, const string& level)
{
	this->className = className;
	this->config = NULL;
	this->level = level;
	if(config->level!=LEVEL_OFF) {
		this->config = config;
	}
}

Logger::Logger()
{
	this->config = NULL;
}

Logger::~Logger()
{
	this->config = NULL;
}

void Logger::write(const string& msg, const string& mod, const bool& newline)
{
	if(config==NULL)return;
	Date dat;
	string te = config->datFormat.format(dat);
	string vhnclsn = this->className + (config->vhostNumber>0?("-"+CastUtil::lexical_cast<string>(config->vhostNumber)):"");
	string fmsg = "[" + te + "] ("+vhnclsn + ") <"+mod+"> :"+msg+(newline?"\n":"");
	config->lock.lock();
	config->out->write(fmsg.c_str(),fmsg.length());
	*config->out << flush;
	config->lock.unlock();
}

void Logger::write(ostream& (*pf) (ostream&), const string& mod)
{
	if(config==NULL)return;
	Date dat;
	string vhnclsn = this->className + (config->vhostNumber>0?("-"+CastUtil::lexical_cast<string>(config->vhostNumber)):"");
	string te = config->datFormat.format(dat);
	string msg = "[" + te + "] ("+vhnclsn + ") <"+mod+"> :";
	config->lock.lock();
	*config->out << msg;
	*config->out << pf;
	config->lock.unlock();
}

void Logger::fatal(const string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->mode]==2)
	{
		write(msg,LEVEL_FATAL,true);
	}
}

void Logger::error(const string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->mode]<=3)
	{
		write(msg,LEVEL_ERROR,true);
	}
}

void Logger::warn(const string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->mode]<=4)
	{
		write(msg,LEVEL_WARN,true);
	}
}

void Logger::info(const string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->mode]<=5)
	{
		write(msg,LEVEL_INFO,true);
	}
}

void Logger::debug(const string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->mode]<=6)
	{
		write(msg,LEVEL_DEBUG,true);
	}
}

void Logger::trace(const string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->mode]<=7)
	{
		write(msg,LEVEL_TRACE,true);
	}
}

Logger& operator<< (Logger& logger, ostream& (*pf) (ostream&))
{
	logger.write(pf, logger.level);
	if(pf == static_cast <std::ostream & (*)(std::ostream &)> (std::endl) ||
			pf == static_cast <std::ostream & (*)(std::ostream &)> (std::flush)) {
		logger.level = Logger::LEVEL_INFO;
	}
	return logger;
}

Logger& Logger::f()
{
	level = LEVEL_FATAL;
	return *this;
}

Logger& Logger::e()
{
	level = LEVEL_ERROR;
	return *this;
}

Logger& Logger::w()
{
	level = LEVEL_WARN;
	return *this;
}

Logger& Logger::i()
{
	level = LEVEL_INFO;
	return *this;
}

Logger& Logger::d()
{
	level = LEVEL_DEBUG;
	return *this;
}

Logger& Logger::t()
{
	level = LEVEL_TRACE;
	return *this;
}
