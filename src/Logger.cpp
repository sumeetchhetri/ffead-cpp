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

// Global static pointer used to ensure a single instance of the class.
string Logger::LEVEL_ERROR = "ERROR";
string Logger::LEVEL_DEBUG = "DEBUG";
string Logger::LEVEL_INFO = "INFO";

Logger::Logger(LoggerConfig *config, string className)
{
	this->className = className;
	this->config = config;
	this->level = level;
}

Logger::Logger(LoggerConfig *config, string className, string level)
{
	this->className = className;
	this->config = config;
	this->level = level;
}

Logger::Logger()
{
	this->config = NULL;
}

Logger::~Logger()
{
}

void Logger::write(string msg,string mod,bool newline)
{
	Date dat;
	string te = config->datFormat.format(dat);
	msg = "[" + te + "] ("+this->className + ") <"+mod+"> :"+msg+(newline?"\n":"");
	//if(config->mode=="FILE")
	{
		config->lock.lock();
		config->out->write(msg.c_str(),msg.length());
		*config->out << flush;
		config->lock.unlock();
	}
	/*else
	{
		config->lock.lock();
		*config->out << msg << flush;
		config->lock.unlock();
	}*/
}

void Logger::write(ostream& (*pf) (ostream&), string mod)
{
	//if(config->mode=="FILE")
	{
		config->lock.lock();
		*config->out << pf;
		config->lock.unlock();
	}
	/*else
	{
		config->lock.lock();
		*config->out << pf;
		config->lock.unlock();
	}*/
}

void Logger::info(string msg)
{
	if(level!=LEVEL_DEBUG)
	{
		write(msg,"info",true);
	}
}

void Logger::debug(string msg)
{
	if(level==LEVEL_DEBUG)
	{
		write(msg,"debug",true);
	}
}

void Logger::error(string msg)
{
	write(msg,"error",true);
}

Logger& operator<< (Logger& logger, ostream& (*pf) (ostream&))
{
	logger.write(pf,"info");
	return logger;
}
