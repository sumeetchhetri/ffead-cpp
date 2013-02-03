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
 * ControllerHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "ControllerHandler.h"

ControllerHandler::ControllerHandler() {
	// TODO Auto-generated constructor stub

}

ControllerHandler::~ControllerHandler() {
	// TODO Auto-generated destructor stub
}

bool ControllerHandler::handle(HttpRequest* req, HttpResponse& res, map<string, string> urlpattMap, map<string, string> mappattMap, void* dlib,
		string ext, resFuncMap rstCntMap, map<string, string> mapMap, map<string, string> urlMap, string pthwofile)
{
	Logger logger = Logger::getLogger("ControllerHandler");
	string claz;
	bool isContrl = false;
	if((urlpattMap[req->getCntxt_name()+"*.*"]!="" || urlMap[req->getCntxt_name()+ext]!=""))
	{
		//logger << "Controller requested for " << req->getCntxt_name() << " name " << urlMap[req->getCntxt_name()+ext] << endl;
		if(urlpattMap[req->getCntxt_name()+"*.*"]!="")
			claz = "getReflectionCIFor" + urlpattMap[req->getCntxt_name()+"*.*"];
		else
			claz = "getReflectionCIFor" + urlMap[req->getCntxt_name()+ext];
		string libName = Constants::INTER_LIB_FILE;
		if(dlib == NULL)
		{
			cerr << dlerror() << endl;
			exit(-1);
		}
		void *mkr = dlsym(dlib, claz.c_str());
		if(mkr!=NULL)
		{
			FunPtr f =  (FunPtr)mkr;
			ClassInfo srv = f();
			args argus;
			Constructor ctor = srv.getConstructor(argus);
			Reflector ref;
			void *_temp = ref.newInstanceGVP(ctor);
			Controller *thrd = (Controller *)_temp;
			try{
				 logger << "Controller called" << endl;
				 res = thrd->service(*req);
				 logger << res.getStatusCode() << endl;
				 logger << res.getContent_type() << endl;
				 logger << res.getContent_len() << endl;
				 if(res.getStatusCode()!="")
					 isContrl = true;
				 ext = AuthHandler::getFileExtension(req->getUrl());
				 //delete mkr;
			}catch(...){ logger << "Controller exception" << endl;}
			logger << "Controller called\n" << flush;
		}
	}
	else if((mappattMap[req->getCntxt_name()+"*.*"]!="" || mapMap[req->getCntxt_name()+ext]!=""))
	{
		string file = req->getFile();
		string fili = file.substr(0,file.find_last_of("."));
		if(mappattMap[req->getCntxt_name()+"*.*"]!="")
		{
			req->setFile(fili+mappattMap[req->getCntxt_name()+"*.*"]);
			logger << "URL mapped from * to " << mappattMap[req->getCntxt_name()+"*.*"] << endl;
		}
		else
		{
			req->setFile(fili+mapMap[req->getCntxt_name()+ext]);
			logger << "URL mapped from " << ext << " to " << mapMap[req->getCntxt_name()+ext] << endl;
		}
	}
	else
	{
		resFuncMap::iterator it;
		RestFunction rft;
		bool flag = false;
		int prsiz = 0;
		vector<string> valss;
		//logger << pthwofile << endl;
		for (it=rstCntMap.begin();it!=rstCntMap.end();it++)
		{
			valss.clear();
			//logger << it->first << endl;
			//if(pthwofile.find(it->first)!=string::npos)
			{
				RestFunction ft = it->second;
				prsiz = ft.params.size();
				string pthwofiletemp(pthwofile);
				if(ft.baseUrl=="")
				{
					logger << "checking url : " << pthwofiletemp << ",param size: " << prsiz <<
							", against url: " << it->first << endl;
					for (int var = 0; var < prsiz; var++)
					{
						//logger << "loop - " << pthwofiletemp << endl;
						string valsvv(pthwofiletemp.substr(pthwofiletemp.find_last_of("/")+1));
						pthwofiletemp = pthwofiletemp.substr(0, pthwofiletemp.find_last_of("/"));
						valss.push_back(valsvv);
					}
					reverse(valss.begin(),valss.end());
					//logger << "after - " << pthwofiletemp << endl;
					/*if(pthwofiletemp.at(pthwofiletemp.length()-1)=='/')
					{
						pthwofiletemp = pthwofiletemp.substr(0, pthwofiletemp.length()-1);
					}*/
					//logger << "after - " << pthwofiletemp << endl;
					logger << "checking url : " << pthwofiletemp << ",param size: " << prsiz << ",vals: " << valss.size() <<
							", against url: " << it->first << endl;
					if(it->first==pthwofiletemp)
					{
						string lhs = StringUtil::toUpperCopy(ft.meth);
						string rhs = StringUtil::toUpperCopy(req->getMethod());
						logger << lhs << " <> " << rhs << endl;
						if(prsiz==(int)valss.size() && lhs==rhs)
						{
							logger << "got correct url -- restcontroller " << endl;
							rft = ft;
							flag = true;
						}
						else
						{
							res.setStatusCode("404");
							res.setStatusMsg("Not Found");
							//res.setContent_type("text/plain");
							/*if(prsiz==valss.size())
								res.setContent_str("Invalid number of arguments");
							else
								res.setContent_str("Invalid HTTPMethod used");*/
							logger << "Rest Controller Param/Method Error" << endl;
						}
						break;
					}
				}
				else
				{
					string baseUrl(req->getCntxt_name()+ft.baseUrl);
					logger << "checking url : " << pthwofiletemp << ",param size: " << prsiz <<
							", against url: " << baseUrl << endl;
					for (int var = 1; var <= prsiz; var++)
					{
						strVec vemp;
						stringstream ss;
						ss << "{";
						ss << var;
						ss << "}";
						string param;
						ss >> param;
						StringUtil::split(vemp, baseUrl, (param));
						if(vemp.size()==2 && pthwofiletemp.find(vemp.at(0))!=string::npos)
						{
							string temp = pthwofiletemp;
							StringUtil::replaceFirst(temp, vemp.at(0), "");
							if(temp.find("/")!=string::npos)
							{
								pthwofiletemp = temp.substr(temp.find("/"));
								temp = temp.substr(0, temp.find("/"));
							}
							valss.push_back(temp);
							baseUrl = vemp.at(1);
							logger << "variable at " << param << " mapped to " << temp << " from URL" << endl;
							logger << baseUrl << endl;
							logger << pthwofiletemp << endl;
						}
						else
						{
							flag = false;
							break;
						}
					}
					string lhs = StringUtil::toUpperCopy(ft.meth);
					string rhs = StringUtil::toUpperCopy(req->getMethod());
					logger << lhs << " <> " << rhs << endl;
					if(prsiz==(int)valss.size() && lhs==rhs)
					{

						logger << "got correct url -- restcontroller " << endl;
						rft = ft;
						flag = true;
						break;
					}
					else
					{
						res.setStatusCode("404");
						res.setStatusMsg("Not Found");
						//res.setContent_type("text/plain");
						/*if(prsiz==valss.size())
							res.setContent_str("Invalid number of arguments");
						else
							res.setContent_str("Invalid HTTPMethod used");*/
						logger << "Rest Controller Param/Method Error" << endl;
					}
				}
			}
		}
		if(flag)
		{
			//logger << "inside restcontroller logic ..." << endl;
			string libName = Constants::INTER_LIB_FILE;
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string clasnam("getReflectionCIFor"+rft.clas);
			void *mkr = dlsym(dlib, clasnam.c_str());
			logger << mkr << endl;
			if(mkr!=NULL)
			{
				FunPtr f =  (FunPtr)mkr;
				ClassInfo srv = f();
				args argus;
				Constructor ctor = srv.getConstructor(argus);
				Reflector ref;
				void *_temp = ref.newInstanceGVP(ctor);
				RestController* rstcnt = (RestController*)_temp;
				rstcnt->request = req;
				rstcnt->response = &res;

				vals valus;
				bool invValue = false;
				for (int var = 0; var < prsiz; var++)
				{
					try
					{
						argus.push_back(rft.params.at(var).type);
						if(rft.params.at(var).type=="int")
						{
							int* ival = new int(CastUtil::lexical_cast<int>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="long")
						{
							long* ival = new long(CastUtil::lexical_cast<long>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="double")
						{
							double* ival = new double(CastUtil::lexical_cast<double>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="float")
						{
							float* ival = new float(CastUtil::lexical_cast<float>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="bool")
						{
							bool* ival = new bool(CastUtil::lexical_cast<bool>(valss.at(var)));
							valus.push_back(ival);
						}
						else if(rft.params.at(var).type=="string")
						{
							string* sval = new string(valss.at(var));
							valus.push_back(sval);
						}
					} catch (...) {
						invValue= true;
						break;
					}
				}

				Method meth = srv.getMethod(rft.name, argus);
				if(meth.getMethodName()!="" && !invValue)
				{
					ref.invokeMethodUnknownReturn(_temp,meth,valus);
					logger << "successfully called restcontroller" << endl;
					//return;
				}
				else
				{
					res.setStatusCode("404");
					res.setStatusMsg("Not Found");
					//res.setContent_type("text/plain");
					/*if(invValue)
						res.setContent_str("Invalid value passed as URL param");
					else
						res.setContent_str("Rest Controller Method Not Found");*/
					logger << "Rest Controller Method Not Found" << endl;
					//return;
				}
			}
		}
	}
	return isContrl;
}
