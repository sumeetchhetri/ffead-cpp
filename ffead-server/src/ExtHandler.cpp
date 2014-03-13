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

ExtHandler::ExtHandler() {
	// TODO Auto-generated constructor stub

}

ExtHandler::~ExtHandler() {
	// TODO Auto-generated destructor stub
}

bool ExtHandler::handle(HttpRequest* req, HttpResponse& res, void* dlib, void* ddlib,string ext)
{
	string resourcePath = ConfigurationData::getInstance()->resourcePath;
	map<string, string> tmplMap = ConfigurationData::getInstance()->tmplMap;
	map<string, string> vwMap = ConfigurationData::getInstance()->vwMap;
	map<string, string> props = ConfigurationData::getInstance()->props;
	map<string, string> ajaxIntfMap = ConfigurationData::getInstance()->ajaxIntfMap;

	Logger logger = LoggerFactory::getLogger("ExtHandler");
	bool cntrlit = false;
	string content, claz;
	string acurl = req->getActUrl();
	StringUtil::replaceFirst(acurl,"//","/");
	if(acurl.length()>1)
		acurl = acurl.substr(1);
	if(acurl.find(req->getCntxt_name())!=0)
		acurl = req->getCntxt_name() + "/" + acurl;

	if(ajaxIntfMap[acurl]!="" && req->getMethod()=="POST" && req->getRequestParam("method")!="")
	{
		cntrlit = true;
		string claz = ajaxIntfMap[acurl];

		logger << "Inside Ajax Interface Execute" << endl;
		strVec vemp;
		string methName = req->getRequestParam("method");
		if(methName=="")
		{
			res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
		}
		else
		{
			string temp = req->getRequestParam("paramsize");
			int paramSize = 0;
			if(temp!="")
			{
				try {
					paramSize = CastUtil::lexical_cast<int>(temp.c_str());
				} catch(...) {
					res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
					paramSize = -1;
				}
			}
			if(paramSize>=0)
			{
				logger << "Reading Ajax params" << endl;
				for(int i=0;i<paramSize;i++)
				{
					stringstream s;
					string ss;
					s << (i+1);
					s >> ss;
					ss = "param_" + ss;
					//logger << ss << flush;
					string tem = req->getRequestParam(ss);
					vemp.push_back(tem);
				}
				string libName = INTER_LIB_FILE;
				string funcName;
				string metn,re;
				StringUtil::replaceAll(claz, "::", "_");
				metn = req->getCntxt_name() + "invokeAjaxMethodFor"+claz+methName;
				void *mkr = dlsym(dlib, metn.c_str());
				if(mkr!=NULL)
				{
					typedef string (*Funptr2) (strVec);
					Funptr2 f2 = (Funptr2)mkr;
					logger << ("Calling method " + metn) << endl;
					re = f2(vemp);
					logger << "Completed method call" << endl;
					res.setHTTPResponseStatus(HTTPResponseStatus::Ok);
					res.addHeaderValue(HttpResponse::ContentType, "text/plain");
					res.setContent(re);
				}
				else
				{
					res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				}
			}
			else
			{
				res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			}
		}

		//AfcUtil::execute(*req, &res, ajaxIntfMap[acurl]);
	}
#ifdef INC_DCP
	else if(ext==".dcp")
	{
		string libName = INTER_LIB_FILE;
		if(ddlib != NULL)
		{
			cntrlit = true;
			int s = req->getUrl().find_last_of("/")+1;
			int en = req->getUrl().find_last_of(".");
			string meth,file;
			file = req->getUrl().substr(s,en-s);
			meth = "_" + req->getCntxt_name() + file + "emittHTML";

			void *mkr = dlsym(ddlib, meth.c_str());
			if(mkr!=NULL)
			{
				//logger << endl << "inside dcp " << meth << endl;
				DCPPtr f =  (DCPPtr)mkr;
				content = f();
				//string patf;
				//patf = req->getCntxt_root() + "/dcp_" + file + ".html";
				//content = getContentStr(patf,lprops[req->getDefaultLocale()],ext);
				//delete mkr;
			}
			else
			{
				logger << ("No dcp found for " + meth) << endl;
			}
			ext = ".html";
			if(ext!="" && content.length()==0)
			{
				res.setHTTPResponseStatus(HTTPResponseStatus::NotFound);
				//res.setContent_len("0");
			}
			else
			{
				res.setHTTPResponseStatus(HTTPResponseStatus::Ok);
				res.addHeaderValue(HttpResponse::ContentType, props[ext]);
				res.setContent(content);
				//res.setContent_len(CastUtil::lexical_cast<string>(content.length()));
			}
		}
	}
#endif
#ifdef INC_DVIEW
	else if(ext==".view" && vwMap[req->getCntxt_name()+req->getFile()]!="")
	{
		cntrlit = true;

		void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("dview_"+req->getCntxt_name()+vwMap[req->getCntxt_name()+req->getFile()], req->getCntxt_name());
		if(_temp!=NULL)
		{
			DynamicView *thrd = (DynamicView*)_temp;
			Document doc = thrd->getDocument();
			View view;
			string t = view.generateDocument(doc);
			content = t;
		}
		else
		{
			logger << "Invalid Dynamic View handler" << endl;
		}

		ext = ".html";
		if(ext!="" && (content.length()==0))
		{
			res.setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			//res.setContent_len("0");
		}
		else
		{
			res.setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res.addHeaderValue(HttpResponse::ContentType, props[ext]);
			res.setContent(content);
			//res.setContent_len(CastUtil::lexical_cast<string>(content.length()));
		}
	}
#endif
#ifdef INC_TPE
	else if(ext==".tpe" && tmplMap[req->getCntxt_name()+req->getFile()]!="")
	{
		ext = ".html";
		if(ddlib != NULL)
		{

			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("template_"+req->getCntxt_name()+tmplMap[req->getCntxt_name()+req->getFile()], req->getCntxt_name());
			if(_temp!=NULL)
			{
				TemplateHandler *thrd = (TemplateHandler*)_temp;
				if(thrd!=NULL)
				{
					Context cnt = thrd->getContext();

					logger << "Done with Template Context fetch" << endl;
					map<string, void*> args;
					Context::iterator it;
					for (it=cnt.begin();it!=cnt.end();it++) {
						string key = it->first;
						Object o = it->second;
						logger << ("Template key=" + key + " Value = ") << o.getVoidPointer()<< endl;
						args[key] = o.getVoidPointer();
					}

					int s = req->getUrl().find_last_of("/")+1;
					int en = req->getUrl().find_last_of(".");
					string meth,file;
					file = req->getUrl().substr(s,en-s);
					meth = "_" + req->getCntxt_name() + file + "emittTemplateHTML";

					void* mkr = dlsym(ddlib, meth.c_str());
					if(mkr!=NULL)
					{
						//logger << endl << "inside dcp " << meth << endl;
						TemplatePtr f =  (TemplatePtr)mkr;
						content = f(args);

						for (it=cnt.begin();it!=cnt.end();it++) {
							Object o = it->second;
							delete o.getVoidPointer();
						}
						//string patf;
						//patf = req->getCntxt_root() + "/dcp_" + file + ".html";
						//content = getContentStr(patf,lprops[req->getDefaultLocale()],ext);
						//delete mkr;
					}
					else
					{
						logger << ("No template found for " + meth) << endl;
					}
				}
				else
				{
					logger << "Invalid TemplateHandler" << endl;
				}
			}
			if(ext!="" && (content.length()==0))
			{
				res.setHTTPResponseStatus(HTTPResponseStatus::NotFound);
				//res.setContent_len("0");
			}
			else
			{
				res.setHTTPResponseStatus(HTTPResponseStatus::Ok);
				res.addHeaderValue(HttpResponse::ContentType, props[ext]);
				res.setContent(content);
				//res.setContent_len(CastUtil::lexical_cast<string>(content.length()));
			}
		}
	}
#endif
	return cntrlit;
}
