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
 * PropFileReader.cpp
 *
 *  Created on: Aug 18, 2009
 *      Author: sumeet
 */

#include "PropFileReader.h"

PropFileReader::PropFileReader()
{
	mergeSimProps = false;
}
PropFileReader::PropFileReader(const bool& mergeSimProps)
{
	this->mergeSimProps = mergeSimProps;
}
PropFileReader::~PropFileReader() {
	
}

propMap PropFileReader::getProperties(const std::string& filepath)
{
	propMap all;
	std::string line;
	if (filepath=="")
	{
		return all;
	}
	std::ifstream myfile (&filepath[0],std::ios::in | std::ios::binary);
    if (myfile.is_open())
    {
      strVec vemp;
	  while(getline(myfile,line,'\n'))
	  {
		  if(line!="")
		  {
		  	  if(line.find("=")!=std::string::npos)
		  	  {
		  		  StringUtil::trim(line);
		  		  if(line.find("=")!=line.length()-1)
		  		  {
		  			  all[line.substr(0, line.find("="))] = StringUtil::trimCopy(line.substr(line.find("=")+1));
		  		  }
		  	  }
		  }
	  }
	  myfile.close();
    }
    return all;
}

propMultiMap PropFileReader::getPropertiesMultiMap(const std::string& filepath)
{
	propMultiMap all;
	std::string line;
	if (filepath=="")
	{
		return all;
	}
	std::ifstream myfile (&filepath[0],std::ios::in | std::ios::binary);
    if (myfile.is_open())
    {
      strVec vemp;
	  while(getline(myfile,line,'\n'))
	  {
		  if(line!="")
		  {
		  	  if(line.find("=")!=std::string::npos)
		  	  {
		  		  StringUtil::trim(line);
		  		  if(line.find("=")!=line.length()-1)
		  		  {
		  			  all[line.substr(0, line.find("="))].push_back(StringUtil::trimCopy(line.substr(line.find("=")+1)));
		  		  }
		  	  }
		  }
	  }
	  myfile.close();
    }
    return all;
}
