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

bool ControllerHandler::getControllerForPath(const string& cntxtName, const string& actUrl, string& className)
{
	map<string, map<string, string> >& controllerObjectMap = ConfigurationData::getInstance()->controllerObjectMap;
	if(controllerObjectMap.find(cntxtName)!=controllerObjectMap.end())
	{
		map<string, string>& controllerMap = controllerObjectMap[cntxtName];
		map<string, string>::iterator it;
		for (it=controllerMap.begin();it!=controllerMap.end();++it) {
			if(ConfigurationData::urlMatchesPath(cntxtName, it->first, actUrl))
			{
				className = it->second;
				return true;
			}
		}
	}
	return false;
}

bool ControllerHandler::getMappingForPath(const string& cntxtName, const string& actUrl, string& to)
{
	map<string, map<string, string> >& mappingObjectMap = ConfigurationData::getInstance()->mappingObjectMap;
	if(mappingObjectMap.find(cntxtName)!=mappingObjectMap.end())
	{
		map<string, string>& mappingMap = mappingObjectMap[cntxtName];
		map<string, string>::iterator it;
		for (it=mappingMap.begin();it!=mappingMap.end();++it) {
			if(ConfigurationData::urlMatchesPath(cntxtName, it->first, actUrl))
			{
				to = it->second;
				return true;
			}
		}
	}
	return false;
}

bool ControllerHandler::handle(HttpRequest* req, HttpResponse* res, const string& ext, const string& pthwofile, Reflector& reflector)
{
	string acurl = req->getActUrl();
	RegexUtil::replace(acurl,"[/]+","/");
	if(acurl.find("/"+req->getCntxt_name())!=0)
		acurl = "/" + req->getCntxt_name() + "/" + acurl;
	RegexUtil::replace(acurl,"[/]+","/");

	Logger logger = LoggerFactory::getLogger("ControllerHandler");
	bool isContrl = false;
	string controller;
	string to;
	string extwodot = ext.substr(1);
	if(getControllerForPath(req->getCntxt_name(), acurl, controller))
	{
		void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("controller_"+controller, req->getCntxt_name());
		args argus;
		argus.push_back("HttpRequest*");
		argus.push_back("HttpResponse*");
		vals valus;
		const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][controller];
		const Method& meth = srv.getMethod("service", argus);
		if(meth.getMethodName()!="")
		{
			 valus.push_back(req);
			 valus.push_back(res);
			 logger << ("Controller " + controller + " called") << endl;
			 bool isDone = reflector.invokeMethod<bool>(_temp,meth,valus);
			 if(isDone && res->getStatusCode()!="")
				 isContrl = true;
			 logger << "Controller call complete" << endl;
		}
		else
		{
			logger << "Invalid Controller" << endl;
			res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			isContrl = true;
		}
	}
	else if(getMappingForPath(req->getCntxt_name(), acurl, to))
	{
		if(!StringUtil::endsWith(acurl, "/"+req->getFile()))
		{
			req->setUrl(to);
			req->setActUrl(to+"/"+req->getFile());
		}
		else
		{
			string topath = to.substr(0, to.find_last_of("/"));
			req->setUrl(topath);
			req->setActUrl(to);
		}
		req->setFile(to);
		logger << ("URL mapped from " + ext + " to " + to) << endl;
	}
	else if(ConfigurationData::getInstance()->mappingextObjectMap.find(req->getCntxt_name())!=ConfigurationData::getInstance()->mappingextObjectMap.end()
			&& ConfigurationData::getInstance()->mappingextObjectMap[req->getCntxt_name()].find(extwodot)!=ConfigurationData::getInstance()->mappingextObjectMap[req->getCntxt_name()].end())
	{
		string file = req->getFile();
		string fili = file.substr(0,file.find_last_of(".")+1);
		req->setFile(fili+ConfigurationData::getInstance()->mappingextObjectMap[req->getCntxt_name()][extwodot]);
		logger << ("URL extension mapped from " + extwodot + " to " + ConfigurationData::getInstance()->mappingextObjectMap[req->getCntxt_name()][extwodot]) << endl;
	}
	else if(ConfigurationData::getInstance()->rstCntMap.find(req->getCntxt_name())!=ConfigurationData::getInstance()->rstCntMap.end())
	{
		resFuncMap rstCntMap = ConfigurationData::getInstance()->rstCntMap[req->getCntxt_name()];
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
			vector<RestFunction> fts = it->second;
			for(int rftc=0;rftc<fts.size();rftc++)
			{
				RestFunction ft = fts.at(rftc);
				prsiz = ft.params.size();
				string pthwofiletemp(acurl);

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
					flag = false;
					res->setHTTPResponseStatus(HTTPResponseStatus::InvalidMethod);
					res->setDone(true);
					//return true;
					break;
				}
				else
				{
					res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
					//res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
					/*if(prsiz==valss.size())
						res->setContent("Invalid number of arguments");
					else
						res->setContent("Invalid HTTPMethod used");*/
					//logger << "Rest Controller Param/Method Error" << endl;
				}
			}
			if(flag || res->isDone()) {
				break;
			}
		}
		if(flag)
		{
			args argus;
			vals valus;

			//logger << "inside restcontroller logic ..." << endl;
			const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][rft.clas];
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("restcontroller_"+rft.clas, req->getCntxt_name());
			if(_temp==NULL){
				throw "Rest Controller Not Found";
			}

			bool invValue = false;
			vector<ifstream*> allStreams;
			map<string, vector<ifstream*>* > mpvecstreams;

			string icont = rft.icontentType;
			string ocont = rft.ocontentType;

			if(icont=="")
				icont = ContentTypes::CONTENT_TYPE_APPLICATION_JSON;
			else if(icont!=req->getHeader(HttpRequest::ContentType) && req->getHeader(HttpRequest::ContentType).find(icont)!=0)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::UnsupportedMedia);
				res->setDone(true);
				delete _temp;
				return true;
			}

			if(ocont=="")
				ocont = ContentTypes::CONTENT_TYPE_APPLICATION_JSON;

			req->addHeaderValue(HttpRequest::ContentType, icont);

			for (int var = 0; var < prsiz; var++)
			{
				try
				{
					string pmvalue;
					if(rft.params.at(var).from=="path")
						pmvalue = mapOfValues[rft.params.at(var).name];
					else if(rft.params.at(var).from=="reqparam")
					{
						if(req->getQueryParams().find(rft.params.at(var).name)!=req->getQueryParams().end())
							pmvalue = req->getQueryParam(rft.params.at(var).name);
						else
							pmvalue = rft.params.at(var).defValue;
					}
					else if(rft.params.at(var).from=="postparam")
					{
						if(req->getRequestParams().find(rft.params.at(var).name)!=req->getRequestParams().end())
							pmvalue = req->getRequestParam(rft.params.at(var).name);
						else
							pmvalue = rft.params.at(var).defValue;
					}
					else if(rft.params.at(var).from=="header")
					{
						if(req->getHeaders().find(rft.params.at(var).name)!=req->getHeaders().end())
							pmvalue = req->getHeader(rft.params.at(var).name);
						else
							pmvalue = rft.params.at(var).defValue;
					}
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
									res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
									delete _temp;
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
							res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
							delete _temp;
							return true;
						}
					}
					else
					{
						if(prsiz>1)
						{
							logger << "Request Body cannot be mapped to more than one argument..." << endl;
							res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
							delete _temp;
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
							voidPvect = JSONSerialize::unSerializeUnknown(pmvalue, stltype+"<"+stlcnt+",",req->getCntxt_name());
						}
#ifdef INC_XMLSER
						else
						{
							voidPvect = XMLSerialize::unSerializeUnknown(pmvalue, stltype+"<"+stlcnt+",",req->getCntxt_name());
						}
#endif
						if(voidPvect==NULL)
						{
							res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
							delete _temp;
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
							res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
							delete _temp;
							return true;
						}
						valus.push_back(voidPvect);
					}
				} catch (const char* ex) {
					logger << "Restcontroller exception occurred" << endl;
					logger << ex << endl;
					invValue= true;
					res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
					delete _temp;
					return true;
				} catch (...) {
					logger << "Restcontroller exception occurred" << endl;
					invValue= true;
					res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
					delete _temp;
					return true;
				}
			}

			try
			{
				const Method& meth = srv.getMethod(rft.name, argus);
				if(meth.getMethodName()!="" && !invValue)
				{
					string outRetType = meth.getReturnType();
					void* ouput = reflector.invokeMethodUnknownReturn(_temp,meth,valus);
					string outcontent = "void";
					if(outRetType=="void")
					{
						if(rft.statusCode=="")
							res->setHTTPResponseStatus(HTTPResponseStatus::NoContent);
						else
							res->setHTTPResponseStatus(HTTPResponseStatus::getStatusByCode(
									CastUtil::lexical_cast<int>(rft.statusCode)));
					}
					else
					{
						if(ocont == ContentTypes::CONTENT_TYPE_APPLICATION_JSON && res->getContent()=="")
						{
							outcontent = JSONSerialize::serializeUnknown(ouput, outRetType, req->getCntxt_name());
							res->setContent(outcontent);
						}
						else if(ocont == ContentTypes::CONTENT_TYPE_APPLICATION_XML && res->getContent()=="")
						{
							outcontent = XMLSerialize::serializeUnknown(ouput, outRetType, req->getCntxt_name());
							res->setContent(outcontent);
						}
						else if(outRetType=="std::string" || outRetType=="string")
						{
							res->setContent(*(string*)ouput);
						}
						res->addHeaderValue(HttpResponse::ContentType, ocont);
						res->setHTTPResponseStatus(HTTPResponseStatus::getStatusByCode(
								CastUtil::lexical_cast<int>(rft.statusCode)));
						delete ouput;
					}
					logger << "Successfully called restcontroller output follows - " << endl;
					logger << outcontent << endl;

					isContrl = true;

					for(int i=0;i<(int)allStreams.size();++i) {
						if(allStreams.at(i)!=NULL) {
							if(allStreams.at(i)->is_open()) {
								allStreams.at(i)->close();
								allStreams.at(i)->clear();
							}
						}
					}

					map<string, vector<ifstream*>* >::iterator it;
					for(it=mpvecstreams.begin();it!=mpvecstreams.end();++it) {
						delete it->second;
					}

					for(int i=0;i<(int)valus.size();++i) {
						if(valus.at(i)!=NULL) {
							delete valus.at(i);
						}
					}
				}
				else
				{
					res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
					//res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
					logger << "Rest Controller Method Not Found" << endl;
					if(_temp!=NULL)delete _temp;
				}
			} catch (const char* ex) {
				logger << "Restcontroller exception occurred" << endl;
				logger << ex << endl;
				invValue= true;
				res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
				if(_temp!=NULL)delete _temp;
				return true;
			} catch (...) {
				logger << "Restcontroller exception occurred" << endl;
				invValue= true;
				res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
				if(_temp!=NULL)delete _temp;
				return true;
			}
		}
	}
	if(isContrl && res->getStatusCode()!="404") {
		res->setDone(true);
	}
	return isContrl;
}
