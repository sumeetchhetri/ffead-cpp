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


HttpResponseParser::HttpResponseParser(string vecstr, HttpResponse &response)
{
	vector<string> vec;
	StringUtil::split(vec, vecstr, ("\n"));
	if(vec.size()!=0)
	{
		this->content = "";
		string conten;
		bool contStarts = false;
		for(unsigned int i=0;i<vec.size();i++)
		{
			//logger << endl << "Reading line " << vec.at(i) << endl << flush;
			vector<string> temp,vemp,memp;
			if((vec.at(i)=="\r" || vec.at(i)==""|| vec.at(i)=="\r\n") && !contStarts)
			{
				contStarts = true;
				continue;
			}
			StringUtil::split(temp, vec.at(i), (": "));
			if(temp.size()>1 && !contStarts)
			{
				//logger << temp.at(0) << " => " << temp.at(1) << endl;
				StringUtil::replaceFirst(temp.at(1),"\r","");
				response.addHeaderValue(temp.at(0), temp.at(1));
			}
			else
			{
				string tem = vec.at(i);
				if(!contStarts)
				{
					//logger << "line => " << vec.at(i) << endl;
					StringUtil::replaceFirst(tem,"\r","");
					vector<string> httpst;
					StringUtil::split(httpst, tem, (" "));
					if(httpst.at(0).find("HTTP")==string::npos)
					{

					}
					else
					{
						response.httpVersion = httpst.at(0);
						response.setStatusCode(httpst.at(1));
						response.setStatusMsg(httpst.at(2));
					}
				}
				else
				{
					conten.append(vec.at(i));
					if(i!=vec.size()-1)
						conten.append("\n");
				}
			}
		}
		this->content = conten;
	}
}

string HttpResponseParser::getContent()
{
	return content;
}
