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
 * Logger.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#include "Logger.h"

// Global static pointer used to ensure a single instance of the class.
boost::mutex* Logger::_theLogmutex = NULL;
string* Logger::level = NULL;
string* Logger::mode = NULL;
string* Logger::filepath = NULL;
DateFormat* Logger::datFormat = NULL;
ofstream* Logger::out = NULL;

string Logger::LEVEL_ERROR = "ERROR";
string Logger::LEVEL_DEBUG = "DEBUG";
string Logger::LEVEL_INFO = "INFO";

Logger Logger::getLogger(string className)
{
	init();
	Logger logger(className);
	return logger;
}

void Logger::init()
{
	if(!_theLogmutex)
	{
		_theLogmutex = new boost::mutex();
		level = new string(LEVEL_ERROR);
		mode = new string("CONSOLE");
		datFormat = new DateFormat("dd/mm/yyyy hh:mi:ss");
	}
}

void Logger::init(string file)
{
	if(!_theLogmutex)
	{
		_theLogmutex = new boost::mutex();

		PropFileReader pf;
		propMap props = pf.getProperties(file);
		if(props.size()==0)
		{
			level = new string(LEVEL_ERROR);
			mode = new string("CONSOLE");
			datFormat = new DateFormat("dd/mm/yyyy hh:mi:ss");cout << mode << endl;
			return;
		}
		level = new string(props["LEVEL"]);
		mode = new string(props["MODE"]);
		filepath = new string(props["FILEPATH"]);
		datFormat = new DateFormat(props["DATEFMT"]);cout << mode << endl;
		if(*mode=="FILE")
		{
			out = new ofstream();
			out->open(filepath->c_str(),ios::app | ios::binary);
		}
	}
}

void Logger::init(string llevel,string lmode,string lfilepath)
{
	if(!_theLogmutex)
	{
		_theLogmutex = new boost::mutex();

		level = new string(llevel);
		mode = new string(lmode);
		filepath = new string(lfilepath);
		datFormat = new DateFormat("dd/mm/yyyy hh:mi:ss");
		if(*mode=="FILE")
		{
			out = new ofstream();
			out->open(filepath->c_str(),ios::app | ios::binary);
		}
	}
}

Logger::Logger()
{}

Logger::Logger(string className)
{
	this->className = className;
}

Logger::~Logger()
{

}

void Logger::write(string msg,string mod)
{
	Date dat;
	string te = datFormat->format(dat);
	msg = "[" + te + "] ("+this->className + ") <"+mod+"> :"+msg+"\n";
	if(*mode=="FILE")
	{
		_theLogmutex->lock();
		out->write(msg.c_str(),msg.length());
		*out << flush;
		_theLogmutex->unlock();
	}
	else
	{
		_theLogmutex->lock();
		cout << msg << flush;
		_theLogmutex->unlock();
	}
}

void Logger::write(ostream& (*pf) (ostream&), string mod)
{
	if(*mode=="FILE")
	{
		_theLogmutex->lock();
		*out << pf;
		_theLogmutex->unlock();
	}
	else
	{
		_theLogmutex->lock();
		cout << pf;
		_theLogmutex->unlock();
	}
}

void Logger::info(string msg)
{
	if(*level!=LEVEL_DEBUG)
	{
		write(msg,"info");
	}
}

void Logger::debug(string msg)
{
	if(*level==LEVEL_DEBUG)
	{
		write(msg,"debug");
	}
}

void Logger::error(string msg)
{
	write(msg,"error");
}

Logger& operator<< (Logger& logger, ostream& (*pf) (ostream&))
{
	logger.write(pf,"info");
	return logger;
}

void Logger::destroy()
{
	if(_theLogmutex)
	{
		delete _theLogmutex;
		delete level;
		delete mode;
		delete datFormat;
		if(filepath!=NULL)delete filepath;
		if(out!=NULL)delete out;
	}
}
