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
 * Date.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: sumeet
 */
#include "Date.h"

Date::Date(const bool& utc)
{
	time_t rawtime;
	time (&rawtime);
	if(utc)
		gmtime_r(&rawtime, &ti);
	else
		localtime_r(&rawtime, &ti);
	populateDateFields();
}

tm* Date::getTimeinfo() {
	return &ti;
}

int Date::getDay() const {
	return day;
}

const std::string& Date::getDayAbbr() const {
	return dayAbbr;
}

const std::string& Date::getHourdesignation() const {
	return hourdesignation;
}

int Date::getMonth() const {
	return month;
}

const std::string& Date::getMonthAbbr() const {
	return monthAbbr;
}

const std::string& Date::getMonthName() const {
	return monthName;
}

long long Date::getNanoseconds() const {
	return nanoseconds;
}

int Date::getPmHours() const {
	return pmHours;
}

const std::string& Date::getTimeZone() const {
	return timeZone;
}

float Date::getTimeZoneOffset() const {
	return timeZoneOffset;
}

void Date::setTimeZoneOffset(const float& timeZoneOffset) {
	this->timeZoneOffset = timeZoneOffset;
}

int Date::getHours() const {
	return hours;
}

int Date::getMinutes() const {
	return minutes;
}

long long Date::getEpochTime() const {
	return epochTime;
}

int Date::getSeconds() const {
	return seconds;
}

int Date::getWeekday() const {
	return weekday;
}

int Date::getYear() const {
	return year;
}

void Date::populateEpochAndTimeZone(const bool& utc)
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);
	epochTime = en.tv_sec;
	nanoseconds = en.tv_nsec;

	time_t rawtime;
	time (&rawtime);
	if(utc)
		gmtime_r(&rawtime, &ti);
	else
		localtime_r(&rawtime, &ti);

	timeZoneOffsetSecs = ti.tm_gmtoff;
	timeZoneOffset = timeZoneOffsetSecs/60;
	if(ti.tm_zone!=NULL) {
		timeZone = std::string(ti.tm_zone);
	}
	isDLS = ti.tm_isdst==1;	/* Daylight Savings Time flag */
}

void Date::populateDateFields()
{
	timespec en;
	clock_gettime(CLOCK_REALTIME, &en);

	epochTime = en.tv_sec;
	nanoseconds = en.tv_nsec;
	seconds = ti.tm_sec;		/* seconds after the minute [0-60] */
	minutes = ti.tm_min;		/* minutes after the hour [0-59] */
	hours = ti.tm_hour;	/* hours since midnight [0-23] */
	pmHours = hours>=13?(hours-12):hours;
	hourdesignation = hours>=12?"PM":"AM";
	day = ti.tm_mday;	/* day of the month [1-31] */
	dayOfYear = ti.tm_yday;
	month = ti.tm_mon;		/* months since January [0-11] */
	monthName = monthInWords(month);
	monthAbbr = monthInWords(month, true);
	year = ti.tm_year + 1900;	/* years since 1900 */
	weekday = ti.tm_wday;
	dayName = dayInWords(weekday);
	dayAbbr = dayInWords(weekday, true);
	timeZoneOffsetSecs = ti.tm_gmtoff;
	timeZoneOffset = timeZoneOffsetSecs/60;
	if(ti.tm_zone!=NULL) {
		timeZone = std::string(ti.tm_zone);
	}
	isDLS = ti.tm_isdst==1;	/* Daylight Savings Time flag */
}

Date::Date(const std::string& strdate, const std::string& formatspec) {
	ti.tm_zone = NULL;
	ti.tm_isdst = 0;
	ti.tm_gmtoff = 0;
	strptime(strdate.c_str(), formatspec.c_str(), &ti);
	populateDateFields();
}

Date::Date(struct tm* timeinfo)
{
	this->ti = *timeinfo;
	populateDateFields();
}

Date::~Date() {
}


std::string Date::toString()
{
	return dayAbbr+" "+CastUtil::fromNumber(year)+" "+monthAbbr+" "+CastUtil::fromNumber(day)
			+" "+CastUtil::fromNumber(hours)+":"+CastUtil::fromNumber(minutes)
			+":"+CastUtil::fromNumber(seconds)+"."+CastUtil::fromNumber(nanoseconds)
			+" "+CastUtil::fromFloat(timeZoneOffset);
}

Date Date::addSecondsGet(const double& seconds)
{
	Date d = *this;
	d.updateSeconds(seconds);
	return d;
}

Date Date::addMinutesGet(const double& minutes)
{
	Date d = *this;
	d.updateMinutes(minutes);
	return d;
}

Date Date::addHoursGet(const double& hours)
{
	Date d = *this;
	d.updateHours(hours);
	return d;
}

Date Date::addDaysGet(const long& days)
{
	long months = month;
	long year = this->year + months/12;
	months = months%12;
	long g = getDays(year,months,day+days);
	Date d = getDateFromDays(g);
	d.epochTime += days*24*3600;
	return d;
}

Date Date::addMonthsGet(long months)
{
	months = month + months;
	long year = this->year + months/12;
	months = months%12;
	long g = getDays(year,months,day);
	Date d = getDateFromDays(g);
	d.epochTime = getSecondsI(d.year, d.month, d.day, d.hours, d.minutes, d.seconds);
	return d;
}

Date Date::addYearsGet(const long& years)
{
	long g = getDays(year+years,month,day);
	Date d = getDateFromDays(g);
	d.epochTime = getSecondsI(d.year, d.month, d.day, d.hours, d.minutes, d.seconds);
	return d;
}

long Date::getDaysInt()
{
	long y = year;
	long m = month;
	long d = day;
	y = y -1900;
	m = (m + 9) % 12;
	y = y - m/10;
	return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ( d - 1 );
}
long Date::getDays(long y,long m,long d) const
{
	y = y -1900;
	m = (m + 9) % 12;
	y = y - m/10;
	return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ( d - 1 );
}

long Date::getHoursI(long y,long m,long d, long hh) const
{
	long g = getDays(y,m,d);
	int hfd = hh/24;
	hh = hh%24;
	return (g+hfd)*24 + hh;
}

long Date::getMinutesI(long y,long m,long d, long hh, long mi) const
{
	long g = getHoursI(y,m,d,hh);
	int mfh = mi/60;
	mi = mi%60;
	return (g+mfh)*60 + mi;
}

unsigned long long Date::getSecondsI(long y,long m,long d, long hh, long mi, long ss) const
{
	long g = getMinutesI(y,m,d,hh,mi);
	int sfm = ss/60;
	ss = ss%60;
	return (g+sfm)*60 + ss;
}

Date Date::getDateFromDaysInternal(const long& days)
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
	Date d;
	d.day = dd;
	d.month = mm;
	d.year = y;
	return d;
}

Date Date::getDateFromDays(const long& days)
{
	Date d  = getDateFromDaysInternal(days);
	d.populateDay();
	d.populateEpochAndTimeZone(false);
	return d;
}

Date Date::getDateFromHours(long hours)
{
	long days = hours/24;
	hours = hours%24;
	Date d = getDateFromDaysInternal(days);
	d.setTime(hours, 0, 0);
	d.populateDay();
	d.populateEpochAndTimeZone(false);
	return d;
}

Date Date::getDateFromMinutes(long long minutes)
{
	long hours = minutes/60;
	minutes = minutes%60;
	long days = hours/24;
	hours = hours%24;
	Date d = getDateFromDaysInternal(days);
	d.setTime(hours, minutes, 0);
	d.populateDay();
	d.populateEpochAndTimeZone(false);
	return d;
}

Date Date::getDateFromSeconds(long long seconds)
{
	long long minutes = seconds/60;
	seconds = seconds%60;
	long hours = minutes/60;
	minutes = minutes%60;
	long days = hours/24;
	hours = hours%24;
	Date d = getDateFromDaysInternal(days);
	d.setTime(hours, minutes, seconds);
	d.populateDay();
	d.populateEpochAndTimeZone(false);
	return d;
}

bool Date::validateDate(const int& dd, const int& mm, const int& yyyy)
{
	bool error = true;
	if(dd<1 || dd>31 || yyyy<0)
		error = false;

	if (mm < 1 || mm > 12)
		error = false;
	std::string mw31days = ",1,3,5,7,8,10,12,";
	std::string mw30oldays =  ",4,6,9,11,";
	std::string mtc = "," + CastUtil::fromNumber(mm) + ",";
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
	else if (mw31days.find(mtc)!=std::string::npos)
	{
		if (dd < 1 || dd > 31)
			error = false;
	}
	else if (mw30oldays.find(mtc)!=std::string::npos)
	{
		if (dd < 1 || dd > 30)
			error = false;
	}
	return error;
}

std::string Date::dayInWords(const int d, const bool& sf) {
	switch(d)
	{
		case 0:return sf?"SUN":"SUNDAY";
		case 1:return sf?"MON":"MONDAY";
		case 2:return sf?"TUE":"TUESDAY";
		case 3:return sf?"WED":"WEDNESDAY";
		case 4:return sf?"THU":"THURSDAY";
		case 5:return sf?"FRI":"FRIDAY";
		case 6:return sf?"SAT":"SATURDAY";
	}
	return "";
}

std::string Date::monthInWords(const int m, const bool& sf) {
	switch(m)
	{
		case 0: return sf?"JAN":"JANUARY";
		case 1: return sf?"FEB":"FEBRUARY";
		case 2: return sf?"MAR":"MARCH";
		case 3: return sf?"APR":"APRIL";
		case 4: return sf?"MAY":"MAY";
		case 5: return sf?"JUN":"JUNE";
		case 6: return sf?"JUL":"JULY";
		case 7: return sf?"AUG":"AUGUST";
		case 8: return sf?"SEP":"SEPTEMBER";
		case 9: return sf?"OCT":"OCTOBER";
		case 10: return sf?"NOV":"NOVEMBER";
		case 11: return sf?"DEC":"DECEMBER";
	}
	return "";
}

//Based on program by Stanley Wong
std::string Date::getDayName(int dd, const int& mm, const int& yyyy)
{
	std::string dayName;
	if(!validateDate(dd,mm,yyyy))return dayName;
	int days = ((yyyy-1)*365 + (yyyy-1)/4 - (yyyy-1)/100 + (yyyy-1)/400) % 7;//Daycode for prev year 31st Dec
	switch(mm)
	{
		case 12:dd += 30;break;
		case 11:dd += 31;break;
		case 10:dd += 30;break;
		case 9:dd += 31;break;
		case 8:dd += 31;break;
		case 7:dd += 30;break;
		case 6:dd += 31;break;
		case 5:dd += 30;break;
		case 4:dd += 31;break;
		case 3:dd += 28;break;
		case 2:dd += 31;break;
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

//Based on program by Stanley Wong
void Date::populateDay()
{
	int yyyy = year;
	int mm = month;
	int dd = day;
	dd += ((yyyy-1)*365 + (yyyy-1)/4 - (yyyy-1)/100 + (yyyy-1)/400) % 7;//Daycode for prev year 31st Dec
	switch(mm)
	{
		case 12:dd += 334;break;
		case 11:dd += 304;break;
		case 10:dd += 273;break;
		case 9:dd += 243;break;
		case 8:dd += 212;break;
		case 7:dd += 181;break;
		case 6:dd += 151;break;
		case 5:dd += 120;break;
		case 4:dd += 90;break;
		case 3:dd += 59;break;
		case 2:dd += 31;break;
	}
	if ((!(yyyy % 4) && ((yyyy % 100) || !(yyyy % 400)))&& mm > 2)
		dd++;
	dd = dd%7;
	switch(dd)
	{
		case 0:dayName = "SUNDAY";weekday = 7;break;
		case 1:dayName = "MONDAY";weekday = 1;break;
		case 2:dayName = "TUESDAY";weekday = 2;break;
		case 3:dayName = "WEDNESDAY";weekday = 3;break;
		case 4:dayName = "THURSDAY";weekday = 4;break;
		case 5:dayName = "FRIDAY";weekday = 5;break;
		case 6:dayName = "SATURDAY";weekday = 6;break;
	}
	if(dayName.length()>3)
		dayAbbr = dayName.substr(0,3);
	pmHours = 0;
	if(hours>=12)
	{
		hourdesignation = "PM";
		pmHours = hours - 12;
	}
}

Date::Date(const int& yyyy, const std::string& mmm, const int& dd)
{
	std::string mm = getMon(mmm);
	if(mm=="-1")throw std::runtime_error("Invalid month");
	if(!validateDate(dd,CastUtil::toInt(mm),yyyy))throw std::runtime_error("Invalid date");
	long g = getDays(yyyy,CastUtil::toLong(mm),dd);
	*this = getDateFromDays(g);
	populateDay();
	populateMonth();
	populateEpochAndTimeZone(false);
}

Date::Date(const int& yyyy, const int& mm, const int& dd)
{
	if(!validateDate(dd,mm,yyyy))throw std::runtime_error("Invalid date");
	long g = getDays(yyyy,mm,dd);
	*this = getDateFromDays(g);
	populateDay();
	populateMonth();
	populateEpochAndTimeZone(false);
}

Date::Date(const int& yy, const std::string& mmm, const int& dd, const bool& te)
{
	Date d;
	std::string syyyy = CastUtil::fromNumber(d.year).substr(0,2) + CastUtil::fromNumber(yy);
	int yyyy = CastUtil::toInt(syyyy);
	std::string mm = getMon(mmm);
	if(mm=="-1")throw std::runtime_error("Invalid month");
	if(!validateDate(dd,CastUtil::toInt(mm),yyyy))throw std::runtime_error("Invalid date");
	long g = getDays(yyyy,CastUtil::toLong(mm),dd);
	*this = getDateFromDays(g);
	populateDay();
	populateMonth();
	populateEpochAndTimeZone(false);
}

Date::Date(const int& yy, const int& mm, const int& dd, const bool& te)
{
	Date d;
	std::string syyyy = CastUtil::fromNumber(d.year).substr(0,2) + CastUtil::fromNumber(yy);
	int yyyy = CastUtil::toInt(syyyy);
	if(!validateDate(dd,mm,yyyy))throw std::runtime_error("Invalid date");
	long g = getDays(yyyy,mm,dd);
	*this = getDateFromDays(g);
	populateDay();
	populateMonth();
	populateEpochAndTimeZone(false);
}

std::string Date::getMon(const std::string& m) const
{
	std::string mmm = StringUtil::toUpperCopy(m);
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

void Date::populateMonth()
{
	switch(month) {
		case 1: monthName = "JANUARY";break;
		case 2: monthName = "FEBRUARY";break;
		case 3: monthName = "MARCH";break;
		case 4: monthName = "APRIL";break;
		case 5: monthName = "MAY";break;
		case 6: monthName = "JUNE";break;
		case 7: monthName = "JULY";break;
		case 8: monthName = "AUGUST";break;
		case 9: monthName = "SEPTEMBER";break;
		case 10: monthName = "OCTOBER";break;
		case 11: monthName = "NOVEMBER";break;
		case 12: monthName = "DECEMBER";break;
	}
	monthAbbr = monthName.substr(0, 3);
}

void Date::setTime(const int& hh, const int& mi, const int& ss, const unsigned long long& ns)
{
	if(hh>24 || hh<0 || mi<0 || mi>60 || ss<0 || ss>60)
		throw std::runtime_error("Invalid Time");
	hours = hh;
	minutes = mi;
	seconds = ss;
	nanoseconds = ns;
}

Date Date::toGMT()
{
	Date d = *this;
	d.updateHours(timeZoneOffset);
	d.timeZone = "UTC";
	d.timeZoneOffset = 0;
	return d;
}

int Date::test()
{
	unsigned long long gg = getDays(2008,12,31);
	getDateFromDays(gg);
	gg = getHoursI(2008,12,31,12);
	getDateFromHours(gg);
	gg = getMinutesI(2008,12,31,12,56);
	getDateFromMinutes(gg);
	gg = getSecondsI(2008,12,31,12,56,56);
	getDateFromSeconds(gg);
	Date d;
	//logger << d.toString() << std::endl;
	Date d1 = addYearsGet(1);
	//logger << d1.toString() << std::endl;
	Date d2 = addMonthsGet(23);
	//logger << d2.toString() << std::endl;
	Date d3 = addDaysGet(17);
	//logger << d3.toString() << std::endl;
	Date d4 = addHoursGet(25);
	//logger << d4.toString() << std::endl;
	Date d5 = addMinutesGet(61);
	//logger << d5.toString() << std::endl;
	Date d6 = addSecondsGet(61);
	//logger << d6.toString() << std::endl;
	return 0;
}

void Date::updateSeconds(const double& dseconds)
{
	long seconds = floor(dseconds);
	double fraction = dseconds - (double)seconds;
	long minutes = (seconds+seconds)/60;
	seconds = (seconds+seconds)%60;
	if(minutes>0)
	{
		updateMinutes(minutes);
	}
	this->seconds = seconds;
	nanoseconds = fraction * 1E9;
	epochTime += dseconds;
}


void Date::updateMinutes(const double& dminutes)
{
	long minutes = floor(dminutes);
	double fraction = dminutes - (double)minutes;
	long extseconds = fraction*60 + seconds;
	minutes += extseconds/60;
	extseconds = extseconds%60;
	long hours = (this->minutes+minutes)/60;
	minutes = (this->minutes+minutes)%60;
	if(hours>0)
	{
		updateHours(hours);
	}
	this->minutes = minutes;
	this->seconds = extseconds;
	epochTime += dminutes*60;
}


void Date::updateHours(const double& dhours)
{
	long hours = floor(dhours);
	double fraction = dhours - (double)hours;
	long extminutes = fraction*60 + minutes;
	hours += extminutes/60;
	extminutes = extminutes%60;
	long days = (this->hours+hours)/24;
	hours = (this->hours+hours)%24;
	if(days>0)
	{
		updateDays(days);
	}
	this->hours = hours;
	this->minutes = extminutes;
	epochTime += dhours*60*60;
}


void Date::updateDays(const long& days)
{
	long months = month;
	long year = this->year + months/12;
	months = months%12;
	long g = getDays(year,months,day+days);
	Date d = getDateFromDays(g);
	this->day = d.day;
	this->month = d.month;
	this->year = d.year;
	populateDay();
	epochTime += days*24*60*60;
}


void Date::updateMonths(long months)
{
	months = month + months;
	long year = this->year + months/12;
	months = months%12;
	long g = getDays(year,months,day);
	Date d = getDateFromDays(g);
	this->day = d.day;
	this->month = d.month;
	this->year = d.year;
	populateDay();
	epochTime = getSecondsI(year, month, day, hours, minutes, seconds);
}


void Date::updateYears(const long& years)
{
	long g = getDays(year+years,month,day);
	Date d = getDateFromDays(g);
	this->day = d.day;
	this->month = d.month;
	this->year = d.year;
	populateDay();
	epochTime = getSecondsI(year, month, day, hours, minutes, seconds);
}

void Date::compare(const Date& d, unsigned long long &thisss, unsigned long long &thtsss) const
{
	thisss = getSecondsI(this->year,this->month,this->day,this->hours,this->minutes,this->seconds);
	thtsss = getSecondsI(d.year,d.month,d.day,d.hours,d.minutes,d.seconds);

	thisss %= 9223372036854775807LL;
	thtsss %= 9223372036854775807LL;
}

bool Date::operator<(const Date& d) const
{
	unsigned long long thisss = -1;
	unsigned long long thtsss = -1;
	this->compare(d, thisss, thtsss);
	return thisss<thtsss;
}

bool Date::operator>(const Date& d) const
{
	unsigned long long thisss = -1;
	unsigned long long thtsss = -1;
	this->compare(d, thisss, thtsss);
	return thisss>thtsss;
}

bool Date::operator==(const Date& d) const
{
	unsigned long long thisss = -1;
	unsigned long long thtsss = -1;
	this->compare(d, thisss, thtsss);
	return thisss==thtsss;
}

bool Date::operator<=(const Date& d) const
{
	unsigned long long thisss = -1;
	unsigned long long thtsss = -1;
	this->compare(d, thisss, thtsss);
	return thisss<=thtsss;
}

bool Date::operator>=(const Date& d) const
{
	unsigned long long thisss = -1;
	unsigned long long thtsss = -1;
	this->compare(d, thisss, thtsss);
	return thisss>=thtsss;
}

bool Date::operator!=(const Date& d) const
{
	unsigned long long thisss = -1;
	unsigned long long thtsss = -1;
	this->compare(d, thisss, thtsss);
	return thisss!=thtsss;
}
