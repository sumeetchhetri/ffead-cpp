/*
	Copyright 2009-2020, Sumeet Chhetri

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

std::string Logger::LEVEL_OFF = "OFF";
std::string Logger::LEVEL_FATAL = "FATAL";
std::string Logger::LEVEL_ERROR = "ERROR";
std::string Logger::LEVEL_WARN = "WARN";
std::string Logger::LEVEL_INFO = "INFO";
std::string Logger::LEVEL_DEBUG = "DEBUG";
std::string Logger::LEVEL_TRACE = "TRACE";

std::map<std::string, int> Logger::levelMap;

LoggerConfig::LoggerConfig() {
	vhostNumber = 0;
	lock = NULL;
	out = NULL;
}

LoggerConfig::~LoggerConfig() {
	lock->lock();
	if(mode=="FILE" && file!="" && out!=NULL) {
		delete out;
	}
	lock->unlock();
	if(lock!=NULL) {
		delete lock;
	}
}

Logger::Logger(LoggerConfig *config, const std::string& className)
{
	this->className = className;
	this->config = NULL;
	this->level = LEVEL_INFO;
	this->oldLevel = level;
	if(config->level!=LEVEL_OFF) {
		this->config = config;
	}
}

void Logger::setClassName(const std::string& className)
{
	this->className = className;
}

Logger::Logger(LoggerConfig *config, const std::string& className, const std::string& level)
{
	this->className = className;
	this->config = NULL;
	this->level = level;
	this->oldLevel = level;
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

void Logger::writeInternal(const std::string& msg, const std::string& mod, const bool& newline)
{
	if(config==NULL)return;
	Date dat;
	std::string te = config->datFormat.format(dat);
	std::string vhnclsn = this->className + (config->vhostNumber>0?("-"+CastUtil::fromNumber(config->vhostNumber)):"");
	std::string fmsg = "[" + te + "] ("+vhnclsn + ") <"+mod+"> :"+msg+(newline?"\n":"");
	config->lock->lock();
	config->out->write(fmsg.c_str(),fmsg.length());
	*config->out << std::flush;
	config->lock->unlock();
}

void Logger::writeToStream(std::ostream& (*pf) (std::ostream&), const std::string& mod)
{
	if(config==NULL)return;
	Date dat;
	std::string vhnclsn = this->className + (config->vhostNumber>0?("-"+CastUtil::fromNumber(config->vhostNumber)):"");
	std::string te = config->datFormat.format(dat);
	std::string msg = "[" + te + "] ("+vhnclsn + ") <"+mod+"> :";
	config->lock->lock();
	*config->out << msg;
	*config->out << pf;
	config->lock->unlock();
}

void Logger::fatal(const std::string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->level]>=7)
	{
		writeInternal(msg,LEVEL_FATAL,true);
	}
}

void Logger::error(const std::string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->level]>=6)
	{
		writeInternal(msg,LEVEL_ERROR,true);
	}
}

void Logger::warn(const std::string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->level]>=5)
	{
		writeInternal(msg,LEVEL_WARN,true);
	}
}

void Logger::info(const std::string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->level]>=4)
	{
		writeInternal(msg,LEVEL_INFO,true);
	}
}

void Logger::debug(const std::string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->level]>=3)
	{
		writeInternal(msg,LEVEL_DEBUG,true);
	}
}

void Logger::trace(const std::string& msg)
{
	if(config==NULL)return;
	if(levelMap[config->level]>=2)
	{
		writeInternal(msg,LEVEL_TRACE,true);
	}
}

Logger& operator<< (Logger& logger, std::ostream& (*pf) (std::ostream&))
{
	if(logger.config==NULL) return logger;
	logger.writeToStream(pf, logger.level);
	if(pf == static_cast <std::ostream & (*)(std::ostream &)> (std::endl) || pf == static_cast <std::ostream & (*)(std::ostream &)> (std::flush)) {
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

Logger& Logger::o()
{
	level = oldLevel;
	return *this;
}
