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

string Date::getMonFd(string mmm)
{
	boost::to_upper(mmm);
	if(mmm=="01" || mmm=="1")return "JAN";
	else if(mmm=="02" || mmm=="2")return "FEB";
	else if(mmm=="03" || mmm=="3")return "MAR";
	else if(mmm=="04" || mmm=="4")return "APR";
	else if(mmm=="05" || mmm=="5")return "MAY";
	else if(mmm=="06" || mmm=="6")return "JUN";
	else if(mmm=="07" || mmm=="7")return "JUL";
	else if(mmm=="08" || mmm=="8")return "AUG";
	else if(mmm=="09" || mmm=="9")return "SEP";
	else if(mmm=="10")return "OCT";
	else if(mmm=="11")return "NOV";
	else if(mmm=="12")return "DEC";
	else return "-1";
}

int Date::getWeekDayVal(string dayName)
{
	boost::to_upper(dayName);
	if(dayName=="SUN" || dayName=="SUNDAY")
		return 7;
	else if(dayName=="MON" || dayName=="MONDAY")
		return 1;
	else if(dayName=="TUE" || dayName=="TUESDAY")
		return 2;
	else if(dayName=="WED" || dayName=="WEDNESDAY")
		return 3;
	else if(dayName=="THU" || dayName=="THURSDAY")
		return 4;
	else if(dayName=="FRI" || dayName=="FRIDAY")
		return 5;
	else if(dayName=="SAT" || dayName=="SATURDAY")
		return 6;
	else return -1;
}

string getHalfDayName(string dayName)
{
	if(dayName.length()>3)
		return dayName.substr(0,3);
	return dayName;
}

Date::Date()
{
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	this->nanoseconds = en.tv_nsec;
	string tem;
	tem.append(asctime(timeinfo));
	boost::replace_all(tem,"\n","");
	boost::replace_all(tem,"  "," ");
	vector<string> temp,vemp;
	boost::iter_split(temp,tem,boost::first_finder(" "));
	this->dayw = boost::to_upper_copy(temp.at(0));
	this->monthw = boost::to_upper_copy(temp.at(1));
	this->month = getMon(monthw);
	this->day = temp.at(2);
	boost::iter_split(vemp,temp.at(3),boost::first_finder(":"));
	this->hh = vemp.at(0);
	this->mm = vemp.at(1);
	this->ss = vemp.at(2);
	this->year = temp.at(4);
	this->weekday = getWeekDayVal(this->dayw);
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
	string t = (month<=9?"0":"");
	this->month = t + boost::lexical_cast<string>(month);
	this->monthw = getMonFd(this->month);
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

int Date::getWeekDay() const
{
	return weekday;
}

void Date::setYear(int year)
{
	string t = (year<=9?"0":"");
	this->year = t + boost::lexical_cast<string>(year);
}

int Date::getDay() const
{
	return boost::lexical_cast<int>(day);
}

void Date::setDay(int day)
{
	string t = (day<=9?"0":"");
	this->day = t + boost::lexical_cast<string>(day);
}

int Date::getHh() const
{
	return boost::lexical_cast<int>(hh);
}

void Date::setHh(int hh)
{
	string t = (hh<=9?"0":"");
	this->hh = t + boost::lexical_cast<string>(hh);
}

int Date::getMm() const
{
	return boost::lexical_cast<int>(mm);
}

void Date::setMm(int mm)
{
	string t = (mm<=9?"0":"");
	this->mm = t + boost::lexical_cast<string>(mm);
}

int Date::getSs() const
{
	return boost::lexical_cast<int>(ss);
}

long long Date::getNanoSeconds() const
{
	return nanoseconds;
}

void Date::setSs(int ss)
{
	string t = (ss<=9?"0":"");
	this->ss = t + boost::lexical_cast<string>(ss);
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

string Date::getNsStr() const
{
	return boost::lexical_cast<string>(nanoseconds);
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
	return dayw+" "+year+" "+monthw+" "+day+" "+hh+":"+mm+":"+ss+"."+boost::lexical_cast<string>(nanoseconds);
}

Date Date::addSeconds(long seconds)
{
	long minutes = (getSs()+seconds)/60;
	seconds = (getSs()+seconds)%60;
	Date d = addMinutes(minutes);
	d.setSs(seconds);
	return d;
}

Date Date::addMinutes(long minutes)
{
	long hours = (getMm()+minutes)/60;
	minutes = (getMm()+minutes)%60;
	Date d = addHours(hours);
	d.setMm(minutes);
	return d;
}

Date Date::addHours(long hours)
{
	long days = (getHh()+hours)/24;
	hours = (getHh()+hours)%24;
	Date d = addDays(days);
	d.setHh(hours);
	return d;
}

Date Date::addDays(long days)
{
	long months = getMonth();
	long year = getYear() + months/12;
	months = months%12;
	long g = getDays(year,months,getDay()+days);
	return getDateFromDays(g);
}

Date Date::addMonths(long months)
{
	months = getMonth() + months;
	long year = getYear() + months/12;
	months = months%12;
	long g = getDays(year,months,getDay());
	return getDateFromDays(g);
}

Date Date::addYears(long years)
{
	long g = getDays(getYear()+years,getMonth(),getDay());
	return getDateFromDays(g);
}

long Date::getDaysInt()
{
	long y = getYear();
	long m = getMonth();
	long d = getDay();
	y = y -1900;
	m = (m + 9) % 12;
	y = y - m/10;
	return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ( d - 1 );
}
long Date::getDays(long y,long m,long d)
{
	y = y -1900;
	m = (m + 9) % 12;
	y = y - m/10;
	return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ( d - 1 );
}

long Date::getHours(long y,long m,long d, long hh)
{
	long g = getDays(y,m,d);
	int hfd = hh/24;
	hh = hh%24;
	return (g+hfd)*24 + hh;
}

long Date::getMinutes(long y,long m,long d, long hh, long mi)
{
	long g = getHours(y,m,d,hh);
	int mfh = mi/60;
	mi = mi%60;
	return (g+mfh)*60 + mi;
}

unsigned long long Date::getSeconds(long y,long m,long d, long hh, long mi, long ss)
{
	long g = getMinutes(y,m,d,hh,mi);
	int sfm = ss/60;
	ss = ss%60;
	return (g+sfm)*60 + ss;
}

Date Date::getDateFromDays(long days)
{
	long y = (10000*days + 14780)/3652425;
	long ddd = days - (365*y + y/4 - y/100 + y/400);
	if (ddd < 0)
	{
		y = y - 1;
		ddd = days - (365*y + y/4 - y/100 + y/400);
	}
	long mi = (100*ddd + 52)/3060;
	long mm = (mi + 2)%12 + 1;
	y = y + (mi + 2)/12 + 1900;
	long dd = ddd - (mi*306 + 5)/10 + 1;
	//cout << y << " " << mm << " " << dd << endl;
	Date d;
	d.setDay(dd);
	d.setMonth(mm);
	d.setYear(y);
	d.setDayw(getHalfDayName(getDayName(dd,mm,y)));
	return d;
}

void Date::getDateFromHours(long hours)
{
	long days = hours/24;
	hours = hours%24;
	long y = (10000*days + 14780)/3652425;
	long ddd = days - (365*y + y/4 - y/100 + y/400);
	if (ddd < 0)
	{
		y = y - 1;
		ddd = days - (365*y + y/4 - y/100 + y/400);
	}
	long mi = (100*ddd + 52)/3060;
	long mm = (mi + 2)%12 + 1;
	y = y + (mi + 2)/12 + 1900;
	long dd = ddd - (mi*306 + 5)/10 + 1;
	//cout << y << " " << mm << " " << dd << " " << hours;
}

void Date::getDateFromMinutes(long long minutes)
{
	long hours = minutes/60;
	minutes = minutes%60;
	long days = hours/24;
	hours = hours%24;
	long y = (10000*days + 14780)/3652425;
	long ddd = days - (365*y + y/4 - y/100 + y/400);
	if (ddd < 0)
	{
		y = y - 1;
		ddd = days - (365*y + y/4 - y/100 + y/400);
	}
	long mi = (100*ddd + 52)/3060;
	long mm = (mi + 2)%12 + 1;
	y = y + (mi + 2)/12 + 1900;
	long dd = ddd - (mi*306 + 5)/10 + 1;
	//cout << y << " " << mm << " " << dd << " " << hours << ":" << minutes;
}

void Date::getDateFromSeconds(long long seconds)
{
	long long minutes = seconds/60;
	seconds = seconds%60;
	long hours = minutes/60;
	minutes = minutes%60;
	long days = hours/24;
	hours = hours%24;
	long y = (10000*days + 14780)/3652425;
	long ddd = days - (365*y + y/4 - y/100 + y/400);
	if (ddd < 0)
	{
		y = y - 1;
		ddd = days - (365*y + y/4 - y/100 + y/400);
	}
	long mi = (100*ddd + 52)/3060;
	long mm = (mi + 2)%12 + 1;
	y = y + (mi + 2)/12 + 1900;
	long dd = ddd - (mi*306 + 5)/10 + 1;
	//cout << y << " " << mm << " " << dd << " " << hours << ":" << minutes << ":" << seconds <<  "\n" << endl << flush;
}

bool Date::validateDate(int dd, int mm, int yyyy)
{
	bool error = true;
	if(dd<1 || dd>31 || yyyy<0)
		error = false;

	if (mm < 1 || mm > 12)
		error = false;
	string mw31days = ",1,3,5,7,8,10,12,";
	string mw30oldays =  ",4,6,9,11,";
	string mtc = "," + boost::lexical_cast<string>(mm) + ",";
	if (mm==2)
	{
		if (!(yyyy % 4) && ((yyyy % 100) || !(yyyy % 400)))
		{
			if (dd < 1 || dd > 29)
				error = false;
		}
		else if (dd < 1 || dd >28)
			error = false;
	}
	else if (mw31days.find(mtc)!=string::npos)
	{
		if (dd < 1 || dd > 31)
			error = false;
	}
	else if (mw30oldays.find(mtc)!=string::npos)
	{
		if (dd < 1 || dd > 30)
			error = false;
	}
	return error;
}

//Based on program by Stanley Wong
string Date::getDayName(int dd, int mm, int yyyy)
{
	string dayName;
	if(!validateDate(dd,mm,yyyy))return dayName;
	int days = ((yyyy-1)*365 + (yyyy-1)/4 - (yyyy-1)/100 + (yyyy-1)/400) % 7;//Daycode for prev year 31st Dec
	switch(mm)
	{
		case 12:dd += 30;
		case 11:dd += 31;
		case 10:dd += 30;
		case 9:dd += 31;
		case 8:dd += 31;
		case 7:dd += 30;
		case 6:dd += 31;
		case 5:dd += 30;
		case 4:dd += 31;
		case 3:dd += 28;
		case 2:dd += 31;
	}
	days += dd;
	if ((!(yyyy % 4) && ((yyyy % 100) || !(yyyy % 400)))&& mm > 2)
		days++;
	days = days%7;
	switch(days)
	{
		case 0:dayName = "SUNDAY";break;
		case 1:dayName = "MONDAY";break;
		case 2:dayName = "TUESDAY";break;
		case 3:dayName = "WEDNESDAY";break;
		case 4:dayName = "THURSDAY";break;
		case 5:dayName = "FRIDAY";break;
		case 6:dayName = "SATURDAY";break;
	}
	return dayName;
}

Date::Date(int yyyy,string mmm,int dd)
{
	string mm = getMon(mmm);
	if(mm=="-1")throw "Invalid month";
	if(!validateDate(dd,boost::lexical_cast<int>(mm),yyyy))throw "Invalid date";
	long g = getDays(yyyy,boost::lexical_cast<long>(mm),dd);
	*this = getDateFromDays(g);
	this->nanoseconds = 0;
}

Date::Date(int yyyy,int mm,int dd)
{
	if(!validateDate(dd,mm,yyyy))throw "Invalid date";
	long g = getDays(yyyy,mm,dd);
	*this = getDateFromDays(g);
	this->nanoseconds = 0;
}

Date::Date(int yy,string mmm,int dd,bool te)
{
	Date d;
	string syyyy = d.year.substr(0,2) + boost::lexical_cast<string>(yy);
	int yyyy = boost::lexical_cast<int>(syyyy);
	string mm = getMon(mmm);
	if(mm=="-1")throw "Invalid month";
	if(!validateDate(dd,boost::lexical_cast<int>(mm),yyyy))throw "Invalid date";
	long g = getDays(yyyy,boost::lexical_cast<long>(mm),dd);
	*this = getDateFromDays(g);
	this->nanoseconds = 0;
}

Date::Date(int yy,int mm,int dd,bool te)
{
	Date d;
	string syyyy = d.year.substr(0,2) + boost::lexical_cast<string>(yy);
	int yyyy = boost::lexical_cast<int>(syyyy);
	if(!validateDate(dd,mm,yyyy))throw "Invalid date";
	long g = getDays(yyyy,mm,dd);
	*this = getDateFromDays(g);
	this->nanoseconds = 0;
}

void Date::setTime(int hh,int mi,int ss)
{
	if(hh>24 || hh<0 || mi<0 || mi>60 || ss<0 || ss>60)
		throw "Invalid Time";
	setHh(hh);
	setMm(mi);
	setSs(ss);
	this->nanoseconds = 0;
}
