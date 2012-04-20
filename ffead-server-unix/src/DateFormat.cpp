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
 * DateFormat.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: sumeet
 */

#include "DateFormat.h"

DateFormat::DateFormat() {
	// TODO Auto-generated constructor stub

}

DateFormat::~DateFormat() {
	// TODO Auto-generated destructor stub
}


DateFormat::DateFormat(string format)
{
	this->formatspec  = format;
}

string DateFormat::format(Date date)
{
	string temp = this->formatspec;
	boost::replace_first(temp,"hh",date.getHhStr());
	boost::replace_first(temp,"mi",date.getMmStr());
	boost::replace_first(temp,"ss",date.getSsStr());
	boost::replace_first(temp,"ddd",date.getDayw());
	boost::replace_first(temp,"dd",date.getDayStr());
	boost::replace_first(temp,"mmm",date.getMonthw());
	boost::replace_first(temp,"mm",date.getMonthStr());
	boost::replace_first(temp,"yyyy",date.getYearStr());
	boost::replace_first(temp,"yy",date.getYearStr().substr(2));
	return temp;
}

Date* DateFormat::parse(string strdate)
{
	string temp = this->formatspec;
	Date* date = new Date;
	if(temp.find("yyyy")!=string::npos)
	{
		string yrf = strdate.substr(temp.find("yyyy"),4);
		date->setYearf(yrf);
	}
	if(temp.find("ddd")!=string::npos)
	{
		string yrf = strdate.substr(temp.find("ddd"),3);
		date->setDayw(yrf);
	}
	if(temp.find("mmm")!=string::npos)
	{
		string yrf = strdate.substr(temp.find("mmm"),3);
		date->setMonthw(yrf);
	}
	if(temp.find("mm")!=string::npos)
	{
		string yrf = strdate.substr(temp.find("mm"),2);
		date->setMmf(yrf);
	}
	if(temp.find("dd")!=string::npos)
	{
		string yrf = strdate.substr(temp.find("dd"),2);
		date->setDayf(yrf);
	}
	if(temp.find("hh")!=string::npos)
	{
		string yrf = strdate.substr(temp.find("hh"),2);
		date->setHhf(yrf);
	}
	if(temp.find("mi")!=string::npos)
	{
		string yrf = strdate.substr(temp.find("mi"),2);
		date->setMif(yrf);
	}
	if(temp.find("ss")!=string::npos)
	{
		string yrf = strdate.substr(temp.find("ss"),2);
		date->setSsf(yrf);
	}
	return date;
}
