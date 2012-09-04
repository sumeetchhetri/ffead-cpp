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
 * Date.h
 *
 *  Created on: Jun 4, 2010
 *      Author: sumeet
 */

#ifndef DATE_H_
#define DATE_H_
#include <time.h>
#include "string"
#include "vector"
#include "CastUtil.h"
#include "StringUtil.h"

using namespace std;

class Date {
	string month;
	string monthw;
	string year;
	string day;
	int weekday;
	string hh;
	string mm;
	string ss;
	string dayw;
	long long nanoseconds;
	string getMon(string);
	string getMonFd(string);
	long getDaysInt();
	long getDays(long y,long m,long d);
	long getHours(long y,long m,long d, long hh);
	long getMinutes(long y,long m,long d, long hh, long mi);
	unsigned long long getSeconds(long y,long m,long d, long hh, long mi, long ss);
	Date getDateFromDays(long days);
	void getDateFromHours(long hours);
	void getDateFromMinutes(long long minutes);
	void getDateFromSeconds(long long seconds);
	void setMonthw(string monthw);
	void setDayw(string dayw);
	int getWeekDayVal(string dayName);
	void setSs(int ss);
	void setMm(int mm);
	void setHh(int hh);
	void setDay(int day);
	void setYear(int year);
	void setMonth(int month);
public:
	Date();
	Date(int yyyy,string mmm,int dd);
	Date(int yyyy,int mm,int dd);
	Date(int yy,string mmm,int dd,bool);
	Date(int yy,int mm,int dd,bool);
	void setTime(int hh,int mi,int ss);
	virtual ~Date();
    int getMonth();
    string getMonthw() const;
    int getYear() const;
    int getDay() const;
    int getHh() const;
    int getMm() const;
    int getSs() const;
	long long getNanoSeconds() const;
    string getDayw() const;
    string toString();
    string getDayStr() const;
    string getMonthStr() const;
    string getHhStr() const;
    string getSsStr() const;
    string getNsStr() const;
    string getMmStr() const;
    string getYearStr() const;
    int getWeekDay() const;
    Date addSeconds(long seconds);
	Date addMinutes(long minutes);
	Date addHours(long hours);
	Date addDays(long days);
	Date addMonths(long months);
	Date addYears(long years);
	static bool validateDate(int dd, int mm, int yyyy);
	static string getDayName(int dd, int mm, int yyyy);
	int test();
};

#endif /* DATE_H_ */
