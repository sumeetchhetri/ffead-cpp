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

CronTimer::CronTimer(string cronExpression)
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

	vector<string> cronParts = StringUtil::split(cronExpression, " ");
	bool valid = true;
	if((int)cronParts.size()>6)
	{
		valid = false;
	}
	else
	{
		for (int position = 0; position < (int)cronParts.size(); ++position)
		{
			string tst = cronParts.at(position);
			CronExpressionPart cpart;
			if(tst.find(" ")==string::npos)
			{
				if(tst=="*")
				{
					cpart.all = true;
				}
				else if(tst.find("-")!=string::npos
						&& tst.find("/")!=string::npos
						&& tst.find("-")<tst.find("/"))
				{
					vector<string> delimiters;
					delimiters.push_back("-");
					delimiters.push_back("/");
					vector<string> parts = StringUtil::split(tst, delimiters);
					int start, end;
					start = starts.at(position);
					end = ends.at(position);
					if(parts.size()==3)
					{
						vector<int> vparts;
						for (int i = 0; i < 3; ++i) {
							int partValue = -1;
							try {
								partValue = CastUtil::lexical_cast<int>(parts.at(i));
							} catch (...) {
								if(position==3)
								{
									string month = StringUtil::toUpperCopy(parts.at(i));
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
									string day = StringUtil::toUpperCopy(parts.at(i));
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
				else if(tst.find("-")!=string::npos)
				{
					vector<string> parts = StringUtil::split(tst, "-");
					int start, end;
					start = starts.at(position);
					end = ends.at(position);
					if(parts.size()==2)
					{
						vector<int> vparts;
						for (int i = 0; i < 2; ++i) {
							int partValue = -1;
							try {
								partValue = CastUtil::lexical_cast<int>(parts.at(i));
							} catch (...) {
								if(position==3)
								{
									string month = StringUtil::toUpperCopy(parts.at(i));
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
									string day = StringUtil::toUpperCopy(parts.at(i));
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
				else if(tst.find("/")!=string::npos)
				{
					vector<string> parts = StringUtil::split(tst, "/");
					int start, end;
					start = starts.at(position);
					end = ends.at(position);
					if(parts.size()==2)
					{
						vector<int> vparts;
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
									string month = StringUtil::toUpperCopy(parts.at(i));
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
									string day = StringUtil::toUpperCopy(parts.at(i));
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
				else if(tst.find(",")!=string::npos)
				{
					vector<string> parts = StringUtil::split(tst, ",");
					int start, end;
					start = starts.at(position);
					end = ends.at(position);

					vector<int> vparts;
					for (int i = 0; i < (int)parts.size(); ++i) {
						int partValue = -1;
						try {
							partValue = CastUtil::lexical_cast<int>(parts.at(i));
						} catch (...) {
							if(position==3)
							{
								string month = StringUtil::toUpperCopy(parts.at(i));
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
								string day = StringUtil::toUpperCopy(parts.at(i));
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
							cpart.list += "," + CastUtil::lexical_cast<string>(vparts.at(j)) + ",";
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
							string month = StringUtil::toUpperCopy(tst);
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
							string day = StringUtil::toUpperCopy(tst);
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
	// TODO Auto-generated destructor stub
}

