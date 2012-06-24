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
 * DLogger.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#include "DLogger.h"

// Global static pointer used to ensure a single instance of the class.
boost::mutex* DLogger::_theLogmutex = NULL;
string DLogger::level = NULL;
string DLogger::mode = NULL;
DateFormat DLogger::datFormat = NULL;
ofstream DLogger::out = NULL;

DLogger* DLogger::getLogger(string className)
{
	DLogger dlogger = new DLogger(className);
	return dlogger;
}

void DLogger::init()
{
	if(_theLogmutex==NULL)
	{
		_theLogmutex = new boost::mutex();
		level = new string("ERROR");
		mode = new string("CONSOLE");
		datFormat = new DateFormat("dd/mm/yyyy hh:mi:ss");
	}
}

void DLogger::init(string file)
{
	if(_theLogmutex==NULL)
	{
		_theLogmutex = new boost::mutex();

		PropFileReader pf;
		propMap props = pf.getProperties(file);
		if(props.size()==0)
		{
			level = new string("ERROR");
			mode = new string("CONSOLE");
			datFormat = new DateFormat("dd/mm/yyyy hh:mi:ss");
			return;
		}
		level = new string(props["LEVEL"]);
		mode = new string(props["MODE"]);
		filepath = new string(props["FILEPATH"]);
		datFormat = new DateFormat(props["DATEFMT"]);
		if(*mode=="FILE")
		{
			out = new ofstream();
			out->open(filepath->c_str(),ios::app | ios::binary);
		}
	}
}

void DLogger::init(string llevel,string lmode,string lfilepath)
{
	if(_theLogmutex==NULL)
	{
		_theLogmutex = new boost::mutex();

		level = new string(llevel);
		mode = new string(lmode);
		filepath = new string(lfilepath);
		datFormat = new DateFormat(props["DATEFMT"]);
		if(*mode=="FILE")
		{
			out = new ofstream();
			out->open(filepath->c_str(),ios::app | ios::binary);
		}
	}
}

DLogger::DLogger(string className)
{
	this->className = className;
}

DLogger::~DLogger()
{
	if(_theLogmutex!=NULL)
	{
		delete _theLogmutex;
		delete level;
		delete mode;
		delete datFormat;
		if(filepath!=NULL)delete filepath;
		if(out!=NULL)delete out;
	}
}

void DLogger::write(string msg,string mod)
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

void DLogger::info(string msg)
{
	write(msg,"info");
}

void DLogger::debug(string msg)
{
	write(msg,"debug");
}

void DLogger::error(string msg)
{
	write(msg,"error");
}
