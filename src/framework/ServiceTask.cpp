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

ServiceTask::ServiceTask(const int& fd, const std::string& serverRootDirectory) {
	this->fd=fd;
	this->serverRootDirectory=serverRootDirectory;
	logger = LoggerFactory::getLogger("ServiceTask");
}

ServiceTask::~ServiceTask() {
	// TODO Auto-generated destructor stub
}

ServiceTask::ServiceTask() {
	logger = LoggerFactory::getLogger("ServiceTask");
}

void ServiceTask::saveSessionDataToFile(const std::string& sessionId, const std::string& value)
{
	std::string lockfil = serverRootDirectory+"/tmp/"+sessionId+".lck";
	std::ifstream ifs(lockfil.c_str(), std::ios::binary);
	int counter = 5000/100;
	while(ifs.is_open()) {
		Thread::mSleep(100);
		ifs.close();
		ifs.open(lockfil.c_str());
		if(counter--<=0)break;
	}

	std::string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
	logger << ("Saving session to file " + filen) << std::endl;
	std::ofstream ofs(filen.c_str(), std::ios::binary);
	ofs.write(value.c_str(),value.length());
	ofs.close();

	remove(lockfil.c_str());
}

std::map<std::string,std::string> ServiceTask::getSessionDataFromFile(const std::string& sessionId)
{
	std::map<std::string,std::string> valss;
	std::string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
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
		logger << ("Read key/value pair " + results1.at(0) + " = " + valss[results1.at(0)]) << std::endl;
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
	} catch (...) {
		logger << "error readin map value"<< std::endl;
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
		std::map<std::string,std::string> vals = req->getSession()->getSessionAttributes();
		std::string prevcookid = req->getCookieInfoAttribute("FFEADID");

		std::string values;
		//logger << "session object modified " << vals.size() << std::endl;
		Date date(true);
		std::string id = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime());
		//int seconds = sessionTimeout;
		date.updateSeconds(sessionTimeout);
		DateFormat dformat("ddd, dd-mmm-yyyy hh:mi:ss");
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
		res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<std::string>(getFileSize(fname.c_str())));
		res->addHeaderValue(HttpResponse::AcceptRanges, "bytes");
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
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
		DateFormat df("ddd, dd mmm yyyy hh:mi:ss GMT");
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
				logger << "Parsed date success" << std::endl;
			} catch(...) {
				isifmodsincvalid = false;
			}

			if(ifmodsince!=NULL)
			{
				logger << "IfModifiedSince header = " + ifmodsincehdr + ", date = " + ifmodsince->toString() << std::endl;
				logger << "Lastmodifieddate value = " + lastmodDate + ", date = " + filemodifieddate.toString() << std::endl;
				logger << "Date Comparisons = " +CastUtil::lexical_cast<std::string>(*ifmodsince>=filemodifieddate)  << std::endl;

				if(isifmodsincvalid && *ifmodsince>=filemodifieddate)
				{
					res->addHeaderValue(HttpResponse::LastModified, ifmodsincehdr);
					logger << ("File not modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << std::endl;
					res->setHTTPResponseStatus(HTTPResponseStatus::NotModified);
					return;
				}
				else if(isifmodsincvalid && *ifmodsince<filemodifieddate)
				{
					logger << ("File modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << std::endl;
					forceLoadFile = true;
				}
				delete ifmodsince;
			}
		}

		res->addHeaderValue(HttpResponse::LastModified, lastmodDate);

		if(res->isHeaderValue(HttpResponse::ContentEncoding, "gzip"))
		{
			bool gengzipfile = true;
			std::string ofname = req->getContextHome() + "/temp/" + req->getFile() + ".gz";
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
			std::string ofname = req->getContextHome() + "/temp/" + req->getFile() + ".z";
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

		logger << ("Content request for " + url + " " + ext + " actual file " + fname) << std::endl;

		if(req->getHttpVers()<1.1 && rangeValuesLst.size()>0)
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::InvalidReqRange);
			return;
		}
		else if(rangeValuesLst.size()>0)
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::PartialContent);
			res->addHeaderValue(HttpResponse::ContentType, "multipart/byteranges");
			unsigned int totlen = getFileSize(fname.c_str());
			res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<std::string>(totlen));
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
					conte.addHeaderValue(MultipartContent::ContentType, type);
					conte.addHeaderValue(HttpResponse::ContentRange, "bytes "+rangesVec.at(var)+"/"+CastUtil::lexical_cast<std::string>(totlen));
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
				res->addHeaderValue(HttpResponse::TransferEncoding, "chunked");
				all = StringUtil::toHEX(techunkSiz) + "\r\n";
				all += getFileContents(fname.c_str(), 0, techunkSiz);
				all += "\r\n";
			}
			else
			{
				res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<std::string>(totlen));
				all = getFileContents(fname.c_str());
			}
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(ext));
			res->setContent(all);
		}
	}
}

/*bool ServiceTask::checkSocketWaitForTimeout(const int& sock_fd, const int& writing, const int& seconds, const int& micros)
{
	#ifdef OS_MINGW
		u_long iMode = 1;
		ioctlsocket(sock_fd, FIONBIO, &iMode);
	#else
		fcntl(sock_fd, F_SETFL, fcntl(sock_fd, F_GETFD, 0) | O_NONBLOCK);
	#endif

	fd_set rset, wset;
	struct timeval tv = {seconds, micros};
	int rc;

	// Guard against closed socket
	if (sock_fd < 0)
	{
		return false;
	}

	// Construct the arguments to select
	FD_ZERO(&rset);
	FD_SET(sock_fd, &rset);
	wset = rset;

	// See if the socket is ready
	switch (writing)
	{
		case 0:
			rc = select(sock_fd+1, &rset, NULL, NULL, &tv);
			break;
		case 1:
			rc = select(sock_fd+1, NULL, &wset, NULL, &tv);
			break;
		case 2:
			rc = select(sock_fd+1, &rset, &wset, NULL, &tv);
			break;
	}
	FD_CLR(sock_fd, &rset);
	#ifdef OS_MINGW
		u_long bMode = 0;
		ioctlsocket(sock_fd, FIONBIO, &bMode);
	#else
		fcntl(sock_fd, F_SETFL, O_SYNC);
	#endif	

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	// Return SOCKET_TIMED_OUT on timeout, SOCKET_OPERATION_OK
	//otherwise
	//(when we are able to write or when there's something to
	//read)
	return rc <= 0 ? false : true;
}*/

void ServiceTask::handleWebsockOpen(WebSocketData* req) {

	Reflector reflector(ConfigurationData::getInstance()->dlib);

	std::string className;
	std::map<std::string, std::map<std::string, std::string> >& websocketMappingMap = ConfigurationData::getInstance()->websocketMappingMap;
	std::map<std::string, std::string> websockcntMap = websocketMappingMap[req->getCntxt_name()];
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
		const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][className];
		Method meth = srv.getMethod("onOpen", argus);
		if(meth.getMethodName()!="")
		{
			// logger << ("WebSocket Controller " + className + " called") << std::endl;
			 reflector.invokeMethodGVP(_temp,meth,valus);
			 logger << "WebSocket Controller onOpen" << std::endl;
		}
		else
		{
			logger << "Invalid WebSocket Controller" << std::endl;
		}
	}
}

void ServiceTask::handleWebsockClose(WebSocketData* req) {

	Reflector reflector(ConfigurationData::getInstance()->dlib);

	std::string className;
	std::map<std::string, std::map<std::string, std::string> >& websocketMappingMap = ConfigurationData::getInstance()->websocketMappingMap;
	std::map<std::string, std::string> websockcntMap = websocketMappingMap[req->getCntxt_name()];
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
		const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][className];
		Method methc = srv.getMethod("onClose", argus);
		if(methc.getMethodName()!="")
		{
			 //logger << ("WebSocket Controller " + className + " called") << std::endl;
			 reflector.invokeMethodGVP(_temp,methc,valus);
			 logger << "WebSocket Controller onClose" << std::endl;
		}
		else
		{
			logger << "Invalid WebSocket Controller" << std::endl;
		}
	}
}

void ServiceTask::handleWebsockMessage(const std::string& url, WebSocketData* req, WebSocketData* response) {

	Reflector reflector(ConfigurationData::getInstance()->dlib);

	std::string className;
	std::map<std::string, std::map<std::string, std::string> >& websocketMappingMap = ConfigurationData::getInstance()->websocketMappingMap;
	std::map<std::string, std::string> websockcntMap = websocketMappingMap[req->getCntxt_name()];
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
		const ClassInfo& srv = ConfigurationData::getInstance()->ffeadContext.classInfoMap[req->getCntxt_name()][className];
		argus.push_back("WebSocketData*");
		valus.push_back(req);
		argus.push_back("WebSocketData*");
		valus.push_back(response);
		Method methc = srv.getMethod("onMessage", argus);
		if(methc.getMethodName()!="")
		{
			 //logger << ("WebSocket Controller " + className + " called") << std::endl;
			 WebSocketData data = reflector.invokeMethod<WebSocketData>(_temp,methc,valus);
			 logger << "WebSocket Controller onMessage" << std::endl;
		}
		else
		{
			logger << "Invalid WebSocket Controller" << std::endl;
		}
	}
}

void ServiceTask::handle(HttpRequest* req, HttpResponse* res)
{

	res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
	this->serverRootDirectory = ConfigurationData::getInstance()->coreServerProperties.serverRootDirectory;
	try
	{
		if(req->getRequestParseStatus().getCode()>0)
		{
			res->setHTTPResponseStatus(req->getRequestParseStatus());
			res->addHeaderValue(HttpResponse::Connection, "close");
			return;
		}

		std::string& webpath = ConfigurationData::getInstance()->coreServerProperties.webPath;

		if(req->getActUrlParts().size()==0)
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
			res->addHeaderValue(HttpResponse::Connection, "close");
			logger << "Context not found, Closing connection..." << std::endl;
			return;
		}

		if(req->getCntxt_name()=="") {
			req->setCntxt_name("default");
		}

		if(ConfigurationData::getInstance()->appAliases.find(req->getCntxt_name())!=ConfigurationData::getInstance()->appAliases.end()) {
			req->setCntxt_name(ConfigurationData::getInstance()->appAliases[req->getCntxt_name()]);
			if(ConfigurationData::getInstance()->servingContexts.find(req->getCntxt_name())==ConfigurationData::getInstance()->servingContexts.end())
			{
				res->addHeaderValue(HttpResponse::Connection, "close");
				logger << "Context not found, Closing connection..." << std::endl;
				return;
			}
		}

		if(!ConfigurationData::isServingContext(req->getCntxt_name()))
		{
			req->setCntxt_name("default");
		}

		req->normalizeUrl();
		req->setCntxt_root(webpath+req->getCntxt_name());
		req->updateContent();

		ConfigurationData::getInstance()->httpRequest.set(req);
		ConfigurationData::getInstance()->httpResponse.set(res);
		CommonUtils::setAppName(req->getCntxt_name());

		SecurityHandler::populateAuthDetails(req);

		if(req->hasCookie())
		{
			//if(!ConfigurationData::getInstance()->coreServerProperties.sessatserv)
			req->getSession()->setSessionAttributes(req->getCookieInfo());
			/*else
			{
				std::string id = req->getCookieInfoAttribute("FFEADID");
				logger << id << std::endl;
				std::map<std::string,std::string> values;
#ifdef INC_DSTC
				if(ConfigurationData::getInstance()->coreServerProperties.sessservdistocache)
					values = getSessionDataFromDistocache(id);
				else
#endif
					values = getSessionDataFromFile(id);
				req->getSession()->setSessionAttributes(values);
			}*/
		}

		Reflector reflector(ConfigurationData::getInstance()->dlib);

#ifdef INC_APPFLOW
		/*if(ConfigurationData::getInstance()->applicationFlowMap.find(req->getCntxt_name())!=
				ConfigurationData::getInstance()->applicationFlowMap.end() &&
				ConfigurationData::getInstance()->applicationFlowMap[req->getCntxt_name()])
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
		long sessionTimeoutVar = ConfigurationData::getInstance()->coreServerProperties.sessionTimeout;

		bool isContrl = false;
		try {
			isContrl = CORSHandler::handle(ConfigurationData::getInstance()->corsConfig, req, res);
		} catch(const HTTPResponseStatus& status) {
			res->setHTTPResponseStatus(status);
			isContrl = true;
		}

		bool hasSecurity = SecurityHandler::hasSecurity(req->getCntxt_name());
		if(!isContrl && hasSecurity)
		{
			isContrl = SecurityHandler::handle(req, res, sessionTimeoutVar, reflector);
			if(isContrl)
			{
				ext = req->getExt();
			}
		}

		bool hasFilters = FilterHandler::hasFilters(req->getCntxt_name());
		if(!isContrl && hasFilters)
		{
			FilterHandler::handleIn(req, ext, reflector);

			isContrl = !FilterHandler::handle(req, res, ext, reflector);
			if(isContrl)
			{
			}
			ext = req->getExt();
		}

		if(!isContrl)
		{
			isContrl = ControllerHandler::handle(req, res, ext, reflector);
			if(isContrl)
			{
			}
			ext = req->getExt();
		}

		if(!isContrl)
		{
			isContrl = ExtHandler::handle(req, res, ConfigurationData::getInstance()->dlib, ConfigurationData::getInstance()->ddlib, ext, reflector);
			if(isContrl)
			{
			}
		}

		/*After going through the controller the response might be blank, just set the HTTP version*/
		res->update(req);

		if(req->getMethod()!="TRACE" && !res->isDone())
		{
			//logger << ("Started processing request - phase II") << std::endl;

			std::string wsUrl = "http://" + ConfigurationData::getInstance()->coreServerProperties.ip_address + req->getCurl();
			if(!isContrl)
			{
#ifdef INC_WEBSVC
				std::string wsName = ConfigurationData::getInstance()->wsdlMap[req->getCntxt_name()][wsUrl];
				if(wsName!="")
				{
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
#endif
				else
				{
					bool cntrlit = false;
#ifdef INC_SCRH
					cntrlit = ScriptHandler::handle(req, res, ConfigurationData::getInstance()->handoffs, ext);
#endif
					if(cntrlit)
					{
					}
					else
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

		if(hasFilters) {
			FilterHandler::handleOut(req, res, ext, reflector);
		}

		if(hasSecurity) {
			storeSessionAttributes(res, req, sessionTimeoutVar, ConfigurationData::getInstance()->coreServerProperties.sessatserv);
		}
	}
	catch(const char* err)
	{
		logger << "Exception occurred while processing ServiceTask request - " << err << std::endl;
	}
	catch(...)
	{
		logger << "Standard exception occurred while processing ServiceTask request " << std::endl;
	}
}

/*bool ServiceTask::sendData(SSL* ssl, BIO* io, const int& fd, const std::string& h1)
{
	if(SSLHandler::getInstance()->getIsSSL())
	{
		int r;
		// Now perform renegotiation if requested
		if(ConfigurationData::getInstance()->securityProperties.client_auth==CLIENT_AUTH_REHANDSHAKE)
		{
			SSL_set_verify(ssl,SSL_VERIFY_PEER |
					SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);

			// Stop the client from just resuming the un-authenticated session
			SSL_set_session_id_context(ssl,
					(const unsigned char*)&SSLHandler::s_server_auth_session_id_context,
					sizeof(SSLHandler::s_server_auth_session_id_context));

			if(SSL_renegotiate(ssl)<=0)
			{
				logger << "SSL renegotiation error" << std::endl;
				closeSocket(ssl, io, fd);
				return false;
			}
			if(SSL_do_handshake(ssl)<=0)
			{
				logger << "SSL renegotiation error" << std::endl;
				closeSocket(ssl, io, fd);
				return false;;
			}
			ssl->state = SSL_ST_ACCEPT;
			if(SSL_do_handshake(ssl)<=0)
			{
				logger << "SSL handshake error" << std::endl;
				closeSocket(ssl, io, fd);
				return false;;
			}
		}
		if((r=BIO_write(io, h1.c_str(), h1.length()))<=0)
		{
			logger << "Send failed" << std::endl;
			closeSocket(ssl, io, fd);
			return false;
		}
		if((r=BIO_flush(io))<0)
		{
			logger << "Error flushing BIO" << std::endl;
			closeSocket(ssl, io, fd);
			return false;;
		}
	}
	else
	{
		int r;
		if ((r=BIO_write(io, h1.c_str() , h1.length())) <= 0)
		{
			logger << "send failed" << std::flush;
			closeSocket(ssl, io, fd);
			return false;
		}
		if((r=BIO_flush(io))<0)
		{
			logger << "Error flushing BIO" << std::endl;
			closeSocket(ssl, io, fd);
			return false;
		}
	}
	return true;
}

void ServiceTask::closeSocket(SSL* ssl, BIO* io, const int& fd)
{
	if(SSLHandler::getInstance()->getIsSSL())
	{
		SSLHandler::getInstance()->closeSSL(fd,ssl,io);
	}
	else
	{
		if(io!=NULL)BIO_free_all(io);
		closesocket(fd);
	}
}

bool ServiceTask::readLine(SSL* ssl, BIO* io, const int& fd, std::string& line)
{
	//if(!checkSocketWaitForTimeout(fd, 0, 0, 10))
	//{
	//	logger << "Closing connection as there was no data to read in 10us..." << std::endl;
	//	closeSocket(ssl, io, fd);
	//	return false;
	//}
	char buf[MAXBUFLENM];
	memset(buf, 0, sizeof(buf));
	if(SSLHandler::getInstance()->getIsSSL())
	{
		int er=-1;
		er = BIO_gets(io,buf,BUFSIZZ-1);
		int bser = SSL_get_error(ssl,er);
		switch(bser)
		{
			case SSL_ERROR_WANT_READ:
			{
				logger << "ssl more to read error" << std::endl;
				break;
			}
			case SSL_ERROR_WANT_WRITE:
			{
				logger << "ssl more to write error" << std::endl;
				break;
			}
			case SSL_ERROR_NONE:
			{
				break;
			}
			case SSL_ERROR_ZERO_RETURN:
			{
				closeSocket(ssl, io, fd);
				return false;
			}
			default:
			{
				logger << "SSL read problem" << std::endl;
				closeSocket(ssl, io, fd);
				return false;
			}
		}
		line.append(buf, er);
		memset(&buf[0], 0, sizeof(buf));
	}
	else
	{
		int er=-1;
		er = BIO_gets(io,buf,BUFSIZZ-1);
		if(er==0)
		{
			closeSocket(ssl, io, fd);
			logger << "Socket closed before being serviced" << std::endl;
			return false;
		}
		line.append(buf, er);
		memset(&buf[0], 0, sizeof(buf));
	}
	return true;
}

bool ServiceTask::readData(SSL* ssl, BIO* io, const int& fd, const int& cntlen, std::string& content)
{
	//if(!checkSocketWaitForTimeout(fd, 0, 0, 10))
	//{
	//	logger << "Closing connection as there was no data to read in 10us..." << std::endl;
	//	closeSocket(ssl, io, fd);
	//	return false;
	//}
	char buf[MAXBUFLENM];
	memset(buf, 0, sizeof(buf));
	if(SSLHandler::getInstance()->getIsSSL() && cntlen>0)
	{
		int er=-1;
		while(cntlen>0)
		{
			int readLen = MAXBUFLENM;
			if(cntlen<MAXBUFLENM)
				readLen = cntlen;
			er = BIO_read(io,buf,readLen);
			switch(SSL_get_error(ssl,er))
			{
				case SSL_ERROR_NONE:
					cntlen -= er;
					break;
				case SSL_ERROR_ZERO_RETURN:
				{
					closeSocket(ssl, io, fd);
					return false;
				}
				default:
				{
					logger << "SSL read problem" << std::endl;
					closeSocket(ssl, io, fd);
					return false;
				}
			}
			cntlen -= er;
			content.append(buf, er);
			memset(&buf[0], 0, sizeof(buf));
		}
	}
	else if(cntlen>0)
	{
		int er=-1;
		while(cntlen>0)
		{
			logger << "Length is " + CastUtil::lexical_cast<std::string>(cntlen) << std::endl;
			int readLen = MAXBUFLENM;
			if(cntlen<MAXBUFLENM)
				readLen = cntlen;
			er = BIO_read(io,buf,readLen);
			logger << "Done reading" << std::endl;
			if(er==0)
			{
				closeSocket(ssl, io, fd);
				logger << "Socket closed before being serviced" << std::endl;
				return false;
			}
			else if(er>0)
			{
				cntlen -= er;
				content.append(buf, er);
				memset(&buf[0], 0, sizeof(buf));
			}
		}
	}
	return true;
}*/

void ServiceTask::handleWebSocket(HttpRequest* req, void* dlib, void* ddlib, SocketUtil* sockUtil)
{}
