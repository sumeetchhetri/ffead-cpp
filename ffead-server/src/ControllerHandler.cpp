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

bool ControllerHandler::handle(HttpRequest* req, HttpResponse& res, string ext, string pthwofile)
{
	map<string, string> urlpattMap = ConfigurationData::getInstance()->urlpattMap;
	map<string, string> mappattMap = ConfigurationData::getInstance()->mappattMap;
	resFuncMap rstCntMap = ConfigurationData::getInstance()->rstCntMap;
	map<string, string> mapMap = ConfigurationData::getInstance()->mapMap;
	map<string, string> urlMap = ConfigurationData::getInstance()->urlMap;

	Logger logger = LoggerFactory::getLogger("ControllerHandler");
	bool isContrl = false;
	if((urlpattMap[req->getCntxt_name()+"*.*"]!="" || urlMap[req->getCntxt_name()+ext]!=""))
	{
		//logger << "Controller requested for " << req->getCntxt_name() << " name " << urlMap[req->getCntxt_name()+ext] << endl;
		string controller;
		if(urlpattMap[req->getCntxt_name()+"*.*"]!="")
			controller = urlpattMap[req->getCntxt_name()+"*.*"];
		else
			controller = urlMap[req->getCntxt_name()+ext];

		void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("controller_"+req->getCntxt_name()+controller, req->getCntxt_name());
		Controller* thrd = (Controller*)_temp;
		if(thrd!=NULL)
		{
			try{
				 logger << ("Controller " + controller + " called") << endl;
				 res = thrd->service(*req);
				 if(res.getStatusCode()!="")
					 isContrl = true;
				 ext = AuthHandler::getFileExtension(req->getUrl());
				 //delete mkr;
			}catch(...){
				logger << "Controller Exception occurred" << endl;
			}
			logger << "Controller call complete" << endl;
		}
		else
		{
			logger << "Invalid Controller" << endl;
			res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			isContrl = true;
		}

	}
	else if((mappattMap[req->getCntxt_name()+"*.*"]!="" || mapMap[req->getCntxt_name()+ext]!=""))
	{
		string file = req->getFile();
		string fili = file.substr(0,file.find_last_of("."));
		if(mappattMap[req->getCntxt_name()+"*.*"]!="")
		{
			req->setFile(fili+mappattMap[req->getCntxt_name()+"*.*"]);
			logger << ("URL mapped from * to " + mappattMap[req->getCntxt_name()+"*.*"]) << endl;
		}
		else
		{
			req->setFile(fili+mapMap[req->getCntxt_name()+ext]);
			logger << ("URL mapped from " + ext + " to " + mapMap[req->getCntxt_name()+ext]) << endl;
		}
	}
	else
	{
		resFuncMap::iterator it;
		RestFunction rft;
		bool flag = false;
		int prsiz = 0;
		vector<string> valss;
		map<string, string> mapOfValues;
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

				string baseUrl(it->first);
				strVec resturlparts;
				StringUtil::split(resturlparts, baseUrl, "/");

				strVec urlparts;
				StringUtil::split(urlparts, pthwofiletemp, "/");

				if(urlparts.size()!=resturlparts.size())
				{
					flag = false;
					//break;
				}
				else
				{
					flag = true;
				}
				if(flag)
				{
					bool fflag = true;
					for (int var = 0; var < (int)resturlparts.size(); var++)
					{
						//logger << "resturlparts.at(var) = " << resturlparts.at(var) << endl;
						if(resturlparts.at(var).find("{")!=string::npos && resturlparts.at(var).find("}")!=string::npos
								&& resturlparts.at(var).length()>2)
						{
							string paramname = resturlparts.at(var);
							string pref, suff;
							int st = paramname.find("{")+1;
							pref = paramname.substr(0, st-1);
							int len = paramname.find("}") - st;
							suff = paramname.substr(paramname.find("}")+1);
							paramname = paramname.substr(st, len);
							string paramvalue = urlparts.at(var);
							if(st>1)
							{
								int stpre = paramvalue.find(pref) + pref.length();
								int len = paramvalue.length() - pref.length() - suff.length();
								paramvalue = paramvalue.substr(stpre, len);
							}
							mapOfValues[paramname] = paramvalue;
							//logger << "mapOfValues(" << paramname << ") = "<< paramvalue << endl;
							logger << ("Restcontroller matched url : " + pthwofiletemp + ",param size: " + CastUtil::lexical_cast<string>(prsiz) +
										", against url: " + baseUrl) << endl;
						}
						else if(urlparts.at(var)!=resturlparts.at(var))
						{
							fflag = false;
							break;
						}
					}
					flag = fflag;
				}

				string lhs = StringUtil::toUpperCopy(ft.meth);
				string rhs = StringUtil::toUpperCopy(req->getMethod());
				//if(prsiz==(int)valss.size() && lhs==rhs)
				if(flag && lhs==rhs)
				{

					logger << "Encountered rest controller url/method match" << endl;
					rft = ft;
					flag = true;
					break;
				}
				else if(flag)
				{
					res.setHTTPResponseStatus(HTTPResponseStatus::InvalidMethod);
					return true;
				}
				else
				{
					res.setHTTPResponseStatus(HTTPResponseStatus::NotFound);
					//res.addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
					/*if(prsiz==valss.size())
						res.setContent("Invalid number of arguments");
					else
						res.setContent("Invalid HTTPMethod used");*/
					//logger << "Rest Controller Param/Method Error" << endl;
				}
			}
		}
		if(flag)
		{
			//logger << "inside restcontroller logic ..." << endl;
			Reflector ref;
			ClassInfo srv = ref.getClassInfo(rft.clas, req->getCntxt_name());
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("restcontroller_"+req->getCntxt_name()+rft.clas, req->getCntxt_name());
			RestController* rstcnt = (RestController*)_temp;
			if(rstcnt==NULL)
			{
				logger << "Invalid Rest Controller" << endl;
				res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				return true;
			}

			rstcnt->request = req;
			rstcnt->response = &res;

			args argus;
			vals valus;
			bool invValue = false;
			vector<ifstream*> allStreams;
			map<string, vector<ifstream*>* > mpvecstreams;
			for (int var = 0; var < prsiz; var++)
			{
				try
				{
					string icont = rft.icontentType;
					string ocont = rft.ocontentType;

					if(icont=="")
						icont = ContentTypes::CONTENT_TYPE_APPLICATION_JSON;
					else if(icont!=req->getHeader(HttpRequest::ContentType) && req->getHeader(HttpRequest::ContentType).find(icont)!=0)
					{
						res.setHTTPResponseStatus(HTTPResponseStatus::UnsupportedMedia);
						return true;
					}

					if(ocont=="")
						ocont = ContentTypes::CONTENT_TYPE_APPLICATION_JSON;

					req->addHeaderValue(HttpRequest::ContentType, icont);
					res.addHeaderValue(HttpResponse::ContentType, ocont);

					string pmvalue;
					if(rft.params.at(var).from=="path")
						pmvalue = mapOfValues[rft.params.at(var).name];
					else if(rft.params.at(var).from=="reqparam")
						pmvalue = req->getQueryParam(rft.params.at(var).name);
					else if(rft.params.at(var).from=="postparam")
						pmvalue = req->getRequestParam(rft.params.at(var).name);
					else if(rft.params.at(var).from=="header")
						pmvalue = req->getHeader(rft.params.at(var).name);
					else if(rft.params.at(var).from=="multipart-content")
					{
						MultipartContent mcont = req->getMultipartContent(rft.params.at(var).name);
						if(mcont.isValid())
						{
							if(mcont.isAFile())
							{
								if(rft.params.at(var).type=="filestream")
								{
									pmvalue = rft.params.at(var).name;
								}
								else if(rft.params.at(var).type!="vector-of-filestream")
								{
									logger << "File can only be mapped to ifstream" << endl;
									res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
									return true;
								}
							}
							else
							{
								pmvalue = mcont.getContent();
							}
						}
						else if(rft.params.at(var).type!="vector-of-filestream")
						{
							logger << "Invalid mapping specified in config, no multipart content found with name " + rft.params.at(var).name << endl;
							res.setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
							return true;
						}
					}
					else
					{
						if(prsiz>1)
						{
							logger << "Request Body cannot be mapped to more than one argument..." << endl;
							res.setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
							return true;
						}
						pmvalue = req->getContent();
					}

					logger << ("Restcontroller parameter type/value = "  + rft.params.at(var).type + "/" + pmvalue) << endl;
					logger << ("Restcontroller content types input/output = " + icont + "/" + ocont) << endl;

					if(rft.params.at(var).type=="int")
					{
						argus.push_back(rft.params.at(var).type);
						int* ival = new int(CastUtil::lexical_cast<int>(pmvalue));
						valus.push_back(ival);
					}
					else if(rft.params.at(var).type=="short")
					{
						argus.push_back(rft.params.at(var).type);
						short* ival = new short(CastUtil::lexical_cast<short>(pmvalue));
						valus.push_back(ival);
					}
					else if(rft.params.at(var).type=="long")
					{
						argus.push_back(rft.params.at(var).type);
						long* ival = new long(CastUtil::lexical_cast<long>(pmvalue));
						valus.push_back(ival);
					}
					else if(rft.params.at(var).type=="double")
					{
						argus.push_back(rft.params.at(var).type);
						double* ival = new double(CastUtil::lexical_cast<double>(pmvalue));
						valus.push_back(ival);
					}
					else if(rft.params.at(var).type=="float")
					{
						argus.push_back(rft.params.at(var).type);
						float* ival = new float(CastUtil::lexical_cast<float>(pmvalue));
						valus.push_back(ival);
					}
					else if(rft.params.at(var).type=="bool")
					{
						argus.push_back(rft.params.at(var).type);
						bool* ival = new bool(CastUtil::lexical_cast<bool>(pmvalue));
						valus.push_back(ival);
					}
					else if(rft.params.at(var).type=="string" || rft.params.at(var).type=="std::string")
					{
						argus.push_back(rft.params.at(var).type);
						string* sval = new string(pmvalue);
						valus.push_back(sval);
					}
					else if(rft.params.at(var).type=="filestream")
					{
						argus.push_back("ifstream*");
						MultipartContent mcont = req->getMultipartContent(pmvalue);
						if(mcont.isValid() && mcont.isAFile())
						{
							ifstream* ifs = new ifstream;
							ifs->open(mcont.getTempFileName().c_str());
							valus.push_back(ifs);
							allStreams.push_back(ifs);
						}
					}
					else if(rft.params.at(var).type=="vector-of-filestream")
					{
						vector<ifstream*> *vifs = NULL;
						if(mpvecstreams.find(rft.params.at(var).name)==mpvecstreams.end())
						{
							argus.push_back("vector<ifstream*>");
							vifs = new vector<ifstream*>;
							vector<MultipartContent> mcontvec = req->getMultiPartFileList(rft.params.at(var).name);
							for(int mci=0;mci<(int)mcontvec.size();mci++) {
								MultipartContent mcont = mcontvec.at(mci);
								if(mcont.isValid() && mcont.isAFile())
								{
									ifstream* ifs = new ifstream;
									ifs->open(mcont.getTempFileName().c_str());
									vifs->push_back(ifs);
									allStreams.push_back(ifs);
								}
							}
							mpvecstreams[rft.params.at(var).name] = vifs;
						}
						else
						{
							vifs = mpvecstreams[rft.params.at(var).name];
						}
						valus.push_back(vifs);
					}
					else if(rft.params.at(var).type.find("vector-of-")==0 || rft.params.at(var).type.find("list-of-")==0
							|| rft.params.at(var).type.find("deque-of-")==0 || rft.params.at(var).type.find("set-of-")==0
							|| rft.params.at(var).type.find("multiset-of-")==0 || rft.params.at(var).type.find("queue-of-")==0)
					{
						string stlcnt = rft.params.at(var).type;
						string stltype;
						string typp;
						if(rft.params.at(var).type.find("vector-of-")==0)
						{
							StringUtil::replaceFirst(stlcnt,"vector-of-","");
							stltype = "std::vector";
							typp = "vector<" + stlcnt + ">";
						}
						else if(rft.params.at(var).type.find("list-of-")==0)
						{
							StringUtil::replaceFirst(stlcnt,"list-of-","");
							stltype = "std::list";
							typp = "list<" + stlcnt + ">";
						}
						else if(rft.params.at(var).type.find("deque-of-")==0)
						{
							StringUtil::replaceFirst(stlcnt,"deque-of-","");
							stltype = "std::deque";
							typp = "deque<" + stlcnt + ">";
						}
						else if(rft.params.at(var).type.find("set-of-")==0)
						{
							StringUtil::replaceFirst(stlcnt,"set-of-","");
							stltype = "std::set";
							typp = "set<" + stlcnt + ">";
						}
						else if(rft.params.at(var).type.find("multiset-of-")==0)
						{
							StringUtil::replaceFirst(stlcnt,"multiset-of-","");
							stltype = "std::multiset";
							typp = "multiset<" + stlcnt + ">";
						}
						else if(rft.params.at(var).type.find("queue-of-")==0)
						{
							StringUtil::replaceFirst(stlcnt,"queue-of-","");
							stltype = "std::queue";
							typp = "queue<" + stlcnt + ">";
						}
						StringUtil::replaceFirst(stlcnt," ","");
						logger << ("Restcontroller param body holds "+stltype+" of type "  + stlcnt) << endl;

						argus.push_back(typp);
						void* voidPvect = NULL;
						if(icont==ContentTypes::CONTENT_TYPE_APPLICATION_JSON)
						{
							voidPvect = JSONSerialize::unSerializeUnknown(pmvalue, stltype+"<"+stlcnt+">",req->getCntxt_name());
						}
#ifdef INC_XMLSER
						else
						{
							voidPvect = XMLSerialize::unSerializeUnknown(pmvalue, stltype+"<"+stlcnt+",",req->getCntxt_name());
						}
#endif
						if(voidPvect==NULL)
						{
							res.setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
							return true;
						}
						valus.push_back(voidPvect);
					}
					else
					{
						argus.push_back(rft.params.at(var).type);
						void* voidPvect = NULL;
						if(icont==ContentTypes::CONTENT_TYPE_APPLICATION_JSON)
						{
							voidPvect = JSONSerialize::unSerializeUnknown(pmvalue, rft.params.at(var).type,req->getCntxt_name());
						}
#ifdef INC_XMLSER
						else
						{
							voidPvect = XMLSerialize::unSerializeUnknown(pmvalue, rft.params.at(var).type,req->getCntxt_name());
						}
#endif
						if(voidPvect==NULL)
						{
							res.setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
							return true;
						}
						valus.push_back(voidPvect);
					}
				} catch (const char* ex) {
					logger << "Restcontroller exception occurred" << endl;
					logger << ex << endl;
					invValue= true;
					res.setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
					return true;
				} catch (...) {
					logger << "Restcontroller exception occurred" << endl;
					invValue= true;
					res.setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
					return true;
				}
			}

			Method meth = srv.getMethod(rft.name, argus);
			if(meth.getMethodName()!="" && !invValue)
			{
				ref.invokeMethodUnknownReturn(_temp,meth,valus);
				logger << "Successfully called restcontroller" << endl;
				//return;

				for(int i=0;i<(int)allStreams.size();++i) {
					if(allStreams.at(i)!=NULL) {
						if(allStreams.at(i)->is_open()) {
							allStreams.at(i)->close();
							allStreams.at(i)->clear();
						}
						delete allStreams.at(i);
					}
				}

				map<string, vector<ifstream*>* >::iterator it;
				for(it=mpvecstreams.begin();it!=mpvecstreams.end();++it) {
					delete it->second;
				}
			}
			else
			{
				res.setHTTPResponseStatus(HTTPResponseStatus::NotFound);
				//res.addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
				logger << "Rest Controller Method Not Found" << endl;
				//return;
			}
		}
	}
	return isContrl;
}
