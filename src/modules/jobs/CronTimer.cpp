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
 * CronTimer.cpp
 *
 *  Created on: 06-Jul-2013
 *      Author: sumeetc
 */

#include "CronTimer.h"

CronTimer::CronTimer(const std::string& cronExpression)
{
	starts.push_back(0);
	starts.push_back(0);
	starts.push_back(1);
	starts.push_back(1);
	starts.push_back(0);
	starts.push_back(1970);

	ends.push_back(59);
	ends.push_back(23);
	ends.push_back(31);
	ends.push_back(12);
	ends.push_back(6);
	ends.push_back(2099);

	months["JAN"] = 1;
	months["FEB"] = 2;
	months["MAR"] = 3;
	months["APR"] = 4;
	months["MAY"] = 5;
	months["JUN"] = 6;
	months["JUL"] = 7;
	months["AUG"] = 8;
	months["SEP"] = 9;
	months["OCT"] = 10;
	months["NOV"] = 11;
	months["DEC"] = 12;

	days["SUN"] = 0;
	days["MON"] = 1;
	days["TUE"] = 2;
	days["WED"] = 3;
	days["THU"] = 4;
	days["FRI"] = 5;
	days["SAT"] = 6;

	std::vector<std::string> cronParts = StringUtil::splitAndReturn<std::vector<std::string> >(cronExpression, " ");
	bool valid = true;
	if((int)cronParts.size()>6)
	{
		valid = false;
	}
	else
	{
		for (int position = 0; position < (int)cronParts.size(); ++position)
		{
			std::string tst = cronParts.at(position);
			CronExpressionPart cpart;
			if(tst.find(" ")==std::string::npos)
			{
				if(tst=="*")
				{
					cpart.all = true;
				}
				else if(tst.find("-")!=std::string::npos
						&& tst.find("/")!=std::string::npos
						&& tst.find("-")<tst.find("/"))
				{
					std::vector<std::string> delimiters;
					delimiters.push_back("-");
					delimiters.push_back("/");
					std::vector<std::string> parts = StringUtil::split(tst, delimiters);
					int start, end;
					start = starts.at(position);
					end = ends.at(position);
					if(parts.size()==3)
					{
						std::vector<int> vparts;
						for (int i = 0; i < 3; ++i) {
							int partValue = -1;
							try {
								partValue = CastUtil::lexical_cast<int>(parts.at(i));
							} catch (...) {
								if(position==3)
								{
									std::string month = StringUtil::toUpperCopy(parts.at(i));
									if(months.find(month)!=months.end())
									{
										partValue = months[month];
									}
									else
									{
										valid = false;
									}
								}
								else if(position==4)
								{
									std::string day = StringUtil::toUpperCopy(parts.at(i));
									if(days.find(day)!=days.end())
									{
										partValue = days[day];
									}
									else
									{
										valid = false;
									}
								}
								else
								{
									valid = false;
								}
							}
							if(partValue==-1)
							{
								break;
							}
							vparts.push_back(partValue);
						}

						if(vparts.at(0)<start || vparts.at(1)>end || vparts.at(2)>(vparts.at(1)-vparts.at(0))) {
							valid = false;
						} else {
							cpart.from = vparts.at(0);
							cpart.to = vparts.at(1);
							cpart.interval = vparts.at(2);
						}
					}
				}
				else if(tst.find("-")!=std::string::npos)
				{
					std::vector<std::string> parts = StringUtil::splitAndReturn<std::vector<std::string> >(tst, "-");
					int start, end;
					start = starts.at(position);
					end = ends.at(position);
					if(parts.size()==2)
					{
						std::vector<int> vparts;
						for (int i = 0; i < 2; ++i) {
							int partValue = -1;
							try {
								partValue = CastUtil::lexical_cast<int>(parts.at(i));
							} catch (...) {
								if(position==3)
								{
									std::string month = StringUtil::toUpperCopy(parts.at(i));
									if(months.find(month)!=months.end())
									{
										partValue = months[month];
									}
									else
									{
										valid = false;
									}
								}
								else if(position==4)
								{
									std::string day = StringUtil::toUpperCopy(parts.at(i));
									if(days.find(day)!=days.end())
									{
										partValue = days[day];
									}
									else
									{
										valid = false;
									}
								}
								else
								{
									valid = false;
								}
							}
							if(partValue==-1)
							{
								break;
							}
							vparts.push_back(partValue);
						}

						if(vparts.at(0)<start || vparts.at(1)>end) {
							valid = false;
						} else {
							cpart.from = vparts.at(0);
							cpart.to = vparts.at(1);
						}
					}
				}
				else if(tst.find("/")!=std::string::npos)
				{
					std::vector<std::string> parts = StringUtil::splitAndReturn<std::vector<std::string> >(tst, "/");
					int start, end;
					start = starts.at(position);
					end = ends.at(position);
					if(parts.size()==2)
					{
						std::vector<int> vparts;
						for (int i = 0; i < 2; ++i) {
							int partValue = -1;
							try {
								partValue = CastUtil::lexical_cast<int>(parts.at(i));
							} catch (...) {
								if(parts.at(i)=="*")
								{
									partValue = start;
								}
								else if(position==3)
								{
									std::string month = StringUtil::toUpperCopy(parts.at(i));
									if(months.find(month)!=months.end())
									{
										partValue = months[month];
									}
									else
									{
										valid = false;
									}
								}
								else if(position==4)
								{
									std::string day = StringUtil::toUpperCopy(parts.at(i));
									if(days.find(day)!=days.end())
									{
										partValue = days[day];
									}
									else
									{
										valid = false;
									}
								}
								else
								{
									valid = false;
								}
							}
							if(partValue==-1)
							{
								break;
							}
							vparts.push_back(partValue);
						}

						if(vparts.at(0)<start || vparts.at(1)>end || vparts.at(1)>(end-vparts.at(0))) {
							valid = false;
						} else {
							cpart.from = vparts.at(0);
							cpart.to = end;
							cpart.interval = vparts.at(1);
						}
					}
				}
				else if(tst.find(",")!=std::string::npos)
				{
					std::vector<std::string> parts = StringUtil::splitAndReturn<std::vector<std::string> >(tst, ",");
					int start, end;
					start = starts.at(position);
					end = ends.at(position);

					std::vector<int> vparts;
					for (int i = 0; i < (int)parts.size(); ++i) {
						int partValue = -1;
						try {
							partValue = CastUtil::lexical_cast<int>(parts.at(i));
						} catch (...) {
							if(position==3)
							{
								std::string month = StringUtil::toUpperCopy(parts.at(i));
								if(months.find(month)!=months.end())
								{
									partValue = months[month];
								}
								else
								{
									valid = false;
								}
							}
							else if(position==4)
							{
								std::string day = StringUtil::toUpperCopy(parts.at(i));
								if(days.find(day)!=days.end())
								{
									partValue = days[day];
								}
								else
								{
									valid = false;
								}
							}
							else
							{
								valid = false;
							}
						}
						if(partValue==-1 || partValue<start || partValue>end)
						{
							break;
						}
						vparts.push_back(partValue);
					}

					if(valid) {
						for (int j = 0; j < (int)vparts.size(); ++j) {
							cpart.list += "," + CastUtil::lexical_cast<std::string>(vparts.at(j)) + ",";
							cpart.lstParts.push_back(vparts.at(j));
							sort(cpart.lstParts.begin(), cpart.lstParts.end());
						}
					}
				}
				else
				{
					int start, end;
					start = starts.at(position);
					end = ends.at(position);

					int partValue = -1;
					try {
						partValue = CastUtil::lexical_cast<int>(tst);
					} catch (...) {
						if(position==3)
						{
							std::string month = StringUtil::toUpperCopy(tst);
							if(months.find(month)!=months.end())
							{
								partValue = months[month];
							}
							else
							{
								valid = false;
							}
						}
						else if(position==4)
						{
							std::string day = StringUtil::toUpperCopy(tst);
							if(days.find(day)!=days.end())
							{
								partValue = days[day];
							}
							else
							{
								valid = false;
							}
						}
						else
						{
							valid = false;
						}
					}
					if(partValue==-1 || partValue<start || partValue>end)
					{
						break;
					}
					cpart.from = partValue;
				}
			}
			if(!valid)
			{
				break;
			}
			else
			{
				cParts.push_back(cpart);
			}
		}

		if(!valid)
		{
			cParts.clear();
			throw "Invalid Cron Expression";
		}

		if(cParts.size()==5) {
			CronExpressionPart cpart;
			cpart.all = true;
			cParts.push_back(cpart);
		}
		//TODO if cparts does not have year part add new * year part
	}

}

CronTimer::~CronTimer() {
	//delete nextRunDate;
}

std::string CronTimer::toString()
{
	std::vector<std::string> types;
	types.push_back("minute");
	types.push_back("hour");
	types.push_back("day");
	types.push_back("month");
	types.push_back("dayOfWeek");
	types.push_back("year");
	std::string builder;
	for (int var = 0; var < (const int&)cParts.size(); ++var) {
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
				builder.append(CastUtil::lexical_cast<std::string>(start) + " ");
				start += cParts.at(var).interval;
			}
			builder.append(" " + types.at(var));
		}
		else if(cParts.at(var).from!=-1 && cParts.at(var).to!=-1)
		{
			int start = cParts.at(var).from;
			int end = cParts.at(var).to;
			builder.append("every " + types.at(var) + " from " + CastUtil::lexical_cast<std::string>(start) + " " + CastUtil::lexical_cast<std::string>(end));
		}
		else
		{
			builder.append("every " + CastUtil::lexical_cast<std::string>(cParts.at(var).from) + " " + types.at(var));
		}
		if(var!=(int)cParts.size()-1)
		{
			builder.append(",");
		}
	}
	return builder;
}
bool CronTimer::isValid(const int& pos, const int& cvalue, const int& nvalue)
{
	bool isValid = false;
	/*if(pos==5 && cParts.size()<6)
	{
		isValid = true;
	}
	else */if(pos<6 && cvalue>0)
	{
		if(cParts.at(pos).all)
		{
			isValid = true;
		}
		else if(cParts.at(pos).interval==-1
				&& cParts.at(pos).from==cvalue)
		{
			isValid = true;
		}
		else if(cParts.at(pos).interval==-1 && cParts.at(pos).to!=-1
				&& cvalue>=cParts.at(pos).from
				&& cvalue<=cParts.at(pos).to)
		{
			isValid = true;
		}
		else if(cParts.at(pos).interval!=-1
				&& cvalue>=cParts.at(pos).from
				&& cvalue<=cParts.at(pos).to
				&& (cvalue - cParts.at(pos).from)%cParts.at(pos).interval == 0)
		{
			isValid = true;
		}
		else if(cParts.at(pos).list.find("," + CastUtil::lexical_cast<std::string>(cvalue) + ",")!=std::string::npos)
		{
			isValid = true;
		}
	}
	if(isValid && cvalue!=nvalue)
	{
		isValid = false;
	}
	return isValid;
}

void CronTimer::correctNextRunDateTime(const int& pos)
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

		//cout << " ========= " << start << " " << end << " " << nextRunDate.toString() << std::endl;

		if(pos==1) {
			if(nextRunDate.getMinutes()>=end)
			{
				nextRunDate.updateMinutes(start - nextRunDate.getMinutes());
			}
		} else if(pos==2) {
			if(nextRunDate.getHours()>=end)
			{
				nextRunDate.updateHours(start - nextRunDate.getHours());
			}
			correctNextRunDateTime(pos-1);
		} else if(pos==3) {
			if(nextRunDate.getDay()>=end)
			{
				nextRunDate.updateDays(start - nextRunDate.getDay());
			}
			correctNextRunDateTime(pos-1);
		} else if(pos==5) {
			if(nextRunDate.getMonth()>=end)
			{
				nextRunDate.updateMonths(start - nextRunDate.getMonth());
			}
			correctNextRunDateTime(pos-2);
		}
	}
}

bool CronTimer::tryIncrement(const int& pos, const int& cvalue)
{
	bool lstExists = false;
	try {
		int val = cvalue;
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
	bool isRangeVal = cvalue>=cParts.at(pos).from
						&& cvalue<cParts.at(pos).to;

	if(lstExists)
	{
		int val = cvalue;
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
		intervalValue = val - cvalue;
	}
	if(cParts.at(pos).interval!=-1 && cvalue + intervalValue > cParts.at(pos).to)
	{
		intervalValue = cParts.at(pos).from - cvalue;
	}
	if(cParts.at(pos).all || cParts.at(pos).interval!=-1 || isRangeVal || lstExists)
	{
		if(pos==0)
		{
			nextRunDate.updateMinutes(intervalValue);
		}
		else if(pos==1)
		{
			nextRunDate.updateHours(intervalValue);
		}
		else if(pos==2)
		{
			nextRunDate.updateDays(intervalValue);
		}
		else if(pos==3)
		{
			nextRunDate.updateMonths(intervalValue);
		}
		else if(pos==5)
		{
			nextRunDate.updateYears(intervalValue);
		}
		correctNextRunDateTime(pos);

		incrementDone = true;

		if(intervalValue<0){
			incrementDone = false;
		}

	}
	return incrementDone;
}

void CronTimer::run()
{
	nextRunDate = Date();
	while(true)
	{
		sleep(1);
		Date d2;
		if(isValid(5, d2.getYear(), nextRunDate.getYear()))
		{
			if(isValid(3, d2.getMonth(), nextRunDate.getMonth()))
			{
				if(isValid(2, d2.getDay(), nextRunDate.getDay()))
				{
					if(isValid(1, d2.getHours(), nextRunDate.getHours()))
					{
						if(isValid(0, d2.getMinutes(), nextRunDate.getMinutes()))
						{
							std::cout << d2.toString() + " Cron run" << std::endl;

							bool incrementDone = false;
							incrementDone = tryIncrement(0, nextRunDate.getMinutes());
							//cout << "inc mins " + nextRunDate.toString() << std::endl;
							if(!incrementDone) {
								incrementDone = tryIncrement(1, nextRunDate.getHours());
								//cout << "inc hours " + nextRunDate.toString() << std::endl;
							}
							if(!incrementDone) {
								incrementDone = tryIncrement(2, nextRunDate.getDay());
								//cout << "inc days " + nextRunDate.toString() << std::endl;
							}
							if(!incrementDone) {
								incrementDone = tryIncrement(3, nextRunDate.getMonth());
								//cout << "inc months " + nextRunDate.toString() << std::endl;
							}
							if(!incrementDone) {
								incrementDone = tryIncrement(5, nextRunDate.getYear());
								//cout << "inc years " + nextRunDate.toString() << std::endl;
							}

							std::cout << "Next Run Date = " + nextRunDate.toString() << std::endl;
						}
					}
				}
			}
		}
	}
}
