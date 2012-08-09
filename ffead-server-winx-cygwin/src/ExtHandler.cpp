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

string ExtHandler::getContentStr(string url,string locale,string ext)
{
	string all;
    string fname = url;
	if (url=="/")
    {
       return all;
    }
    ifstream myfile;
    if(locale.find("english")==string::npos && (ext==".html" || ext==".htm"))
    {
            string fnj = fname;
            boost::replace_first(fnj,".",("_" + locale+"."));
            myfile.open(fnj.c_str(),ios::in | ios::binary);
			if (myfile.is_open())
			{
				string line;
				while(getline(myfile,line)){all.append(line+"\n");}
				myfile.close();
				return all;
			}
    }
	ifstream myfile1;
    myfile1.open(fname.c_str(),ios::in | ios::binary);
    if (myfile1.is_open())
    {
		string line;
		while(getline(myfile1,line)){all.append(line+"\n");}
        myfile1.close();
    }
    return all;
}

bool ExtHandler::handle(HttpRequest* req, HttpResponse& res, void* dlib, string resourcePath,
		map<string, string> tmplMap, map<string, string> vwMap,string ext, map<string, string> props)
{
	Logger logger = Logger::getLogger("ExtHandler");
	bool cntrlit = false;
	string content, claz;
	if(req->getMethod()=="POST" && req->getRequestParam("claz")!="" && req->getRequestParam("method")!="")
	{
		cntrlit = true;
		content = AfcUtil::execute(*req);
		res.setStatusCode("200");
		res.setStatusMsg("OK");
		res.setContent_type(props[".txt"]);
		res.setContent_str(content);
		//res.setContent_len(boost::lexical_cast<string>(content.length()));
	}
	else if(ext==".dcp")
	{
		cntrlit = true;
		string libName = Constants::INTER_LIB_FILE;
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		int s = req->getUrl().find_last_of("/")+1;
		int en = req->getUrl().find_last_of(".");
		string meth,file;
		file = req->getUrl().substr(s,en-s);
		meth = "_" + file + "emittHTML";

		void *mkr = dlsym(dlib, meth.c_str());
		if(mkr!=NULL)
		{
			logger << endl << "inside dcp " << meth << endl;
			DCPPtr f =  (DCPPtr)mkr;
			content = f();
			//string patf;
			//patf = req->getCntxt_root() + "/dcp_" + file + ".html";
			//content = getContentStr(patf,lprops[req->getDefaultLocale()],ext);
			//delete mkr;
		}
		ext = ".html";
		if(ext!="" && content.length()==0)
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	else if(ext==".view" && vwMap[req->getCntxt_name()+req->getFile()]!="")
	{
		cntrlit = true;
		string libName = Constants::INTER_LIB_FILE;
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		claz = "getReflectionCIFor" + vwMap[req->getCntxt_name()+req->getFile()];
		void *mkr = dlsym(dlib, claz.c_str());
		if(mkr!=NULL)
		{
			FunPtr f =  (FunPtr)mkr;
			ClassInfo srv = f();
			args argus;
			Reflector ref;
			Constructor ctor = srv.getConstructor(argus);
			void *_temp = ref.newInstanceGVP(ctor);
			DynamicView *thrd = (DynamicView *)_temp;
			Document doc = thrd->getDocument();
			View view;
			string t = view.generateDocument(doc);
			content = t;
		}
		ext = ".html";
		if(ext!="" && (content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	else if(ext==".tpe" && tmplMap[req->getCntxt_name()+req->getFile()]!="")
	{
		cntrlit = true;
		TemplateEngine te;
		ext = ".html";
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		claz = "getReflectionCIFor" + tmplMap[req->getCntxt_name()+req->getFile()];
		void *mkr = dlsym(dlib, claz.c_str());
		if(mkr!=NULL)
		{
			FunPtr f =  (FunPtr)mkr;
			ClassInfo srv = f();
			args argus;
			Constructor ctor = srv.getConstructor(argus);
			Reflector ref;
			void *_temp = ref.newInstanceGVP(ctor);
			TemplateHandler *thrd = (TemplateHandler *)_temp;
			Context cnt = thrd->getContext();
			string t = te.evaluate(req->getUrl(),cnt);
			content = t;
		}
		if(ext!="" && (content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	else if(ext==".wsdl")
	{
		cntrlit = true;
		content = getContentStr(resourcePath+req->getFile(),"english",ext);
		if((content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[ext]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	return cntrlit;
}
