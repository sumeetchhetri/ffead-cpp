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
 * DateFormat.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: sumeet
 */

#include "DateFormat.h"

DateFormat::DateFormat() {
}

DateFormat::~DateFormat() {
}


DateFormat::DateFormat(const std::string& format)
{
	this->formatspec  = format;
}

std::string DateFormat::appendZero(const int& value)
{
	std::string te = CastUtil::lexical_cast<std::string>(value);
	if(te.length()==1)
		te = "0" + te;
	return te;
}

std::string DateFormat::format(Date* date)
{
	std::string temp = this->formatspec;
	StringUtil::replaceAll(temp,"hh",appendZero(date->getHours()));
	StringUtil::replaceAll(temp,"mi",appendZero(date->getMinutes()));
	StringUtil::replaceAll(temp,"ss",appendZero(date->getSeconds()));
	StringUtil::replaceAll(temp,"ns",appendZero(date->getNanoseconds()));
	StringUtil::replaceAll(temp,"ddd",date->getDayAbbr());
	StringUtil::replaceAll(temp,"dd",appendZero(date->getDay()));
	StringUtil::replaceAll(temp,"mmm",date->getMonthAbbr());
	StringUtil::replaceAll(temp,"mm",appendZero(date->getMonth()));
	StringUtil::replaceAll(temp,"yyyy",appendZero(date->getYear()));
	StringUtil::replaceAll(temp,"yy",appendZero(date->getYear()).substr(2));
	StringUtil::replaceAll(temp,"z",date->getTimeZone());
	std::string tz = CastUtil::lexical_cast<std::string>(date->getTimeZoneOffset()*100);
	if(tz.find(".")!=std::string::npos) {
		tz = tz.substr(0, tz.find("."));
	}
	if(date->getTimeZoneOffset()>0)
	{
		tz = "+" + tz;
		StringUtil::replaceAll(temp,"Z",tz);
	}
	else if(date->getTimeZoneOffset()<0)
	{
		tz = "-" + tz;
		StringUtil::replaceAll(temp,"Z",tz);
	}
	return temp;
}

std::string DateFormat::format(const Date& date)
{
	std::string temp = this->formatspec;
	StringUtil::replaceAll(temp,"hh",appendZero(date.getHours()));
	StringUtil::replaceAll(temp,"mi",appendZero(date.getMinutes()));
	StringUtil::replaceAll(temp,"ss",appendZero(date.getSeconds()));
	StringUtil::replaceAll(temp,"ns",appendZero(date.getNanoseconds()));
	StringUtil::replaceAll(temp,"ddd",date.getDayAbbr());
	StringUtil::replaceAll(temp,"dd",appendZero(date.getDay()));
	StringUtil::replaceAll(temp,"mmm",date.getMonthAbbr());
	StringUtil::replaceAll(temp,"mm",appendZero(date.getMonth()));
	StringUtil::replaceAll(temp,"yyyy",appendZero(date.getYear()));
	StringUtil::replaceAll(temp,"yy",appendZero(date.getYear()).substr(2));
	StringUtil::replaceAll(temp,"z",date.getTimeZone());
	std::string tz = CastUtil::lexical_cast<std::string>(date.getTimeZoneOffset()*100);
	if(tz.find(".")!=std::string::npos) {
		tz = tz.substr(0, tz.find("."));
	}
	if(date.getTimeZoneOffset()>0)
	{
		tz = "+" + tz;
		StringUtil::replaceAll(temp,"Z",tz);
	}
	else if(date.getTimeZoneOffset()<0)
	{
		tz = "-" + tz;
		StringUtil::replaceAll(temp,"Z",tz);
	}
	return temp;
}

Date* DateFormat::parse(std::string strdate)
{
	std::string temp = this->formatspec;
	Date* date = NULL;
	std::string yyyy,yy,ddd,dd,mmm,mm,hh,mi,ss,tzv;
	if(temp.find("yyyy")!=std::string::npos)
	{
		yyyy = strdate.substr(temp.find("yyyy"),4);
		std::string ytemp = temp;
		while(ytemp.find("yyyy")!=std::string::npos)
		{
			std::string tyyyy = strdate.substr(ytemp.find("yyyy"),4);
			if(yyyy!=tyyyy)
				throw std::runtime_error("Multiple instances with different year(yyyy) values found");
			strdate = strdate.substr(0, ytemp.find("yyyy")) +
						strdate.substr(ytemp.find("yyyy")+4);
			StringUtil::replaceFirst(ytemp, "yyyy", "");
		}
		temp = ytemp;
	}
	else if(temp.find("yy")!=std::string::npos)
	{
		yy = strdate.substr(temp.find("yy"),2);
		std::string ytemp = temp;
		while(ytemp.find("yy")!=std::string::npos)
		{
			std::string tyy = strdate.substr(ytemp.find("yy"),2);
			if(yy!=tyy)
				throw std::runtime_error("Multiple instances with different year(yy) values found");
			strdate = strdate.substr(0, ytemp.find("yy")) +
						strdate.substr(ytemp.find("yy")+2);
			StringUtil::replaceFirst(ytemp, "yy", "");
		}
		temp = ytemp;
	}
	if(temp.find("ddd")!=std::string::npos)
	{
		ddd = strdate.substr(temp.find("ddd"),3);
		std::string ytemp = temp;
		while(ytemp.find("ddd")!=std::string::npos)
		{
			std::string tddd = strdate.substr(ytemp.find("ddd"),3);
			if(ddd!=tddd)
				throw std::runtime_error("Multiple instances with different day(ddd) values found");
			strdate = strdate.substr(0, ytemp.find("ddd")) +
						strdate.substr(ytemp.find("ddd")+3);
			StringUtil::replaceFirst(ytemp, "ddd", "");
		}
		temp = ytemp;
	}
	if(temp.find("dd")!=std::string::npos)
	{
		dd = strdate.substr(temp.find("dd"),2);
		std::string ytemp = temp;
		while(ytemp.find("dd")!=std::string::npos)
		{
			std::string tdd = strdate.substr(ytemp.find("dd"),2);
			if(dd!=tdd)
				throw std::runtime_error("Multiple instances with different day(dd) values found");
			strdate = strdate.substr(0, ytemp.find("dd")) +
						strdate.substr(ytemp.find("dd")+2);
			StringUtil::replaceFirst(ytemp, "dd", "");
		}
		temp = ytemp;
	}
	if(temp.find("mmm")!=std::string::npos)
	{
		mmm = strdate.substr(temp.find("mmm"),3);
		std::string ytemp = temp;
		while(ytemp.find("mmm")!=std::string::npos)
		{
			std::string tmmm = strdate.substr(ytemp.find("mmm"),3);
			if(mmm!=tmmm)
				throw std::runtime_error("Multiple instances with different month(mmm) values found");
			strdate = strdate.substr(0, ytemp.find("mmm")) +
						strdate.substr(ytemp.find("mmm")+3);
			StringUtil::replaceFirst(ytemp, "mmm", "");
		}
		temp = ytemp;
	}
	else if(temp.find("mm")!=std::string::npos)
	{
		mm = strdate.substr(temp.find("mm"),2);
		std::string ytemp = temp;
		while(ytemp.find("mm")!=std::string::npos)
		{
			std::string tmm = strdate.substr(ytemp.find("mm"),2);
			if(mm!=tmm)
				throw std::runtime_error("Multiple instances with different month(mm) values found");
			strdate = strdate.substr(0, ytemp.find("mm")) +
						strdate.substr(ytemp.find("mm")+2);
			StringUtil::replaceFirst(ytemp, "mm", "");
		}
		temp = ytemp;
	}
	if(temp.find("hh")!=std::string::npos)
	{
		hh = strdate.substr(temp.find("hh"),2);
		std::string ytemp = temp;
		while(ytemp.find("hh")!=std::string::npos)
		{
			std::string thh = strdate.substr(ytemp.find("hh"),2);
			if(hh!=thh)
				throw std::runtime_error("Multiple instances with different hours(hh) values found");
			strdate = strdate.substr(0, ytemp.find("hh")) +
						strdate.substr(ytemp.find("hh")+2);
			StringUtil::replaceFirst(ytemp, "hh", "");
		}
		temp = ytemp;
	}
	if(temp.find("mi")!=std::string::npos)
	{
		mi = strdate.substr(temp.find("mi"),2);
		std::string ytemp = temp;
		while(ytemp.find("mi")!=std::string::npos)
		{
			std::string tmi = strdate.substr(ytemp.find("mi"),2);
			if(mi!=tmi)
				throw std::runtime_error("Multiple instances with different minutes(mi) values found");
			strdate = strdate.substr(0, ytemp.find("mi")) +
						strdate.substr(ytemp.find("mi")+2);
			StringUtil::replaceFirst(ytemp, "mi", "");
		}
		temp = ytemp;
	}
	if(temp.find("ss")!=std::string::npos)
	{
		ss = strdate.substr(temp.find("ss"),2);
		std::string ytemp = temp;
		while(ytemp.find("ss")!=std::string::npos)
		{
			std::string tss = strdate.substr(ytemp.find("ss"),2);
			if(ss!=tss)
				throw std::runtime_error("Multiple instances with different seconds(ss) values found");
			strdate = strdate.substr(0, ytemp.find("ss")) +
						strdate.substr(ytemp.find("ss")+2);
			StringUtil::replaceFirst(ytemp, "ss", "");
		}
		temp = ytemp;
	}
	if(temp.find("Z")!=std::string::npos)
	{
		tzv = strdate.substr(temp.find("Z"),5);
		std::string ytemp = temp;
		while(ytemp.find("Z")!=std::string::npos)
		{
			std::string ttzv = strdate.substr(ytemp.find("Z"), 1);
			if(tzv!=ttzv)
				throw std::runtime_error("Multiple instances with different timezone offsets(Z) values found");
			strdate = strdate.substr(0, ytemp.find("Z")) +
						strdate.substr(ytemp.find("Z")+5);
			StringUtil::replaceFirst(ytemp, "Z", "");
		}
		temp = ytemp;
	}
	if(temp!=strdate) {
		throw std::runtime_error("Invalid Date specified");
	}
	try
	{
		if(yyyy!="" && dd!="")
		{
			if(mmm!="")
			{
				date = new Date(CastUtil::lexical_cast<int>(yyyy),
						mmm, CastUtil::lexical_cast<int>(dd));
			}
			else if(mm!="")
			{
				date = new Date(CastUtil::lexical_cast<int>(yyyy),
						CastUtil::lexical_cast<int>(mm), CastUtil::lexical_cast<int>(dd));
			}
			else
			{
				throw std::runtime_error("Invalid Date month specified");
			}
		}
		else if(yy!="" && dd!="")
		{
			if(mmm!="")
			{
				date = new Date(CastUtil::lexical_cast<int>(yy),
						mmm, CastUtil::lexical_cast<int>(dd));
			}
			else if(mm!="")
			{
				date = new Date(CastUtil::lexical_cast<int>(yy),
						CastUtil::lexical_cast<int>(mm), CastUtil::lexical_cast<int>(dd));
			}
			else
			{
				throw std::runtime_error("Invalid Date month specified");
			}
		}
		else
		{
			throw std::runtime_error("Invalid Date year/day specified");
		}
		if(tzv!="")
		{
			try {
				date->setTimeZoneOffset(CastUtil::lexical_cast<float>(tzv));
			} catch(const std::exception& e) {
				throw std::runtime_error("Invalid Timezone specified");
			}
		}
	} catch(const std::exception& e) {
		throw std::runtime_error("Invalid Date specified");
	}
	if(date!=NULL)
	{
		if(ddd!="" && date->getDayAbbr()!=StringUtil::toUpperCopy(ddd))
		{
			throw std::runtime_error("Invalid Day(ddd) specified");
		}
	}
	if(hh=="")
		hh = CastUtil::lexical_cast<std::string>(date->getHours());
	if(mi=="")
		mi = CastUtil::lexical_cast<std::string>(date->getMinutes());
	if(ss=="")
		ss = CastUtil::lexical_cast<std::string>(date->getSeconds());
	date->setTime(CastUtil::lexical_cast<int>(hh),
		CastUtil::lexical_cast<int>(mi), CastUtil::lexical_cast<int>(ss));
	return date;
}

const std::string& DateFormat::getFormatspec() const
{
	return formatspec;
}

void DateFormat::setFormatspec(const std::string& formatspec)
{
	this->formatspec = formatspec;
}
