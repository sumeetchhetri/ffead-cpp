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

bool FilterHandler::hasFilters(const std::string& cntxtName) {
	return ConfigurationData::getInstance()->filterObjectMap[cntxtName].size()>0;
}

bool FilterHandler::getFilterForPath(const std::string& cntxtName, const std::string& actUrl, std::vector<std::string>& filters, const std::string& type)
{
	std::map<std::string, std::map<std::string, std::vector<std::string> > >& filterObjectMap = ConfigurationData::getInstance()->filterObjectMap;
	std::map<std::string, std::vector<std::string> > filterMap = filterObjectMap[cntxtName];
	std::map<std::string, std::vector<std::string> >::iterator it;
	for (it=filterMap.begin();it!=filterMap.end();++it) {
		std::string pathurl = it->first.substr(0, it->first.length()-type.length());
		if(StringUtil::endsWith(it->first, type) && ConfigurationData::urlMatchesPath(cntxtName, pathurl, actUrl))
		{
			filters = it->second;
			return true;
		}
	}
	return false;
}

void FilterHandler::handleIn(HttpRequest* req, const std::string& ext, Reflector& reflector)
{
	Logger logger = LoggerFactory::getLogger("FilterHandler");
	std::vector<std::string> filters;
	if(getFilterForPath(req->getCntxt_name(), req->getCurl(), filters, "in"))
	{
		for (int var = 0; var < (int)filters.size(); ++var)
		{
			std::string claz = filters.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+claz, req->getCntxt_name());
			args argus;
			argus.push_back("HttpRequest*");
			vals valus;
			ClassInfo* srv = ConfigurationData::getClassInfo(claz, req->getCntxt_name());
			Method meth = srv->getMethod("doInputFilter", argus);
			if(meth.getMethodName()!="")
			{
				valus.push_back(req);
				reflector.invokeMethod<void*>(_temp,meth,valus);
				//logger << "Input Filter called" << std::endl;
			}
			ConfigurationData::getInstance()->ffeadContext.release(_temp, "filter_"+claz, req->getCntxt_name());
		}
	}
}


bool FilterHandler::handle(HttpRequest* req, HttpResponse* res, const std::string& ext, Reflector& reflector)
{
	bool continue_proc_request = true;
	Logger logger = LoggerFactory::getLogger("FilterHandler");
	std::vector<std::string> filters;
	if(getFilterForPath(req->getCntxt_name(), req->getCurl(), filters, "handle"))
	{
		for (int var = 0; var < (int)filters.size(); ++var)
		{
			std::string claz = filters.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+claz, req->getCntxt_name());
			args argus;
			argus.push_back("HttpRequest*");
			argus.push_back("HttpResponse*");
			vals valus;
			ClassInfo* srv = ConfigurationData::getClassInfo(claz, req->getCntxt_name());
			Method meth = srv->getMethod("doHandle", argus);
			if(meth.getMethodName()!="")
			{
				valus.push_back(req);
				valus.push_back(res);
				continue_proc_request = reflector.invokeMethod<bool>(_temp,meth,valus);
				//logger << "Handler Filter called" << std::endl;
			}
			ConfigurationData::getInstance()->ffeadContext.release(_temp, "filter_"+claz, req->getCntxt_name());
		}
	}
	return continue_proc_request;
}

void FilterHandler::handleOut(HttpRequest* req, HttpResponse* res, const std::string& ext, Reflector& reflector)
{
	Logger logger = LoggerFactory::getLogger("FilterHandler");
	std::vector<std::string> filters;
	if(getFilterForPath(req->getCntxt_name(), req->getCurl(), filters, "out"))
	{
		for (int var = 0; var < (int)filters.size(); ++var)
		{
			std::string claz = filters.at(var);
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("filter_"+claz, req->getCntxt_name());
			args argus;
			argus.push_back("HttpResponse*");
			vals valus;
			ClassInfo* srv = ConfigurationData::getClassInfo(claz, req->getCntxt_name());
			Method meth = srv->getMethod("doOutputFilter", argus);
			if(meth.getMethodName()!="")
			{
				valus.push_back(res);
				reflector.invokeMethod<void*>(_temp,meth,valus);
				//logger << "Output Filter called" << std::endl;
			}
			ConfigurationData::getInstance()->ffeadContext.release(_temp, "filter_"+claz, req->getCntxt_name());
		}
	}
}
