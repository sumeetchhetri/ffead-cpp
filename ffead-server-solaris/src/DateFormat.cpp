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
	boost::replace_all(temp,"hh",date.getHhStr());
	boost::replace_all(temp,"mi",date.getMmStr());
	boost::replace_all(temp,"ss",date.getSsStr());
	boost::replace_all(temp,"ns",date.getNsStr());
	boost::replace_all(temp,"ddd",date.getDayw());
	boost::replace_all(temp,"dd",date.getDayStr());
	boost::replace_all(temp,"mmm",date.getMonthw());
	boost::replace_all(temp,"mm",date.getMonthStr());
	boost::replace_all(temp,"yyyy",date.getYearStr());
	boost::replace_all(temp,"yy",date.getYearStr().substr(2));
	return temp;
}

Date* DateFormat::parse(string strdate)
{
	string temp = this->formatspec;
	Date* date = NULL;
	string yyyy,yy,ddd,dd,mmm,mm,hh,mi,ss;
	if(temp.find("yyyy")!=string::npos)
	{
		yyyy = strdate.substr(temp.find("yyyy"),4);
	}
	else if(temp.find("yy")!=string::npos)
	{
		yy = strdate.substr(temp.find("yy"),2);
	}
	if(temp.find("ddd")!=string::npos)
	{
		ddd = strdate.substr(temp.find("ddd"),3);
	}
	if(temp.find("dd", temp.find("ddd")+3)!=string::npos)
	{
		dd = strdate.substr(temp.find("dd", temp.find("ddd")+3),2);
	}
	if(temp.find("mmm")!=string::npos)
	{
		mmm = strdate.substr(temp.find("mmm"),3);
	}
	else if(temp.find("mm")!=string::npos)
	{
		mm = strdate.substr(temp.find("mm"),2);
	}
	if(temp.find("hh")!=string::npos)
	{
		hh = strdate.substr(temp.find("hh"),2);
	}
	if(temp.find("mi")!=string::npos)
	{
		mi = strdate.substr(temp.find("mi"),2);
	}
	if(temp.find("ss")!=string::npos)
	{
		ss = strdate.substr(temp.find("ss"),2);
	}
	try
	{
		if(yyyy!="")
		{
			if(mmm!="")
			{
				date = new Date(boost::lexical_cast<int>(yyyy),
						mmm, boost::lexical_cast<int>(dd));
			}
			else if(mm!="")
			{
				date = new Date(boost::lexical_cast<int>(yyyy),
						boost::lexical_cast<int>(mm), boost::lexical_cast<int>(dd));
			}
			else
			{
				throw "Invalid Date month specified";
			}
		}
		else if(yy!="")
		{
			if(mmm!="")
			{
				date = new Date(boost::lexical_cast<int>(yy),
						mmm, boost::lexical_cast<int>(dd));
			}
			else if(mm!="")
			{
				date = new Date(boost::lexical_cast<int>(yy),
						boost::lexical_cast<int>(mm), boost::lexical_cast<int>(dd));
			}
			else
			{
				throw "Invalid Date month specified";
			}
		}
		else
		{
			throw "Invalid Date year specified";
		}
	} catch (const char* s) {
		throw s;
	} catch (...) {
		throw "Invalid Date specified";
	}
	date->setTime(boost::lexical_cast<int>(hh),
		boost::lexical_cast<int>(mi), boost::lexical_cast<int>(ss));
	return date;
}
