/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * SimpleCsvReader.cpp
 *
 *  Created on: 29-Jan-2013
 *      Author: sumeetc
 */

#include "SimpleCsvFileReader.h"

strVecVec SimpleCsvReader::getRows(const std::string& filepath)
{
	strVecVec all;
	std::string line;
	if (filepath=="")
	{
		return all;
	}
	std::ifstream myfile (&filepath[0],std::ios::in | std::ios::binary);
	if (myfile.is_open())
	{
		while(getline(myfile,line,'\n'))
		{
			if(line!="")
			{
				StringUtil::replaceAll(line,"\n","");
				StringUtil::replaceAll(line,"\r","");
				std::vector<std::string> vemp = StringUtil::splitAndReturn<std::vector<std::string> >(line, (","));
			    if(vemp.size()>0) {
			    	all.push_back(vemp);
			    }
			}
		}
		myfile.close();
	}
	return all;
}

strVecVec AdvancedCsvReader::getRows(const std::string& filepath)
{
	strVecVec all;
	std::string line;
	if (filepath=="")
	{
		return all;
	}
	std::ifstream myfile (&filepath[0],std::ios::in | std::ios::binary);
	csv::CSVReader reader(filepath);
	for (csv::CSVRow& row: reader) {
		std::vector<std::string> vemp;
	    for (csv::CSVField& field: row) {
	    	vemp.push_back(field.get());
	    }
	    if(vemp.size()>0) {
	    	all.push_back(vemp);
	    }
	}
	return all;
}
