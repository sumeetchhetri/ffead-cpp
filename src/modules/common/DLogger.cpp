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
 * DLogger.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#include "DLogger.h"

// Global static pointer used to ensure a single instance of the class.
DLogger* DLogger::m_pInstance = NULL;

DLogger* DLogger::getDLogger()
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new DLogger();
	}
	return m_pInstance;
}

void DLogger::init()
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new DLogger();
	}
}

void DLogger::clear()
{
	if(m_pInstance!=NULL)
	{
		delete m_pInstance;
	}
}

void DLogger::init(std::string file)
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new DLogger(file);
	}
}

void DLogger::init(std::string level,std::string mode,std::string file)
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new DLogger(level,mode,file);
	}
}

DLogger::DLogger()
{
	PropFileReader pf;
	level = "ERROR";
	mode = "CONSOLE";
	datFormat.setFormatspec("%d/%m/%Y %H:%M:%S");
}

DLogger::DLogger(std::string file)
{
	PropFileReader pf;
	propMap props = pf.getProperties(file);
	if(props.size()==0)
	{
		level = "ERROR";
		mode = "CONSOLE";
		datFormat.setFormatspec("%d/%m/%Y %H:%M:%S");
		return;
	}
	level = props["LEVEL"];
	mode = props["MODE"];
	filepath = props["FILEPATH"];
	if(mode=="FILE")
		out.open(filepath.c_str(),std::ios::app | std::ios::binary);
	datFormat.setFormatspec(props["DATEFMT"]);
}
DLogger::DLogger(std::string level,std::string mode,std::string filepath)
{
	this->level = level;
	this->mode = mode;
	this->filepath = filepath;
}

DLogger::~DLogger()
{
	out.close();
}

void DLogger::write(std::string msg,std::string mod)
{
	Date dat;
	std::string te = this->datFormat.format(dat);
	if(mode=="FILE")
	{
		msg = "[" + te + "] <"+mod+"> :"+msg+"\n";
		m_pInstance->p_mutex.lock();
		m_pInstance->out.write(msg.c_str(),msg.length());
		m_pInstance->out << std::flush;
		m_pInstance->p_mutex.unlock();
	}
	else
	{
		msg = "[" + te + "] <"+mod+"> :"+msg+"\n";
		m_pInstance->p_mutex.lock();
		std::cout << msg << std::flush;
		m_pInstance->p_mutex.unlock();
	}
}

void DLogger::info(std::string msg)
{
	m_pInstance->write(msg,"info");
}

void DLogger::debug(std::string msg)
{
	m_pInstance->write(msg,"debug");
}

void DLogger::error(std::string msg)
{
	m_pInstance->write(msg,"error");
}
