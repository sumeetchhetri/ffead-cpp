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
	logger = Logger::getLogger("ServiceTask");
}

ServiceTask::~ServiceTask() {
	// TODO Auto-generated destructor stub
}

void ServiceTask::writeToSharedMemeory(string sessionId, string value,bool napp)
{
	string filen = serverRootDirectory+"/tmp/"+sessionId+".sess";
	logger << ("Saving session to file " + filen) << endl;
	ofstream ofs;
	if(napp)
		ofs.open(filen.c_str());
	else
		ofs.open(filen.c_str(),ios::app);
	ofs.write(value.c_str(),value.length());
	ofs.close();
}

map<string,string> ServiceTask::readFromSharedMemeory(string sessionId)
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

string ServiceTask::getFileExtension(string file)
{
	if(file.find_last_of(".")!=string::npos)return file.substr(file.find_last_of("."));
	return file;
}

void ServiceTask::createResponse(HttpResponse &res,bool flag,map<string,string> vals,string prevcookid, long sessionTimeout, bool sessatserv)
{
	if(flag)
	{
		string values;
		//logger << "session object modified " << vals.size() << endl;
		Date date;
		string id = CastUtil::lexical_cast<string>(Timer::getCurrentTime());
		//int seconds = sessionTimeout;
		date = date.addSeconds(sessionTimeout);
		DateFormat dformat("ddd, dd-mmm-yyyy hh:mi:ss");
		map<string,string>::iterator it;
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
		if(sessatserv)
		{
			if(values!="")
			{
				if(prevcookid!="")
					writeToSharedMemeory(prevcookid,values,true);
				else
				{
					writeToSharedMemeory(id,values,false);
					res.addCookie("FFEADID=" + id + "; expires="+dformat.format(date)+" GMT; path=/; HttpOnly");
				}
			}
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

		if(res->isHeaderValue(HttpResponse::ContentEncoding, "gzip"))
		{
			string ofname = req->getCntxt_root() + "/temp/" + req->getFile() + ".gz";
			ifstream gzipdfile(ofname.c_str());
			if(gzipdfile.good())
			{
				gzipdfile.close();
			}
			else
			{
				CompressionUtil::gzipCompressFile((char*)fname.c_str(), false, (char*)ofname.c_str());
			}
			fname = ofname;
			res->setCompressed(true);
			req->setUrl(fname);
		}
		else if(res->isHeaderValue(HttpResponse::ContentEncoding, "deflate"))
		{
			string ofname = req->getCntxt_root() + "/temp/" + req->getFile() + ".z";
			ifstream gzipdfile(ofname.c_str());
			if(gzipdfile.good())
			{
				gzipdfile.close();
			}
			else
			{
				CompressionUtil::zlibCompressFile((char*)fname.c_str(), false, (char*)ofname.c_str());
			}
			fname = ofname;
			res->setCompressed(true);
			req->setUrl(fname);
		}

		logger << ("Content request for " + url + " " + ext + " actual file " + fname) << endl;

		struct tm* tim;
		struct stat attrib;
		stat(fname.c_str(), &attrib);
		tim = gmtime(&(attrib.st_mtime));
		Date fdate(tim);
		DateFormat df("ddd, dd mmm yyyy hh:mm:ss GMT");
		string lastmodDate = df.format(fdate.toGMT());
		res->addHeaderValue(HttpResponse::LastModified, lastmodDate);

		if(req->isHeaderValue(HttpRequest::IfModifiedSince, lastmodDate))
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::NotModified);
			return;
		}

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
					MultipartContent conte;
					conte.setContent(cont);
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
			res->setContent_str(all);
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
		cout << r << endl;
		int bser = SSL_get_error(ssl,r);
		cout << bser << endl;
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
					sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, res.generateResponse());
					logger << "Closing connection..." << endl;
					closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
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
				sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, res.generateResponse());
				logger << "Closing connection..." << endl;
				closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
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
					sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, res.generateResponse());
					logger << "Closing connection..." << endl;
					closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
					return;
				}
			}
			catch(const char* ex)
			{
				logger << "Bad lexical cast exception while reading http Content-Length" << endl;
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
					int length = 0;
					while(cntlen>0)
					{
						if(cntlen>102400)
						{
							length = 102400;
						}
						else
						{
							length = cntlen;
						}
						cntlen -= length;
						if(!readData(isSSLEnabled, ssl, sslHandler, io, fd, length, content))
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

			//logger << req->toString() << endl;
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
					map<string,string> values = readFromSharedMemeory(id);
					req->getSession()->setSessionAttributes(values);
				}
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
			//logger << req->getCntxt_name() << req->getCntxt_root() << req->getUrl() << endl;

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

			if(!isContrl)
			{
				filterHandler.handleIn(req, res, configData, ext);

				isContrl = !filterHandler.handle(req, res, configData, ext);
				if(isContrl)
				{
					logger << ("Request handled by FilterHandler") << endl;
				}
			}

			if(!isContrl)
			{
				isContrl = authHandler.handle(configData, req, res, ext);
				if(isContrl)
				{
					logger << ("Request handled by AuthHandler") << endl;
				}
			}

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
				else
				{
					bool cntrlit = scriptHandler.handle(req, res, configData.handoffs, ext, configData.props);
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

						if(res.getContent_str()=="")
							updateContent(req, &res, configData, ext, techunkSiz);
						else
						{
							content = res.getContent_str();
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
								res.setContent_str(content);
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

			Date cdate;
			DateFormat df("ddd, dd mmm yyyy hh:mm:ss GMT");
			res.addHeaderValue(HttpResponse::Date, df.format(cdate.toGMT()));

			alldatlg += "--processed data";
			string h1;
			bool sessionchanged = !req->hasCookie();
			sessionchanged |= req->getSession()->isDirty();
			//if(req->getConnection()!="")
			//	res.setConnection("close");
			createResponse(res,sessionchanged,req->getSession()->getSessionAttributes(),req->getCookieInfoAttribute("FFEADID"), sessionTimeoutVar, configData.sessatserv);

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
			if(req->getMethod()=="HEAD")
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
			}

			if(res.isHeaderValue(HttpResponse::TransferEncoding, "chunked"))
			{
				sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, h1);

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
						sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, h1);
					}
					sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, "0\r\n\r\n");
				}
			}
			else
			{
				sendData(isSSLEnabled, configData, ssl, sslHandler, io, fd, h1);
			}

			if(!cont)
			{
				logger << "Closing connection..." << endl;
				closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			}

			//Logger::info("got new connection to process\n"+req->getFile()+" :: " + res.getStatusCode() + "\n"+req->getCntxt_name() + "\n"+req->getCntxt_root() + "\n"+req->getUrl());
			delete req;
			//logger << (alldatlg + "--sent data--DONE") << endl;
			//sessionMap[sessId] = sess;
		}
		catch(...)
		{
			logger << "Standard exception occurred while processing ServiceTask request " << endl;
		}
	}
}


void ServiceTask::sendData(bool isSSLEnabled, ConfigurationData configData, SSL* ssl, SSLHandler sslHandler, BIO* io, int fd, string h1)
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
			  return;
		  }
		  if(SSL_do_handshake(ssl)<=0)
		  {
			  logger << "SSL renegotiation error" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return;
		  }
		  ssl->state=SSL_ST_ACCEPT;
		  if(SSL_do_handshake(ssl)<=0)
		  {
			  logger << "SSL handshake error" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return;
		  }
		}
		if((r=BIO_write(io,h1.c_str(),h1.length()))<=0)
		{
			  logger << "Send failed" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return;
		}
		if((r=BIO_flush(io))<0)
		{
			  logger << "Error flushing BIO" << endl;
			  closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			  return;
		}
		//sslHandler.closeSSL(fd,ssl,io);
	}
	else
	{
		int size;
		if ((size=send(fd,&h1[0] , h1.length(), 0)) == -1)
			logger << "Socket send failed" << endl;
		else if(size==0)
		{
			closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
			logger << "Socket closed for writing" << endl;
			return;
		}

		//if(io!=NULL)BIO_free_all(io);
	}
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
		if(cntlen>0)
		{
			er = BIO_read(io,buf,cntlen);
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
			content.append(buf, er);
			memset(&buf[0], 0, sizeof(buf));
		}
	}
	else if(cntlen>0)
	{
		int er=-1;
		if(cntlen>0)
		{
			er = BIO_read(io,buf,cntlen);
			if(er==0)
			{
				closeSocket(isSSLEnabled, ssl, sslHandler, io, fd);
				logger << "Socket closed before being serviced" << endl;
				return false;
			}
			else if(er>0)
			{
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
				map<string,string> values = readFromSharedMemeory(id);
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
			else if((req->getHeader(HttpRequest::ContentType).find("application/soap+xml")!=string::npos || req->getHeader(HttpRequest::ContentType).find("text/xml")!=string::npos)
					&& (req->getContent().find("<soap:Envelope")!=string::npos || req->getContent().find("<soapenv:Envelope")!=string::npos)
					&& configData.wsdlmap[req->getFile()]==req->getCntxt_name())
			{
				soapHandler.handle(req, res, dlib, configData);
			}
			else
			{
				bool cntrlit = scriptHandler.handle(req, res, configData.handoffs, ext, configData.props);
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
					if(res.getContent_str()=="")
						updateContent(req, &res, configData, ext, 8192);
					else
					{
						content = res.getContent_str();
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
							res.setContent_str(content);
						}
					}
				}
			}

			filterHandler.handleOut(req, res, configData, ext);
		}

		Date cdate;
		DateFormat df("ddd, dd mmm yyyy hh:mm:ss GMT");
		res.addHeaderValue(HttpResponse::Date, df.format(cdate.toGMT()));

		alldatlg += "--processed data";
		string h1;
		bool sessionchanged = !req->hasCookie();
		sessionchanged |= req->getSession()->isDirty();
		if(req->getHeader("Connection")!="")
			res.addHeaderValue(HttpResponse::Connection, "close");
		createResponse(res,sessionchanged,req->getSession()->getSessionAttributes(),req->getCookieInfoAttribute("FFEADID"), sessionTimeoutVar, configData.sessatserv);
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
