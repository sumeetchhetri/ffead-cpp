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
 * CsvFileReader.cpp
 *
 *  Created on: 29-Jan-2013
 *      Author: sumeetc
 */

#include "CsvFileReader.h"

CsvFileReader::CsvFileReader() {
	// TODO Auto-generated constructor stub

}

CsvFileReader::~CsvFileReader() {
	// TODO Auto-generated destructor stub
}

strVecVec CsvFileReader::getRows(string filepath)
{
	strVecVec all;
	string line;
	if (filepath=="")
	{
		return all;
	}
	ifstream myfile (&filepath[0],ios::in | ios::binary);
	if (myfile.is_open())
	{
		while(getline(myfile,line,'\n'))
		{
			if(line!="")
			{
				StringUtil::replaceAll(line,"\n","");
				StringUtil::replaceAll(line,"\r","");
				vector<string> vemp = StringUtil::split(line, (","));
				if(vemp.size()>0)
					all.push_back(vemp);
			}
		}
		myfile.close();
	}
	return all;
}
