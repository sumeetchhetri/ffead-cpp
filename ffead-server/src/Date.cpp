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
 * Date.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: sumeet
 */

#include "Date.h"

string Date::getMon(string mmm)
{
	boost::to_upper(mmm);
	if(mmm=="JAN")return "01";
	else if(mmm=="FEB")return "02";
	else if(mmm=="MAR")return "03";
	else if(mmm=="APR")return "04";
	else if(mmm=="MAY")return "05";
	else if(mmm=="JUN")return "06";
	else if(mmm=="JUL")return "07";
	else if(mmm=="AUG")return "08";
	else if(mmm=="SEP")return "09";
	else if(mmm=="OCT")return "10";
	else if(mmm=="NOV")return "11";
	else if(mmm=="DEC")return "12";
	else return "-1";
}

Date::Date()
{
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	string tem;
	tem.append(asctime(timeinfo));
	boost::replace_all(tem,"\n","");
	boost::replace_all(tem,"  "," ");
	vector<string> temp,vemp;
	boost::iter_split(temp,tem,boost::first_finder(" "));
	this->dayw = temp.at(0);
	this->monthw = temp.at(1);
	this->month = getMon(monthw);
	this->day = temp.at(2);
	boost::iter_split(vemp,temp.at(3),boost::first_finder(":"));
	this->hh = vemp.at(0);
	this->mm = vemp.at(1);
	this->ss = vemp.at(2);
	this->year = temp.at(4);
}

Date::~Date() {
	// TODO Auto-generated destructor stub
}

int Date::getMonth()
{
	if(this->month!="")
		return boost::lexical_cast<int>(this->month);
	else
		return boost::lexical_cast<int>(this->getMon(this->monthw));
}

void Date::setMonth(int month)
{
	this->month = month;
}

string Date::getMonthw() const
{
	return monthw;
}

void Date::setMonthw(string monthw)
{
	this->monthw = monthw;
}

int Date::getYear() const
{
	return boost::lexical_cast<int>(year);
}

void Date::setYear(int year)
{
	this->year = year;
}

int Date::getDay() const
{
	return boost::lexical_cast<int>(day);
}

void Date::setDay(int day)
{
	this->day = day;
}

int Date::getHh() const
{
	return boost::lexical_cast<int>(hh);
}

void Date::setHh(int hh)
{
	this->hh = hh;
}

int Date::getMm() const
{
	return boost::lexical_cast<int>(mm);
}

void Date::setMm(int mm)
{
	this->mm = mm;
}

int Date::getSs() const
{
	return boost::lexical_cast<int>(ss);
}

void Date::setSs(int ss)
{
	this->ss = ss;
}

string Date::getDayw() const
{
	return dayw;
}

void Date::setDayw(string dayw)
{
	this->dayw = dayw;
}

string Date::getDayStr() const
{
	return day;
}

string Date::getMonthStr() const
{
	return month;
}

string Date::getHhStr() const
{
	return hh;
}

string Date::getSsStr() const
{
	return ss;
}

string Date::getMmStr() const
{
	return mm;
}

string Date::getYearStr() const
{
	return year;
}

string Date::toString()
{
	return dayw+" "+monthw+" "+day+" "+hh+":"+mm+":"+ss+" "+year;
}
