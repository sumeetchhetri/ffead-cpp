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

Logger::Logger()
{
	PropFileReader pf;
	level = "ERROR";
	mode = "CONSOLE";
	datFormat.setFormatspec("dd/mm/yyyy hh:mi:ss");
}

Logger::Logger(string file)
{
	PropFileReader pf;
	propMap props = pf.getProperties(file);
	level = props["LEVEL"];
	mode = props["MODE"];
	filepath = props["FILEPATH"];
	datFormat.setFormatspec(props["DATEFMT"]);
}
Logger::Logger(string level,string mode,string file)
{
	this->level = level;
	this->mode = mode;
	this->filepath = filepath;
}

Logger::~Logger()
{
	out.close();
}

void Logger::write(string msg,string mod)
{
	Date dat;
	string te = this->datFormat.format(dat);
	if(mode=="FILE")
	{
		out.open(filepath.c_str(),ios::app | ios::binary);
		msg = "[" + te + "] <"+mod+"> :"+msg+"\n";
		out.write(msg.c_str(),msg.length());
		out.close();
	}
	else
	{
		msg = "[" + te + "] <"+mod+"> :"+msg+"\n";
		cout << msg << flush;
	}
}

void Logger::info(string msg)
{
	write(msg,"info");
}

void Logger::debug(string msg)
{
	write(msg,"debug");
}

void Logger::error(string msg)
{
	write(msg,"error");
}
