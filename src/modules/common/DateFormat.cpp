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
 * DateFormat.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: sumeet
 */

#include "DateFormat.h"

const std::string DateFormat::DF = "%Y-%m-%d %H:%M:%S";

DateFormat::DateFormat() {
	this->formatspec = DF;
}

DateFormat::~DateFormat() {
}

DateFormat::DateFormat(const std::string& format)
{
	this->formatspec  = format;
}

std::string DateFormat::appendZero(const int& value)
{
	std::string te = CastUtil::fromNumber(value);
	if(te.length()==1)
		te = "0" + te;
	return te;
}

std::string DateFormat::format(Date* date)
{
	char buffer[100];
	strftime(buffer, sizeof(buffer), formatspec.c_str(), date->getTimeinfo());
	return std::string(buffer);
}

std::string DateFormat::format(Date& date)
{
	char buffer[100];
	strftime(buffer, sizeof(buffer), formatspec.c_str(), date.getTimeinfo());
	return std::string(buffer);
}

Date* DateFormat::parse(std::string strdate)
{
	return new Date(strdate, formatspec);
}

const std::string& DateFormat::getFormatspec() const
{
	return formatspec;
}

void DateFormat::setFormatspec(const std::string& formatspec)
{
	this->formatspec = formatspec;
}
