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
 * ServiceTask.cpp
 *
 *  Created on: 20-Jun-2012
 *      Author: sumeetc
 */

#include "ServiceTask.h"

ServiceTask::ServiceTask() {
	//logger = LoggerFactory::getLogger("ServiceTask");
}

ServiceTask::~ServiceTask() {
}

void ServiceTask::saveSessionDataToFile(const std::string& sessionId, const std::string& value)
{
	std::string lockfil = ConfigurationData::getInstance()->coreServerProperties.serverRootDirectory+"/tmp/"+sessionId+".lck";
	std::ifstream ifs(lockfil.c_str(), std::ios::binary);
	int counter = 5000/100;
	while(ifs.is_open()) {
		Thread::mSleep(100);
		ifs.close();
		ifs.open(lockfil.c_str());
		if(counter--<=0)break;
	}

	std::string filen = ConfigurationData::getInstance()->coreServerProperties.serverRootDirectory+"/tmp/"+sessionId+".sess";
	//logger << ("Saving session to file " + filen) << std::endl;
	std::ofstream ofs(filen.c_str(), std::ios::binary);
	ofs.write(value.c_str(),value.length());
	ofs.close();

	remove(lockfil.c_str());
}

std::map<std::string,std::string> ServiceTask::getSessionDataFromFile(const std::string& sessionId)
{
	std::map<std::string,std::string> valss;
	std::string filen = ConfigurationData::getInstance()->coreServerProperties.serverRootDirectory+"/tmp/"+sessionId+".sess";
	std::ifstream ifs(filen.c_str(), std::ios::binary);
	std::string tem,all;
	while(getline(ifs,tem))
	{
		all.append(tem+"\n");
	}
	ifs.close();
	strVec results;
	StringUtil::split(results, all, ("; "));
	for(int j=0;j<(int)results.size()-1;j++)
	{
		if(results.at(j)=="")continue;
		strVec results1;
		StringUtil::replaceAll(results.at(j),"%3B%20","; ");
		StringUtil::split(results1, results.at(j), ("="));
		if(results1.size()==2)
		{
			StringUtil::replaceAll(results1.at(0),"%3D","=");
			StringUtil::replaceAll(results1.at(1),"%3D","=");
			valss[results1.at(0)] = results1.at(1);
		}
		else
		{
			StringUtil::replaceAll(results1.at(0),"%3D","=");
			valss[results1.at(0)] = "true";
		}
		//logger << ("Read key/value pair " + results1.at(0) + " = " + valss[results1.at(0)]) << std::endl;
	}
	return valss;
}

#ifdef INC_DSTC
void ServiceTask::saveSessionDataToDistocache(const std::string& sessionId, std::map<std::string,std::string>& sessAttrs)
{
	DistGlobalCache globalMap;
	globalMap.addMap(sessionId, sessAttrs);
}


std::map<std::string,std::string> ServiceTask::getSessionDataFromDistocache(const std::string& sessionId)
{
	std::map<std::string,std::string> mp;
	DistGlobalCache globalMap;
	try {
		mp = globalMap.getMap<std::string,std::string>(sessionId);
	} catch(const std::exception& e) {
		//logger << "error readin map value"<< std::endl;
	}
	return mp;
}
#endif

void ServiceTask::storeSessionAttributes(HttpResponse* res, HttpRequest* req, const long& sessionTimeout, const bool& sessatserv)
{
	bool sessionchanged = !req->hasCookie();
	sessionchanged |= req->getSession()->isDirty();

	if(sessionchanged)
	{
		std::map<std::string,std::string, cicomp> vals = req->getSession()->getSessionAttributes();
		std::string prevcookid = req->getCookieInfoAttribute("FFEADID");

		std::string values;
		//logger << "session object modified " << vals.size() << std::endl;
		Date date(true);
		std::string id = CastUtil::fromNumber(Timer::getCurrentTime());
		//int seconds = sessionTimeout;
		date.updateSeconds(sessionTimeout);
		DateFormat dformat("%a, %d %b %Y %H:%M:%S");
		std::map<std::string,std::string>::iterator it;

		/*if(sessatserv)
		{
			if(prevcookid=="")
			{
				res->addCookie("FFEADID=" + id + "; expires="+dformat.format(date)+" GMT; path=/; HttpOnly");
			}
			else
			{
				id = prevcookid;
			}
		}*/
		if(!sessatserv || (sessatserv && !ConfigurationData::getInstance()->coreServerProperties.sessservdistocache))
		{
			for(it=vals.begin();it!=vals.end();it++)
			{
				std::string key = it->first;
				std::string value = it->second;
				StringUtil::replaceAll(key,"; ","%3B%20");
				StringUtil::replaceAll(key,"=","%3D");
				StringUtil::replaceAll(value,"; ","%3B%20");
				StringUtil::replaceAll(value,"=","%3D");
				//logger << it->first << " = " << it->second << std::endl;
				if(!sessatserv)
					res->addCookie(key + "=" + value + "; expires="+dformat.format(date)+" GMT; path=/; HttpOnly");
				else
				{
					values += key + "=" + value + "; ";
				}
			}
		}
		/*if(req->getSession()->isDirty())
		{
#ifdef INC_DSTC
			if(ConfigurationData::getInstance()->coreServerProperties.sessservdistocache)
				saveSessionDataToDistocache(id, vals);
			else
#endif
				saveSessionDataToFile(id, values);
		}*/
	}
}

unsigned int ServiceTask::getFileSize(const char *fileName)
{
	std::ifstream myfile;
	unsigned int siz = 0;
	myfile.open(fileName, std::ios::binary | std::ios::ate);
	if (myfile.is_open())
	{
		myfile.seekg(0, std::ios::end);
		siz = myfile.tellg();
		myfile.close();
	}
	return siz;
}

std::string ServiceTask::getFileContents(const char *fileName, const int& start, const int& end)
{
	std::ifstream myfile;
	std::string all;
	myfile.open(fileName, std::ios::in | std::ios::binary);
	if (myfile.is_open())
	{
		if(start==-1 && end==-1)
		{
			std::string content((std::istreambuf_iterator<char>(myfile)), (std::istreambuf_iterator<char>()));
			all = content;
		}
		else
		{
			myfile.seekg(start);
			std::string s(end, '\0');
			myfile.read(&s[0], end);
			all = s;
		}
		myfile.close();
	}
	return all;
}

void ServiceTask::updateContent(HttpRequest* req, HttpResponse *res, const std::string& ext, const int& techunkSiz)
{
	std::vector<std::string> rangesVec;
	std::vector<std::vector<int> > rangeValuesLst = req->getRanges(rangesVec);

	std::string url = req->getUrl();
	std::string locale = CommonUtils::getLocale(StringUtil::toLowerCopy(req->getDefaultLocale()));
	std::string type = CommonUtils::getMimeType(ext);

	std::string all;
    std::string fname = url;
	if (url=="/")
    {
		res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
		return;
    }

	std::ifstream myfile;
    if(locale.find("english")==std::string::npos && (ext==".html" || ext==".htm"))
    {
    	std::string tfname = fname;
    	StringUtil::replaceFirst(tfname, "." , ("_" + locale+"."));
    	std::ifstream gzipdfile(tfname.c_str(), std::ios::binary);
		if(gzipdfile.good())
		{
			fname = tfname;
			gzipdfile.close();
		}
    }

	if(req->getMethod()=="HEAD")
	{
		res->addHeader(HttpResponse::ContentLength, CastUtil::fromNumber(getFileSize(fname.c_str())));
		res->addHeader(HttpResponse::AcceptRanges, "bytes");
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeader(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
	}
	else if(req->getMethod()=="OPTIONS" || req->getMethod()=="TRACE")
	{
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	}
	else
	{
		std::ifstream infile(fname.c_str(), std::ios::binary);
		if(infile.good())
		{
			infile.close();
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			return;
		}

		struct tm tim;
		struct stat attrib;
		stat(fname.c_str(), &attrib);
		//as per suggestion at http://stackoverflow.com/questions/10446526/get-last-modified-time-of-file-in-linux
		gmtime_r(&(attrib.st_mtime), &tim);
		
		Date filemodifieddate(&tim);
		DateFormat df("%a, %d %b %Y %H:%M:%S GMT");
		std::string lastmodDate = df.format(filemodifieddate);

		bool isifmodsincvalid = false;

		std::string ifmodsincehdr = req->getHeader(HttpRequest::IfModifiedSince);

		bool forceLoadFile = false;
		if(ifmodsincehdr!="")
		{
			Date* ifmodsince = NULL;
			try {
				ifmodsince = df.parse(ifmodsincehdr);
				isifmodsincvalid = true;
				//logger << "Parsed date success" << std::endl;
			} catch(const std::exception& e) {
				isifmodsincvalid = false;
			}

			if(ifmodsince!=NULL)
			{
				//logger << "IfModifiedSince header = " + ifmodsincehdr + ", date = " + ifmodsince->toString() << std::endl;
				//logger << "Lastmodifieddate value = " + lastmodDate + ", date = " + filemodifieddate.toString() << std::endl;
				//logger << "Date Comparisons = " +CastUtil::fromBool(*ifmodsince>=filemodifieddate)  << std::endl;

				if(isifmodsincvalid && *ifmodsince>=filemodifieddate)
				{
					res->addHeader(HttpResponse::LastModified, ifmodsincehdr);
					//logger << ("File not modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << std::endl;
					res->setHTTPResponseStatus(HTTPResponseStatus::NotModified);
					return;
				}
				else if(isifmodsincvalid && *ifmodsince<filemodifieddate)
				{
					//logger << ("File modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << std::endl;
					forceLoadFile = true;
				}
				delete ifmodsince;
			}
		}

		res->addHeader(HttpResponse::LastModified, lastmodDate);

		if(res->isHeaderValue(HttpResponse::ContentEncoding, "gzip"))
		{
			bool gengzipfile = true;
			std::string ofname = req->getCntxt_root() + "/temp/" + req->getFile() + ".gz";
			if(!forceLoadFile)
			{
				std::ifstream gzipdfile(ofname.c_str(), std::ios::binary);
				if(gzipdfile.good())
				{
					gzipdfile.close();
					gengzipfile = false;
				}
			}
			if(gengzipfile)
			{
				CompressionUtil::gzipCompressFile((char*)fname.c_str(), false, (char*)ofname.c_str());
			}
			fname = ofname;
			res->setCompressed(true);
			req->setUrl(fname);
		}
		else if(res->isHeaderValue(HttpResponse::ContentEncoding, "deflate"))
		{
			bool genzlibfile = true;
			std::string ofname = req->getCntxt_root() + "/temp/" + req->getFile() + ".z";
			if(!forceLoadFile)
			{
				std::ifstream gzipdfile(ofname.c_str(), std::ios::binary);
				if(gzipdfile.good())
				{
					gzipdfile.close();
					genzlibfile = false;
				}
			}
			if(genzlibfile)
			{
				CompressionUtil::zlibCompressFile((char*)fname.c_str(), false, (char*)ofname.c_str());
			}
			fname = ofname;
			res->setCompressed(true);
			req->setUrl(fname);
		}

		//logger << ("Content request for " + url + " " + ext + " actual file " + fname) << std::endl;

		if(req->getHttpVers()<1.1 && rangeValuesLst.size()>0)
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::InvalidReqRange);
			return;
		}
		else if(rangeValuesLst.size()>0)
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::PartialContent);
			res->addHeader(HttpResponse::ContentType, "multipart/byteranges");
			unsigned int totlen = getFileSize(fname.c_str());
			res->addHeader(HttpResponse::ContentLength, CastUtil::fromNumber(totlen));
			for (int var = 0; var <(int)rangeValuesLst.size(); ++var) {
				int start = rangeValuesLst.at(var).at(0);
				int end = rangeValuesLst.at(var).at(1);
				if(end!=-1 && end>=(int)totlen && start!=-1 && start<(int)totlen)
				{
					//error
				}
				else
				{
					if(start==-1)
						start = 0;
					if(end==-1)
						end = totlen - 1;
					else
						end += 1;
					std::string cont = getFileContents(fname.c_str(), start, end);
					MultipartContent conte(cont);
					conte.addHeader(MultipartContent::ContentType, type);
					conte.addHeader(HttpResponse::ContentRange, "bytes "+rangesVec.at(var)+"/"+CastUtil::fromNumber(totlen));
					res->addContent(conte);
				}
			}
		}
		else
		{
			unsigned int totlen = getFileSize(fname.c_str());
			float parts = techunkSiz!=0?(float)totlen/techunkSiz:0;
			parts = (floor(parts)<parts?floor(parts)+1:floor(parts));

			if(parts>1 && techunkSiz>0)
			{
				res->addHeader(HttpResponse::TransferEncoding, "chunked");
				all = StringUtil::toHEX(techunkSiz) + "\r\n";
				all += getFileContents(fname.c_str(), 0, techunkSiz);
				all += "\r\n";
			}
			else
			{
				res->addHeader(HttpResponse::ContentLength, CastUtil::fromNumber(totlen));
				all = getFileContents(fname.c_str());
			}
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeader(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
			res->setContent(all);
		}
	}
}

void ServiceTask::handleWebsockOpen(WebSocketData* req) {

	Reflector& reflector = ConfigurationData::getInstance()->reflector;

	std::string className;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> >& websocketMappingMap = ConfigurationData::getInstance()->websocketMappingMap;
	std::map<std::string, std::string, std::less<> > websockcntMap = websocketMappingMap.find(req->getCntxt_name())->second;
	std::map<std::string, std::string>::iterator it;
	for (it=websockcntMap.begin();it!=websockcntMap.end();++it) {
		if(ConfigurationData::urlMatchesPath(req->getCntxt_name(), it->first, req->getUrl()))
		{
			className = it->second;
			break;
		}
	}
	if(className!="")
	{
		void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("websocketclass_"+className, req->getCntxt_name());
		args argus;
		vals valus;
		ClassInfo* srv = ConfigurationData::getClassInfo(className, req->getCntxt_name());
		Method meth = srv->getMethod("onOpen", argus);
		if(meth.getMethodName()!="")
		{
			// logger << ("WebSocket Controller " + className + " called") << std::endl;
			 reflector.invokeMethodGVP(_temp,meth,valus);
			 //logger << "WebSocket Controller onOpen" << std::endl;
		}
		else
		{
			//logger << "Invalid WebSocket Controller" << std::endl;
		}
	}
}

void ServiceTask::handleWebsockClose(WebSocketData* req) {

	Reflector& reflector = ConfigurationData::getInstance()->reflector;

	std::string className;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> >& websocketMappingMap = ConfigurationData::getInstance()->websocketMappingMap;
	std::map<std::string, std::string, std::less<> > websockcntMap = websocketMappingMap.find(req->getCntxt_name())->second;
	std::map<std::string, std::string>::iterator it;
	for (it=websockcntMap.begin();it!=websockcntMap.end();++it) {
		if(ConfigurationData::urlMatchesPath(req->getCntxt_name(), it->first, req->getUrl()))
		{
			className = it->second;
			break;
		}
	}
	if(className!="")
	{
		void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("websocketclass_"+className, req->getCntxt_name());
		args argus;
		vals valus;
		ClassInfo* srv = ConfigurationData::getClassInfo(className, req->getCntxt_name());
		Method methc = srv->getMethod("onClose", argus);
		if(methc.getMethodName()!="")
		{
			 //logger << ("WebSocket Controller " + className + " called") << std::endl;
			 reflector.invokeMethodGVP(_temp,methc,valus);
			 //logger << "WebSocket Controller onClose" << std::endl;
		}
		else
		{
			//logger << "Invalid WebSocket Controller" << std::endl;
		}
	}
}

void ServiceTask::handleWebsockMessage(const std::string& url, WebSocketData* req, WebSocketData* response) {

	Reflector& reflector = ConfigurationData::getInstance()->reflector;

	std::string className;
	std::map<std::string, std::map<std::string, std::string, std::less<> >, std::less<> >& websocketMappingMap = ConfigurationData::getInstance()->websocketMappingMap;
	std::map<std::string, std::string, std::less<> > websockcntMap = websocketMappingMap.find(req->getCntxt_name())->second;
	std::map<std::string, std::string>::iterator it;
	for (it=websockcntMap.begin();it!=websockcntMap.end();++it) {
		if(ConfigurationData::urlMatchesPath(req->getCntxt_name(), it->first, req->getUrl()))
		{
			className = it->second;
			break;
		}
	}
	if(className!="")
	{
		void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("websocketclass_"+className, req->getCntxt_name());
		args argus;
		vals valus;
		ClassInfo* srv = ConfigurationData::getClassInfo(className, req->getCntxt_name());
		argus.push_back("WebSocketData*");
		valus.push_back(req);
		argus.push_back("WebSocketData*");
		valus.push_back(response);
		Method methc = srv->getMethod("onMessage", argus);
		if(methc.getMethodName()!="")
		{
			 //logger << ("WebSocket Controller " + className + " called") << std::endl;
			 WebSocketData data;
			 reflector.invokeMethod<WebSocketData>(&data,_temp,methc,valus);
			 //logger << "WebSocket Controller onMessage" << std::endl;
		}
		else
		{
			//logger << "Invalid WebSocket Controller" << std::endl;
		}
	}
}

void ServiceTask::handle(HttpRequest* req, HttpResponse* res)
{
	Timer t1;
	t1.start();

	res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
	/*After going through the controller the response might be blank, just set the HTTP version*/
	res->update(req);

	try
	{
		if(req->getRequestParseStatus()!=NULL)
		{
			res->setHTTPResponseStatus(*req->getRequestParseStatus());
			res->addHeader(HttpResponse::Connection, "close");
			return;
		}

		if(req->getCntxt_name()=="") {
			req->setCntxt_name(HttpRequest::DEFAULT_CTX);
		}

		Reflector& reflector = ConfigurationData::getInstance()->reflector;

		if(ConfigurationData::getInstance()->servingContextRouters.find(req->getCntxt_name())!=
				ConfigurationData::getInstance()->servingContextRouters.end()) {
			CommonUtils::setAppName(ConfigurationData::getInstance()->servingContextAppNames.find(req->getCntxt_name())->second);
			Router* router = ConfigurationData::getInstance()->servingContextRouters.find(req->getCntxt_name())->second;
			if(router!=NULL) {
				req->setCntxt_root(ConfigurationData::getInstance()->servingContextAppRoots.find(req->getCntxt_name())->second);
				router->route(req, res, ConfigurationData::getInstance()->dlib, ConfigurationData::getInstance()->ddlib);
				t1.end();
				CommonUtils::tsServicePre += t1.timerNanoSeconds();
				return;
			}
		} else {
			if(ConfigurationData::getInstance()->servingContextRouters.find(HttpRequest::DEFAULT_CTX)!=
					ConfigurationData::getInstance()->servingContextRouters.end()) {
				req->setCntxt_name(HttpRequest::DEFAULT_CTX);
				CommonUtils::setAppName(ConfigurationData::getInstance()->servingContextAppNames.find(req->getCntxt_name())->second);
				Router* router = ConfigurationData::getInstance()->servingContextRouters.find(req->getCntxt_name())->second;
				if(router!=NULL) {
					req->setCntxt_root(ConfigurationData::getInstance()->servingContextAppRoots.find(req->getCntxt_name())->second);
					router->route(req, res, ConfigurationData::getInstance()->dlib, ConfigurationData::getInstance()->ddlib);
					t1.end();
					CommonUtils::tsServicePre += t1.timerNanoSeconds();
					return;
				}
			}
		}

		if(!ConfigurationData::isServingContext(req->getCntxt_name())) {
			if(ConfigurationData::getInstance()->appAliases.find(req->getCntxt_name())!=ConfigurationData::getInstance()->appAliases.end()) {
				req->setCntxt_name(std::string_view{ConfigurationData::getInstance()->appAliases.find(req->getCntxt_name())->second});
				if(ConfigurationData::getInstance()->servingContexts.find(req->getCntxt_name())==ConfigurationData::getInstance()->servingContexts.end())
				{
					res->addHeader(HttpResponse::Connection, "close");
					//logger << "Context not found, Closing connection..." << std::endl;
					return;
				}
			}

			if(!ConfigurationData::isServingContext(req->getCntxt_name())) {
				req->setCntxt_name(HttpRequest::DEFAULT_CTX);
			}
		}

		CommonUtils::setAppName(ConfigurationData::getInstance()->servingContextAppNames.find(req->getCntxt_name())->second);
		req->setCntxt_root(ConfigurationData::getInstance()->servingContextAppRoots.find(req->getCntxt_name())->second);

		req->normalizeUrl();
		req->updateContent();

		if(req->getExt().length()>0) {
			std::string mimeType = CommonUtils::getMimeType(req->getExt());
			std::string cntEncoding = getCntEncoding();
			if(req->isAgentAcceptsCE() && (cntEncoding=="gzip" || cntEncoding=="deflate") && req->isNonBinary(mimeType)) {
				res->addHeader(HttpResponse::ContentEncoding, cntEncoding);
			}
		}

		ConfigurationData::getInstance()->httpRequest.reset(req);
		ConfigurationData::getInstance()->httpResponse.reset(res);

		if(ConfigurationData::getInstance()->enableSecurity) {
			SecurityHandler::populateAuthDetails(req);

			if(req->hasCookie())
			{
				req->getSession()->setSessionAttributes(req->getCookieInfo());
			}
		}

#ifdef INC_APPFLOW
		/*if(ConfigurationData::getInstance()->applicationFlowMap.find(req->getCntxt_name())!=
				ConfigurationData::getInstance()->applicationFlowMap.end() &&
				ConfigurationData::getInstance()->applicationFlowMap.find(req->getCntxt_name())->second)
		{
			if(ConfigurationData::getInstance().dlib == NULL)
			{
				std::cerr << dlerror() << std::endl;
				exit(-1);
			}
			std::string meth1 = (req->getCntxt_name()+"checkRules");
			std::string path1;
			void *mkr1 = dlsym(ConfigurationData::getInstance().dlib, meth1.c_str());
			if(mkr1!=NULL)
			{
				typedef std::string (*DCPPtr1) (std::string,HttpSession);
				DCPPtr1 f =  (DCPPtr1)mkr1;
				path1 = f(req->getUrl(),*(req->getSession()));
				//logger << path1 << std::flush;
				if(path1=="FAILED")
				{
					req->setUrl("");
				}
				else if(path1!="" && path1!=req->getUrl())
				{
					req->setUrl(path1);
				}
			}
		}*/
#endif

		std::string ext = req->getExt();

		t1.end();
		CommonUtils::tsServicePre += t1.timerNanoSeconds();

		bool isContrl = false;

		t1.start();
		if(ConfigurationData::getInstance()->enableCors) {
			try {
				isContrl = CORSHandler::handle(ConfigurationData::getInstance()->corsConfig, req, res);
			} catch(const HTTPResponseStatus& status) {
				res->setHTTPResponseStatus(status);
				isContrl = true;
			}
		}
		t1.end();
		CommonUtils::tsServiceCors += t1.timerNanoSeconds();

		t1.start();
		bool hasSecurity = false;
		if(!isContrl && ConfigurationData::getInstance()->enableSecurity) {
			hasSecurity = SecurityHandler::hasSecurity(req->getCntxt_name());
			if(hasSecurity)
			{
				isContrl = SecurityHandler::handle(req, res, ConfigurationData::getInstance()->coreServerProperties.sessionTimeout, reflector);
				if(isContrl)
				{
					ext = req->getExt();
				}
			}
		}
		t1.end();
		CommonUtils::tsServiceSec += t1.timerNanoSeconds();

		t1.start();
		bool hasFilters = false;
		if(!isContrl && ConfigurationData::getInstance()->enableFilters) {
			hasFilters = FilterHandler::hasFilters(req->getCntxt_name());
			if(hasFilters)
			{
				FilterHandler::handleIn(req, ext, reflector);
				isContrl = !FilterHandler::handle(req, res, ext, reflector);
				ext = req->getExt();
			}
		}
		t1.end();
		CommonUtils::tsServiceFlt += t1.timerNanoSeconds();

		t1.start();
		if(!isContrl && ConfigurationData::getInstance()->enableControllers) {
			isContrl = ControllerHandler::handle(req, res, ext, reflector);
			ext = req->getExt();
		}
		t1.end();
		CommonUtils::tsServiceCnt += t1.timerNanoSeconds();

		t1.start();
		if(!isContrl && ConfigurationData::getInstance()->enableExtra) {
			isContrl = ExtHandler::handle(req, res, ConfigurationData::getInstance()->dlib, ConfigurationData::getInstance()->ddlib, ext, reflector);
		}
		t1.end();
		CommonUtils::tsServiceExt += t1.timerNanoSeconds();

		t1.start();

		if(req->getMethod()!="TRACE" && !res->isDone())
		{
			if(!isContrl)
			{
				bool isWbsvc = false;
#ifdef INC_WEBSVC
				if(ConfigurationData::getInstance()->enableSoap) {
					std::string wsUrl = "http://" + ConfigurationData::getInstance()->coreServerProperties.ip_address + req->getCurl();
					std::string wsName = ConfigurationData::getInstance()->wsdlMap.find(req->getCntxt_name())->second[wsUrl];
					if(wsName!="")
					{
						isWbsvc = true;
						if(req->getHeader(HttpRequest::ContentType).find("application/soap+xml")!=std::string::npos || req->getHeader(HttpRequest::ContentType).find("text/xml")!=std::string::npos
								|| req->getHeader(HttpRequest::ContentType).find("application/xml")!=std::string::npos)
						{
							SoapHandler::handle(req, res, ConfigurationData::getInstance()->dlib, wsName);
						}
						else
						{
							res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
						}
					}
				}
#endif
				if(!isWbsvc)
				{
					bool cntrlit = false;
#ifdef INC_SCRH
					if(ConfigurationData::getInstance()->enableScripts) {
						cntrlit = ScriptHandler::handle(req, res, ConfigurationData::getInstance()->handoffs, ext);
					}
#endif
					if(!cntrlit)
					{
						std::string pubUrlPath = req->getCntxt_root() + "/public/";
						if(req->getUrl().find(pubUrlPath)!=0) {
							std::string post = "";
							if(req->getUrl()!=req->getCntxt_root()) {
								post = req->getUrl().substr(req->getCntxt_root().length()+1);
							}
							if(post=="" || post=="/") {
								post = "index.html";
							}
							req->setUrl(pubUrlPath+post);
						}
						res->setDone(false);
						//logger << ("Static file requested") << std::endl;
					}
				}
			}
		}

		if(ConfigurationData::getInstance()->enableFilters && hasFilters) {
			FilterHandler::handleOut(req, res, ext, reflector);
		}

		if(ConfigurationData::getInstance()->enableSecurity && hasSecurity) {
			storeSessionAttributes(res, req, ConfigurationData::getInstance()->coreServerProperties.sessionTimeout, ConfigurationData::getInstance()->coreServerProperties.sessatserv);
		}

		t1.end();
		CommonUtils::tsServicePost += t1.timerNanoSeconds();
	}
	catch(const std::exception& e)
	{
		//logger << "Standard exception occurred while processing ServiceTask request " << std::endl;
	}
}

void ServiceTask::handleWebSocket(HttpRequest* req, void* dlib, void* ddlib, SocketInterface* sockUtil)
{
}
