/*
	Copyright 2009-2012, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (const the& "License"); 
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
#include "AppDefines.h"
#include "Compatibility.h"
#include <unistd.h>
#include <sys/types.h>
#include "string"
#include "vector"
#include "CastUtil.h"
#include "StringUtil.h"
#include <math.h>



class Date {
	struct tm ti;
	int month;
	std::string monthName;
	std::string monthAbbr;
	int year;
	int day;
	int dayOfYear;
	std::string dayName;
	std::string dayAbbr;
	int hours;
	int minutes;
	int seconds;
	std::string hourdesignation;
	long long nanoseconds;
	long timeZoneOffsetSecs;
	float timeZoneOffset;
	std::string timeZone;
	int weekday;
	int pmHours;
	bool isDLS;
	long long epochTime;
	void populateDay();
	void populateMonth();
	void populateDateFields(struct tm* timeinfo);
	void populateEpochAndTimeZone(const bool& utc);
	long getDaysInt();
	long getDays(const long y, const long m, const long d) const;
	long getHoursI(const long y, const long m, const long d, const long hh) const;
	long getMinutesI(const long y, const long m, const long d, const long hh, const long mi) const;
	unsigned long long getSecondsI(const long y, const long m, const long d, const long hh, const long mi, const long ss) const;
	static Date getDateFromDaysInternal(const long& days);
	void compare(const Date& d, unsigned long long &thisss, unsigned long long &thtsss) const;
	std::string getMon(const std::string& m) const;
public:
	tm* getTimeinfo();
	static std::string dayInWords(const int d, const bool& sf = false);
	static std::string monthInWords(const int m, const bool& sf = false);
	Date(const bool& utc= false);
	Date(struct tm* tim);
	Date(const int& yyyy, const std::string& mmm, const int& dd);
	Date(const int& yyyy, const int& mm, const int& dd);
	Date(const int& yy, const std::string& mmm, const int& dd, const bool&);
	Date(const int& yy, const int& mm, const int& dd, const bool&);
	void setTime(const int& hh, const int& mi, const int& ss, const unsigned long long& ns = 0);
	virtual ~Date();
    std::string toString();
    Date addSecondsGet(const double& seconds);
	Date addMinutesGet(const double& minutes);
	Date addHoursGet(const double& hours);
	Date addDaysGet(const long& days);
	Date addMonthsGet(long months);
	Date addYearsGet(const long& years);
	void updateSeconds(const double& dseconds);
	void updateMinutes(const double& dminutes);
	void updateHours(const double& dhours);
	void updateDays(const long& days);
	void updateMonths(long months);
	void updateYears(const long& years);
	static bool validateDate(const int& dd, const int& mm, const int& yyyy);
	static std::string getDayName(int dd, const int& mm, const int& yyyy);
	static Date getDateFromDays(const long& days);
	static Date getDateFromHours(long hours);
	static Date getDateFromMinutes(long long minutes);
	static Date getDateFromSeconds(long long seconds);
	Date toGMT();
	int test();
	bool operator<(const Date& d) const;
	bool operator>(const Date& d) const;
	bool operator==(const Date& d) const;
	bool operator<=(const Date& d) const;
	bool operator>=(const Date& d) const;
	bool operator!=(const Date& d) const;
	int getDay() const;
	const std::string& getDayAbbr() const;
	const std::string& getHourdesignation() const;
	int getMonth() const;
	const std::string& getMonthAbbr() const;
	const std::string& getMonthName() const;
	long long getNanoseconds() const;
	int getPmHours() const;
	const std::string& getTimeZone() const;
	float getTimeZoneOffset() const;
	int getWeekday() const;
	int getYear() const;
	void setTimeZoneOffset(const float& timeZoneOffset);
	int getHours() const;
	int getMinutes() const;
	int getSeconds() const;
	long long getEpochTime() const;
};

#endif /* DATE_H_ */
