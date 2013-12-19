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
 * FilterHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "FilterHandler.h"

FilterHandler::FilterHandler() {
	// TODO Auto-generated constructor stub

}

FilterHandler::~FilterHandler() {
	// TODO Auto-generated destructor stub
}

void FilterHandler::handleIn(HttpRequest* req, HttpResponse& res, string ext)
{
	map<string, vector<string> > filterMap = ConfigurationData::getInstance()->filterMap;
	Logger logger = LoggerFactory::getLogger("FilterHandler");
	if(filterMap.find(req->getCntxt_name()+"*.*in")!=filterMap.end() || filterMap.find(req->getCntxt_name()+ext+"in")!=filterMap.end())
	{
		vector<string> tempp;
		if(filterMap.find(req->getCntxt_name()+"*.*in")!=filterMap.end())
			tempp = filterMap[req->getCntxt_name()+"*.*in"];
		else
			tempp = filterMap[req->getCntxt_name()+ext+"in"];

		for (int var = 0; var < (int)tempp.size(); ++var)
		{
			string claz = tempp.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+req->getCntxt_name()+claz, req->getCntxt_name());
			Filter *filter = (Filter*)_temp;
			filter->doInputFilter(req);
			logger << "Input Filter called" << endl;
		}
	}
}


bool FilterHandler::handle(HttpRequest* req, HttpResponse& res, string ext)
{
	bool continue_proc_request = true;
	map<string, vector<string> > filterMap = ConfigurationData::getInstance()->filterMap;
	Logger logger = LoggerFactory::getLogger("FilterHandler");
	if(filterMap.find(req->getCntxt_name()+"*.*handle")!=filterMap.end() || filterMap.find(req->getCntxt_name()+ext+"handle")!=filterMap.end())
	{
		vector<string> tempp;
		if(filterMap.find(req->getCntxt_name()+"*.*in")!=filterMap.end())
			tempp = filterMap[req->getCntxt_name()+"*.*handle"];
		else
			tempp = filterMap[req->getCntxt_name()+ext+"handle"];

		for (int var = 0; var < (int)tempp.size(); ++var)
		{
			string claz = tempp.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+req->getCntxt_name()+claz, req->getCntxt_name());
			Filter *filter = (Filter*)_temp;
			continue_proc_request = filter->doHandle(req, &res);
			logger << "Handler Filter called" << endl;
		}
	}
	return continue_proc_request;
}

void FilterHandler::handleOut(HttpRequest* req, HttpResponse& res, string ext)
{
	map<string, vector<string> > filterMap = ConfigurationData::getInstance()->filterMap;
	Logger logger = LoggerFactory::getLogger("FilterHandler");
	if(filterMap.find(req->getCntxt_name()+"*.*out")!=filterMap.end() || filterMap.find(req->getCntxt_name()+ext+"out")!=filterMap.end())
	{
		vector<string> tempp;
		if(filterMap.find(req->getCntxt_name()+"*.*out")!=filterMap.end())
			tempp = filterMap[req->getCntxt_name()+"*.*out"];
		else
			tempp = filterMap[req->getCntxt_name()+ext+"out"];

		for (int var = 0; var < (int)tempp.size(); ++var)
		{
			string claz = tempp.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+req->getCntxt_name()+claz, req->getCntxt_name());
			Filter *filter = (Filter*)_temp;
			filter->doOutputFilter(&res);
			logger << "Output Filter called" << endl;
		}
	}
}
