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
