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
 * CronTimer.h
 *
 *  Created on: 06-Jul-2013
 *      Author: sumeetc
 */

#ifndef CRONTIMER_H_
#define CRONTIMER_H_
#include <algorithm>
#include <iostream>
#include "vector"
#include "string"
#include "map"
#include "Date.h"
#include "CastUtil.h"
using namespace std;

class CronTimer {
	class CronExpressionPart
	{
		bool all;
		int from;
		int to;
		int interval;
		string list;
		int nextRun;
		vector<int> lstParts;
		friend class CronTimer;
		CronExpressionPart()
		{
			interval = -1;
			from = -1;
			to = -1;
			all = false;
			nextRun = -1;
		}
	};
	vector<CronExpressionPart> cParts;
	Date* nextRunDate;
	vector<int> starts, ends, lstParts;
	map<string, int> months, days;
	friend class JobScheduler;
public:
	CronTimer(string cronExpression);
	virtual ~CronTimer();
	string toString()
	{
		vector<string> types;
		types.push_back("minute");
		types.push_back("hour");
		types.push_back("day");
		types.push_back("month");
		types.push_back("dayOfWeek");
		types.push_back("year");
		string builder;
		for (int var = 0; var < (int)cParts.size(); ++var) {
			if(cParts.at(var).all)
			{
				builder.append("every " + types.at(var));
			}
			else if(cParts.at(var).list!="")
			{
				builder.append(cParts.at(var).list + " " + types.at(var));
			}
			else if(cParts.at(var).interval!=-1)
			{
				int start = cParts.at(var).from;
				int end = cParts.at(var).to;
				while(start<=end) {
					builder.append(CastUtil::lexical_cast<string>(start) + " ");
					start += cParts.at(var).interval;
				}
				builder.append(" " + types.at(var));
			}
			else if(cParts.at(var).from!=-1 && cParts.at(var).to!=-1)
			{
				int start = cParts.at(var).from;
				int end = cParts.at(var).to;
				builder.append("every " + types.at(var) + " from " + CastUtil::lexical_cast<string>(start) + " " + CastUtil::lexical_cast<string>(end));
			}
			else
			{
				builder.append("every " + CastUtil::lexical_cast<string>(cParts.at(var).from) + " " + types.at(var));
			}
			if(var!=(int)cParts.size()-1)
			{
				builder.append(",");
			}
		}
		return builder;
	}
	bool isValid(int pos, string cvalue, string nvalue)
	{
		bool isValid = false;
		/*if(pos==5 && cParts.size()<6)
		{
			isValid = true;
		}
		else */if(pos<6 && cvalue!="")
		{
			if(cParts.at(pos).all)
			{
				isValid = true;
			}
			else if(cParts.at(pos).interval==-1
					&& cParts.at(pos).from==CastUtil::lexical_cast<int>(cvalue))
			{
				isValid = true;
			}
			else if(cParts.at(pos).interval==-1 && cParts.at(pos).to!=-1
					&& CastUtil::lexical_cast<int>(cvalue)>=cParts.at(pos).from
					&& CastUtil::lexical_cast<int>(cvalue)<=cParts.at(pos).to)
			{
				isValid = true;
			}
			else if(cParts.at(pos).interval!=-1
					&& CastUtil::lexical_cast<int>(cvalue)>=cParts.at(pos).from
					&& CastUtil::lexical_cast<int>(cvalue)<=cParts.at(pos).to
					&& (CastUtil::lexical_cast<int>(cvalue) - cParts.at(pos).from)%cParts.at(pos).interval == 0)
			{
				isValid = true;
			}
			else if(cParts.at(pos).list.find("," + cvalue + ",")!=string::npos)
			{
				isValid = true;
			}
		}
		if(isValid && CastUtil::lexical_cast<int>(cvalue)!=CastUtil::lexical_cast<int>(nvalue))
		{
			isValid = false;
		}
		return isValid;
	}

	void correctNextRunDateTime(int pos)
	{
		int start, end;
		if(pos>0)
		{
			int index = pos - 1;
			if(pos == 5)
				index = 3;

			start = cParts.at(index).from;
			end = cParts.at(index).to;
			if(cParts.at(index).interval==-1 && end==-1 && start!=-1 && !cParts.at(index).all) {
				end = start;
			}

			if(start==-1)
				start = starts.at(index);
			if(end==-1)
				end = ends.at(index);

			if(cParts.at(index).lstParts.size()>0)
			{
				start = cParts.at(index).lstParts.at(0);
				end = cParts.at(index).lstParts.at(cParts.at(index).lstParts.size()-1);
			}

			//cout << " ========= " << start << " " << end << " " << nextRunDate->toString() << endl;

			if(pos==1) {
				if(nextRunDate->getMm()>=end)
				{
					nextRunDate->updateMinutes(start - nextRunDate->getMm());
				}
			} else if(pos==2) {
				if(nextRunDate->getHh()>=end)
				{
					nextRunDate->updateHours(start - nextRunDate->getHh());
				}
				correctNextRunDateTime(pos-1);
			} else if(pos==3) {
				if(nextRunDate->getDay()>=end)
				{
					nextRunDate->updateDays(start - nextRunDate->getDay());
				}
				correctNextRunDateTime(pos-1);
			} else if(pos==5) {
				if(nextRunDate->getMonth()>=end)
				{
					nextRunDate->updateMonths(start - nextRunDate->getMonth());
				}
				correctNextRunDateTime(pos-2);
			}
		}
	}

	bool tryIncrement(int pos, string cvalue)
	{
		bool lstExists = false;
		try {
			int val = CastUtil::lexical_cast<int>(cvalue);
			for (int var = 0; var < (int)cParts.at(pos).lstParts.size(); ++var) {
				if(val == cParts.at(pos).lstParts.at(var))
				{
					lstExists = true;
					break;
				}
			}
		} catch(...) {

		}

		bool incrementDone = false;
		int intervalValue = 1;
		if(cParts.at(pos).interval!=-1)
		{
			intervalValue = cParts.at(pos).interval;
		}
		bool isRangeVal = CastUtil::lexical_cast<int>(cvalue)>=cParts.at(pos).from
							&& CastUtil::lexical_cast<int>(cvalue)<cParts.at(pos).to;

		if(lstExists)
		{
			int val = CastUtil::lexical_cast<int>(cvalue);
			int j = 0;
			for (j = 0; j < (int)cParts.at(pos).lstParts.size(); ++j) {
				if(val==cParts.at(pos).lstParts.at(j))
					break;
			}
			if(j==(int)cParts.at(pos).lstParts.size()-1)
			{
				val = cParts.at(pos).lstParts.at(0);
			}
			else
			{
				val = cParts.at(pos).lstParts.at(j+1);
			}
			intervalValue = val - CastUtil::lexical_cast<int>(cvalue);
		}
		if(cParts.at(pos).interval!=-1 && CastUtil::lexical_cast<int>(cvalue) + intervalValue > cParts.at(pos).to)
		{
			intervalValue = cParts.at(pos).from - CastUtil::lexical_cast<int>(cvalue);
		}
		if(cParts.at(pos).all || cParts.at(pos).interval!=-1 || isRangeVal || lstExists)
		{
			if(pos==0)
			{
				nextRunDate->updateMinutes(intervalValue);
			}
			else if(pos==1)
			{
				nextRunDate->updateHours(intervalValue);
			}
			else if(pos==2)
			{
				nextRunDate->updateDays(intervalValue);
			}
			else if(pos==3)
			{
				nextRunDate->updateMonths(intervalValue);
			}
			else if(pos==5)
			{
				nextRunDate->updateYears(intervalValue);
			}
			correctNextRunDateTime(pos);

			incrementDone = true;

			if(intervalValue<0){
				incrementDone = false;
			}

		}
		return incrementDone;
	}

	void run()
	{
		nextRunDate = new Date;
		while(true)
		{
			sleep(1);
			Date d2;
			if(isValid(5, d2.getYearStr(), nextRunDate->getYearStr()))
			{
				if(isValid(3, d2.getMonthStr(), nextRunDate->getMonthStr()))
				{
					if(isValid(2, d2.getDayStr(), nextRunDate->getDayStr()))
					{
						if(isValid(1, d2.getHhStr(), nextRunDate->getHhStr()))
						{
							if(isValid(0, d2.getMmStr(), nextRunDate->getMmStr()))
							{
								cout << d2.toString() + " Cron run" << endl;

								bool incrementDone = false;
								incrementDone = tryIncrement(0, nextRunDate->getMmStr());
								//cout << "inc mins " + nextRunDate->toString() << endl;
								if(!incrementDone) {
									incrementDone = tryIncrement(1, nextRunDate->getHhStr());
									//cout << "inc hours " + nextRunDate->toString() << endl;
								}
								if(!incrementDone) {
									incrementDone = tryIncrement(2, nextRunDate->getDayStr());
									//cout << "inc days " + nextRunDate->toString() << endl;
								}
								if(!incrementDone) {
									incrementDone = tryIncrement(3, nextRunDate->getMonthStr());
									//cout << "inc months " + nextRunDate->toString() << endl;
								}
								if(!incrementDone) {
									incrementDone = tryIncrement(5, nextRunDate->getYearStr());
									//cout << "inc years " + nextRunDate->toString() << endl;
								}

								cout << "Next Run Date = " + nextRunDate->toString() << endl;
							}
						}
					}
				}
			}
		}
	}
};

#endif /* CRONTIMER_H_ */
