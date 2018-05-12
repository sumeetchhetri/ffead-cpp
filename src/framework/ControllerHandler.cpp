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

bool ControllerHandler::getControllerForPath(const std::string& cntxtName, const std::string& actUrl, std::string& className)
{
	std::map<std::string, std::map<std::string, std::string> >& controllerObjectMap = ConfigurationData::getInstance()->controllerObjectMap;
	std::map<std::string, std::string>& controllerMap = controllerObjectMap[cntxtName];
	std::map<std::string, std::string>::iterator it;
	for (it=controllerMap.begin();it!=controllerMap.end();++it) {
		if(ConfigurationData::urlMatchesPath(cntxtName, it->first, actUrl))
		{
			className = it->second;
			return true;
		}
	}
	return false;
}

bool ControllerHandler::getMappingForPath(const std::string& cntxtName, const std::string& actUrl, std::string& to)
{
	std::map<std::string, std::map<std::string, std::string> >& mappingObjectMap = ConfigurationData::getInstance()->mappingObjectMap;
	std::map<std::string, std::string>& mappingMap = mappingObjectMap[cntxtName];
	std::map<std::string, std::string>::iterator it;
	for (it=mappingMap.begin();it!=mappingMap.end();++it) {
		if(ConfigurationData::urlMatchesPath(cntxtName, it->first, actUrl))
		{
			to = it->second;
			return true;
		}
	}
	return false;
}

bool ControllerHandler::handle(HttpRequest* req, HttpResponse* res, const std::string& ext, Reflector& reflector)
{
	Logger logger = LoggerFactory::getLogger("ControllerHandler");
	bool isContrl = false;
	std::string controller;
	std::string to;
	std::string extwodot = ext!=""?ext.substr(1):"";
	if(getControllerForPath(req->getCntxt_name(), req->getCurl(), controller))
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
			 //logger << ("Controller " + controller + " called") << std::endl;
			 bool isDone = reflector.invokeMethod<bool>(_temp,meth,valus);
			 if(isDone && res->getStatusCode()!="")
				 isContrl = true;
			 //logger << "Controller call complete" << std::endl;
		}
		else
		{
			logger << "Invalid Controller" << std::endl;
			res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			isContrl = true;
		}
		ConfigurationData::getInstance()->ffeadContext.release(_temp, "controller_"+controller, req->getCntxt_name());
	}
	else if(getMappingForPath(req->getCntxt_name(), req->getCurl(), to))
	{
		if(!StringUtil::endsWith(req->getCurl(), "/"+req->getFile()))
		{
			req->setUrl(to);
			req->setActUrl(to+"/"+req->getFile());
		}
		else
		{
			std::string topath = to.substr(0, to.find_last_of("/"));
			req->setUrl(topath);
			req->setActUrl(to);
		}
		req->setFile(to);
		logger << ("URL mapped from " + ext + " to " + to) << std::endl;
	}
	else if(ConfigurationData::getInstance()->mappingextObjectMap.find(req->getCntxt_name())!=ConfigurationData::getInstance()->mappingextObjectMap.end()
			&& ConfigurationData::getInstance()->mappingextObjectMap[req->getCntxt_name()].find(extwodot)!=ConfigurationData::getInstance()->mappingextObjectMap[req->getCntxt_name()].end())
	{
		std::string file = req->getFile();
		std::string fili = file.substr(0,file.find_last_of(".")+1);
		req->setFile(fili+ConfigurationData::getInstance()->mappingextObjectMap[req->getCntxt_name()][extwodot]);
		logger << ("URL extension mapped from " + extwodot + " to " + ConfigurationData::getInstance()->mappingextObjectMap[req->getCntxt_name()][extwodot]) << std::endl;
	}
	else if(ConfigurationData::getInstance()->rstCntMap.find(req->getCntxt_name())!=ConfigurationData::getInstance()->rstCntMap.end())
	{
		resFuncMap& rstCntMap = ConfigurationData::getInstance()->rstCntMap[req->getCntxt_name()];
		resFuncMap::iterator it;
		RestFunction rft;
		bool flag = false;
		int prsiz = 0;
		std::map<std::string, std::string> mapOfValues;
		std::string hmeth = StringUtil::toUpperCopy(req->getMethod());
		std::string rkey = hmeth+req->getCurl();
		if(rstCntMap.find(rkey)!=rstCntMap.end()) {
			rft = rstCntMap[rkey][0];
			flag = true;
			prsiz = rft.params.size();
		}
		if(!flag)
		{
			for (it=rstCntMap.begin();it!=rstCntMap.end();it++)
			{
				std::vector<RestFunction> fts = it->second;
				for(int rftc=0;rftc<(int)fts.size();rftc++)
				{
					RestFunction ft = fts.at(rftc);
					prsiz = ft.params.size();

					std::string baseUrl(it->first);
					strVec resturlparts;
					StringUtil::split(resturlparts, baseUrl, "/");

					if(req->getActUrlParts().size()!=resturlparts.size()-1)
					{
						flag = false;
					}
					else
					{
						flag = true;
					}
					if(flag)
					{
						bool fflag = true;
						for (int var = 1; var < (int)resturlparts.size(); var++)
						{
							//logger << "resturlparts.at(var) = " << resturlparts.at(var) << std::endl;
							if(resturlparts.at(var).find("{")!=std::string::npos && resturlparts.at(var).find("}")!=std::string::npos
									&& resturlparts.at(var).length()>2)
							{
								std::string paramname = resturlparts.at(var);
								std::string pref, suff;
								int st = paramname.find("{")+1;
								pref = paramname.substr(0, st-1);
								int len = paramname.find("}") - st;
								suff = paramname.substr(paramname.find("}")+1);
								paramname = paramname.substr(st, len);
								std::string paramvalue = req->getActUrlParts().at(var-1);
								if(st>1)
								{
									int stpre = paramvalue.find(pref) + pref.length();
									int len = paramvalue.length() - pref.length() - suff.length();
									paramvalue = paramvalue.substr(stpre, len);
								}
								mapOfValues[paramname] = paramvalue;
								//logger << ("Restcontroller matched url : " + req->getCurl() + ",param size: " + CastUtil::lexical_cast<std::string>(prsiz) +
								//			", against url: " + baseUrl) << std::endl;
							}
							else if(req->getActUrlParts().at(var-1)!=resturlparts.at(var))
							{
								fflag = false;
								break;
							}
						}
						flag = fflag;
					}

					std::string lhs = StringUtil::toUpperCopy(ft.meth);
					if(flag && lhs==hmeth)
					{

						//logger << "Encountered rest controller url/method match" << std::endl;
						rft = ft;
						flag = true;
						break;
					}
					else if(flag)
					{
						flag = false;
						res->setHTTPResponseStatus(HTTPResponseStatus::InvalidMethod);
						res->setDone(true);
						break;
					}
					else
					{
						res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
					}
				}
				if(flag || res->isDone()) {
					break;
				}
			}
		}
		if(flag)
		{
			args argus;
			vals valus;

			const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][rft.clas];
			void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("restcontroller_"+rft.clas, req->getCntxt_name());
			if(_temp==NULL) {
				logger << "Rest Controller Not Found" << std::endl;
				res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				res->setDone(true);
				return true;
			}

			bool invValue = false;
			std::vector<std::ifstream*> allStreams;
			std::map<std::string, std::vector<std::ifstream*>* > mpvecstreams;

			std::string icont = rft.icontentType;
			std::string ocont = rft.ocontentType;

			if(icont=="")
				icont = ContentTypes::CONTENT_TYPE_APPLICATION_JSON;
			else if(icont!=req->getHeader(HttpRequest::ContentType) && req->getHeader(HttpRequest::ContentType).find(icont)!=0)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::UnsupportedMedia);
				res->setDone(true);
				ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
				return true;
			}

			if(ocont=="")
				ocont = ContentTypes::CONTENT_TYPE_APPLICATION_JSON;

			req->addHeaderValue(HttpRequest::ContentType, icont);

			for (int var = 0; var < prsiz; var++)
			{
				try
				{
					std::string pmvalue;
					if(rft.params.at(var).from=="path")
					{
						pmvalue = mapOfValues[rft.params.at(var).name];
					}
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
									logger << "File can only be mapped to ifstream" << std::endl;
									res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
									res->setDone(true);
									ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
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
							logger << "Invalid mapping specified in config, no multipart content found with name " + rft.params.at(var).name << std::endl;
							res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
							res->setDone(true);
							ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
							return true;
						}
					}
					else
					{
						if(prsiz>1)
						{
							logger << "Request Body cannot be mapped to more than one argument..." << std::endl;
							res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
							res->setDone(true);
							ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
							return true;
						}
						pmvalue = req->getContent();
					}

					//logger << ("Restcontroller parameter type/value = "  + rft.params.at(var).type + "/" + pmvalue) << std::endl;
					//logger << ("Restcontroller content types input/output = " + icont + "/" + ocont) << std::endl;

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
					else if(rft.params.at(var).type=="long long")
					{
						argus.push_back(rft.params.at(var).type);
						long long* ival = new long long(CastUtil::lexical_cast<long long>(pmvalue));
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
						std::string* sval = new std::string(pmvalue);
						valus.push_back(sval);
					}
					else if(rft.params.at(var).type=="filestream")
					{
						argus.push_back("ifstream*");
						MultipartContent mcont = req->getMultipartContent(pmvalue);
						if(mcont.isValid() && mcont.isAFile())
						{
							std::ifstream* ifs = new std::ifstream;
							ifs->open(mcont.getTempFileName().c_str());
							valus.push_back(ifs);
							allStreams.push_back(ifs);
						}
					}
					else if(rft.params.at(var).type=="vector-of-filestream")
					{
						std::vector<std::ifstream*> *vifs = NULL;
						if(mpvecstreams.find(rft.params.at(var).name)==mpvecstreams.end())
						{
							argus.push_back("vector<ifstream*>");
							vifs = new std::vector<std::ifstream*>;
							std::vector<MultipartContent> mcontvec = req->getMultiPartFileList(rft.params.at(var).name);
							for(int mci=0;mci<(int)mcontvec.size();mci++) {
								MultipartContent mcont = mcontvec.at(mci);
								if(mcont.isValid() && mcont.isAFile())
								{
									std::ifstream* ifs = new std::ifstream;
									ifs->open(mcont.getTempFileName().c_str());
									vifs->push_back(ifs);
									//allStreams.push_back(ifs);
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
						std::string stlcnt = rft.params.at(var).type;
						std::string stltype;
						std::string typp;
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
						logger << ("Restcontroller param body holds "+stltype+" of type "  + stlcnt) << std::endl;

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
							res->setDone(true);
							ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
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
							res->setDone(true);
							ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
							return true;
						}
						valus.push_back(voidPvect);
					}
				} catch (const char* ex) {
					logger << "Restcontroller exception occurred" << std::endl;
					logger << ex << std::endl;
					invValue= true;
					res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
					res->setDone(true);
					ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
					return true;
				} catch (...) {
					logger << "Restcontroller exception occurred" << std::endl;
					invValue= true;
					res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
					res->setDone(true);
					ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
					return true;
				}
			}

			try
			{
				const Method& meth = srv.getMethod(rft.name, argus);
				if(meth.getMethodName()!="" && !invValue)
				{
					std::string outRetType = meth.getReturnType();
					void* ouput = reflector.invokeMethodUnknownReturn(_temp,meth,valus,true);
					std::string outcontent = "void";
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
							res->setContent(*(std::string*)ouput);
						}
						res->addHeaderValue(HttpResponse::ContentType, ocont);
						res->setHTTPResponseStatus(HTTPResponseStatus::getStatusByCode(
								CastUtil::lexical_cast<int>(rft.statusCode)));
						//delete ouput;
						reflector.destroy(ouput, outRetType, req->getCntxt_name());
					}
					//logger << "Successfully called restcontroller output follows - " << std::endl;
					//logger << outcontent << std::endl;

					isContrl = true;

					for(int i=0;i<(int)allStreams.size();++i) {
						if(allStreams.at(i)!=NULL) {
							if(allStreams.at(i)->is_open()) {
								allStreams.at(i)->close();
								allStreams.at(i)->clear();
								//delete allStreams.at(i);
							}
						}
					}

					std::map<std::string, std::vector<std::ifstream*>* >::iterator it;
					for(it=mpvecstreams.begin();it!=mpvecstreams.end();++it) {
						for(int i=0;i<(int)it->second->size();++i) {
							if(it->second->at(i)!=NULL) {
								if(it->second->at(i)->is_open()) {
									it->second->at(i)->close();
									it->second->at(i)->clear();
									delete it->second->at(i);
								}
							}
						}
						it->second->clear();
					}
					mpvecstreams.clear();
				}
				else
				{
					res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
					//res->addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
					logger << "Rest Controller Method Not Found" << std::endl;
					ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
				}

				for(int i=0;i<(int)valus.size();++i) {
					if(valus.at(i)!=NULL) {
						if(argus.at(i)=="filestream")
						{
						}
						else if(argus.at(i)=="vector-of-filestream")
						{
						}
						else
						{
							//reflector.destroy(valus.at(i), argus.at(i), req->getCntxt_name());
						}
					}
				}
			} catch (const char* ex) {
				logger << "Restcontroller exception occurred" << std::endl;
				logger << ex << std::endl;
				invValue= true;
				res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				res->setDone(true);
				ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
				return true;
			} catch (...) {
				logger << "Restcontroller exception occurred" << std::endl;
				invValue= true;
				res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				res->setDone(true);
				ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft.clas, req->getCntxt_name());
				return true;
			}
		}
	}
	if(isContrl && res->getStatusCode()!="404") {
		res->setDone(true);
	}
	return isContrl;
}
