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
 * HttpResponseParser.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: sumeet
 */

#include "HttpResponseParser.h"

HttpResponseParser::HttpResponseParser() {
	logger = LoggerFactory::getLogger("HttpResponseParser");
}

HttpResponseParser::~HttpResponseParser() {
}


HttpResponseParser::HttpResponseParser(const std::string& vecstr, HttpResponse &response)
{
	std::vector<std::string> vec;
	StringUtil::split(vec, vecstr, ("\n"));
	if(vec.size()!=0)
	{
		this->content = "";
		std::string conten;
		bool contStarts = false;
		for(unsigned int i=0;i<vec.size();i++)
		{
			//logger << endl << "Reading line " << vec.at(i) << endl << std::flush;
			std::vector<std::string> temp,vemp,memp;
			if((vec.at(i)=="\r" || vec.at(i)==""|| vec.at(i)=="\r\n") && !contStarts)
			{
				contStarts = true;
				continue;
			}

			if(i==0)
			{
				StringUtil::replaceFirst(vec.at(i),"\r","");
				std::vector<std::string> httpst;
				StringUtil::split(httpst, vec.at(i), (" "));
				if(httpst.at(0).find("HTTP")==std::string::npos)
				{

				}
				else
				{
					response.httpVersion = httpst.at(0);
					response.setHTTPResponseStatus(HTTPResponseStatus::getStatusByCode(httpst.at(1)));
				}
			}
			else if(vec.at(i).find_first_of(":")!=std::string::npos && !contStarts)
			{
				if(vec.at(i).find(":")==vec.at(i).find(": ")) {
					temp.push_back(StringUtil::toLowerCopy(vec.at(i).substr(0, vec.at(i).find_first_of(": "))));
					temp.push_back(vec.at(i).substr(vec.at(i).find_first_of(": ")+2));
				} else {
					temp.push_back(StringUtil::toLowerCopy(vec.at(i).substr(0, vec.at(i).find_first_of(":"))));
					temp.push_back(vec.at(i).substr(vec.at(i).find_first_of(":")+1));
				}
				//logger << temp.at(0) << " => " << temp.at(1) << std::endl;
				StringUtil::replaceFirst(temp.at(1),"\r","");
				response.addHeader(temp.at(0), temp.at(1));
			}
			else if(contStarts)
			{
				conten.append(vec.at(i));
				if(i!=vec.size()-1)
					conten.append("\n");
			}
		}
		this->content = conten;
	}
}

std::string HttpResponseParser::getContent()
{
	return content;
}
