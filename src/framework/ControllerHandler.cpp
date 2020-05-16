/*
	Copyright 2009-2020, Sumeet Chhetri

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

std::string ControllerExtensionHandler::BLANK = "";
std::string ControllerHandler::BLANK = "";

bool ControllerExtensionHandler::getControllerForPath(std::string_view cntxtName, const std::string& actUrl, std::string& className)
{
	//Timer t;
	//t.start();
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> >& controllerObjectMap = ConfigurationData::getInstance()->controllerObjectMap;
	std::map<std::string, std::string, std::less<> >& controllerMap = controllerObjectMap.find(cntxtName)->second;
	std::map<std::string, std::string, std::less<> >::iterator it;
	for (it=controllerMap.begin();it!=controllerMap.end();++it) {
		if(ConfigurationData::urlMatchesPath(cntxtName, it->first, actUrl))
		{
			className = it->second;
			//t.end();
			//CommonUtils::tsContMpg += t.timerNanoSeconds();
			return true;
		}
	}
	//t.end();
	//CommonUtils::tsContMpg += t.timerNanoSeconds();
	return false;
}

bool ControllerExtensionHandler::getMappingForPath(std::string_view cntxtName, const std::string& actUrl, std::string& to)
{
	//Timer t;
	//t.start();
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> >& mappingObjectMap = ConfigurationData::getInstance()->mappingObjectMap;
	std::map<std::string, std::string, std::less<> >& mappingMap = mappingObjectMap.find(cntxtName)->second;
	std::map<std::string, std::string, std::less<> >::iterator it;
	for (it=mappingMap.begin();it!=mappingMap.end();++it) {
		if(ConfigurationData::urlMatchesPath(cntxtName, it->first, actUrl))
		{
			to = it->second;
			//t.end();
			//CommonUtils::tsContPath += t.timerNanoSeconds();
			return true;
		}
	}
	//t.end();
	//CommonUtils::tsContPath += t.timerNanoSeconds();
	return false;
}

bool ControllerExtensionHandler::hasMappingExtension(std::string extwodot, HttpRequest* req) {
	//Timer t;
	//t.start();
	bool f = ConfigurationData::getInstance()->mappingextObjectMap.find(req->getCntxt_name())!=ConfigurationData::getInstance()->mappingextObjectMap.end()
		&& ConfigurationData::getInstance()->mappingextObjectMap.find(req->getCntxt_name())->second.find(extwodot)
			!=ConfigurationData::getInstance()->mappingextObjectMap.find(req->getCntxt_name())->second.end();
	//t.end();
	//CommonUtils::tsContExt += t.timerNanoSeconds();
	return f;
}

bool ControllerExtensionHandler::handle(HttpRequest* req, HttpResponse* res, const std::string& ext, Reflector& reflector)
{
	bool isContrl = false;
	std::string controller;
	std::string to;
	std::string extwodot = ext.length()>0?ext.substr(1):BLANK;
	if(ConfigurationData::getInstance()->enableContMpg && getControllerForPath(req->getCntxt_name(), req->getCurl(), controller))
	{
		//Timer t;
		//t.start();
		void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("controller_"+controller, req->getCntxt_name());
		args argus;
		argus.push_back("HttpRequest*");
		argus.push_back("HttpResponse*");
		vals valus;
		ClassInfo* srv = ConfigurationData::getClassInfo(controller, req->getCntxt_name());
		const Method& meth = srv->getMethod("service", argus);
		if(meth.getMethodName()!="")
		{
			 valus.push_back(req);
			 valus.push_back(res);
			 //logger << ("Controller " + controller + " called") << std::endl;
			 bool isDone = false;
			 reflector.invokeMethod<bool>(&isDone,_temp,meth,valus);
			 if(isDone && res->getStatusCode()!="")
				 isContrl = true;
			 //logger << "Controller call complete" << std::endl;
		}
		else
		{
			//logger << "Invalid Controller" << std::endl;
			res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			isContrl = true;
		}
		ConfigurationData::getInstance()->ffeadContext.release(_temp, "controller_"+controller, req->getCntxt_name());
		//t.end();
		//CommonUtils::tsContExec += t.timerNanoSeconds();
	}
	else if(ConfigurationData::getInstance()->enableContPath && getMappingForPath(req->getCntxt_name(), req->getCurl(), to))
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
		//logger << ("URL mapped from " + ext + " to " + to) << std::endl;
	}
	else if(ConfigurationData::getInstance()->enableContExt && hasMappingExtension(extwodot, req))
	{
		std::string file = req->getFile();
		std::string fili = file.substr(0,file.find_last_of(".")+1);
		req->setFile(fili+ConfigurationData::getInstance()->mappingextObjectMap.find(req->getCntxt_name())->second[extwodot]);
		//logger << ("URL extension mapped from " + extwodot + " to " + ConfigurationData::getInstance()->mappingextObjectMap.find(req->getCntxt_name())->second[extwodot]) << std::endl;
	}
	if(isContrl && res->getStatusCode()!="404") {
		res->setDone(true);
	}
	return isContrl;
}

bool ControllerHandler::handle(HttpRequest* req, HttpResponse* res, const std::string& ext, Reflector& reflector)
{
	bool isContrl = false;
	//Logger logger = LoggerFactory::getLogger("ControllerHandler");
	if(ConfigurationData::getInstance()->enableContRst && ConfigurationData::getInstance()->rstCntMap.find(req->getCntxt_name())!=ConfigurationData::getInstance()->rstCntMap.end())
	{
		//Timer t;
		//t.start();

		resFuncMap& rstCntMap = ConfigurationData::getInstance()->rstCntMap.find(req->getCntxt_name())->second;
		resFuncMap::iterator it;
		RestFunction* rft = NULL;
		bool flag = false;
		int prsiz = 0;
		std::map<std::string, std::string> mapOfValues;
		std::string rkey = std::string(req->methodv)+req->getCurl();
		if(rstCntMap.find(rkey)!=rstCntMap.end()) {
			rft = &(rstCntMap[rkey][0]);
			flag = true;
			prsiz = rft->params.size();
		}
		if(!flag)
		{
			for (it=rstCntMap.begin();it!=rstCntMap.end();it++)
			{
				std::vector<RestFunction> fts = it->second;
				for(int rftc=0;rftc<(int)fts.size();rftc++)
				{
					rft = &(rstCntMap[it->first][rftc]);
					if(rft->unmapped) continue;

					prsiz = rft->params.size();

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
							}
							else if(req->getActUrlParts().at(var-1)!=resturlparts.at(var))
							{
								fflag = false;
								break;
							}
						}
						flag = fflag;
					}

					std::string lhs = StringUtil::toUpperCopy(rft->meth);
					if(flag && lhs==req->methodv)
					{

						//logger << "Encountered rest controller url/method match" << std::endl;
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
		//t.end();
		//CommonUtils::tsContRstLkp += t.timerNanoSeconds();

		if(flag)
		{
			//t.start();
			ClassInfo* srv = ConfigurationData::getInstance()->ffeadContext.contInsMap["restcontroller_"+rft->clas+rft->appName];
			//t.end();
			//CommonUtils::tsContRstCsiLkp += t.timerNanoSeconds();

			//t.start();
			void *_temp = srv->getSI();
			if(_temp==NULL) {
				_temp = ConfigurationData::getInstance()->ffeadContext.getBean("restcontroller_"+rft->clas, rft->appName);
				if(_temp==NULL) {
					//logger << "Rest Controller Not Found" << std::endl;
					res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
					res->setDone(true);
					return true;
				}
			}

			if(rft->icontentType.length()>0 && rft->icontentType!=req->getHeader(HttpRequest::ContentType) && req->getHeader(HttpRequest::ContentType).find(rft->icontentType)!=0)
			{
				res->setHTTPResponseStatus(HTTPResponseStatus::UnsupportedMedia);
				res->setDone(true);
				if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
				return true;
			}
			req->addHeader(HttpRequest::ContentType, rft->icontentType);

			//t.end();
			//CommonUtils::tsContRstInsLkp += t.timerNanoSeconds();

			//t.start();
			args argus;
			vals valus;
			bool invValue = false;
			std::vector<std::ifstream*> allStreams;
			std::map<std::string, std::vector<std::ifstream*>* > mpvecstreams;

			if(rft->unmapped) {
				argus.push_back("HttpRequest*");
				argus.push_back("HttpResponse*");
				valus.push_back(req);
				valus.push_back(res);
			} else {
				for (int var = 0; var < prsiz; var++)
				{
					try
					{
						std::string& pmvalue = BLANK;
						if(rft->params.at(var).from=="path")
						{
							pmvalue = mapOfValues[rft->params.at(var).name];
						}
						else if(rft->params.at(var).from=="reqparam")
						{
							if(req->getQueryParams().find(rft->params.at(var).name)!=req->getQueryParams().end())
								pmvalue = req->getQueryParam(rft->params.at(var).name);
							else
								pmvalue = rft->params.at(var).defValue;
						}
						else if(rft->params.at(var).from=="postparam")
						{
							if(req->getRequestParams().find(rft->params.at(var).name)!=req->getRequestParams().end())
								pmvalue = req->getRequestParam(rft->params.at(var).name);
							else
								pmvalue = rft->params.at(var).defValue;
						}
						else if(rft->params.at(var).from=="header")
						{
							if(req->getHeaders().find(rft->params.at(var).name)!=req->getHeaders().end())
								pmvalue = req->getHeader(rft->params.at(var).name);
							else
								pmvalue = rft->params.at(var).defValue;
						}
						else if(rft->params.at(var).from=="multipart-content")
						{
							MultipartContent mcont = req->getMultipartContent(rft->params.at(var).name);
							if(mcont.isValid())
							{
								if(mcont.isAFile())
								{
									if(rft->params.at(var).type=="filestream")
									{
										pmvalue = rft->params.at(var).name;
									}
									else if(rft->params.at(var).type!="vector-of-filestream")
									{
										//logger << "File can only be mapped to ifstream" << std::endl;
										res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
										res->setDone(true);
										if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
										return true;
									}
								}
								else
								{
									pmvalue = mcont.getContent();
								}
							}
							else if(rft->params.at(var).type!="vector-of-filestream")
							{
								//logger << "Invalid mapping specified in config, no multipart content found with name " + rft->params.at(var).name << std::endl;
								res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
								res->setDone(true);
								if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
								return true;
							}
						}
						else
						{
							if(prsiz>1)
							{
								//logger << "Request Body cannot be mapped to more than one argument..." << std::endl;
								res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
								res->setDone(true);
								if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
								return true;
							}
							pmvalue = req->getContent();
						}

						//logger << ("Restcontroller parameter type/value = "  + rft->params.at(var).type + "/" + pmvalue) << std::endl;
						//logger << ("Restcontroller content types input/output = " + icont + "/" + ocont) << std::endl;

						switch(rft->params.at(var).serOpt) {
							case 0: {
								void* voidPvect = NULL;
								if(rft->icontentType==ContentTypes::CONTENT_TYPE_APPLICATION_JSON)
								{
									voidPvect = JSONSerialize::unSerializeUnknown(pmvalue, rft->params.at(var).serOpt, rft->params.at(var).type, rft->appName);
								}
								#ifdef INC_XMLSER
								else
								{
									voidPvect = XMLSerialize::unSerializeUnknown(pmvalue, rft->params.at(var).serOpt, rft->params.at(var).type, rft->appName);
								}
								#endif
								if(voidPvect==NULL)
								{
									res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
									res->setDone(true);
									if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
									for(int i=0;i<(int)valus.size();++i) {
										if(valus.at(i)!=NULL) {
											reflector.destroy(valus.at(i), argus.at(i));
										}
									}
									return true;
								}
								valus.push_back(voidPvect);
								argus.push_back(rft->params.at(var).type);
								break;
							}
							case 1: argus.push_back(rft->params.at(var).type);valus.push_back(new std::string(pmvalue));break;
							case 2: argus.push_back(rft->params.at(var).type);valus.push_back(new char(CastUtil::lexical_cast<char>(pmvalue)));break;
							case 3: argus.push_back(rft->params.at(var).type);valus.push_back(new unsigned char(CastUtil::lexical_cast<unsigned char>(pmvalue)));break;
							case 4: argus.push_back(rft->params.at(var).type);valus.push_back(new int(CastUtil::toInt(pmvalue)));break;
							case 5: argus.push_back(rft->params.at(var).type);valus.push_back(new unsigned int(CastUtil::toUInt(pmvalue)));break;
							case 6: argus.push_back(rft->params.at(var).type);valus.push_back(new short(CastUtil::toShort(pmvalue)));break;
							case 7: argus.push_back(rft->params.at(var).type);valus.push_back(new unsigned short(CastUtil::toUShort(pmvalue)));break;
							case 8: argus.push_back(rft->params.at(var).type);valus.push_back(new long(CastUtil::toLonglong(pmvalue)));break;
							case 9: argus.push_back(rft->params.at(var).type);valus.push_back(new unsigned long(CastUtil::toULong(pmvalue)));break;
							case 10: argus.push_back(rft->params.at(var).type);valus.push_back(new long long(CastUtil::toLonglong(pmvalue)));break;
							case 11: argus.push_back(rft->params.at(var).type);valus.push_back(new unsigned long long(CastUtil::toULonglong(pmvalue)));break;
							case 12: argus.push_back(rft->params.at(var).type);valus.push_back(new float(CastUtil::toFloat(pmvalue)));break;
							case 13: argus.push_back(rft->params.at(var).type);valus.push_back(new double(CastUtil::toDouble(pmvalue)));break;
							case 14: argus.push_back(rft->params.at(var).type);valus.push_back(new long double(CastUtil::toLongdouble(pmvalue)));break;
							case 15: argus.push_back(rft->params.at(var).type);valus.push_back(new bool(CastUtil::toBool(pmvalue)));break;
							case 16: {
								argus.push_back(rft->params.at(var).type);
								DateFormat formt;
								valus.push_back(formt.parse(pmvalue));break;
							}
							case 19: {
								argus.push_back("ifstream*");
								MultipartContent mcont = req->getMultipartContent(pmvalue);
								if(mcont.isValid() && mcont.isAFile())
								{
									std::ifstream* ifs = new std::ifstream;
									ifs->open(mcont.getTempFileName().c_str());
									valus.push_back(ifs);
									allStreams.push_back(ifs);
								}
								else
								{
									valus.push_back(NULL);
								}
								break;
							}
							case 100:
							case 101:
							case 102:
							case 103:
							case 104:
							case 105:
							case 106:
							case 107:
							case 108:
							case 109:
							case 110:
							case 111:
							case 112:
							case 113:
							case 114:
							case 115:
							case 116: {
								void* voidPvect = NULL;
								if(rft->icontentType==ContentTypes::CONTENT_TYPE_APPLICATION_JSON)
								{
									voidPvect = JSONSerialize::unSerializeUnknown(pmvalue, rft->params.at(var).serOpt, rft->params.at(var).type, rft->appName);
								}
								#ifdef INC_XMLSER
								else
								{
									voidPvect = XMLSerialize::unSerializeUnknown(pmvalue, rft->params.at(var).serOpt, rft->params.at(var).type, rft->appName);
								}
								#endif
								if(voidPvect==NULL)
								{
									res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
									res->setDone(true);
									if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
									for(int i=0;i<(int)valus.size();++i) {
										if(valus.at(i)!=NULL) {
											reflector.destroy(valus.at(i), argus.at(i));
										}
									}
									return true;
								}
								valus.push_back(voidPvect);
								argus.push_back(rft->params.at(var).type);
								break;
							}
							case 119: {
								argus.push_back("vector<ifstream*>");
								std::vector<std::ifstream*> *vifs = NULL;
								if(mpvecstreams.find(rft->params.at(var).name)==mpvecstreams.end())
								{
									vifs = new std::vector<std::ifstream*>;
									std::vector<MultipartContent> mcontvec = req->getMultiPartFileList(rft->params.at(var).name);
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
									mpvecstreams[rft->params.at(var).name] = vifs;
								}
								else
								{
									vifs = mpvecstreams[rft->params.at(var).name];
								}
								valus.push_back(vifs);
								break;
							}
						}
					} catch(const std::exception& e) {
						//logger << "Restcontroller exception occurred" << std::endl;
						invValue= true;
						res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
						res->setDone(true);
						if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
						for(int i=0;i<(int)valus.size();++i) {
							if(valus.at(i)!=NULL) {
								reflector.destroy(valus.at(i), argus.at(i));
							}
						}
						return true;
					}
				}
			}
			//t.end();
			//CommonUtils::tsContRstPrsArgs += t.timerNanoSeconds();

			try
			{
				//t.start();
				isContrl = true;
				const Method& meth = srv->getMethod(rft->name, argus);
				if(meth.getMethodName()!="" && !invValue)
				{
					void* ouput = reflector.invokeMethodUnknownReturn(_temp,meth,valus,rft->unmapped?false:true);
					//t.end();
					//CommonUtils::tsContRstExec += t.timerNanoSeconds();

					//t.start();
					if(rft->unmapped) {
						res->addHeader(HttpResponse::ContentType, rft->ocontentType);
						res->setHTTPResponseStatus(HTTPResponseStatus::getStatusByCode(rft->statusCode));
						if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
					} else {
						int serOpt = rft->serOpt>=2000?-3:(rft->serOpt>=1000?-2:rft->serOpt);
						switch(serOpt) {
							case -3: {
								if(ouput!=NULL) {
									res->setContent(XMLSerialize::serializeUnknown(ouput, rft->serOpt-2000, rft->rtype, rft->appName));
									res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_APPLICATION_XML);
								}
								break;
							}
							case -2: {
								if(ouput!=NULL) {
									res->setContent(JSONSerialize::serializeUnknown(ouput, rft->serOpt-1000, rft->rtype, rft->s, rft->sc, rft->scm, rft->appName));
									res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
								}
								break;
							}
							case -1: {
								if(rft->statusCode=="" && res->getContent().length()==0) {
									res->setHTTPResponseStatus(HTTPResponseStatus::NoContent);
								} else {
									res->setHTTPResponseStatus(HTTPResponseStatus::getStatusByCode(CastUtil::toInt(rft->statusCode)));
								}
								break;
							}
							case 1: {
								if(ouput!=NULL) {
									res->setContent(*(std::string*)ouput);
									res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
								}
								break;
							}
							default: {
								if(rft->serOpt>17 || rft->serOpt==0) {
									if(ouput!=NULL) {
										if(rft->ocontentType==ContentTypes::CONTENT_TYPE_TEXT_PLAIN) {
											res->setContent(SerializeBase::trySerialize(ouput, rft->serOpt, rft->rtype, rft->appName));
											res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
										} else {
											res->setContent(JSONSerialize::serializeUnknown(ouput, rft->serOpt, rft->rtype, rft->appName));
											res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_APPLICATION_JSON);
										}
									}
								} else {
									if(ouput!=NULL) {
										res->setContent(SerializeBase::trySerialize(ouput, rft->serOpt, rft->rtype, rft->appName));
										res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
									}
								}
								break;
							}
						}
						res->setHTTPResponseStatus(HTTPResponseStatus::getStatusByCode(rft->statusCode));
						int rserOpt = rft->serOpt>=2000?rft->serOpt-2000:(rft->serOpt>=1000?rft->serOpt-1000:rft->serOpt);
						reflector.destroy(rserOpt, ouput, rft->rtype);
						//logger << "Successfully called restcontroller output follows - " << std::endl;
						//logger << outcontent << std::endl;

						for(int i=0;i<(int)allStreams.size();++i) {
							if(allStreams.at(i)!=NULL) {
								if(allStreams.at(i)->is_open()) {
									allStreams.at(i)->close();
									allStreams.at(i)->clear();
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
						if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
					}

					//t.end();
					//CommonUtils::tsContRstSer += t.timerNanoSeconds();
				}
				else
				{
					res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
					//res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
					//logger << "Rest Controller Method Not Found" << std::endl;
					if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
				}
			} catch(const std::exception& e) {
				//logger << "Restcontroller exception occurred" << std::endl;
				invValue= true;
				res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
				res->setDone(true);
				if(srv->getSI()==NULL)ConfigurationData::getInstance()->ffeadContext.release(_temp, "restcontroller_"+rft->clas, rft->appName);
				return true;
			}
		}
	}
	if(isContrl && res->getStatusCode()!="404") {
		res->setDone(true);
	}
	return isContrl;
}
