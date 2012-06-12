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
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;

class Date {
	string month;
	string monthw;
	string year;
	string day;
	string hh;
	string mm;
	string ss;
	string dayw;
	string getMon(string);
	long getDays(long y,long m,long d)
	{
		y = y -1900;
		m = (m + 9) % 12;
		y = y - m/10;
		return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ( d - 1 );
	}

	long getHours(long y,long m,long d, long hh)
	{
		long g = getDays(y,m,d);
		int hfd = hh/24;
		hh = hh%24;
		return (g+hfd)*24 + hh;
	}

	long getMinutes(long y,long m,long d, long hh, long mi)
	{
		long g = getHours(y,m,d,hh);
		int mfh = mi/60;
		mi = mi%60;
		return (g+mfh)*60 + mi;
	}

	unsigned long long getSeconds(long y,long m,long d, long hh, long mi, long ss)
	{
		long g = getMinutes(y,m,d,hh,mi);
		int sfm = ss/60;
		ss = ss%60;
		return (g+sfm)*60 + ss;
	}

	void getDateFromDays(long days)
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
		cout << y << " " << mm << " " << dd;
	}

	void getDateFromHours(long hours)
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
		cout << y << " " << mm << " " << dd << " " << hours;
	}

	void getDateFromMinutes(long long minutes)
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
		cout << y << " " << mm << " " << dd << " " << hours << ":" << minutes;
	}

	void getDateFromSeconds(long long seconds)
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
		cout << y << " " << mm << " " << dd << " " << hours << ":" << minutes << ":" << seconds <<  "\n" << endl << flush;
	}

	int test()
	{
		unsigned long long gg = getDays(2008,12,31);
		getDateFromDays(gg);
		cout << endl;
		gg = getHours(2008,12,31,12);
		getDateFromHours(gg);
		cout << endl;
		gg = getMinutes(2008,12,31,12,56);
		getDateFromMinutes(gg);
		cout << endl;
		gg = getSeconds(2008,12,31,12,56,56);
		getDateFromSeconds(gg);
		return 0;
	}
public:
	Date();
	virtual ~Date();
    int getMonth();
    void setMonth(int month);
    string getMonthw() const;
    void setMonthw(string monthw);
    int getYear() const;
    void setYear(int year);
    int getDay() const;
    void setDay(int day);
    int getHh() const;
    void setHh(int hh);
    int getMm() const;
    void setMm(int mm);
    int getSs() const;
    void setSs(int ss);
    string getDayw() const;
    void setDayw(string dayw);
    string toString();
    string getDayStr() const;
    string getMonthStr() const;
    string getHhStr() const;
    string getSsStr() const;
    string getMmStr() const;
    string getYearStr() const;
    void setYearf(string year)
    {
    	this->year = year;
    }
    void setDayf(string day)
	{
		this->day = day;
	}
    void setMmf(string month)
	{
		this->month = month;
	}
    void setHhf(string hh)
	{
		this->hh = hh;
	}
    void setMif(string mi)
	{
		this->mm = mi;
	}
    void setSsf(string ss)
	{
		this->ss = ss;
	}
};

#endif /* DATE_H_ */
