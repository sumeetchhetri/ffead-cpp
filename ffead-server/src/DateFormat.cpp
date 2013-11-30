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
	StringUtil::replaceAll(temp,"hh",date.getHhStr());
	StringUtil::replaceAll(temp,"mi",date.getMmStr());
	StringUtil::replaceAll(temp,"ss",date.getSsStr());
	StringUtil::replaceAll(temp,"ns",date.getNsStr());
	StringUtil::replaceAll(temp,"ddd",date.getDayw());
	StringUtil::replaceAll(temp,"dd",date.getDayStr());
	StringUtil::replaceAll(temp,"mmm",date.getMonthw());
	StringUtil::replaceAll(temp,"mm",date.getMonthStr());
	StringUtil::replaceAll(temp,"yyyy",date.getYearStr());
	StringUtil::replaceAll(temp,"yy",date.getYearStr().substr(2));
	if(date.getTimeZoneOffset()>0)
	{
		temp += ("+" + CastUtil::lexical_cast<string>(date.getTimeZoneOffset()));
	}
	else if(date.getTimeZoneOffset()<0)
	{
		temp += ("-" + CastUtil::lexical_cast<string>(date.getTimeZoneOffset()));
	}
	return temp;
}

Date* DateFormat::parse(string strdate)
{
	string temp = this->formatspec;
	Date* date = NULL;
	string yyyy,yy,ddd,dd,mmm,mm,hh,mi,ss,tzv;
	if(temp.find("yyyy")!=string::npos)
	{
		yyyy = strdate.substr(temp.find("yyyy"),4);
		string ytemp = temp;
		while(ytemp.find("yyyy")!=string::npos)
		{
			string tyyyy = strdate.substr(ytemp.find("yyyy"),4);
			if(yyyy!=tyyyy)
				throw "Multiple instances with different year(yyyy) values found";
			strdate = strdate.substr(0, ytemp.find("yyyy")) +
						strdate.substr(ytemp.find("yyyy")+4);
			StringUtil::replaceFirst(ytemp, "yyyy", "");
		}
		temp = ytemp;
	}
	else if(temp.find("yy")!=string::npos)
	{
		yy = strdate.substr(temp.find("yy"),2);
		string ytemp = temp;
		while(ytemp.find("yy")!=string::npos)
		{
			string tyy = strdate.substr(ytemp.find("yy"),2);
			if(yy!=tyy)
				throw "Multiple instances with different year(yy) values found";
			strdate = strdate.substr(0, ytemp.find("yy")) +
						strdate.substr(ytemp.find("yy")+2);
			StringUtil::replaceFirst(ytemp, "yy", "");
		}
		temp = ytemp;
	}
	if(temp.find("ddd")!=string::npos)
	{
		ddd = strdate.substr(temp.find("ddd"),3);
		string ytemp = temp;
		while(ytemp.find("ddd")!=string::npos)
		{
			string tddd = strdate.substr(ytemp.find("ddd"),3);
			if(ddd!=tddd)
				throw "Multiple instances with different day(ddd) values found";
			strdate = strdate.substr(0, ytemp.find("ddd")) +
						strdate.substr(ytemp.find("ddd")+3);
			StringUtil::replaceFirst(ytemp, "ddd", "");
		}
		temp = ytemp;
	}
	if(temp.find("dd")!=string::npos)
	{
		dd = strdate.substr(temp.find("dd"),2);
		string ytemp = temp;
		while(ytemp.find("dd")!=string::npos)
		{
			string tdd = strdate.substr(ytemp.find("dd"),2);
			if(dd!=tdd)
				throw "Multiple instances with different day(dd) values found";
			strdate = strdate.substr(0, ytemp.find("dd")) +
						strdate.substr(ytemp.find("dd")+2);
			StringUtil::replaceFirst(ytemp, "dd", "");
		}
		temp = ytemp;
	}
	if(temp.find("mmm")!=string::npos)
	{
		mmm = strdate.substr(temp.find("mmm"),3);
		string ytemp = temp;
		while(ytemp.find("mmm")!=string::npos)
		{
			string tmmm = strdate.substr(ytemp.find("mmm"),3);
			if(mmm!=tmmm)
				throw "Multiple instances with different month(mmm) values found";
			strdate = strdate.substr(0, ytemp.find("mmm")) +
						strdate.substr(ytemp.find("mmm")+3);
			StringUtil::replaceFirst(ytemp, "mmm", "");
		}
		temp = ytemp;
	}
	else if(temp.find("mm")!=string::npos)
	{
		mm = strdate.substr(temp.find("mm"),2);
		string ytemp = temp;
		while(ytemp.find("mm")!=string::npos)
		{
			string tmm = strdate.substr(ytemp.find("mm"),2);
			if(mm!=tmm)
				throw "Multiple instances with different month(mm) values found";
			strdate = strdate.substr(0, ytemp.find("mm")) +
						strdate.substr(ytemp.find("mm")+2);
			StringUtil::replaceFirst(ytemp, "mm", "");
		}
		temp = ytemp;
	}
	if(temp.find("hh")!=string::npos)
	{
		hh = strdate.substr(temp.find("hh"),2);
		string ytemp = temp;
		while(ytemp.find("hh")!=string::npos)
		{
			string thh = strdate.substr(ytemp.find("hh"),2);
			if(hh!=thh)
				throw "Multiple instances with different hours(hh) values found";
			strdate = strdate.substr(0, ytemp.find("hh")) +
						strdate.substr(ytemp.find("hh")+2);
			StringUtil::replaceFirst(ytemp, "hh", "");
		}
		temp = ytemp;
	}
	if(temp.find("mi")!=string::npos)
	{
		mi = strdate.substr(temp.find("mi"),2);
		string ytemp = temp;
		while(ytemp.find("mi")!=string::npos)
		{
			string tmi = strdate.substr(ytemp.find("mi"),2);
			if(mi!=tmi)
				throw "Multiple instances with different minutes(mi) values found";
			strdate = strdate.substr(0, ytemp.find("mi")) +
						strdate.substr(ytemp.find("mi")+2);
			StringUtil::replaceFirst(ytemp, "mi", "");
		}
		temp = ytemp;
	}
	if(temp.find("ss")!=string::npos)
	{
		ss = strdate.substr(temp.find("ss"),2);
		string ytemp = temp;
		while(ytemp.find("ss")!=string::npos)
		{
			string tss = strdate.substr(ytemp.find("ss"),2);
			if(ss!=tss)
				throw "Multiple instances with different seconds(ss) values found";
			strdate = strdate.substr(0, ytemp.find("ss")) +
						strdate.substr(ytemp.find("ss")+2);
			StringUtil::replaceFirst(ytemp, "ss", "");
		}
		temp = ytemp;
	}
	if(strdate.find("+")!=string::npos)
	{
		tzv = strdate.substr(temp.find("+")+1);
	}
	else if(strdate.find("-")!=string::npos)
	{
		tzv = strdate.substr(temp.find("-"));
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
				throw "Invalid Date month specified";
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
				throw "Invalid Date month specified";
			}
		}
		else
		{
			throw "Invalid Date year/day specified";
		}
		if(tzv!="")
		{
			try {
				date->setTimeZoneOffset(CastUtil::lexical_cast<float>(tzv));
			} catch (...) {
				throw "Invalid Timezone specified";
			}
		}
	} catch (const char* s) {
		throw s;
	} catch (...) {
		throw "Invalid Date specified";
	}
	if(date!=NULL)
	{
		if(ddd!="" && date->getDayw()!=StringUtil::toUpperCopy(ddd))
		{
			throw "Invalid Day(ddd) specified";
		}
	}
	if(hh=="")
		hh = date->getHh();
	if(mi=="")
		mi = date->getMm();
	if(ss=="")
		ss = date->getSs();
	date->setTime(CastUtil::lexical_cast<int>(hh),
		CastUtil::lexical_cast<int>(mi), CastUtil::lexical_cast<int>(ss));
	return date;
}

string DateFormat::getFormatspec() const
{
	return formatspec;
}

void DateFormat::setFormatspec(string formatspec)
{
	this->formatspec = formatspec;
}
