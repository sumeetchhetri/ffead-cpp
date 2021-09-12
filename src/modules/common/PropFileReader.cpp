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
 * PropFileReader.cpp
 *
 *  Created on: Aug 18, 2009
 *      Author: sumeet
 */

#include "PropFileReader.h"

PropFileReader::PropFileReader()
{
	this->mergeSimProps = false;
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
			  StringUtil::trim(line);
			  if(line.find("#")==0) continue;//Single line comment
		  	  if(line.find("=")!=std::string::npos)
		  	  {
		  		  if(line.find("=")!=line.length()-1)
		  		  {
		  			  std::string v = StringUtil::trimCopy(line.substr(line.find("=")+1));
		  			  if(v.at(0)=='\'' && v.at(v.length()-1)=='\'') {
		  				  v = v.substr(1, v.length()-2);
		  			  } else if(v.at(0)=='"' && v.at(v.length()-1)=='"') {
		  				  v = v.substr(1, v.length()-2);
		  			  }
		  			  all[StringUtil::trimCopy(line.substr(0, line.find("=")))] = v;
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
			  StringUtil::trim(line);
			  if(line.find("#")==0) continue;//Single line comment
		  	  if(line.find("=")!=std::string::npos)
		  	  {
		  		  if(line.find("=")!=line.length()-1)
		  		  {
		  			  std::string v = StringUtil::trimCopy(line.substr(line.find("=")+1));
		  			  if(v.at(0)=='\'' && v.at(v.length()-1)=='\'') {
		  				  v = v.substr(1, v.length()-2);
		  			  } else if(v.at(0)=='"' && v.at(v.length()-1)=='"') {
		  				  v = v.substr(1, v.length()-2);
		  			  }
		  			  all[StringUtil::trimCopy(line.substr(0, line.find("=")))].push_back(v);
		  		  }
		  	  }
		  }
	  }
	  myfile.close();
    }
    return all;
}
