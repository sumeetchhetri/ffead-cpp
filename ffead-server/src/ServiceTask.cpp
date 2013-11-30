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

ServiceTask::ServiceTask(int fd,string serverRootDirectory,map<string,string> *params,
		bool isSSLEnabled, SSL_CTX *ctx, SSLHandler sslHandler, ConfigurationData configData, void* dlib, void* ddlib) {
	this->fd=fd;
	this->serverRootDirectory=serverRootDirectory;
	this->params= params;
	this->isSSLEnabled = isSSLEnabled;
	this->ctx = ctx;
	this->sslHandler = sslHandler;
	this->configData = configData;
	this->dlib = dlib;
	this->ddlib = ddlib;
	logger = LoggerFactory::getLogger("ServiceTask");
}

ServiceTask::~ServiceTask() {
	// TODO Auto-generated destructor stub
}

void ServiceTask::saveSessionDataToFile(string sessionId, string value)
{
	string lockfil = serverRootDirectory+"/tmp/"+sessionId+".lck";
	ifstream ifs(lockfil.c_str());
	int counter = 5000/100;
	while(ifs.is_open()) {
		Thread::mSleep(100);
		ifs.close();
		ifs.open(lockfil.c_str());
		if(counter--<=0)break;
	}

	string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
	logger << ("Saving session to file " + filen) << endl;
	ofstream ofs(filen.c_str());
	ofs.write(value.c_str(),value.length());
	ofs.close();

	remove(lockfil.c_str());
}

map<string,string> ServiceTask::getSessionDataFromFile(string sessionId)
{
	map<string,string> valss;
	string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
	ifstream ifs(filen.c_str());
	string tem,all;
	while(getline(ifs,tem))
		all.append(tem+"\n");
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
		logger << ("Read key/value pair " + results1.at(0) + " = " + valss[results1.at(0)]) << endl;
	}
	return valss;
}

#ifdef INC_DSTC
void ServiceTask::saveSessionDataToDistocache(string sessionId, map<string,string> sessAttrs)
{
	DistGlobalCache globalMap;
	globalMap.addMap(sessionId, sessAttrs);
}


map<string,string> ServiceTask::getSessionDataFromDistocache(string sessionId)
{
	map<string,string> mp;
	DistGlobalCache globalMap;
	try {
		mp = globalMap.getMap<string,string>(sessionId);
	} catch (...) {
		logger << "error readin map value"<< endl;
	}
	return mp;
}
#endif

string ServiceTask::getFileExtension(string file)
{
	if(file.find_last_of(".")!=string::npos)return file.substr(file.find_last_of("."));
	return file;
}

void ServiceTask::storeSessionAttributes(HttpResponse &res,HttpRequest* req, long sessionTimeout, bool sessatserv)
{
	bool sessionchanged = !req->hasCookie();
	sessionchanged |= req->getSession()->isDirty();

	if(sessionchanged)
	{
		map<string,string> vals = req->getSession()->getSessionAttributes();
		string prevcookid = req->getCookieInfoAttribute("FFEADID");

		string values;
		//logger << "session object modified " << vals.size() << endl;
		Date date;
		string id = CastUtil::lexical_cast<string>(Timer::getCurrentTime());
		//int seconds = sessionTimeout;
		date.updateSeconds(sessionTimeout);
		DateFormat dformat("ddd, dd-mmm-yyyy hh:mi:ss");
		map<string,string>::iterator it;

		if(sessatserv)
		{
			if(prevcookid=="")
			{
				res.addCookie("FFEADID=" + id + "; expires="+dformat.format(date)+" GMT; path=/; HttpOnly");
			}
			else
			{
				id = prevcookid;
			}
		}
		if(!sessatserv || (sessatserv && !configData.sessservdistocache))
		{
			for(it=vals.begin();it!=vals.end();it++)
			{
				string key = it->first;
				string value = it->second;
				StringUtil::replaceAll(key,"; ","%3B%20");
				StringUtil::replaceAll(key,"=","%3D");
				StringUtil::replaceAll(value,"; ","%3B%20");
				StringUtil::replaceAll(value,"=","%3D");
				//logger << it->first << " = " << it->second << endl;
				if(!sessatserv)
					res.addCookie(key + "=" + value + "; expires="+dformat.format(date)+" GMT; path=/; HttpOnly");
				else
				{
					values += key + "=" + value + "; ";
				}
			}
		}
		if(req->getSession()->isDirty())
		{
#ifdef INC_DSTC
			if(configData.sessservdistocache)
				saveSessionDataToDistocache(id, vals);
			else
#endif
				saveSessionDataToFile(id, values);
		}
	}
}

unsigned int ServiceTask::getFileSize(const char *fileName)
{
	ifstream myfile;
	unsigned int siz = 0;
	myfile.open(fileName, ios::binary | ios::ate);
	if (myfile.is_open())
	{
		myfile.seekg(0, ios::end);
		siz = myfile.tellg();
		myfile.close();
	}
	return siz;
}

string ServiceTask::getFileContents(const char *fileName, int start, int end)
{
	ifstream myfile;
	string all;
	myfile.open(fileName, ios::in | ios::binary);
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

void ServiceTask::updateContent(HttpRequest* req, HttpResponse *res, ConfigurationData configData, string ext, int techunkSiz)
{
	vector<string> rangesVec;
	vector<vector<int> > rangeValuesLst = req->getRanges(rangesVec);

	string url = req->getUrl();
	string locale = configData.lprops[StringUtil::toLowerCopy(req->getDefaultLocale())];
	string type = configData.props[ext];

	string all;
    string fname = url;
	if (url=="/")
    {
		res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
		return;
    }

	ifstream myfile;
    if(locale.find("english")==string::npos && (ext==".html" || ext==".htm"))
    {
    	string tfname = fname;
    	StringUtil::replaceFirst(tfname, "." , ("_" + locale+"."));
    	ifstream gzipdfile(tfname.c_str());
		if(gzipdfile.good())
		{
			fname = tfname;
			gzipdfile.close();
		}
    }

	if(req->getMethod()=="HEAD")
	{
		res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<string>(getFileSize(fname.c_str())));
		res->addHeaderValue(HttpResponse::AcceptRanges, "bytes");
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->addHeaderValue(HttpResponse::ContentType, configData.props[ext]);
	}
	else if(req->getMethod()=="OPTIONS" || req->getMethod()=="TRACE")
	{
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	}
	else
	{
		ifstream infile(fname.c_str());
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
		string lastmodDate = df.format(filemodifieddate);

		bool isifmodsincvalid = false;

		string ifmodsincehdr = req->getHeader(HttpRequest::IfModifiedSince);

		bool forceLoadFile = false;
		if(ifmodsincehdr!="")
		{
			Date* ifmodsince = NULL;
			try {
				ifmodsince = df.parse(ifmodsincehdr);
				isifmodsincvalid = true;
				logger << "Parsed date success" << endl;
			} catch(...) {
				isifmodsincvalid = false;
			}

			if(ifmodsince!=NULL)
			{
				logger << "IfModifiedSince header = " + ifmodsincehdr + ", date = " + ifmodsince->toString() << endl;
				logger << "Lastmodifieddate value = " + lastmodDate + ", date = " + filemodifieddate.toString() << endl;
				logger << "Date Comparisons = " +CastUtil::lexical_cast<string>(*ifmodsince>=filemodifieddate)  << endl;

				if(isifmodsincvalid && *ifmodsince>=filemodifieddate)
				{
					res->addHeaderValue(HttpResponse::LastModified, ifmodsincehdr);
					logger << ("File not modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << endl;
					res->setHTTPResponseStatus(HTTPResponseStatus::NotModified);
					return;
				}
				else if(isifmodsincvalid && *ifmodsince<filemodifieddate)
				{
					logger << ("File modified - IfModifiedSince date = " + ifmodsincehdr + ", FileModified date = " + lastmodDate) << endl;
					forceLoadFile = true;
				}
			}
		}

		res->addHeaderValue(HttpResponse::LastModified, lastmodDate);

		if(res->isHeaderValue(HttpResponse::ContentEncoding, "gzip"))
		{
			bool gengzipfile = true;
			string ofname = req->getCntxt_root() + "/temp/" + req->getFile() + ".gz";
			if(!forceLoadFile)
			{
				ifstream gzipdfile(ofname.c_str());
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
			string ofname = req->getCntxt_root() + "/temp/" + req->getFile() + ".z";
			if(!forceLoadFile)
			{
				ifstream gzipdfile(ofname.c_str());
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

		logger << ("Content request for " + url + " " + ext + " actual file " + fname) << endl;

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
			res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<string>(totlen));
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
					string cont = getFileContents(fname.c_str(), start, end);
					MultipartContent conte(cont);
					conte.addHeaderValue(MultipartContent::ContentType, type);
					conte.addHeaderValue(HttpResponse::ContentRange, "bytes "+rangesVec.at(var)+"/"+CastUtil::lexical_cast<string>(totlen));
					res->addContent(conte);
				}
			}
		}
		else
		{
			unsigned int totlen = getFileSize(fname.c_str());
			float parts = (float)totlen/techunkSiz;
			parts = (floor(parts)<parts?floor(parts)+1:floor(parts));

			if(parts>1)
			{
				res->addHeaderValue(HttpResponse::TransferEncoding, "chunked");
				all = StringUtil::toHEX(techunkSiz) + "\r\n";
				all += getFileContents(fname.c_str(), 0, techunkSiz);
				all += "\r\n";
			}
			else
			{
				res->addHeaderValue(HttpResponse::ContentLength, CastUtil::lexical_cast<string>(totlen));
				all = getFileContents(fname.c_str());
			}
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, configData.props[ext]);
			res->setContent(all);
		}
	}
}

bool ServiceTask::checkSocketWaitForTimeout(int sock_fd, int writing, int seconds, int micros)
{
	fcntl(sock_fd, F_SETFL, fcntl(sock_fd, F_GETFD, 0) | O_NONBLOCK);

	fd_set rset, wset;
	struct timeval tv = {seconds, micros};
	int rc;

	/* Guard against closed socket */
	if (sock_fd < 0)
	{
		return false;
	}

	/* Construct the arguments to select */
	FD_ZERO(&rset);
	FD_SET(sock_fd, &rset);
	wset = rset;

	/* See if the socket is ready */
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
	fcntl(sock_fd, F_SETFL, O_SYNC);

	/* Return SOCKET_TIMED_OUT on timeout, SOCKET_OPERATION_OK
	otherwise
	(when we are able to write or when there's something to
	read) */
	return rc == 0 ? false : true;
}

void ServiceTask::run()
{
	//logger << dlib << endl;
	string ip = "invalid session";
	string alldatlg = "\ngot fd from parent";
	SSL *ssl=NULL;
	BIO *sbio=NULL;
	BIO *io=NULL,*ssl_bio=NULL;
	Timer timer;
	timer.start();
	int connKeepAlive = 10, techunkSiz = 8192, maxReqHdrCnt = 100, maxEntitySize = 2147483648;
	string cntEnc = "";
	try {
		connKeepAlive = CastUtil::lexical_cast<int>(configData.sprops["KEEP_ALIVE_SECONDS"]);
	} catch (...) {
	}
	try {
		techunkSiz = CastUtil::lexical_cast<int>(configData.sprops["TRANSFER_ENCODING_CHUNK_SIZE"]);
	} catch (...) {
	}
	try {
		maxReqHdrCnt = CastUtil::lexical_cast<int>(configData.sprops["MAX_REQUEST_HEADERS_COUNT"]);
	} catch (...) {
	}
	try {
		maxEntitySize = CastUtil::lexical_cast<int>(configData.sprops["MAX_REQUEST_ENTITY_SIZE"]);
	} catch (...) {
	}
	cntEnc = StringUtil::toLowerCopy(configData.sprops["CONTENT_ENCODING"]);

	bool cont = true;

	if(isSSLEnabled)
	{
		sbio=BIO_new_socket(fd,BIO_NOCLOSE);
		ssl=SSL_new(ctx);
		SSL_set_bio(ssl,sbio,sbio);

		io=BIO_new(BIO_f_buffer());
		ssl_bio=BIO_new(BIO_f_ssl());
		BIO_set_ssl(ssl_bio,ssl,BIO_CLOSE);
		BIO_push(io,ssl_bio);

		int r = SSL_accept(ssl);
		int bser = SSL_get_error(ssl,r);
		if(r<=0)
		{
			sslHandler.error_occurred((char*)"SSL accept error",fd,ssl);
			return;
		}
	}
	else
	{
		sbio=BIO_new_socket(fd,BIO_CLOSE);
		io=BIO_new(BIO_f_buffer());
		BIO_push(io,sbio);
	}

	while(cont)
	{
		try
		{
			HttpResponse res;

			//Close the connection after inactivity period of connKeepAlive seconds
			if(!checkSocketWaitForTimeout(fd, 0, connKeepAlive))
			{
				logger << "Closing connection as read operation timed out..." << endl;
				if(isSSLEnabled)
				{
					sslHandler.closeSSL(fd,ssl,io);
				}
				else
				{
					if(io!=NULL)BIO_free_all(io);
					close(fd);
				}
				return;
			}

			int cntlen = 0;
			strVec results;
			logger << "Into Http Service method" << endl;
			int headerCount = 0;
			while(true)
			{
				headerCount++;
				string temp;
				bool fl = readLine(isSSLEnabled, ssl, sslHandler, io, fd, temp);
				if(temp.length()>32765)
				{
					res.setHTTPResponseStatus(HTTPResponseStatus::ReqUrlLarge);
					res.addHeaderValue(HttpResponse::Connection, "close");
					bool sendSuccess = sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, res.generateResponse());
					logger << "Closing connection..." << endl;
					if(sendSuccess)closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
					return;
				}
				if(!fl)
				{
					return;
				}
				if(temp=="\r" || temp=="\r\n" || temp=="\n")
				{
					break;
				}
				if(temp=="")continue;
				temp = temp.substr(0,temp.length()-1);
				results.push_back(temp);
				if(headerCount>=maxReqHdrCnt)
				{
					sslHandler.error_occurred((char*)("Cannot accept more than "+CastUtil::lexical_cast<string>(maxReqHdrCnt)+" headers").c_str(),fd,ssl);
					if(io!=NULL)BIO_free(io);
					break;
				}
			}

			alldatlg += "--read data";
			string webpath = serverRootDirectory + "web/";
			//Parse the HTTP headers
			HttpRequest* req= new HttpRequest(results, webpath);

			if(req->getRequestParseStatus().getCode()>0)
			{
				res.setHTTPResponseStatus(req->getRequestParseStatus());
				res.addHeaderValue(HttpResponse::Connection, "close");
				bool sendSuccess = sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, res.generateResponse());
				logger << "Closing connection..." << endl;
				if(sendSuccess)closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
				return;
			}

			string cntle = req->getHeader(HttpRequest::ContentLength);
			try
			{
				cntlen = CastUtil::lexical_cast<int>(cntle);
				if(cntlen>maxEntitySize)
				{
					res.setHTTPResponseStatus(HTTPResponseStatus::ReqEntityLarge);
					res.addHeaderValue(HttpResponse::Connection, "close");
					bool sendSuccess = sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, res.generateResponse());
					logger << "Closing connection..." << endl;
					if(sendSuccess)closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
					return;
				}
			}
			catch(const char* ex)
			{
				logger << "Bad lexical cast exception while reading http Content-Length" << endl;
			}

			if(configData.cntMap[req->getCntxt_name()]!="true")
			{
				req->setCntxt_name("default");
				req->setCntxt_root(webpath+"default");

				string folder = webpath+req->getCntxt_name();
				struct stat attrib;
				int rv = stat(folder.c_str(), &attrib);
				if(rv>=0)
				{
					req->setUrl(folder+req->getActUrl());
				}
				else
				{
					req->setUrl(webpath+"default"+req->getActUrl());
				}
			}

			if(req->getHeader(HttpRequest::ContentType).find("application/x-www-form-urlencoded")!=string::npos)
			{
				string content;
				/*
				 * Clients can not send chunked requests, due to a requirement of pre-negotiation
				 * in terms of how the request should be sent, this may be required in the future
				 */
				/*
				if(req->isHeaderValue(HttpRequest::TransferEncoding, "chunked"))
				{
					while(true)
					{
						string chunksizstr;
						bool fl = readLine(isSSLEnabled, ssl, sslHandler, io, fd, chunksizstr);
						if(!fl)
						{
							return;
						}
						chunksizstr = chunksizstr.substr(0, chunksizstr.length()-1);
						if(chunksizstr=="0")
						{
							closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
							break;
						}
						long techunkSiz = StringUtil::fromHEX(chunksizstr) + 2;//2 - \r\n
						fl = readData(isSSLEnabled, ssl, sslHandler, io, fd, (int)techunkSiz, content);
						if(!fl)
						{
							return;
						}
					}
				}
				else */
				if(cntlen>0)
				{
					if(!readData(isSSLEnabled, ssl, sslHandler, io, fd, cntlen, content))
					{
						return;
					}
				}
				req->setContent(content);
			}
			else
			{
				/*
				 * Clients can not send chunked requests, due to a requirement of pre-negotiation
				 * in terms of how the request should be sent, this may be required in the future
				 */
				/*
				if(req->isHeaderValue(HttpRequest::TransferEncoding, "chunked"))
				{
					string content;
					ofstream filei;
					string tfilen;
					while(true)
					{
						string chunksizstr;
						bool fl = readLine(isSSLEnabled, ssl, sslHandler, io, fd, chunksizstr);
						if(!fl)
						{
							return;
						}
						chunksizstr = chunksizstr.substr(0, chunksizstr.length()-1);
						if(chunksizstr=="0")
						{
							closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
							break;
						}
						long techunkSiz = StringUtil::fromHEX(chunksizstr) + 2;//2 - \r\n
						fl = readData(isSSLEnabled, ssl, sslHandler, io, fd, (int)techunkSiz, content);
						if(!fl)
						{
							return;
						}
						if(content.length()>2*102400)
						{
							if(!filei.is_open())
							{
								tfilen = req->getCntxt_root() + "/temp/"+ "HttpRequest_" + CastUtil::lexical_cast<string>(Timer::getCurrentTime()) + ".req";
								filei.open(tfilen.c_str(), ios::app | ios::binary);
								filei.write(content.c_str(), content.length());
								content = "";
							}
							else
							{
								filei.write(content.c_str(), content.length());
								content = "";
							}
						}
					}
					if(filei.is_open())
					{
						filei.write(content.c_str(), content.length());
						filei.close();
						req->setContent_tfile(tfilen);
					}
					else
					{
						req->setContent(content);
					}
				}
				else */
				if(cntlen>0)
				{
					bool fmode = cntlen > 2*102400;
					string tfilen;
					ofstream filei;
					if(fmode)
					{
						tfilen = req->getCntxt_root() + "/temp/"+ "HttpRequest_" + CastUtil::lexical_cast<string>(Timer::getCurrentTime()) + ".req";
						filei.open(tfilen.c_str(), ios::app | ios::binary);
					}
					string content;
					if(cntlen>0)
					{
						if(!readData(isSSLEnabled, ssl, sslHandler, io, fd, cntlen, content))
						{
							return;
						}
						if(fmode && filei.is_open())
						{
							filei.write(content.c_str(), content.length());
							content = "";
						}
					}
					if(filei.is_open())
					{
						filei.write(content.c_str(), content.length());
						filei.close();
						req->setContent_tfile(tfilen);
					}
					else
					{
						req->setContent(content);
					}
				}
			}

			/*
			 * Clients can not send compressed requests, due to a requirement of pre-negotiation
			 * in terms of how the request should be sent, this may be required in the future
			 */
			/*
			bool isCEGzip = req->isHeaderValue(HttpRequest::ContentEncoding, "gzip");
			bool isCEDef = req->isHeaderValue(HttpRequest::ContentEncoding, "deflate");
			if(isCEGzip || isCEDef)
			{
				if(req->getContent()!="")
				{
					if(isCEGzip)
						req->setContent(CompressionUtil::gzipUnCompress(req->getContent()));
					else
						req->setContent(CompressionUtil::zlibUnCompress(req->getContent()));

				}
				else
				{
					unsigned int totlen = getFileSize(req->getContent_tfile().c_str());
					float parts = (float)totlen/techunkSiz;
					parts = (floor(parts)<parts?floor(parts)+1:floor(parts));

					if(parts>0)
					{
						ofstream filei;
						string tfilen = req->getCntxt_root() + "/temp/"+ "HttpRequest_" + CastUtil::lexical_cast<string>(Timer::getCurrentTime()) + ".req" + ".unc";
						filei.open(tfilen.c_str(), ios::app | ios::binary);
						for (int var = 0; var <= parts; var++) {
							unsigned int len = totlen - techunkSiz*var;
							if((int)len>techunkSiz)
							{
								len = techunkSiz;
							}

							string temp = getFileContents(req->getContent_tfile().c_str(), techunkSiz*var, len);
							if(isCEGzip)
								temp = CompressionUtil::gzipUnCompress(temp);
							else
								temp = CompressionUtil::zlibUnCompress(temp);
							filei.write(temp.c_str(), temp.length());
						}
						filei.close();
						req->setContent_tfile(tfilen);
					}
				}
			}
			*/

			req->updateContent();

			if(req->getFile()=="")
			{
				logger << ("File requested -> " + req->getFile()) << endl;
				req->setFile("index.html");
			}
			if(req->hasCookie())
			{
				if(!configData.sessatserv)
					req->getSession()->setSessionAttributes(req->getCookieInfo());
				else
				{
					string id = req->getCookieInfoAttribute("FFEADID");
					logger << id << endl;
					map<string,string> values;
#ifdef INC_DSTC
					if(configData.sessservdistocache)
						values = getSessionDataFromDistocache(id);
					else
#endif
						values = getSessionDataFromFile(id);
					req->getSession()->setSessionAttributes(values);
				}
			}

			//logger << req->getCntxt_name() << req->getCntxt_root() << req->getUrl() << endl;
#ifdef INC_APPFLOW
			if(configData.appMap[req->getCntxt_name()]!="false")
			{
				if(dlib == NULL)
				{
					cerr << dlerror() << endl;
					exit(-1);
				}
				string meth1 = (req->getCntxt_name()+"checkRules");
				string path1;
				void *mkr1 = dlsym(dlib, meth1.c_str());
				if(mkr1!=NULL)
				{
					typedef string (*DCPPtr1) (string,HttpSession);
					DCPPtr1 f =  (DCPPtr1)mkr1;
					path1 = f(req->getUrl(),*(req->getSession()));
					//logger << path1 << flush;
					if(path1=="FAILED")
					{
						req->setUrl("");
					}
					else if(path1!="" && path1!=req->getUrl())
					{
						req->setUrl(path1);
					}
				}
			}
#endif

			string ext = getFileExtension(req->getUrl());
			vector<unsigned char> test;
			string content;
			string claz;
			long sessionTimeoutVar = configData.sessionTimeout;

			bool isContrl = false;
			try {
				isContrl = CORSHandler::handle(req, &res, configData);
			} catch(const HTTPResponseStatus& status) {
				res.setHTTPResponseStatus(status);
				isContrl = true;
			}

			if(!isContrl)
			{
				isContrl = securityHandler.handle(configData, req, res, sessionTimeoutVar);
				if(isContrl)
				{
					logger << ("Request handled by SecurityHandler") << endl;
				}
			}

			ext = getFileExtension(req->getUrl());

			if(!isContrl)
			{
				filterHandler.handleIn(req, res, configData, ext);

				isContrl = !filterHandler.handle(req, res, configData, ext);
				if(isContrl)
				{
					logger << ("Request handled by FilterHandler") << endl;
				}
			}

			ext = getFileExtension(req->getUrl());

			if(!isContrl)
			{
				isContrl = authHandler.handle(configData, req, res, ext);
				if(isContrl)
				{
					logger << ("Request handled by AuthHandler") << endl;
				}
			}

			ext = getFileExtension(req->getUrl());

			string pthwofile = req->getCntxt_name()+req->getActUrl();
			if(req->getCntxt_name()!="default" && configData.cntMap[req->getCntxt_name()]=="true")
			{
				pthwofile = req->getActUrl();
			}
			if(!isContrl)
			{
				isContrl = controllerHandler.handle(req, res, configData, ext, pthwofile);
				if(isContrl)
				{
					logger << ("Request handled by ControllerHandler") << endl;
				}
			}

			ext = getFileExtension(req->getUrl());

			/*After going through the controller the response might be blank, just set the HTTP version*/
			res.update(req);
			res.addHeaderValue(HttpResponse::AcceptRanges, "none");
			//logger << req->toString() << endl;
			if(req->getMethod()!="TRACE")
			{
				string wsUrl = "http://" + configData.ip_address + "/" + req->getCntxt_name() + "/" + req->getFile();
				if(isContrl)
				{

				}
				else if(ext==".form")
				{
					formHandler.handle(req, res, configData);
					logger << ("Request handled by FormHandler") << endl;
				}
#ifdef INC_WEBSVC
				else if(configData.wsdlmap[wsUrl]!="")
				{
					if(req->getHeader(HttpRequest::ContentType).find("application/soap+xml")!=string::npos || req->getHeader(HttpRequest::ContentType).find("text/xml")!=string::npos
							|| req->getHeader(HttpRequest::ContentType).find("application/xml")!=string::npos)
					{
						soapHandler.handle(req, res, dlib, configData);
					}
					else
					{
						logger << ("Invalid Content type for soap request") << endl;
						res.setHTTPResponseStatus(HTTPResponseStatus::BadRequest);
					}
					logger << ("Request handled by SoapHandler for Url "+wsUrl) << endl;
				}
#endif
				else
				{
					bool cntrlit = false;
#ifdef INC_SCRH
					cntrlit = scriptHandler.handle(req, res, configData.handoffs, ext, configData.props);
#endif
					if(cntrlit)
					{
						logger << ("Request handled by ScriptHandler") << endl;
					}
					else
					{
						cntrlit = extHandler.handle(req, res, dlib, ddlib, configData, ext);
						if(cntrlit)
						{
							logger << ("Request handled by ExtHandler") << endl;
						}
					}
					if(!cntrlit && ext==".fview")
					{
						fviewHandler.handle(req, res, configData.fviewmap);
						logger << ("Request handled by FviewHandler") << endl;
					}
					else
					{
						logger << ("Request for static resource/file") << endl;
						if(req->isAgentAcceptsCE() && (cntEnc=="gzip" || cntEnc=="deflate") && req->isNonBinary(configData.props[ext]))
						{
							res.addHeaderValue(HttpResponse::ContentEncoding, cntEnc);
						}

						if(res.getContent()=="")
							updateContent(req, &res, configData, ext, techunkSiz);
						else
						{
							content = res.getContent();
							if(content.length()==0 && res.getHeader(HttpResponse::ContentLength)!="")
							{
								res.setHTTPResponseStatus(HTTPResponseStatus::NotFound);
							}
							else
							{
								res.setHTTPResponseStatus(HTTPResponseStatus::Ok);
								if(res.getHeader(HttpResponse::ContentType)=="")
								{
									res.addHeaderValue(HttpResponse::ContentType, configData.props[ext]);
								}
								res.setContent(content);
							}
						}
					}
				}

				filterHandler.handleOut(req, res, configData, ext);
			}

			bool isTE = res.isHeaderValue(HttpResponse::TransferEncoding, "chunked");
			if(!isTE && req->isAgentAcceptsCE() && (cntEnc=="gzip" || cntEnc=="deflate") && res.isNonBinary())
			{
				res.addHeaderValue(HttpResponse::ContentEncoding, cntEnc);
			}

			Date cdate(true);
			DateFormat df("ddd, dd mmm yyyy hh:mi:ss GMT");
			res.addHeaderValue(HttpResponse::Date, df.format(cdate));

			alldatlg += "--processed data";
			string h1;

			//if(req->getConnection()!="")
			//	res.setConnection("close");
			storeSessionAttributes(res, req, sessionTimeoutVar, configData.sessatserv);

			//An errored request/response phase will close the connection
			if(StringUtil::toLowerCopy(req->getHeader(HttpRequest::Connection))!="keep-alive" || CastUtil::lexical_cast<int>(res.getStatusCode())>307
					 || req->getHttpVers()<1.1)
			{
				res.addHeaderValue(HttpResponse::Connection, "close");
				cont = false;
			}
			else
			{
				res.addHeaderValue(HttpResponse::Connection, "keep-alive");
			}

			//Head should behave exactly as Get but there should be no entity body
			h1 = res.generateResponse(req->getMethod(), req);
			/*if(req->getMethod()=="HEAD")
			{
				h1 = res.generateHeadResponse();
			}
			else if(req->getMethod()=="OPTIONS")
			{
				h1 = res.generateOptionsResponse();
			}
			else if(req->getMethod()=="TRACE")
			{
				h1 = res.generateTraceResponse(req);
			}
			else
			{
				h1 = res.generateResponse();
			}*/

			if(res.isHeaderValue(HttpResponse::TransferEncoding, "chunked"))
			{
				bool sendSuccess = sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, h1);
				if(!sendSuccess)return;
				unsigned int totlen = getFileSize(req->getUrl().c_str());
				float parts = (float)totlen/techunkSiz;
				parts = (floor(parts)<parts?floor(parts)+1:floor(parts)) - 1;

				if(parts>0)
				{
					for (int var = 1; var <= parts; var++) {
						unsigned int len = totlen - techunkSiz*var;
						if((int)len>techunkSiz)
						{
							len = techunkSiz;
						}
						h1 = StringUtil::toHEX(len) + "\r\n";
						h1 += getFileContents(req->getUrl().c_str(), techunkSiz*var, len);
						h1 += "\r\n";
						bool sendSuccess = sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, h1);
						if(!sendSuccess)return;
					}
					bool sendSuccess = sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, "0\r\n\r\n");
					if(!sendSuccess)return;
				}
			}
			else
			{
				bool sendSuccess = sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, h1);
				if(!sendSuccess)return;
			}

			if(!cont)
			{
				logger << "Closing connection..." << endl;
				closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
				return;
			}

			//Logger::info("got new connection to process\n"+req->getFile()+" :: " + res.getStatusCode() + "\n"+req->getCntxt_name() + "\n"+req->getCntxt_root() + "\n"+req->getUrl());
			delete req;
			//logger << (alldatlg + "--sent data--DONE") << endl;
			//sessionMap[sessId] = sess;
		}
		catch(const char* err)
		{
			logger << "Exception occurred while processing ServiceTask request - " << err << endl;
		}
		catch(...)
		{
			logger << "Standard exception occurred while processing ServiceTask request " << endl;
		}
	}
}


bool ServiceTask::sendData(bool isSSLEnabled, ConfigurationData configData, SSL* ssl, SSLHandler sslHandler, BIO* io, int fd, string h1)
{
	if(isSSLEnabled)
	{
		int r;
		/* Now perform renegotiation if requested */
		if(configData.client_auth==CLIENT_AUTH_REHANDSHAKE){
		  SSL_set_verify(ssl,SSL_VERIFY_PEER |
			SSL_VERIFY_FAIL_IF_NO_PEER_CERT,0);

		  /* Stop the client from just resuming the
			 un-authenticated session */
		  SSL_set_session_id_context(ssl,
			(const unsigned char*)&SSLHandler::s_server_auth_session_id_context,
			sizeof(SSLHandler::s_server_auth_session_id_context));

		  if(SSL_renegotiate(ssl)<=0)
		  {
			  logger << "SSL renegotiation error" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return false;
		  }
		  if(SSL_do_handshake(ssl)<=0)
		  {
			  logger << "SSL renegotiation error" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return false;;
		  }
		  ssl->state=SSL_ST_ACCEPT;
		  if(SSL_do_handshake(ssl)<=0)
		  {
			  logger << "SSL handshake error" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return false;;
		  }
		}
		if((r=BIO_write(io, h1.c_str(),h1.length()))<=0)
		{
			  logger << "Send failed" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return false;;
		}
		if((r=BIO_flush(io))<0)
		{
			  logger << "Error flushing BIO" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return false;;
		}
		//sslHandler.closeSSL(fd,ssl,io);
	}
	else
	{
		int size;
		if ((size=send(fd,&h1[0] , h1.length(), 0)) <= 0)
		{
			logger << "send failed" << flush;
			closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			return false;
		}
		//if(io!=NULL)BIO_free_all(io);
	}
	return true;
}

void ServiceTask::closeSocket(bool isSSLEnabled, SSL* ssl, SSLHandler sslHandler, BIO* io, int fd)
{
	if(isSSLEnabled)
	{
		sslHandler.closeSSL(fd,ssl,io);
		if(io!=NULL)BIO_free(io);
	}
	else
	{
		if(io!=NULL)BIO_free_all(io);
		close(fd);
	}
}

bool ServiceTask::readLine(bool isSSLEnabled, SSL* ssl, SSLHandler sslHandler, BIO* io, int fd, string& line)
{
	/*if(!checkSocketWaitForTimeout(fd, 0, 0, 10))
	{
		logger << "Closing connection as there was no data to read in 10us..." << endl;
		closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
		return false;
	}*/
	char buf[MAXBUFLENM];
	if(isSSLEnabled)
	{
		int er=-1;
		er = BIO_gets(io,buf,BUFSIZZ-1);
		int bser = SSL_get_error(ssl,er);
		switch(bser)
		{
			case SSL_ERROR_WANT_READ:
			{
				logger << "ssl more to read error" << endl;
				break;
			}
			case SSL_ERROR_WANT_WRITE:
			{
				logger << "ssl more to write error" << endl;
				break;
			}
			case SSL_ERROR_NONE:
			{
				break;
			}
			case SSL_ERROR_ZERO_RETURN:
			{
				closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
				return false;
			}
			default:
			{
				logger << "SSL read problem" << endl;
				closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
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
			closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			logger << "Socket closed before being serviced" << endl;
			return false;
		}
		line.append(buf, er);
		memset(&buf[0], 0, sizeof(buf));
	}
	return true;
}

bool ServiceTask::readData(bool isSSLEnabled, SSL* ssl, SSLHandler sslHandler, BIO* io, int fd, int cntlen, string& content)
{
	/*if(!checkSocketWaitForTimeout(fd, 0, 0, 10))
	{
		logger << "Closing connection as there was no data to read in 10us..." << endl;
		closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
		return false;
	}*/
	char buf[MAXBUFLENM];
	if(isSSLEnabled && cntlen>0)
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
					closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
					return false;
				}
				default:
				{
					logger << "SSL read problem" << endl;
					closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
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
			logger << "Length is " + CastUtil::lexical_cast<string>(cntlen) << endl;
			int readLen = MAXBUFLENM;
			if(cntlen<MAXBUFLENM)
				readLen = cntlen;
			er = BIO_read(io,buf,readLen);
			logger << "Done reading" << endl;
			if(er==0)
			{
				closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
				logger << "Socket closed before being serviced" << endl;
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
}


HttpResponse ServiceTask::apacheRun(HttpRequest* req)
{
	//logger << dlib << endl;
	HttpResponse res;
	string ip;
	string alldatlg = "\ngot fd from parent";
	try
	{
		string webpath = serverRootDirectory + "web/";

		if(req->getFile()=="")
		{
			logger << req->getFile() << endl;
			req->setFile("index.html");
		}
		if(req->hasCookie())
		{
			if(!configData.sessatserv)
				req->getSession()->setSessionAttributes(req->getCookieInfo());
			else
			{
				string id = req->getCookieInfoAttribute("FFEADID");
				map<string,string> values;
#ifdef INC_DSTC
				if(configData.sessservdistocache)
					values = getSessionDataFromDistocache(id);
				else
#endif
					values = getSessionDataFromFile(id);
				req->getSession()->setSessionAttributes(values);
			}
		}

		if(configData.cntMap[req->getCntxt_name()]!="true")
		{
			req->setCntxt_name("default");
			req->setCntxt_root(webpath+"default");
			req->setUrl(webpath+"default"+req->getActUrl());
		}
		//logger << req->getCntxt_name() << req->getCntxt_root() << req->getUrl() << endl;

#ifdef INC_APPFLOW
		if(configData.appMap[req->getCntxt_name()]!="false")
		{
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			string meth1 = (req->getCntxt_name()+"checkRules");
			string path1;
			void *mkr1 = dlsym(dlib, meth1.c_str());
			if(mkr1!=NULL)
			{
				typedef string (*DCPPtr1) (string,HttpSession);
				DCPPtr1 f =  (DCPPtr1)mkr1;
				path1 = f(req->getUrl(),*(req->getSession()));
				//logger << path1 << flush;
				if(path1=="FAILED")
				{
					req->setUrl("");
				}
				else if(path1!="" && path1!=req->getUrl())
				{
					req->setUrl(path1);
				}
			}
		}
#endif

		string ext = getFileExtension(req->getUrl());
		vector<unsigned char> test;
		string content;
		string claz;
		//bool isoAuthRes = false;
		long sessionTimeoutVar = configData.sessionTimeout;
		bool isContrl = securityHandler.handle(configData, req, res, sessionTimeoutVar);

		filterHandler.handleIn(req, res, configData, ext);

		if(!isContrl)
		{
			isContrl = authHandler.handle(configData, req, res, ext);
		}
		string pthwofile = req->getCntxt_name()+req->getActUrl();
		if(req->getCntxt_name()!="default" && configData.cntMap[req->getCntxt_name()]=="true")
		{
			pthwofile = req->getActUrl();
		}
		if(!isContrl)
		{
			isContrl = controllerHandler.handle(req, res, configData, ext, pthwofile);
		}

		/*After going through the controller the response might be blank, just set the HTTP version*/
		res.update(req);
		//logger << req->toString() << endl;
		if(req->getMethod()!="TRACE")
		{
			if(isContrl)
			{

			}
			else if(ext==".form")
			{
				formHandler.handle(req, res, configData);
			}
#ifdef INC_WEBSVC
			else if((req->getHeader(HttpRequest::ContentType).find("application/soap+xml")!=string::npos || req->getHeader(HttpRequest::ContentType).find("text/xml")!=string::npos)
					&& (req->getContent().find("<soap:Envelope")!=string::npos || req->getContent().find("<soapenv:Envelope")!=string::npos)
					&& configData.wsdlmap[req->getFile()]==req->getCntxt_name())
			{
				soapHandler.handle(req, res, dlib, configData);
			}
#endif
			else
			{
				bool cntrlit = false;
#ifdef INC_SCRH
				cntrlit = scriptHandler.handle(req, res, configData.handoffs, ext, configData.props);
#endif
				if(cntrlit)
				{

				}
				else
				{
					cntrlit = extHandler.handle(req, res, dlib, ddlib, configData, ext);
				}
				if(!cntrlit && ext==".fview")
				{
					fviewHandler.handle(req, res, configData.fviewmap);
				}
				else
				{
					if(res.getContent()=="")
						updateContent(req, &res, configData, ext, 8192);
					else
					{
						content = res.getContent();
						if(content.length()==0)
						{
							res.setHTTPResponseStatus(HTTPResponseStatus::NotFound);
						}
						else
						{
							res.setHTTPResponseStatus(HTTPResponseStatus::Ok);
							if(res.getHeader(HttpResponse::ContentType)=="")
							{
								res.addHeaderValue(HttpResponse::ContentType, configData.props[ext]);
							}
							res.setContent(content);
						}
					}
				}
			}

			filterHandler.handleOut(req, res, configData, ext);
		}

		Date cdate(true);
		DateFormat df("ddd, dd mmm yyyy hh:mi:ss GMT");
		res.addHeaderValue(HttpResponse::Date, df.format(cdate));

		alldatlg += "--processed data";
		string h1;
		if(req->getHeader("Connection")!="")
			res.addHeaderValue(HttpResponse::Connection, "close");
		storeSessionAttributes(res, req, sessionTimeoutVar, configData.sessatserv);
		//h1 = res.generateResponse();
		delete req;
		//logger << (alldatlg + "--sent data--DONE") << endl;
		//sessionMap[sessId] = sess;
	}
	catch(const char* ex)
	{
		logger << ex << endl;
	}
	catch(...)
	{
		logger << "Standard exception occurred while processing ServiceTask request " << endl;
	}
	return res;
}
