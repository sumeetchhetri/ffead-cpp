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

bool FilterHandler::getFilterForPath(const string& cntxtName, const string& actUrl, vector<string>& filters, const string& type)
{
	map<string, map<string, vector<string> > > filterObjectMap = ConfigurationData::getInstance()->filterObjectMap;
	if(filterObjectMap.find(cntxtName)!=filterObjectMap.end())
	{
		map<string, vector<string> > filterMap = filterObjectMap[cntxtName];
		map<string, vector<string> >::iterator it;
		for (it=filterMap.begin();it!=filterMap.end();++it) {
			string pathurl = it->first.substr(0, it->first.length()-type.length());
			if(StringUtil::endsWith(it->first, type) && ConfigurationData::urlMatchesPath(cntxtName, pathurl, actUrl))
			{
				filters = it->second;
				return true;
			}
		}
	}
	return false;
}

void FilterHandler::handleIn(HttpRequest* req, const string& ext, Reflector& reflector)
{
	string acurl = req->getActUrl();
	RegexUtil::replace(acurl,"[/]+","/");
	if(acurl.find("/"+req->getCntxt_name())!=0)
		acurl = "/" + req->getCntxt_name() + "/" + acurl;
	RegexUtil::replace(acurl,"[/]+","/");

	Logger logger = LoggerFactory::getLogger("FilterHandler");
	vector<string> filters;
	if(getFilterForPath(req->getCntxt_name(), acurl, filters, "in"))
	{
		for (int var = 0; var < (int)filters.size(); ++var)
		{
			string claz = filters.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+claz, req->getCntxt_name());
			args argus;
			argus.push_back("HttpRequest*");
			vals valus;
			const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][claz];
			Method meth = srv.getMethod("doInputFilter", argus);
			if(meth.getMethodName()!="")
			{
				valus.push_back(req);
				reflector.invokeMethod<void*>(_temp,meth,valus);
				logger << "Input Filter called" << endl;
			}
		}
	}
}


bool FilterHandler::handle(HttpRequest* req, HttpResponse* res, const string& ext, Reflector& reflector)
{
	string acurl = req->getActUrl();
	RegexUtil::replace(acurl,"[/]+","/");
	if(acurl.find("/"+req->getCntxt_name())!=0)
		acurl = "/" + req->getCntxt_name() + "/" + acurl;
	RegexUtil::replace(acurl,"[/]+","/");

	bool continue_proc_request = true;
	Logger logger = LoggerFactory::getLogger("FilterHandler");
	vector<string> filters;
	if(getFilterForPath(req->getCntxt_name(), acurl, filters, "handle"))
	{
		for (int var = 0; var < (int)filters.size(); ++var)
		{
			string claz = filters.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+claz, req->getCntxt_name());
			args argus;
			argus.push_back("HttpRequest*");
			argus.push_back("HttpResponse*");
			vals valus;
			const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][claz];
			Method meth = srv.getMethod("doHandle", argus);
			if(meth.getMethodName()!="")
			{
				valus.push_back(req);
				valus.push_back(res);
				continue_proc_request = reflector.invokeMethod<bool>(_temp,meth,valus);
				logger << "Handler Filter called" << endl;
			}
		}
	}
	return continue_proc_request;
}

void FilterHandler::handleOut(HttpRequest* req, HttpResponse* res, const string& ext, Reflector& reflector)
{
	string acurl = req->getActUrl();
	RegexUtil::replace(acurl,"[/]+","/");
	if(acurl.find("/"+req->getCntxt_name())!=0)
		acurl = "/" + req->getCntxt_name() + "/" + acurl;
	RegexUtil::replace(acurl,"[/]+","/");

	Logger logger = LoggerFactory::getLogger("FilterHandler");
	vector<string> filters;
	if(getFilterForPath(req->getCntxt_name(), acurl, filters, "out"))
	{
		for (int var = 0; var < (int)filters.size(); ++var)
		{
			string claz = filters.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+claz, req->getCntxt_name());
			args argus;
			argus.push_back("HttpResponse*");
			vals valus;
			const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][claz];
			Method meth = srv.getMethod("doOutputFilter", argus);
			if(meth.getMethodName()!="")
			{
				valus.push_back(res);
				reflector.invokeMethod<void*>(_temp,meth,valus);
				logger << "Output Filter called" << endl;
			}
		}
	}
}
