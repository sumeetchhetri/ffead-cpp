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
 * HttpResponseParser.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: sumeet
 */

#include "HttpResponseParser.h"

string* HttpResponseParser::headernames = NULL;

HttpResponseParser::HttpResponseParser() {
	logger = Logger::getLogger("HttpResponseParser");
}

HttpResponseParser::~HttpResponseParser() {
	// TODO Auto-generated destructor stub
}



HttpResponseParser::HttpResponseParser(string vecstr,string path)
{
	vector<string> vec;
	boost::iter_split(vec, vecstr, boost::first_finder("\n"));
	if(HttpResponseParser::headernames==NULL)
	{
		HttpResponseParser::headernames = new string();
		ifstream ifs((path+"http-res-headers").c_str(),ifstream::in);
		string tempios;
		while(getline(ifs,tempios,'\n'))
		{
			HttpResponseParser::headernames->append(tempios);
		}
		logger << endl << "done reading header types" << endl;
	}
	if(vec.size()!=0)
	{
		this->content = "";
		string conten;
		bool contStarts = false;
		for(unsigned int i=0;i<vec.size();i++)
		{
			//logger << endl << "Reading line " << vec.at(i) << endl << flush;
			vector<string> temp,vemp,memp;

			if(vec.at(i)=="\r" || vec.at(i)==""|| vec.at(i)=="\r\n")
			{
				contStarts = true;
				continue;
			}
			boost::iter_split(temp, vec.at(i), boost::first_finder(": "));
			if(temp.size()>1)
			{
				logger << "line => " << vec.at(i) << endl;
				boost::replace_first(temp.at(1),"\r","");
				if(HttpResponseParser::headernames->find(temp.at(0)+":")!=string::npos)
				{
					//logger << temp.at(0) << " = " << temp.at(1) << endl;
					this->headers[temp.at(0)] = temp.at(1);
				}
				else
					logger << "\nnot a valid header" << temp.at(0) << endl;
			}
			else
			{

				string tem = vec.at(i);
				if(!contStarts)
				{
					logger << "line => " << vec.at(i) << endl;
					vector<string> httpst;
					boost::iter_split(httpst, tem, boost::first_finder(" "));
					if(httpst.at(0).find("HTTP")==string::npos)
					{

					}
					else
					{
						this->headers["Version"] = httpst.at(0);
						this->headers["StatusCode"] = httpst.at(1);
						this->headers["StatusMsg"] = httpst.at(2);
					}
				}
				else
				{
					string temp;
					if(vec.at(i).find("<?")!=string::npos && vec.at(i).find("?>")!=string::npos)
					{
						temp = vec.at(i).substr(vec.at(i).find("?>")+2);
						conten.append(temp);
					}
					else
						conten.append(vec.at(i));
					if(i!=vec.size()-1)
						conten.append("\n");
				}
			}
		}
		this->content = conten;
		if(this->content!="")
		{
			//logger << this->content << flush;
		}
	}
}

HttpResponseParser::HttpResponseParser(string vecstr)
{
	vector<string> vec;
	boost::iter_split(vec, vecstr, boost::first_finder("\n"));
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
			boost::iter_split(temp, vec.at(i), boost::first_finder(": "));
			if(temp.size()>1 && !contStarts)
			{
				logger << temp.at(0) << " => " << temp.at(1) << endl;
				boost::replace_first(temp.at(1),"\r","");
				this->headers[temp.at(0)] = temp.at(1);
			}
			else
			{
				string tem = vec.at(i);
				if(!contStarts)
				{
					logger << "line => " << vec.at(i) << endl;
					boost::replace_first(tem,"\r","");
					vector<string> httpst;
					boost::iter_split(httpst, tem, boost::first_finder(" "));
					if(httpst.at(0).find("HTTP")==string::npos)
					{

					}
					else
					{
						this->headers["Version"] = httpst.at(0);
						this->headers["StatusCode"] = httpst.at(1);
						this->headers["StatusMsg"] = httpst.at(2);
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
		if(this->content!="")
		{
			//logger << this->content << flush;
		}
	}
}
