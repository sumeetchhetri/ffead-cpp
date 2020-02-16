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
 * ExtHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "ExtHandler.h"

bool ExtHandler::handle(HttpRequest* req, HttpResponse* res, void* dlib, void* ddlib, const std::string& ext, Reflector& reflector)
{
	std::map<std::string, std::string, std::less<> >* tmplMap = &(ConfigurationData::getInstance()->templateMappingMap.find(req->getCntxt_name())->second);

	//Logger logger = LoggerFactory::getLogger("ExtHandler");
	bool cntrlit = false;
	std::string content, claz;
#ifdef INC_TPE
	if(tmplMap->find(req->getCurl())!=tmplMap->end())
	{
		if(ddlib != NULL)
		{
			std::string tpefilename = tmplMap->find(req->getCurl())->second.substr(tmplMap->find(req->getCurl())->second.find(";")+1);
			std::string tpeclasname = tmplMap->find(req->getCurl())->second.substr(0, tmplMap->find(req->getCurl())->second.find(";"));
			cntrlit = true;
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("template_"+tpeclasname, req->getCntxt_name());
			if(_temp!=NULL)
			{
				try {
					args argus;
					vals valus;
					ClassInfo* srv = ConfigurationData::getClassInfo(tpeclasname, req->getCntxt_name());
					argus.push_back("HttpRequest*");
					argus.push_back("Context*");
					const Method& meth = srv->getMethod("getContext", argus);
					if(meth.getMethodName()!="")
					{
						Context cnt;
						valus.push_back(req);
						valus.push_back(&cnt);
						reflector.invokeMethodGVP(_temp,meth,valus);

						std::string fname = "_" + tpefilename + "emittTemplateHTML";
						void* mkr = dlsym(ddlib, fname.c_str());
						if(mkr!=NULL)
						{
							TemplatePtr f =  (TemplatePtr)mkr;
							content = f(&cnt);
						}
						else
						{
							//logger << ("No template found for " + fname) << std::endl;
							res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
						}
					}
					else
					{
						//logger << "Invalid Template handler, no method getContext found..." << std::endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
					}
				} catch(const std::exception& e) {
					//logger << "Template exception occurred" << std::endl;
					res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				}
				ConfigurationData::getInstance()->ffeadContext.release(_temp, "template_"+tpeclasname, req->getCntxt_name());
			}
			if(content.length()>0)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
				res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_SHTML);
				res->setContent(content);
			}
		}
		if(cntrlit && res->getStatusCode()!="404") {
			res->setDone(true);
		}
		return cntrlit;
	}
#endif
	std::map<std::string, std::string, std::less<> >* dcpMap = &(ConfigurationData::getInstance()->dcpMappingMap.find(req->getCntxt_name())->second);
	std::map<std::string, std::string, std::less<> >* vwMap = &(ConfigurationData::getInstance()->viewMappingMap.find(req->getCntxt_name())->second);
	std::map<std::string, std::string, std::less<> >* ajaxIntfMap = &(ConfigurationData::getInstance()->ajaxInterfaceMap.find(req->getCntxt_name())->second);
	std::map<std::string, std::string, std::less<> >* fviewMap = &(ConfigurationData::getInstance()->fviewMappingMap.find(req->getCntxt_name())->second);
	std::map<std::string, Element, std::less<> >* formMap = &(ConfigurationData::getInstance()->fviewFormMap.find(req->getCntxt_name())->second);

	if(ajaxIntfMap->find(req->getCurl())!=ajaxIntfMap->end() && req->getMethod()=="POST" && req->getRequestParam("method")!="")
	{
		cntrlit = true;
		std::string& claz = ajaxIntfMap->find(req->getCurl())->second;

		//logger << "Inside Ajax Interface Execute" << std::endl;
		strVec vemp;
		std::string methName = req->getRequestParam("method");
		if(methName=="")
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			res->setDone(true);
			return true;
		}
		else
		{
			std::string temp = req->getRequestParam("paramsize");
			int paramSize = 0;
			if(temp!="")
			{
				try {
					paramSize = CastUtil::toInt(temp.c_str());
				} catch(const std::exception& e) {
					res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
					paramSize = -1;
				}
			}
			if(paramSize>=0)
			{
				//logger << "Reading Ajax params" << std::endl;
				for(int i=0;i<paramSize;i++)
				{
					std::stringstream s;
					std::string ss;
					s << (i+1);
					s >> ss;
					ss = "param_" + ss;
					//logger << ss << std::flush;
					std::string tem = req->getRequestParam(ss);
					vemp.push_back(tem);
				}
				std::string libName = INTER_LIB_FILE;
				std::string funcName;
				std::string metn,re;
				StringUtil::replaceAll(claz, "::", "_");
				metn = std::string(req->getCntxt_name()) + "invokeAjaxMethodFor"+claz+methName;
				void *mkr = dlsym(dlib, metn.c_str());
				if(mkr!=NULL)
				{
					typedef std::string (*Funptr2) (strVec);
					Funptr2 f2 = (Funptr2)mkr;
					//logger << ("Calling method " + metn) << std::endl;
					try {
						re = f2(vemp);
						//logger << "Completed method call" << std::endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
						res->addHeader(HttpResponse::ContentType, (re.find("{")==0 || re.find("[")==0)?"application/json":"text/plain");
						res->setContent(re);
					} catch(const std::exception& e) {
						//logger << "AjaxInterface exception occurred" << std::endl;
						res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
					}
				}
				else
				{
					res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				}
			}
			else
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			}
		}
	}
	else if(ext==".form" && formMap->find(req->getFile())!=formMap->end())
	{
		try {
			cntrlit = FormHandler::handle(req, res, reflector, &((*formMap)[req->getFile()]));
			//logger << ("Request handled by FormHandler") << std::endl;
		} catch(const std::exception& e) {
			//logger << "FormHandler exception occurred" << std::endl;
			res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
		}
	}
	else if(ext==".fview" && fviewMap->find(req->getFile())!=fviewMap->end())
	{
		cntrlit = FviewHandler::handle(req, res);
		//logger << ("Request handled by FviewHandler") << std::endl;
	}
#ifdef INC_DCP
	else if(dcpMap->find(req->getCurl())!=dcpMap->end())
	{
		std::string libName = INTER_LIB_FILE;
		if(ddlib != NULL)
		{
			cntrlit = true;
			std::string meth;
			std::string file = dcpMap->find(req->getCurl())->second;
			meth = "_" + file + "emittHTML";

			void *mkr = dlsym(ddlib, meth.c_str());
			if(mkr!=NULL)
			{
				DCPPtr f =  (DCPPtr)mkr;
				try {
					content = f();
					if(content.length()>0)
					{
						res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
						res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_SHTML);
						res->setContent(content);
					}
				} catch(const std::exception& e) {
					//logger << "DCP exception occurred" << std::endl;
					res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				}
			}
			else
			{
				//logger << ("No dcp found for " + meth) << std::endl;
			}
		}
	}
#endif
#ifdef INC_DVIEW
	else if(ext==".view" && vwMap->find(req->getCurl())!=vwMap->end())
	{
		void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("dview_"+vwMap->find(req->getCurl())->second, req->getCntxt_name());
		if(_temp!=NULL)
		{
			cntrlit = true;
			try {
				args argus;
				argus.push_back("Document*");
				vals valus;
				ClassInfo* srv = ConfigurationData::getClassInfo(vwMap->find(req->getCurl())->second, req->getCntxt_name());
				const Method& meth = srv->getMethod("getDocument", argus);
				if(meth.getMethodName()!="")
				{
					Document doc;
					valus.push_back(&doc);
					reflector.invokeMethodGVP(_temp,meth,valus);
					View view;
					std::string t = view.generateDocument(doc);
					content = t;
				}
				else
				{
					//logger << "Invalid Dynamic View handler, no method getDocument found..." << std::endl;
					res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				}
			} catch(const std::exception& e) {
				//logger << "Dview exception occurred" << std::endl;
				res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			}
			ConfigurationData::getInstance()->ffeadContext.release(_temp, "dview_"+vwMap->find(req->getCurl())->second, req->getCntxt_name());
		}
		else
		{
			//logger << "Invalid Dynamic View handler" << std::endl;
		}

		if(content.length()>0)
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_SHTML);
			res->setContent(content);
		}
	}
#endif
	if(cntrlit && res->getStatusCode()!="404") {
		res->setDone(true);
	}
	return cntrlit;
}
